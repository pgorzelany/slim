/* Appended to the test-only host translation unit. No source semantics. */
int main(void) {
    static const unsigned char block[HOST_DIAGNOSTIC_LIMIT] = {0};
    for (uint64_t fault = 0; fault <= 14; ++fault) {
        host_clear_capture();
        host_allocations = 0;
        host_fail_at = fault;
        for (unsigned i = 0; i < HOST_DIAGNOSTIC_LIMIT; ++i) {
            host_capture(block, 1);
            if (host_capture_failure) break;
        }
        if (fault >= 1 && fault <= 13) {
            if (host_capture_failure != 71 || host_allocations != fault) abort();
            size_t previous = host_diagnostic_length;
            host_capture(block, HOST_DIAGNOSTIC_LIMIT);
            if (host_capture_failure != 71 || host_diagnostic_length != previous) abort();
        } else {
            if (host_capture_failure || host_diagnostic_length != HOST_DIAGNOSTIC_LIMIT ||
                host_diagnostic_capacity != HOST_DIAGNOSTIC_LIMIT || host_allocations != 13) abort();
            host_capture(block, 0);
            if (host_capture_failure) abort();
            host_capture(block, 1);
            if (host_capture_failure != 65 || host_diagnostic_length != HOST_DIAGNOSTIC_LIMIT) abort();
        }
    }
    host_clear_capture();
    host_allocations = host_fail_at = 0;
    host_capture(block, HOST_DIAGNOSTIC_LIMIT + 1u);
    if (host_capture_failure != 65 || host_diagnostics != NULL || host_allocations != 0) abort();
    host_clear_capture();
    slim_print_i64(INT64_MIN);
    slim_println(slim_bytes_static((const uint8_t *)"\0x", 2));
    const char expected[] = "-9223372036854775808\0x\n";
    if (host_capture_failure || host_diagnostic_length != sizeof(expected) - 1 ||
        memcmp(host_diagnostics, expected, sizeof(expected) - 1)) abort();
    host_clear_capture();
    SlimBytes invalid = {.data = NULL, .len = -1};
    slim_print_bytes(invalid);
    if (host_capture_failure != 65) abort();
    host_clear_capture();
    if (host_diagnostics != NULL || host_diagnostic_capacity || host_diagnostic_length) abort();
    return 0;
}
