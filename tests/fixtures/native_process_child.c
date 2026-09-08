#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static void output(size_t count) {
    char bytes[8192]; memset(bytes, 'x', sizeof(bytes));
    while (count) {
        size_t size = count < sizeof(bytes) ? count : sizeof(bytes);
        ssize_t written = write(STDOUT_FILENO, bytes, size);
        if (written < 0 && errno == EINTR) continue;
        if (written <= 0) _exit(70);
        count -= (size_t)written;
    }
}
int main(int argc, char **argv) {
    assert(argc == 2);
    struct rlimit cpu, file;
    assert(getrlimit(RLIMIT_CPU, &cpu) == 0 && cpu.rlim_cur == 180 && cpu.rlim_max == 180);
    assert(getrlimit(RLIMIT_FSIZE, &file) == 0);
    assert(file.rlim_cur == file.rlim_max);
    assert(file.rlim_cur == (strcmp(argv[1], "setup-limit") == 0 ? 536870912 : 67108864));
    assert(strcmp(getenv("LC_ALL"), "C") == 0 && strcmp(getenv("PATH"), "/usr/bin:/bin") == 0);
    assert(getenv("SLIM_NATIVE_TEST_SECRET") == NULL);
    char cwd[4096], expected[4100]; assert(getcwd(cwd, sizeof(cwd)));
    snprintf(expected, sizeof(expected), "%s/tmp", cwd);
    assert(strcmp(getenv("TMPDIR"), expected) == 0);
    char byte; assert(read(STDIN_FILENO, &byte, 1) == 0);
    if (strcmp(argv[1], "okay") == 0) return 0;
    if (strcmp(argv[1], "failure") == 0) return 7;
    if (strcmp(argv[1], "signal") == 0) { raise(SIGTERM); return 70; }
    if (strcmp(argv[1], "cpu-signal") == 0) { raise(SIGXCPU); return 70; }
    if (strcmp(argv[1], "diagnostics") == 0) {
        assert(write(1, "abc", 3) == 3 && write(2, "def", 3) == 3); return 0;
    }
    if (strcmp(argv[1], "diag-boundary") == 0) { output(1048576); return 0; }
    if (strcmp(argv[1], "diag-over") == 0) { output(1048577); return 0; }
    if (strcmp(argv[1], "file-boundary") == 0 || strcmp(argv[1], "file-over") == 0 || strcmp(argv[1], "setup-limit") == 0) {
        int fd = open("limit.bin", O_WRONLY | O_CREAT | O_TRUNC, 0600); assert(fd >= 0);
        assert(ftruncate(fd, (off_t)file.rlim_cur) == 0);
        if (strcmp(argv[1], "file-over") == 0) {
            assert(pwrite(fd, "x", 1, (off_t)file.rlim_cur) == 1); return 70;
        }
        assert(close(fd) == 0); return 0;
    }
    if (strcmp(argv[1], "closed") == 0) {
        close(1); close(2); struct timespec pause = {0, 50000000};
        assert(nanosleep(&pause, NULL) == 0); return 0;
    }
    if (strcmp(argv[1], "hang-open") == 0 || strcmp(argv[1], "hang-closed") == 0) {
        if (strcmp(argv[1], "hang-closed") == 0) { close(1); close(2); }
        alarm(3); for (;;) pause();
    }
    if (strcmp(argv[1], "flood") == 0) { alarm(3); for (;;) output(8192); }
    if (strcmp(argv[1], "descendant-open") == 0 || strcmp(argv[1], "descendant-closed") == 0) {
        int ready[2]; assert(pipe(ready) == 0);
        pid_t descendant = fork(); assert(descendant >= 0);
        if (descendant == 0) {
            close(ready[0]);
            if (strcmp(argv[1], "descendant-closed") == 0) { close(1); close(2); }
            alarm(3); assert(write(ready[1], "r", 1) == 1); close(ready[1]);
            for (;;) pause();
        }
        close(ready[1]); assert(read(ready[0], &byte, 1) == 1); close(ready[0]);
        assert(printf("%ld %ld\n", (long)getpid(), (long)descendant) > 0);
        return 0;
    }
    return 64;
}
