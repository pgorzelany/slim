/* Native protocol witness only: this does not implement SLIM capture checking. */
#include "slim_pool.h"
#include "slim_rt.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint64_t pool_test_allocations, pool_test_frees;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "worker pool check %d: %s\n", __LINE__, #x); exit(1); } } while (0)

typedef struct {
    atomic_bool start;
    atomic_uint completed;
    atomic_uint first_completed;
    unsigned first;
} Schedule;

typedef struct {
    SlimPoolBlock input, result;
    SlimPool *input_domain, *result_domain;
    Schedule *schedule;
    unsigned id;
    bool called;
} Context;

static void forbidden_nested(void *opaque) { (void)opaque; CHECK(false); }

static void run(void *opaque) {
    Context *context = opaque;
    Schedule *schedule = context->schedule;
    CHECK(!context->called);
    context->called = true;
    /* Test-only scheduling instrumentation forces each actual completion order. */
    while (!atomic_load_explicit(&schedule->start, memory_order_acquire)) {}
    if (context->id != schedule->first) {
        while (atomic_load_explicit(&schedule->completed, memory_order_acquire) == 0) {}
    }
    SlimTask nested;
    CHECK(!slim_task_spawn(&nested, forbidden_nested, NULL));
    for (uint64_t i = 0; i < context->input.requested; ++i) {
        context->input.data[i] = (uint8_t)((i * 17 + context->id * 73) % 256);
    }
    context->result = context->input;
    context->result_domain = context->input_domain;
    context->input = (SlimPoolBlock){0};
    context->input_domain = NULL;
    if (context->id == schedule->first) {
        atomic_store_explicit(&schedule->first_completed, context->id, memory_order_relaxed);
    }
    atomic_fetch_add_explicit(&schedule->completed, 1, memory_order_release);
}

static void same_identity(SlimPoolBlock a, SlimPoolBlock b) {
    CHECK(a.data == b.data && a.generation == b.generation && a.offset == b.offset);
    CHECK(a.requested == b.requested && a.usable == b.usable);
    CHECK(a.order == b.order && a.header_bytes == b.header_bytes);
}

int main(int argc, char **argv) {
    CHECK(argc == 4);
    unsigned mask = (unsigned)strtoul(argv[1], NULL, 10);
    unsigned first = (unsigned)strtoul(argv[2], NULL, 10);
    uint64_t fail_at = strtoull(argv[3], NULL, 10);
    CHECK(mask < 4 && first < 2 && fail_at <= 2);
    SlimAllocStatus status;
    SlimRegion root;
    slim_alloc_status_init(&status);
    slim_rt_init(&root, &status);
    SlimPool pool = {0};
    CHECK(slim_pool_init(&pool, SLIM_POOL_MIN_CAPACITY, fail_at));
    Context contexts[2] = {0};
    SlimPoolBlock originals[2] = {0};
    for (unsigned i = 0; i < 2; ++i) {
        uint64_t bytes = (mask & (1U << i)) ? 64 : 0;
        SlimPoolResult result = slim_pool_allocate(&pool, bytes, 1, false, &contexts[i].input);
        if (result == SLIM_POOL_EXHAUSTED) {
            CHECK(fail_at != 0 && pool.attempts == fail_at);
            for (unsigned j = 0; j < i; ++j) {
                if (contexts[j].input.data) CHECK(slim_pool_release(&pool, contexts[j].input));
            }
            CHECK(pool.live_blocks == 0 && slim_pool_destroy(&pool));
            slim_rt_shutdown();
            CHECK(pool_test_allocations == 1 && pool_test_frees == 1);
            printf("admission_failed attempt=%" PRIu64 " tasks=0\n", fail_at);
            return 0;
        }
        CHECK(result == SLIM_POOL_OK);
        originals[i] = contexts[i].input;
        contexts[i].input_domain = &pool;
        contexts[i].id = i;
    }
    unsigned char control[sizeof(SlimPool)], index[512], headers[2][16];
    CHECK(pool.index_bytes <= sizeof(index));
    memcpy(control, &pool, sizeof(pool));
    memcpy(index, pool.availability, pool.index_bytes);
    for (unsigned i = 0; i < 2; ++i) {
        if (originals[i].data) memcpy(headers[i], originals[i].data - 16, 16);
    }
    Schedule schedule;
    atomic_init(&schedule.start, false);
    atomic_init(&schedule.completed, 0);
    atomic_init(&schedule.first_completed, 2);
    for (unsigned i = 0; i < 2; ++i) contexts[i].schedule = &schedule;
    SlimTask task;
    bool spawned = slim_task_spawn(&task, run, &contexts[0]);
    schedule.first = spawned ? first : 1;
    atomic_store_explicit(&schedule.start, true, memory_order_release);
    slim_task_run_inline(run, &contexts[1]);
    if (spawned) slim_task_join(&task);
    else slim_task_run_inline(run, &contexts[0]);
    CHECK(atomic_load(&schedule.completed) == 2);
    CHECK(atomic_load(&schedule.first_completed) == schedule.first);
    CHECK(memcmp(control, &pool, sizeof(pool)) == 0);
    CHECK(memcmp(index, pool.availability, pool.index_bytes) == 0);
    CHECK(pool_test_allocations == 1 && pool_test_frees == 0);
    for (unsigned i = 0; i < 2; ++i) {
        CHECK(contexts[i].called && contexts[i].input.data == NULL);
        CHECK(contexts[i].input_domain == NULL && contexts[i].result_domain == &pool);
        SlimPoolBlock result = contexts[i].result;
        contexts[i].result = (SlimPoolBlock){0};
        same_identity(result, originals[i]);
        if (result.data) CHECK(memcmp(headers[i], result.data - 16, 16) == 0);
        for (uint64_t j = 0; j < result.requested; ++j) {
            CHECK(result.data[j] == (uint8_t)((j * 17 + i * 73) % 256));
        }
        /* Explicit parent growth after join, including formerly empty owners. */
        pool.failure_at = 0;
        SlimPoolBlock grown = {0};
        CHECK(slim_pool_allocate(&pool, 4096, 1, false, &grown) == SLIM_POOL_OK);
        if (result.requested) memcpy(grown.data, result.data, result.requested);
        if (result.data) CHECK(slim_pool_release(&pool, result));
        for (uint64_t j = 0; j < originals[i].requested; ++j) {
            CHECK(grown.data[j] == (uint8_t)((j * 17 + i * 73) % 256));
        }
        CHECK(slim_pool_release(&pool, grown));
    }
    CHECK(pool.live_blocks == 0 && pool.live_requested == 0 && pool.live_rounded == 0);
    CHECK(slim_pool_is_free(&pool, pool.max_order, 0));
    CHECK(slim_pool_destroy(&pool));
    slim_rt_shutdown();
    CHECK(pool_test_allocations == 1 && pool_test_frees == 1);
    printf("returned mask=%u spawned=%u first=%u completed=2 metadata=unchanged reused=1\n",
           mask, (unsigned)spawned, schedule.first);
    return 0;
}
