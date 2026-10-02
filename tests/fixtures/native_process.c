/* Appended to exact extracted production host process/capture functions. */
#include <assert.h>
static int process_case(const char *helper, const char *mode, bool diagnostics, bool setup) {
    char *arguments[] = {(char *)helper, (char *)mode, NULL};
    uint64_t starts = 0, elapsed = 0;
    int result = native_process(arguments, setup, diagnostics, &starts, &elapsed);
    assert(starts == 1 && elapsed > 0);
    printf("native-process\t%s\t%d\t%zu\t%d\n", mode, result, host_diagnostic_length, host_capture_failure);
    return result;
}
int main(int argc, char **argv) {
    assert(argc == 3 && strlen(argv[2]) < sizeof(native_directory));
    strcpy(native_directory, argv[2]);
    assert(setenv("SLIM_NATIVE_TEST_SECRET", "must-not-reach-child", 1) == 0);
    assert(process_case(argv[1], "okay", true, false) == 0);
    /* Real parent group creation followed by child EPERM must still execute.
       Wrong group and non-EPERM failures must reject before helper diagnostics. */
    const int group_expected[] = {0, 3, 3};
    for (unsigned mode = 1; mode <= 3; ++mode) {
        host_clear_capture();
        test_group_mode = mode;
        assert(pipe(test_group_gate) == 0);
        assert(process_case(argv[1], "diagnostics", true, false) == group_expected[mode-1]);
        assert(!native_cleanup_failure);
        if (mode == 1) {
            assert(host_diagnostic_length == 6 && memcmp(host_diagnostics, "abcdef", 6) == 0);
        } else {
            assert(host_diagnostic_length == 0);
        }
        assert(test_group_gate[0] == -1 && test_group_gate[1] == -1);
        test_group_mode = 0;
    }
    host_clear_capture();
    assert(process_case(argv[1], "failure", true, false) == 3);
    assert(process_case(argv[1], "signal", true, false) == 3);
    assert(process_case(argv[1], "cpu-signal", true, false) == 4);
    assert(process_case(argv[1], "diagnostics", true, false) == 0);
    assert(host_diagnostic_length == 6 && memcmp(host_diagnostics, "abcdef", 6) == 0);
    host_clear_capture();
    assert(process_case(argv[1], "diag-boundary", true, false) == 0);
    assert(host_diagnostic_length == HOST_DIAGNOSTIC_LIMIT && host_capture_failure == 0);
    for (size_t i = 0; i < host_diagnostic_length; ++i) assert(host_diagnostics[i] == 'x');
    host_clear_capture();
    assert(process_case(argv[1], "diag-over", true, false) == 4);
    assert(host_diagnostic_length <= HOST_DIAGNOSTIC_LIMIT && host_capture_failure == 65);
    /* Cleanup must run even when diagnostic collection has already failed. */
    assert(process_case(argv[1], "diagnostics", false, false) == 0);
    host_clear_capture();
    host_fail_at = host_allocations + 1;
    assert(process_case(argv[1], "diagnostics", true, false) == 4 && host_capture_failure == 71);
    assert(process_case(argv[1], "diagnostics", false, false) == 0);
    host_fail_at = 0; host_clear_capture();
    assert(process_case(argv[1], "file-boundary", true, false) == 0);
    assert(process_case(argv[1], "file-over", true, false) == 4);
    assert(process_case(argv[1], "setup-limit", true, true) == 0);
    uint64_t begin = native_real_now();
    assert(process_case(argv[1], "closed", true, false) == 0);
    assert(native_real_now() - begin >= 40000000);
    const char *timeouts[] = {"hang-open", "hang-closed", "flood"};
    for (unsigned i = 0; i < 3; ++i) {
        test_clock_origin = 0; test_clock_advance = true;
        assert(process_case(argv[1], timeouts[i], false, false) == 4);
        test_clock_advance = false;
    }
    const char *descendants[] = {"descendant-open", "descendant-closed"};
    for (unsigned i = 0; i < 2; ++i) {
        host_clear_capture();
        begin = native_real_now();
        assert(process_case(argv[1], descendants[i], true, false) == 0);
        assert(native_real_now() - begin < 1000000000);
        assert(host_diagnostic_length < 100);
        char pids[101]; memcpy(pids, host_diagnostics, host_diagnostic_length); pids[host_diagnostic_length] = 0;
        long leader, descendant; assert(sscanf(pids, "%ld %ld", &leader, &descendant) == 2);
        assert(leader > 1 && descendant > 1);
        int status; errno = 0;
        assert(waitpid((pid_t)leader, &status, WNOHANG) == -1 && errno == ECHILD);
        /* The OS reaps an orphan killed by its group; it must disappear well
           before the child's three-second emergency alarm. */
        begin = native_real_now();
        while (kill((pid_t)descendant, 0) == 0 && native_real_now() - begin < 1000000000)
            (void)poll(NULL, 0, 10);
        errno = 0; assert(kill((pid_t)descendant, 0) == -1 && errno == ESRCH);
    }
    host_clear_capture();
    assert(process_case("/nonexistent/slim-native-test", "okay", true, false) == 3);
    strcat(native_directory, "/absent");
    assert(process_case(argv[1], "okay", true, false) == 3);
    strcpy(native_directory, argv[2]);
    struct rlimit before, limited;
    assert(getrlimit(RLIMIT_NOFILE, &before) == 0);
    limited = before; limited.rlim_cur = 3;
    assert(setrlimit(RLIMIT_NOFILE, &limited) == 0);
    char *arguments[] = {(char *)argv[1], "okay", NULL};
    uint64_t starts = 0, elapsed = 0;
    int status = native_process(arguments, false, true, &starts, &elapsed);
    assert(setrlimit(RLIMIT_NOFILE, &before) == 0);
    assert(status == 3 && starts == 0);
    assert(!native_cleanup_failure);
    const int expected[] = {0, -1, -1, -1, -1, 0, 1, 0};
    for (unsigned mode = 1; mode <= 8; ++mode) {
        test_probe_mode = mode; test_probe_infos = 0;
        assert(native_group_live(12345) == expected[mode-1]);
        if (mode == 1) assert(test_probe_infos == NATIVE_PROCESS_LIMIT);
        if (mode >= 2 && mode <= 4) assert(test_probe_infos == 0);
    }
    test_probe_mode = 5;
    assert(process_case(argv[1], "okay", true, false) == 3 && native_cleanup_failure);
    test_probe_mode = 0;
    errno = 0; assert(waitpid(-1, NULL, WNOHANG) == -1 && errno == ECHILD);
    host_clear_capture();
    puts("native-process: lifetime, environment, actual limits, 512/513 process-query bounds and accelerated-clock deadlines exact; no direct children remain");
    return 0;
}
