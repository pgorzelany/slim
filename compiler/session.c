#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE 1
/* Compiler transport only. Source acceptance and retained state stay in SLIM. */
#include "session-identity.h"
#define main slim_seed_main
#include "slimc-seed.c"
#undef main
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define HOST_PATH_LIMIT 4096u
#define HOST_DIAGNOSTIC_LIMIT 1048576u
#define HOST_CODE_LIMIT 67108864u

static unsigned char *host_diagnostics;
static size_t host_diagnostic_length, host_diagnostic_capacity;
static int host_capture_failure;
static uint64_t host_allocations, host_fail_at;

static void host_capture(const void *data, size_t size) {
    if (host_capture_failure || size == 0) return;
    if (size > HOST_DIAGNOSTIC_LIMIT - host_diagnostic_length) {
        host_capture_failure = 65;
        return;
    }
    size_t needed = host_diagnostic_length + size;
    if (needed > host_diagnostic_capacity) {
        size_t capacity = host_diagnostic_capacity ? host_diagnostic_capacity : 256;
        while (capacity < needed) capacity *= 2;
        if (host_allocations == UINT64_MAX || ++host_allocations == host_fail_at) {
            host_capture_failure = 71;
            return;
        }
        void *next = realloc(host_diagnostics, capacity);
        if (!next) {
            host_capture_failure = 71;
            return;
        }
        host_diagnostics = next;
        host_diagnostic_capacity = capacity;
    }
    memcpy(host_diagnostics + host_diagnostic_length, data, size);
    host_diagnostic_length = needed;
}

SlimUnit slim_print_bytes(SlimBytes value) {
    if (value.len < 0) host_capture_failure = 65;
    else host_capture(value.data, (size_t)value.len);
    return (SlimUnit){0};
}

SlimUnit slim_print_i64(int64_t value) {
    char text[32];
    int size = snprintf(text, sizeof(text), "%" PRId64, value);
    if (size < 0 || (size_t)size >= sizeof(text)) host_capture_failure = 65;
    else host_capture(text, (size_t)size);
    return (SlimUnit){0};
}

SlimUnit slim_println(SlimBytes value) {
    slim_print_bytes(value);
    host_capture("\n", 1);
    return (SlimUnit){0};
}

static void host_clear_capture(void) {
    free(host_diagnostics);
    host_diagnostics = NULL;
    host_diagnostic_length = host_diagnostic_capacity = 0;
    host_capture_failure = 0;
}

static void host_u32(unsigned char *out, uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) out[i] = (unsigned char)(value >> (24 - 8 * i));
}

static void host_u64(unsigned char *out, uint64_t value) {
    for (unsigned i = 0; i < 8; ++i) out[i] = (unsigned char)(value >> (56 - 8 * i));
}

static bool host_write(const void *data, size_t size) {
    return size == 0 || fwrite(data, 1, size, stdout) == size;
}

static bool host_header(unsigned char tag, uint32_t size) {
    unsigned char header[5];
    header[0] = tag;
    host_u32(header + 1, size);
    return host_write(header, sizeof(header));
}

static bool host_frame(unsigned char tag, const void *data, uint32_t size) {
    return host_header(tag, size) && host_write(data, size) && fflush(stdout) == 0;
}

static int host_error(const char *reason, int status) {
    (void)host_frame('E', reason, (uint32_t)strlen(reason));
    return status;
}

static bool host_read(void *out, size_t size) {
    return size == 0 || fread(out, 1, size, stdin) == size;
}

static SlimBytes host_bytes(const char *text) {
    return slim_bytes_static((const uint8_t *)text, (int64_t)strlen(text));
}

static int host_response(Slim_type_session_95Report report, SlimBytes code) {
    SlimBytes reason = report.slim_field_reason;
    if (reason.len < 0 || reason.len > 4096 || code.len < 0 || code.len > HOST_CODE_LIMIT)
        return host_error("H0005", 65);
    if ((report.slim_field_status == 0 && code.len == 0) ||
        (report.slim_field_status != 0 && code.len != 0))
        return host_error("H0005", 65);
    unsigned char prefix[87];
    int64_t words[8] = {
        report.slim_field_attempted.slim_field_epoch,
        report.slim_field_attempted.slim_field_serial,
        report.slim_field_published.slim_field_epoch,
        report.slim_field_published.slim_field_serial,
        report.slim_field_status,
        report.slim_field_work.slim_field_executed,
        report.slim_field_work.slim_field_reused,
        report.slim_field_work.slim_field_imported
    };
    for (unsigned i = 0; i < 8; ++i) host_u64(prefix + 8 * i, (uint64_t)words[i]);
    prefix[64] = report.slim_field_work.slim_field_capacity ? 1 : 0;
    prefix[65] = report.slim_field_snapshot_95reused ? 1 : 0;
    host_u64(prefix + 66, (uint64_t)report.slim_field_generated);
    prefix[74] = report.slim_field_code_95reused ? 1 : 0;
    host_u32(prefix + 75, (uint32_t)reason.len);
    host_u32(prefix + 79, (uint32_t)host_diagnostic_length);
    host_u32(prefix + 83, (uint32_t)code.len);
    uint32_t size = (uint32_t)(sizeof(prefix) + (size_t)reason.len + host_diagnostic_length + (size_t)code.len);
    if (!host_header('S', size) || !host_write(prefix, sizeof(prefix)) ||
        !host_write(reason.data, (size_t)reason.len) ||
        !host_write(host_diagnostics, host_diagnostic_length) ||
        !host_write(code.data, (size_t)code.len) || fflush(stdout) != 0)
        /* Never append an error frame inside a partially written S frame. */
        return 65;
    return 0;
}

#include "native.c"

int main(int argc, char **argv) {
    (void)argv;
    if (argc != 1) {
        fputs("usage: slimc session (framed protocol on stdin/stdout)\n", stderr);
        return 64;
    }
#ifdef SIGPIPE
    (void)signal(SIGPIPE, SIG_IGN);
#endif
    const char *failure = getenv("SLIM_HOST_ALLOC_FAIL_AT");
    if (failure && *failure) {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(failure, &end, 10);
        if (*failure >= '1' && *failure <= '9' && !errno && end != failure && !*end && value > 0)
            host_fail_at = (uint64_t)value;
    }
    const char *native_failure = getenv("SLIM_NATIVE_ALLOC_FAIL_AT");
    if (native_failure && *native_failure >= '1' && *native_failure <= '9') {
        char *end = NULL;
        errno = 0;
        unsigned long long value = strtoull(native_failure, &end, 10);
        if (!errno && end != native_failure && !*end && value > 0) native_fail_at = (uint64_t)value;
    }
    const char greeting[] = "slim-session\t1\ncompiler\t" SLIM_SESSION_COMPILER_ID
        "\nruntime\t" SLIM_SESSION_RUNTIME_ID "\ntarget\t" SLIM_SESSION_TARGET
        "\noptions\t" SLIM_SESSION_OPTIONS_ID "\n";
    if (!host_frame('H', greeting, (uint32_t)(sizeof(greeting) - 1))) return 65;
    SlimAllocStatus allocation;
    SlimRegion root;
    slim_alloc_status_init(&allocation);
    slim_rt_init(&root, &allocation);
    const Slim_type_session_95Limits limits = {64, 67108864, 1000000, 67108864};
    const Slim_type_session_95Config config = {
        host_bytes(SLIM_SESSION_COMPILER_ID), host_bytes(SLIM_SESSION_RUNTIME_ID),
        host_bytes(SLIM_SESSION_TARGET), host_bytes(SLIM_SESSION_OPTIONS_ID)
    };
    int64_t epoch = 1;
    Slim_type_session_95State state = slim_fn_session_95start(epoch, limits, &root);
    int result = 0;
    for (;;) {
        if (slim_region_failed(&root)) {
            slim_alloc_report(&allocation);
            result = host_error("H0006", 71);
            break;
        }
        int tag = fgetc(stdin);
        if (tag == EOF) {
            if (ferror(stdin)) result = host_error("H0001", 65);
            break;
        }
        unsigned char length[4];
        if (!host_read(length, sizeof(length))) { result = host_error("H0001", 65); break; }
        uint32_t size = ((uint32_t)length[0] << 24) | ((uint32_t)length[1] << 16) |
            ((uint32_t)length[2] << 8) | (uint32_t)length[3];
        if (tag == 'R' || tag == 'Q') {
            if (size != 0) { result = host_error("H0001", 65); break; }
            if (tag == 'Q') {
                if (!native_cleanup()) { result = host_error("H0007", 65); break; }
                if (!host_frame('Q', NULL, 0)) result = 65;
                break;
            }
            if (epoch == INT64_MAX) { result = host_error("H0003", 65); break; }
            native_reset();
            slim_rt_shutdown();
            host_clear_capture();
            slim_alloc_status_init(&allocation);
            slim_rt_init(&root, &allocation);
            state = slim_fn_session_95start(++epoch, limits, &root);
            if (slim_region_failed(&root)) continue;
            unsigned char value[8];
            host_u64(value, (uint64_t)epoch);
            if (!host_frame('A', value, sizeof(value))) { result = 65; break; }
            continue;
        }
        if (tag == 'B') {
            unsigned char payload[17];
            if (size != sizeof(payload) || !host_read(payload, sizeof(payload))) { result = host_error("H0001", 65); break; }
            result = native_request(state, payload, &root);
            if (result != 0) break;
            continue;
        }
        if (tag != 'U') { result = host_error("H0001", 65); break; }
        if (size == 0 || size > HOST_PATH_LIMIT) { result = host_error("H0002", 65); break; }
        unsigned char path[HOST_PATH_LIMIT];
        if (!host_read(path, size)) { result = host_error("H0001", 65); break; }
        if (memchr(path, 0, size)) { result = host_error("H0002", 65); break; }
        host_diagnostic_length = 0;
        host_capture_failure = 0;
        SlimBytes source = slim_bytes_static(path, (int64_t)size);
        Slim_type_session_95Report report = slim_fn_session_95update_95path(&state, source, config, &root);
        if (slim_region_failed(&root)) continue;
        if (host_capture_failure) {
            result = host_error(host_capture_failure == 71 ? "H0006" : "H0003", host_capture_failure);
            break;
        }
        SlimBytes code = slim_bytes_static(NULL, 0);
        if (report.slim_field_status == 0) {
            Slim_type_session_95Artifact artifact = slim_fn_session_95artifact(state.slim_field_good, &root);
            if (slim_region_failed(&root)) continue;
            code = artifact.slim_field_code;
        }
        result = host_response(report, code);
        if (result != 0) break;
    }
    if (!native_cleanup() && result == 0) result = host_error("H0007", 65);
    slim_rt_shutdown();
    host_clear_capture();
    return result;
}
