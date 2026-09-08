/* Verification-only host of the generated SLIM query; no native cache in C. */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#define main native_fixture_main
#ifdef NATIVE_QUERY_OBSERVED
#include "observed.c"
#else
#include "probe.c"
#endif
#undef main
#include "slim_rt.c"

static uint64_t now_ns(void) {
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0) abort();
    return (uint64_t)value.tv_sec * UINT64_C(1000000000) + (uint64_t)value.tv_nsec;
}
static SlimBytes literal(const char *value) {
    return slim_bytes_static((const uint8_t *)value, (int64_t)strlen(value));
}
static void require(bool value) { if (!value) abort(); }
static void report(const char *stage, uint64_t begin, uint64_t end,
                   const SlimRegion *region) {
    uint64_t live = 0;
    for (const SlimAllocation *a = region->newest; a != NULL; a = a->next)
        live += a->size + offsetof(SlimAllocation, data);
    printf("%s\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64,
           stage, end - begin, live, atomic_load(&region->status->attempts));
#ifdef NATIVE_QUERY_OBSERVED
    for (unsigned i = 0; i < 6; ++i) printf("\t%" PRIu64, native_query_counts[i]);
#endif
    putchar('\n');
}
static void reset_counts(void) {
#ifdef NATIVE_QUERY_OBSERVED
    memset(native_query_counts, 0, sizeof(native_query_counts));
#endif
}
int main(int argc, char **argv) {
    require(argc == 3);
    const bool boundary = strcmp(argv[1], "boundary") == 0;
    char *end;
    long size = strtol(argv[2], &end, 10);
    require(*end == '\0' && size >= 1 && size <= 67108864);
    uint8_t *buffer = malloc((size_t)size);
    require(buffer != NULL);
    memset(buffer, 255, (size_t)size);
    SlimAllocStatus allocation;
    SlimRegion region;
    slim_alloc_status_init(&allocation);
    slim_rt_init(&region, &allocation);
    Slim_type_nativecache_95State state = slim_fn_nativecache_95start(
        1, literal("context"), 256, 67108864, &region);
    SlimBytes input = slim_bytes_static(buffer, boundary ? 1 : size);
    SlimBytes output = slim_bytes_static(buffer, boundary ? size - 1 : size);
    Slim_type_nativecache_95Key key = slim_fn_zzprobe_95key(
        1, literal("context"), 0, 0, input, literal(""), &region);
    if (boundary) {
        /* The complete byte limit is crossed before any retained byte clone. */
        Slim_type_nativecache_95Stored invalid = slim_fn_nativecache_95publish(
            &state, key, slim_bytes_static(buffer, size), &region);
        require(!invalid.slim_field_accepted);
        require(atomic_load(&allocation.attempts) == 2);
    }
    reset_counts();
    uint64_t begin = now_ns();
    Slim_type_nativecache_95Stored saved = slim_fn_nativecache_95publish(&state, key, output, &region);
    uint64_t finished = now_ns();
    require(saved.slim_field_accepted);
    report("publish", begin, finished, &region);
    reset_counts();
    begin = now_ns();
    Slim_type_nativecache_95Probe found = slim_fn_nativecache_95lookup(&state, key, &region);
    finished = now_ns();
    require(found.slim_field_hit && found.slim_field_artifact.len == output.len);
    require(memcmp(found.slim_field_artifact.data, output.data, (size_t)output.len) == 0);
    report("hit", begin, finished, &region);
    reset_counts();
    Slim_type_nativecache_95Key different = slim_fn_zzprobe_95key(
        1, literal("context"), 0, 0, literal("different"), literal(""), &region);
    begin = now_ns();
    Slim_type_nativecache_95Probe missed = slim_fn_nativecache_95lookup(&state, different, &region);
    finished = now_ns();
    require(!missed.slim_field_hit && missed.slim_field_artifact.len == 0);
    report("miss", begin, finished, &region);
    reset_counts();
    begin = now_ns();
    Slim_type_nativecache_95Stored duplicate = slim_fn_nativecache_95publish(&state, key, output, &region);
    finished = now_ns();
    require(duplicate.slim_field_accepted);
    report("duplicate", begin, finished, &region);
    slim_rt_shutdown();
    free(buffer);
    return 0;
}
