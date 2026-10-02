/* RFC-0146 process/file transport. Query and source decisions remain in SLIM. */
#include "native-inputs.h"
#include <fcntl.h>
#include <poll.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#if defined(__APPLE__) && defined(__aarch64__)
#include <mach/mach.h>
#include <mach-o/dyld_images.h>
#include <libproc.h>
#include <sys/proc.h>
#endif

#define NATIVE_BYTE_LIMIT 67108864u
#define NATIVE_MANIFEST_LIMIT (512u * (64u + 1u + 4096u + 1u))
#define NATIVE_PROCESS_NS UINT64_C(180000000000)
#define NATIVE_CLEANUP_NS UINT64_C(5000000000)
#define NATIVE_PROCESS_LIMIT 512u

typedef struct {
    SlimBytes bytes;
    void *owned;
} NativeBytes;
typedef struct {
    uint64_t starts[3], elapsed[3], setup;
    bool hits[3];
    SlimBytes retention[3];
} NativeWork;
static char native_directory[128], native_context[1024];
static NativeBytes native_inputs;
static int native_context_state; /* 0 lazy, 1 captured, 2 declined */
static SlimRegion native_region;
static bool native_region_live, native_allocation_failure;
static bool native_cleanup_failure;
static Slim_type_pnativecache_95_95State native_cache;
static uint64_t native_allocations, native_fail_at;

static uint64_t native_now(void) {
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) return 0;
    return (uint64_t)value.tv_sec * UINT64_C(1000000000) + (uint64_t)value.tv_nsec;
}
static void *native_allocate(size_t size) {
    if (native_allocations == UINT64_MAX || ++native_allocations == native_fail_at) {
        native_allocation_failure = true;
        return NULL;
    }
    void *result = malloc(size ? size : 1);
    if (!result) native_allocation_failure = true;
    return result;
}
static bool native_path(char out[4096], const char *name) {
    int n = snprintf(out, 4096, "%s/%s", native_directory, name);
    return n > 0 && n < 4096;
}
static bool native_write_file(const char *name, SlimBytes data) {
    char path[4096];
    if (data.len < 0 || data.len > NATIVE_BYTE_LIMIT || !native_path(path, name)) return false;
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0600);
    if (fd < 0) return false;
    size_t at = 0, size = (size_t)data.len;
    while (at < size) {
        ssize_t n = write(fd, data.data + at, size - at);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { (void)close(fd); return false; }
        at += (size_t)n;
    }
    return close(fd) == 0;
}
static NativeBytes native_read_file(const char *name, size_t limit) {
    NativeBytes result = {{NULL, 0}, NULL};
    char path[4096];
    if (!native_path(path, name)) return result;
    int fd = open(path, O_RDONLY | O_NOFOLLOW);
    if (fd < 0) return result;
    struct stat info;
    if (fstat(fd, &info) != 0 || !S_ISREG(info.st_mode) || info.st_size <= 0 || (uint64_t)info.st_size > limit) {
        (void)close(fd); return result;
    }
    size_t size = (size_t)info.st_size, at = 0;
    uint8_t *buffer = native_allocate(size);
    if (!buffer) { (void)close(fd); return result; }
    while (at < size) {
        ssize_t n = read(fd, buffer + at, size - at);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) break;
        at += (size_t)n;
    }
    uint8_t extra;
    ssize_t trailing;
    do { trailing = read(fd, &extra, 1); } while (trailing < 0 && errno == EINTR);
    bool complete = close(fd) == 0 && at == size && trailing == 0;
    if (!complete) { free(buffer); return result; }
    result.bytes = slim_bytes_static(buffer, (int64_t)size);
    result.owned = buffer;
    return result;
}

/* The capture recipe emits SHA-256 TAB relative-path LF, once per input.
   Retain that exact bounded list with the connection. A private subtree alone
   does not establish that a linker input belongs to the captured context. */
static bool native_manifest_valid(SlimBytes manifest) {
    if (manifest.len <= 0 || manifest.len > NATIVE_MANIFEST_LIMIT) return false;
    size_t at = 0, count = 0;
    while (at < (size_t)manifest.len) {
        if (++count > 512 || (size_t)manifest.len - at < 67) return false;
        for (unsigned i = 0; i < 64; ++i) {
            uint8_t b = manifest.data[at++];
            if (!((b >= '0' && b <= '9') || (b >= 'a' && b <= 'f'))) return false;
        }
        if (manifest.data[at++] != '\t') return false;
        size_t begin = at, component = at;
        while (at < (size_t)manifest.len && manifest.data[at] != '\n') {
            uint8_t b = manifest.data[at];
            if (!b || !strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_./+-", b)) return false;
            if (b == '/') {
                size_t length = at - component;
                if (!length || (length <= 2 && manifest.data[component] == '.' &&
                    (length == 1 || manifest.data[component+1] == '.'))) return false;
                component = at + 1;
            }
            if (++at - begin > 4096) return false;
        }
        size_t length = at - component, size = at - begin;
        if (!length || (length <= 2 && manifest.data[component] == '.' &&
            (length == 1 || manifest.data[component+1] == '.')) || at == (size_t)manifest.len) return false;
        if (!((size > 4 && memcmp(manifest.data+begin, "sdk/", 4) == 0) ||
              (size > 10 && memcmp(manifest.data+begin, "toolchain/", 10) == 0))) return false;
        ++at;
    }
    return true;
}
static bool native_manifest_contains(const char *path, size_t size) {
    size_t at = 0;
    /* native_inputs was validated before context publication. It is immutable
       and owned by this host; no query or request can write its bytes. */
    while (at < (size_t)native_inputs.bytes.len) {
        at += 65;
        size_t begin = at;
        while (native_inputs.bytes.data[at] != '\n') ++at;
        if (at - begin == size && memcmp(native_inputs.bytes.data+begin, path, size) == 0) return true;
        ++at;
    }
    return false;
}

/* One process group per operation. Drain output while waiting; limit exhaustion
   kills that group and reaps its leader. Capture setup is distinct from backend
   starts; application executables are never passed to this function. */
static int native_group_live(pid_t leader) {
#if defined(__APPLE__) && defined(__aarch64__)
    pid_t members[NATIVE_PROCESS_LIMIT + 1];
    errno = 0;
    int size = proc_listpids(PROC_PGRP_ONLY, (uint32_t)leader, members, sizeof(members));
    if (size < 0 || (size == 0 && errno) || size % sizeof(pid_t) != 0 ||
        (size_t)size > NATIVE_PROCESS_LIMIT * sizeof(pid_t)) return -1;
    bool live = false;
    for (size_t i = 0; i < (size_t)size / sizeof(pid_t); ++i) {
        if (members[i] <= 0) return -1;
        struct proc_bsdinfo info;
        errno = 0;
        int bytes = proc_pidinfo(members[i], PROC_PIDTBSDINFO, 0, &info, sizeof(info));
        if (bytes == 0 && errno == ESRCH) continue; /* Exited, including zombies. */
        if (bytes != sizeof(info)) return -1;
        /* An already reaped descendant's PID may have been reused elsewhere;
           the unreaped leader still pins this group's identity. */
        if (info.pbi_pgid == (uint32_t)leader && info.pbi_status != SZOMB) live = true;
    }
    return live ? 1 : 0;
#else
    (void)leader;
    return -1; /* No native provider promises this process context. */
#endif
}
static bool native_stop_group(pid_t leader, uint64_t begin) {
    int signalled = kill(-leader, SIGKILL), error = errno;
    /* Darwin's group signal skips zombies and returns EPERM for a zombie-only
       group. Never infer emptiness from that error; inspect the pinned group. */
    if (signalled != 0 && error != ESRCH && error != EPERM) return false;
    for (;;) {
        int live = native_group_live(leader);
        if (live == 0) return true;
        if (live < 0 || signalled != 0) return false;
        uint64_t now = native_now();
        if (!now || now < begin || now - begin >= NATIVE_CLEANUP_NS) return false;
        (void)poll(NULL, 0, 10);
    }
}
static int native_process(char *const argv[], bool setup, bool diagnostics, uint64_t *starts, uint64_t *elapsed) {
    int pipes[2];
    if (pipe(pipes) != 0) return 3;
    uint64_t begin = native_now();
    if (!begin) { close(pipes[0]); close(pipes[1]); return 3; }
    pid_t child = fork();
    if (child == 0) {
        (void)close(pipes[0]);
        if (setpgid(0, 0) != 0) {
            int group_error = errno;
            /* The parent may already have established this exact group. */
            if (group_error != EPERM || getpgrp() != getpid()) _exit(126);
        }
        if (chdir(native_directory) != 0 ||
            dup2(pipes[1], STDOUT_FILENO) < 0 || dup2(pipes[1], STDERR_FILENO) < 0) _exit(126);
        (void)close(pipes[1]);
        int null_fd = open("/dev/null", O_RDONLY);
        if (null_fd < 0 || dup2(null_fd, STDIN_FILENO) < 0) _exit(126);
        if (null_fd != STDIN_FILENO) (void)close(null_fd);
        struct rlimit file_limit = {setup ? 536870912 : NATIVE_BYTE_LIMIT, setup ? 536870912 : NATIVE_BYTE_LIMIT};
        struct rlimit cpu_limit = {180, 180};
        if (setrlimit(RLIMIT_FSIZE, &file_limit) != 0 || setrlimit(RLIMIT_CPU, &cpu_limit) != 0) _exit(126);
        char temporary[256];
        if (snprintf(temporary, sizeof(temporary), "TMPDIR=%s/tmp", native_directory) >= (int)sizeof(temporary)) _exit(126);
        char *environment[] = {"LC_ALL=C", "PATH=/usr/bin:/bin", temporary, NULL};
        execve(argv[0], argv, environment);
        _exit(127);
    }
    (void)close(pipes[1]);
    if (child < 0) { close(pipes[0]); return 3; }
    if (starts) ++*starts;
    /* The child also sets its group, closing the parent/exec scheduling race. */
    (void)setpgid(child, child);
    int flags = fcntl(pipes[0], F_GETFL);
    int result = 0;
    if (flags < 0 || fcntl(pipes[0], F_SETFL, flags | O_NONBLOCK) != 0) result = 3;
    bool eof = false, finished = false, stopped = false;
    uint64_t cleanup_begin = 0;
    while ((!eof || !finished) && !result) {
        if (!finished) {
            siginfo_t event = {0};
            int observed = waitid(P_PID, (id_t)child, &event, WEXITED | WNOHANG | WNOWAIT);
            if (observed < 0) {
                if (errno == EINTR) continue;
                result = 3; break;
            }
            finished = event.si_pid == child;
        }
        if (finished && !stopped) {
            /* Keep the leader waitable until every group member is stopped.
               Reaping first permits its PID/group identity to be reused. A
               successful tool may still have descendants with closed stdio. */
            stopped = true;
            cleanup_begin = native_now();
            if (!cleanup_begin || !native_stop_group(child, cleanup_begin)) {
                native_cleanup_failure = true; result = 3; break;
            }
        }
        if (eof && finished) break;
        uint64_t now = native_now();
        if (!now || now < begin || now - begin >= NATIVE_PROCESS_NS) { result = 4; break; }
        if (eof) { (void)poll(NULL, 0, 10); continue; }
        struct pollfd ready = {pipes[0], POLLIN, 0};
        int available = poll(&ready, 1, 100);
        if (available < 0) { if (errno == EINTR) continue; result = 3; break; }
        if (available == 0) continue;
        /* One bounded read per iteration also checks exit and the deadline for
           a continuously writing child, including diagnostic-free cleanup. */
        unsigned char buffer[8192];
        ssize_t n = read(pipes[0], buffer, sizeof(buffer));
        if (n > 0) {
            if (diagnostics) {
                host_capture(buffer, (size_t)n);
                if (host_capture_failure) result = 4;
            }
        } else if (n == 0) eof = true;
        else if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) result = 3;
    }
    (void)close(pipes[0]);
    if (!cleanup_begin) cleanup_begin = native_now();
    /* The direct child is still ours even if it failed before creating its
       process group. Never leave that child live on the error path. */
    if (!finished && kill(child, SIGKILL) != 0 && errno != ESRCH) {
        native_cleanup_failure = true; result = 3;
    }
    if (!stopped && (!cleanup_begin || !native_stop_group(child, cleanup_begin))) {
        native_cleanup_failure = true; result = 3;
    }
    int status = 0;
    pid_t waited;
    for (;;) {
        waited = waitpid(child, &status, WNOHANG);
        if (waited == child) break;
        if (waited < 0 && errno != EINTR) { native_cleanup_failure = true; result = 3; break; }
        uint64_t now = native_now();
        if (!now || !cleanup_begin || now < cleanup_begin || now - cleanup_begin >= NATIVE_CLEANUP_NS) {
            native_cleanup_failure = true; result = 3; break;
        }
        (void)poll(NULL, 0, 10);
    }
    uint64_t end = native_now();
    *elapsed = end >= begin ? end - begin : 0;
    if (!result && (waited != child || !WIFEXITED(status) || WEXITSTATUS(status) != 0)) {
        result = WIFSIGNALED(status) && (WTERMSIG(status) == SIGXCPU || WTERMSIG(status) == SIGXFSZ) ? 4 : 3;
    }
    return result;
}

static bool native_system_identity(char *output, size_t size) {
#if defined(__APPLE__) && defined(__aarch64__)
    task_dyld_info_data_t info;
    mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_DYLD_INFO, (task_info_t)&info, &count) != KERN_SUCCESS || count != TASK_DYLD_INFO_COUNT) return false;
    const struct dyld_all_image_infos *images = (const void *)(uintptr_t)info.all_image_info_addr;
    if (!images || images->version < 13 || !images->sharedCacheBaseAddress) return false;
    char uuid[33];
    for (unsigned i = 0; i < 16; ++i) (void)snprintf(uuid + 2*i, 3, "%02x", images->sharedCacheUUID[i]);
    struct utsname host;
    if (uname(&host) != 0) return false;
    int n = snprintf(output, size, "%s|%s|%s|%s", uuid, host.machine, host.release, host.version);
    return n > 0 && (size_t)n < size;
#else
    (void)output; (void)size;
    return false;
#endif
}
static void native_reset(void) {
    if (native_region_live) {
        slim_region_destroy(&native_region);
        native_region_live = false;
    }
    /* Captured tools belong to the loaded connection, just like its compiler.
       Reset reclaims query history; it cannot silently replace backend inputs. */
    if (native_context_state == 2 && !*native_directory) native_context_state = 0;
}
static bool native_cleanup_work(void) {
    if (!*native_directory) return true;
    const char *names[] = {"program.c", "program.o", "runtime.o", "program", "link.bin"};
    bool okay = true;
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); ++i) {
        char path[4096];
        if (!native_path(path, names[i]) || (unlink(path) != 0 && errno != ENOENT)) okay = false;
    }
    return okay;
}
static bool native_cleanup(void) {
    bool okay = true;
    native_reset();
    free(native_inputs.owned);
    native_inputs = (NativeBytes){{NULL, 0}, NULL};
    if (*native_directory) {
        char *args[] = {"/bin/rm", "-rf", native_directory, NULL};
        uint64_t elapsed = 0;
        okay = native_process(args, false, false, NULL, &elapsed) == 0;
        if (okay) *native_directory = 0;
    }
    native_context_state = 0;
    *native_context = 0;
    return okay && !native_cleanup_failure;
}
static int native_setup(int64_t epoch, SlimRegion *root, NativeWork *work) {
    if (native_context_state == 1) {
        if (!native_region_live) {
            slim_region_init(&native_region, root);
            native_region_live = true;
            native_cache = slim_fn_pnativecache_95_95start(epoch, host_bytes(native_context), 256, NATIVE_BYTE_LIMIT, &native_region);
        }
        return 0;
    }
    if (native_context_state == 2) return 2;
    native_context_state = 2;
    char system[512];
    if (!native_system_identity(system, sizeof(system))) return 2;
    strcpy(native_directory, "/tmp/slim-native.XXXXXX");
    if (!mkdtemp(native_directory)) { *native_directory = 0; return 2; }
    char canonical[4096];
    if (!realpath(native_directory, canonical) || strlen(canonical) >= sizeof(native_directory)) return 2;
    strcpy(native_directory, canonical);
    if (!native_write_file("slim_rt.c", slim_bytes_static(native_runtime_c, sizeof(native_runtime_c))) ||
        !native_write_file("slim_rt.h", slim_bytes_static(native_runtime_h, sizeof(native_runtime_h))) ||
        !native_write_file("capture.sh", slim_bytes_static(native_capture_recipe, sizeof(native_capture_recipe)))) return 2;
    char script[4096], copier[4096];
    if (!native_path(script, "capture.sh") || !native_path(copier, "copy-inputs") ||
        !native_write_file("copy-inputs", slim_bytes_static(native_copy_program, sizeof(native_copy_program))) || chmod(copier, 0500) != 0) return 2;
    char *args[] = {"/bin/sh", script, native_directory, NULL};
    int setup_status = native_process(args, true, true, NULL, &work->setup);
    if (setup_status != 0) return setup_status == 4 ? 4 : 2;
    NativeBytes digest = native_read_file("context.sha256", 65);
    bool valid = digest.bytes.len == 65;
    if (valid) {
        for (unsigned i = 0; i < 64; ++i) {
            uint8_t b = digest.bytes.data[i];
            if (!((b >= '0' && b <= '9') || (b >= 'a' && b <= 'f'))) valid = false;
        }
        if (digest.bytes.data[64] != '\n') valid = false;
    }
    if (valid) {
        int length = snprintf(native_context, sizeof(native_context), "apple-clang-arm64-v1|%s|%.*s|%s",
            SLIM_SESSION_COMPILER_ID, 64, (const char *)digest.bytes.data, system);
        valid = length > 0 && length < (int)sizeof(native_context);
    }
    free(digest.owned);
    if (!valid) return 2;
    native_inputs = native_read_file("captured-sha256.tsv", NATIVE_MANIFEST_LIMIT);
    if (!native_manifest_valid(native_inputs.bytes)) { *native_context = 0; return 2; }
    slim_region_init(&native_region, root);
    native_region_live = true;
    native_cache = slim_fn_pnativecache_95_95start(epoch, host_bytes(native_context), 256, NATIVE_BYTE_LIMIT, &native_region);
    if (slim_region_failed(root)) return 2;
    native_context_state = 1;
    return 0;
}

static int native_compile(unsigned role, int64_t workers, NativeWork *work) {
    char compiler[4096], resource[4096], sdk[4096], include[4096], headers[4096];
    if (!native_path(compiler, "toolchain/bin/clang") || !native_path(resource, "toolchain/lib/clang/21") ||
        !native_path(sdk, "sdk") || !native_path(include, "toolchain/lib/clang/21/include") || !native_path(headers, "sdk/usr/include")) return 3;
    char *args[32]; unsigned n = 0;
    args[n++] = compiler;
    args[n++] = "--no-default-config";
    args[n++] = "-std=c11"; args[n++] = "-O3"; args[n++] = "-DNDEBUG";
    args[n++] = "-Wall"; args[n++] = "-Wextra"; args[n++] = "-Werror";
    if (workers) args[n++] = "-DSLIM_PARALLEL=1";
    if (workers == 2) { args[n++] = "-DSLIM_POSIX_WORKERS=1"; args[n++] = "-pthread"; }
    args[n++] = "-resource-dir"; args[n++] = resource;
    args[n++] = "-isysroot"; args[n++] = sdk; args[n++] = "-nostdinc";
    args[n++] = "-isystem"; args[n++] = include; args[n++] = "-isystem"; args[n++] = headers;
    args[n++] = "-I."; args[n++] = "-c"; args[n++] = role == 0 ? "program.c" : "slim_rt.c";
    args[n++] = "-o"; args[n++] = role == 0 ? "program.o" : "runtime.o"; args[n] = NULL;
    return native_process(args, false, true, &work->starts[role], &work->elapsed[role]);
}
static int native_link(NativeWork *work) {
    char compiler[4096], sdk[4096], system[4096], archive[4096];
    if (!native_path(compiler, "toolchain/bin/clang") || !native_path(sdk, "sdk") ||
        !native_path(system, "sdk/usr/lib/libSystem.tbd") || !native_path(archive, "toolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a")) return 3;
    char *args[] = {compiler, "--no-default-config", "-O3", "-isysroot", sdk, "-nostdlib", "program.o", "runtime.o", system, archive,
        "-Wl,-dependency_info,link.bin", "-o", "program", NULL};
    return native_process(args, false, true, &work->starts[2], &work->elapsed[2]);
}

static bool native_link_report_known(SlimBytes report) {
    if (report.len <= 0 || report.len > 1048576) return false;
    size_t at = 0, count = 0, versions = 0, outputs = 0;
    unsigned required = 0;
    bool okay = true;
    while (at < (size_t)report.len && okay) {
        uint8_t tag = report.data[at++];
        size_t begin = at;
        while (at < (size_t)report.len && report.data[at]) ++at;
        size_t size = at - begin;
        if (at == (size_t)report.len || !size || size > 4096 || ++count > 4096) { okay = false; break; }
        const char *path = (const char *)report.data + begin;
        ++at;
        if (tag == 0) { if (++versions != 1 || count != 1) okay = false; }
        else if (tag == 64) {
            ++outputs;
            size_t prefix = strlen(native_directory);
            if (strcmp(path, "program") != 0 && !(size == prefix + 8 &&
                memcmp(path, native_directory, prefix) == 0 && strcmp(path+prefix, "/program") == 0)) okay = false;
        }
        else if (tag == 16) {
            size_t prefix = strlen(native_directory);
            if (strcmp(path, "program.o") == 0) { required |= 1; continue; }
            if (strcmp(path, "runtime.o") == 0) { required |= 2; continue; }
            if (size <= prefix || memcmp(path, native_directory, prefix) != 0 || path[prefix] != '/') okay = false;
            else {
                const char *relative = path + prefix + 1;
                if (strcmp(relative, "program.o") == 0) required |= 1;
                if (strcmp(relative, "runtime.o") == 0) required |= 2;
                if (strcmp(relative, "sdk/usr/lib/libSystem.tbd") == 0) required |= 4;
                if (strcmp(relative, "toolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a") == 0) required |= 8;
                if (strcmp(relative, "program.o") != 0 && strcmp(relative, "runtime.o") != 0 &&
                    !native_manifest_contains(relative, size - prefix - 1)) okay = false;
            }
        } else if (tag != 17) okay = false;
    }
    return okay && versions == 1 && outputs == 1 && required == 15;
}
static bool native_known_link_inputs(void) {
    NativeBytes report = native_read_file("link.bin", 1048576);
    bool okay = native_link_report_known(report.bytes);
    free(report.owned);
    return okay;
}
static NativeBytes native_lookup(Slim_type_pnativecache_95_95Key key, unsigned role, NativeWork *work) {
    Slim_type_pnativecache_95_95Probe result = slim_fn_pnativecache_95_95lookup(&native_cache, key, &native_region);
    work->hits[role] = result.slim_field_hit;
    SlimBytes reason = result.slim_field_reason;
    if (!result.slim_field_hit && !(reason.len == 7 && memcmp(reason.data,"missing",7) == 0)) work->retention[role] = reason;
    return (NativeBytes){result.slim_field_artifact, NULL};
}
static void native_publish(Slim_type_pnativecache_95_95Key key, unsigned role, NativeWork *work, NativeBytes output) {
    Slim_type_pnativecache_95_95Stored result = slim_fn_pnativecache_95_95publish(&native_cache, key, output.bytes, &native_region);
    if (!result.slim_field_accepted && work->retention[role].len == 0) work->retention[role] = result.slim_field_reason;
}
static int native_build(Slim_type_pnativebuild_95_95Selection selected, int64_t epoch,
                        NativeWork *work, NativeBytes *executable) {
    SlimBytes empty = slim_bytes_static(NULL, 0), context = host_bytes(native_context);
    Slim_type_pnativecache_95_95Key program_key = {epoch, context, 0, selected.slim_field_workers, selected.slim_field_code, empty};
    Slim_type_pnativecache_95_95Key runtime_key = {epoch, context, 1, selected.slim_field_workers,
        slim_bytes_static(native_runtime_c, sizeof(native_runtime_c)), slim_bytes_static(native_runtime_h, sizeof(native_runtime_h))};
    NativeBytes objects[2] = {{{NULL, 0}, NULL}, {{NULL, 0}, NULL}};
    Slim_type_pnativecache_95_95Key keys[2] = {program_key, runtime_key};
    int status = 0;
    for (unsigned role = 0; role < 2 && !status; ++role) {
        objects[role] = native_lookup(keys[role], role, work);
        if (slim_region_failed(&native_region)) { status = 3; break; }
        if (!work->hits[role]) {
            if (role == 0 && !native_write_file("program.c", selected.slim_field_code)) { status = 3; break; }
            status = native_compile(role, selected.slim_field_workers, work);
            if (!status) {
                objects[role] = native_read_file(role == 0 ? "program.o" : "runtime.o", NATIVE_BYTE_LIMIT);
                if (!objects[role].bytes.len) status = 3;
                else native_publish(keys[role], role, work, objects[role]);
            }
        }
    }
    if (!status && !slim_region_failed(&native_region)) {
        Slim_type_pnativecache_95_95Key link_key = {epoch, context, 2, selected.slim_field_workers, objects[0].bytes, objects[1].bytes};
        *executable = native_lookup(link_key, 2, work);
        if (!work->hits[2] && !slim_region_failed(&native_region)) {
            if (!native_write_file("program.o", objects[0].bytes) || !native_write_file("runtime.o", objects[1].bytes)) status = 3;
            else status = native_link(work);
            if (!status && !native_known_link_inputs()) status = 3;
            if (!status) {
                *executable = native_read_file("program", NATIVE_BYTE_LIMIT);
                if (!executable->bytes.len) status = 3;
                else native_publish(link_key, 2, work, *executable);
            }
        }
    }
    free(objects[0].owned); free(objects[1].owned);
    return status;
}

static int native_response(int64_t epoch, int64_t serial, int status, int64_t workers,
                           uint64_t elapsed, NativeWork work, const char *reason, SlimBytes executable) {
    size_t context_size = strlen(native_context), reason_size = strlen(reason);
    if (context_size > 1024 || reason_size > 4096 || executable.len < 0 || executable.len > NATIVE_BYTE_LIMIT ||
        (status == 0 && executable.len == 0) || (status != 0 && executable.len != 0)) return host_error("H0005", 65);
    unsigned char prefix[116];
    prefix[0] = 1;
    int64_t words[5] = {epoch, serial, status, workers, (int64_t)elapsed};
    for (unsigned i = 0; i < 5; ++i) host_u64(prefix + 1 + 8*i, (uint64_t)words[i]);
    for (unsigned i = 0; i < 3; ++i) host_u64(prefix + 41 + 8*i, work.starts[i]);
    for (unsigned i = 0; i < 3; ++i) prefix[65+i] = work.hits[i] ? 1 : 0;
    host_u64(prefix + 68, work.setup);
    for (unsigned i = 0; i < 3; ++i) host_u64(prefix + 76 + 8*i, work.elapsed[i]);
    host_u32(prefix + 100, (uint32_t)context_size);
    host_u32(prefix + 104, (uint32_t)reason_size);
    host_u32(prefix + 108, (uint32_t)host_diagnostic_length);
    host_u32(prefix + 112, (uint32_t)executable.len);
    uint32_t length = (uint32_t)(116 + context_size + reason_size + host_diagnostic_length + (size_t)executable.len);
    if (!host_header('N', length) || !host_write(prefix, sizeof(prefix)) ||
        !host_write(native_context, context_size) || !host_write(reason, reason_size) ||
        !host_write(host_diagnostics, host_diagnostic_length) ||
        !host_write(executable.data, (size_t)executable.len) || fflush(stdout) != 0) return 65;
    return 0;
}
static int64_t native_request_word(const unsigned char *data) {
    uint64_t bits = 0;
    for (unsigned i = 0; i < 8; ++i) bits = (bits << 8) | data[i];
    int64_t value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}
static int native_request(Slim_type_psession_95_95State state, const unsigned char payload[17], SlimRegion *root) {
    uint64_t begin = native_now();
    int64_t epoch = native_request_word(payload), serial = native_request_word(payload+8);
    NativeWork work = {0};
    host_diagnostic_length = 0;
    host_capture_failure = 0;
    Slim_type_pnativebuild_95_95Selection selected = slim_fn_pnativebuild_95_95select(state, epoch, serial, payload[16], root);
    NativeBytes executable = {{NULL, 0}, NULL};
    const char *reason = "";
    char selection_reason[128];
    int status = 0;
    if (!selected.slim_field_ready) {
        status = 1;
        if (selected.slim_field_reason.len < 0 || selected.slim_field_reason.len >= (int64_t)sizeof(selection_reason)) return host_error("H0005", 65);
        memcpy(selection_reason, selected.slim_field_reason.data, (size_t)selected.slim_field_reason.len);
        selection_reason[selected.slim_field_reason.len] = 0;
        reason = selection_reason;
    } else {
        status = native_setup(epoch, root, &work);
        if (status) {
            reason = status == 4 ? "native-resource-limit" : "native-context-unavailable";
            if (!native_cleanup()) return host_error("H0007", 65);
            native_context_state = 2;
        } else {
            status = native_build(selected, epoch, &work, &executable);
            if (status) reason = status == 4 ? "native-resource-limit" : "native-build-failed";
        }
    }
    if (selected.slim_field_ready && !native_cleanup_work()) {
        status = 3;
        reason = "native-work-cleanup-failed";
    }
    if (native_cleanup_failure) { free(executable.owned); return host_error("H0007", 65); }
    if (slim_region_failed(root) || native_allocation_failure || host_capture_failure == 71) {
        free(executable.owned);
        if (slim_region_failed(root)) slim_alloc_report(root->status);
        return host_error("H0006", 71);
    }
    if (!status && !executable.bytes.len) { status = 3; reason = "missing-native-artifact"; }
    char retention_reason[512];
    size_t retained_length = 0;
    if (!status) {
        const char *roles[] = {"program", "runtime", "link"};
        for (unsigned i=0;i<3;++i) {
            SlimBytes value=work.retention[i];
            if (value.len < 0 || value.len > 128) { status=3; reason="inconsistent-native-query"; break; }
            if (value.len) {
                int n=snprintf(retention_reason+retained_length,sizeof(retention_reason)-retained_length,
                    "%scache-%s:%.*s",retained_length?";":"",roles[i],(int)value.len,(const char *)value.data);
                if(n<0 || (size_t)n>=sizeof(retention_reason)-retained_length) { status=3; reason="inconsistent-native-query"; break; }
                retained_length+=(size_t)n;
            }
        }
        if (!status && retained_length) reason=retention_reason;
    }
    SlimBytes output = status ? slim_bytes_static(NULL, 0) : executable.bytes;
    uint64_t end = native_now();
    int result = native_response(epoch, serial, status, selected.slim_field_workers,
        end >= begin ? end - begin : 0, work, reason, output);
    free(executable.owned);
    return result;
}
