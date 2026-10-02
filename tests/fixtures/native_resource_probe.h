/* Verification-only physical allocation/copy/process observations. */
#include <assert.h>
extern void slim_test_track_region(SlimRegion *region);
extern uint64_t slim_test_region_bytes(const SlimRegion *region);
extern uint64_t slim_test_freed_bytes(void);
static bool native_resource_declined;
static bool native_resource_decline(void) {
    const char *mode = getenv("SLIM_NATIVE_TEST_CASE");
    if (mode && strcmp(mode, "context-decline") == 0 && !native_resource_declined) {
        native_resource_declined = true;
        return true;
    }
    return false;
}
static void native_resource_report(const char *stage) {
    const char *path = getenv("SLIM_NATIVE_RESOURCE_REPORT");
    if (!path || !*path) return;
    slim_test_track_region(&native_region);
    uint64_t live = 0, used = 0, records = 0;
    if (native_region_live) {
        live = slim_test_region_bytes(&native_region);
        assert(native_cache.slim_field_headers.len == 1);
        Slim_type_pnativecache_95_95Header *header = (void *)native_cache.slim_field_headers.data;
        used = (uint64_t)header->slim_field_used;
        records = (uint64_t)native_cache.slim_field_entries.len;
    }
    struct rusage self, children;
    assert(getrusage(RUSAGE_SELF, &self) == 0 && getrusage(RUSAGE_CHILDREN, &children) == 0);
    uint64_t resident = 0;
#if defined(__APPLE__) && defined(__aarch64__)
    mach_task_basic_info_data_t information;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    assert(task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&information, &count) == KERN_SUCCESS);
    resident = information.resident_size;
#endif
    char row[512];
    int size = snprintf(row, sizeof(row), "%s\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64
        "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%ld\t%ld\t%" PRId64 "\t%zu\n",
        stage, records, used, live, slim_test_freed_bytes(), native_resource_cloned,
        resident, self.ru_maxrss, children.ru_maxrss, native_inputs.bytes.len, host_diagnostic_capacity);
    assert(size > 0 && (size_t)size < sizeof(row));
    int file = open(path, O_WRONLY | O_APPEND | O_CREAT | O_NOFOLLOW, 0600);
    assert(file >= 0 && write(file, row, (size_t)size) == size);
    assert(close(file) == 0);
}
