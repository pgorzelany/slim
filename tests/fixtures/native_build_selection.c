/* Verification-only source selection oracle; production hosts cannot edit state. */
#define main native_selection_seed_main
#include "slimc-seed.c"
#undef main
#include <stdio.h>
#include <stdlib.h>

static SlimBytes input(const char *text) {
    return slim_bytes_static((const uint8_t *)text, (int64_t)strlen(text));
}
#define require(okay) do { if (!(okay)) { fprintf(stderr, "native selection failed at line %d\n", __LINE__); abort(); } } while (0)
static void rejection(const Slim_type_psession_95_95State *state, int64_t epoch,
                      int64_t serial, int64_t workers, const char *reason,
                      SlimRegion *region) {
    uint64_t allocations = atomic_load(&region->status->attempts);
    Slim_type_pnativebuild_95_95Selection value = slim_fn_pnativebuild_95_95select(
        *state, epoch, serial, workers, region);
    require(!value.slim_field_ready && value.slim_field_code.len == 0 && value.slim_field_workers == -1);
    require(value.slim_field_reason.len == (int64_t)strlen(reason));
    require(memcmp(value.slim_field_reason.data, reason, strlen(reason)) == 0);
    require(atomic_load(&region->status->attempts) == allocations);
}
static SlimBytes success(const Slim_type_psession_95_95State *state, int64_t epoch,
                         int64_t serial, int64_t workers, int64_t profile,
                         SlimRegion *region) {
    uint64_t allocations = atomic_load(&region->status->attempts);
    Slim_type_pnativebuild_95_95Selection value = slim_fn_pnativebuild_95_95select(
        *state, epoch, serial, workers, region);
    require(value.slim_field_ready && value.slim_field_code.len > 0 && value.slim_field_workers == profile);
    require(value.slim_field_reason.len == 0);
    Slim_type_psession_95_95Artifact artifact = slim_fn_psession_95_95artifact(state->slim_field_good, region);
    require(value.slim_field_code.len == artifact.slim_field_code.len);
    require(memcmp(value.slim_field_code.data, artifact.slim_field_code.data, (size_t)value.slim_field_code.len) == 0);
    require(atomic_load(&region->status->attempts) == allocations);
    return value.slim_field_code;
}
int main(int argc, char **argv) {
    require(argc == 5);
    SlimAllocStatus allocation;
    SlimRegion region;
    slim_alloc_status_init(&allocation);
    slim_rt_init(&region, &allocation);
    const Slim_type_psession_95_95Limits limits = {64, 67108864, 1000000, 67108864};
    const Slim_type_psession_95_95Config config = {input("compiler"), input("runtime"), input("target"), input("options")};
    Slim_type_psession_95_95State state = slim_fn_psession_95_95start(7, limits, &region);
    rejection(&state, 7, 1, 0, "missing-successful-revision", &region);
    rejection(&state, 0, 1, 0, "invalid-revision", &region);
    rejection(&state, INT64_MIN, 1, 0, "invalid-revision", &region);
    rejection(&state, 7, 0, 0, "invalid-revision", &region);
    rejection(&state, 7, INT64_MIN, 0, "invalid-revision", &region);
    rejection(&state, 7, 1, -1, "invalid-workers", &region);
    rejection(&state, 7, 1, 2, "invalid-workers", &region);
    Slim_type_psession_95_95Report first = slim_fn_psession_95_95update_950path(&state, input(argv[1]), config, &region);
    require(first.slim_field_status == 0);
    int64_t serial = first.slim_field_published.slim_field_serial;
    SlimBytes original = success(&state, 7, serial, 0, 0, &region);
    (void)success(&state, 7, serial, 1, 0, &region);
    rejection(&state, 8, serial, 0, "stale-revision", &region);
    rejection(&state, 7, INT64_MAX, 0, "stale-revision", &region);
    Slim_type_psession_95_95Report bad = slim_fn_psession_95_95update_950path(&state, input(argv[3]), config, &region);
    require(bad.slim_field_status != 0);
    rejection(&state, 7, bad.slim_field_attempted.slim_field_serial, 0, "stale-revision", &region);
    (void)success(&state, 7, serial, 0, 0, &region);
    Slim_type_psession_95_95Report after = slim_fn_psession_95_95update_950path(&state, input(argv[2]), config, &region);
    require(after.slim_field_status == 0);
    rejection(&state, 7, serial, 0, "stale-revision", &region);
    serial = after.slim_field_published.slim_field_serial;
    SlimBytes changed = success(&state, 7, serial, 0, 0, &region);
    require(original.len != changed.len || memcmp(original.data, changed.data, (size_t)original.len) != 0);
    Slim_type_psession_95_95Artifact *artifact = (void *)state.slim_field_good.slim_field_artifacts.data;
    artifact->slim_field_checksum++;
    rejection(&state, 7, serial, 0, "corrupt-source-artifact", &region);
    artifact->slim_field_checksum--;
    int64_t nodes = state.slim_field_good.slim_field_canonical_95nodes;
    state.slim_field_good.slim_field_canonical_95nodes = INT64_MIN;
    rejection(&state, 7, serial, 0, "corrupt-source-snapshot", &region);
    state.slim_field_good.slim_field_canonical_95nodes = nodes;
    (void)success(&state, 7, serial, 0, 0, &region);
    Slim_type_psession_95_95Report parallel = slim_fn_psession_95_95update_950path(&state, input(argv[4]), config, &region);
    require(parallel.slim_field_status == 0);
    serial = parallel.slim_field_published.slim_field_serial;
    (void)success(&state, 7, serial, 0, 1, &region);
    (void)success(&state, 7, serial, 1, 2, &region);
    slim_rt_shutdown();
    slim_alloc_status_init(&allocation);
    slim_rt_init(&region, &allocation);
    state = slim_fn_psession_95_95start(8, limits, &region);
    rejection(&state, 7, serial, 1, "missing-successful-revision", &region);
    first = slim_fn_psession_95_95update_950path(&state, input(argv[1]), config, &region);
    require(first.slim_field_status == 0);
    rejection(&state, 7, first.slim_field_published.slim_field_serial, 0, "stale-revision", &region);
    (void)success(&state, 8, first.slim_field_published.slim_field_serial, 0, 0, &region);
    require(!slim_region_failed(&region));
    slim_rt_shutdown();
    puts("native selection exact; no selection allocation");
    return 0;
}
