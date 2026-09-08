/* Verification only: included in a copied native adapter, never production. */
#include <assert.h>
static unsigned native_probe_requests;
static bool native_probe_group_failure, native_probe_allocation;
static const char *native_probe_case(void) {
    const char *value = getenv("SLIM_NATIVE_TEST_CASE");
    return value ? value : "";
}
static void native_probe_before(void) {
    ++native_probe_requests;
    if (native_probe_requests != 2) return;
    const char *name = native_probe_case();
    assert(native_region_live && native_cache.slim_field_headers.len == 1);
    Slim_type_nativecache_95Header *header = (void *)native_cache.slim_field_headers.data;
    if (strcmp(name, "metadata") == 0) header->slim_field_seal ^= 1;
    else if (strncmp(name, "artifact-", 9) == 0) {
        unsigned role = (unsigned)(name[9] - '0');
        assert(role < 3 && native_cache.slim_field_entries.len == 3);
        Slim_type_nativecache_95Entry *entries = (void *)native_cache.slim_field_entries.data;
        assert(entries[role].slim_field_key.slim_field_role == role);
        SlimBytes artifact = entries[role].slim_field_artifact;
        assert(artifact.len > 0);
        /* Production publish owns this allocation; change one real owned byte. */
        ((uint8_t *)artifact.data)[0] ^= 1;
    } else if (strcmp(name, "byte-capacity") == 0) {
        native_cache.slim_field_bytes_95limit = header->slim_field_used;
        header->slim_field_seal = slim_fn_nativecache_95metadata_95seal(native_cache, &native_region);
    } else if (strcmp(name, "cleanup") == 0) native_probe_group_failure = true;
}
static void native_probe_publish(unsigned role) {
    const char *name = native_probe_case();
    if (!native_probe_allocation && strncmp(name, "allocation-", 11) == 0 &&
        name[11] == (char)('0' + role) && name[12] == 0) {
        native_probe_allocation = true;
        uint64_t attempts = atomic_load(&native_region.status->attempts);
        assert(attempts < UINT64_MAX);
        native_region.status->fail_at = attempts + 1;
    }
}
static void native_probe_report(void) {
    const char *path = getenv("SLIM_NATIVE_STATE_REPORT");
    if (!path || !*path || !native_region_live) return;
    assert(native_cache.slim_field_headers.len == 1);
    Slim_type_nativecache_95Header *header = (void *)native_cache.slim_field_headers.data;
    char row[256];
    int length = snprintf(row, sizeof(row), "%u\t%" PRId64 "\t%" PRId64 "\t%" PRId64
        "\t%" PRId64 "\t%d\t%" PRId64 "\n", native_probe_requests,
        native_cache.slim_field_epoch, native_cache.slim_field_entries.len,
        header->slim_field_used, native_cache.slim_field_bytes_95limit,
        header->slim_field_disabled, native_cache.slim_field_records_95limit);
    assert(length > 0 && (size_t)length < sizeof(row));
    int file = open(path, O_WRONLY | O_APPEND | O_CREAT | O_NOFOLLOW, 0600);
    assert(file >= 0 && write(file, row, (size_t)length) == length);
    assert(close(file) == 0);
}
