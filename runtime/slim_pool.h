#ifndef SLIM_POOL_H
#define SLIM_POOL_H

/* Private RFC-0154 component. Not linked into current generated programs. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SLIM_POOL_MIN_CAPACITY UINT64_C(65536)
#define SLIM_POOL_MAX_CAPACITY UINT64_C(1073741824)
#define SLIM_POOL_DEFAULT_CAPACITY UINT64_C(268435456)
#define SLIM_POOL_ORDERS 25
#define SLIM_POOL_LEVELS 4

typedef struct {
    uint64_t *words[SLIM_POOL_LEVELS];
    uint32_t counts[SLIM_POOL_LEVELS];
    uint32_t bits;
    unsigned levels;
} SlimPoolIndex;

#ifdef SLIM_POOL_INSTRUMENT
typedef struct {
    uint64_t hierarchy_reads;
    uint64_t hierarchy_writes;
    uint64_t availability_writes;
    uint64_t bit_steps;
    uint64_t splits;
    uint64_t buddy_tests;
    uint64_t merges;
} SlimPoolWork;
#endif

typedef struct {
    void *reservation;
    uint8_t *base;
    uint64_t *availability;
    uint64_t capacity;
    size_t index_bytes;
    size_t reservation_bytes;
    unsigned max_order;
    uint64_t attempts;
    uint64_t failure_at;
    uint64_t live_blocks;
    uint64_t live_requested;
    uint64_t live_rounded;
    SlimPoolIndex index[SLIM_POOL_ORDERS];
#ifdef SLIM_POOL_INSTRUMENT
    SlimPoolWork work;
#endif
} SlimPool;

typedef struct {
    uint8_t *data;
    uint64_t generation;
    uint64_t usable;
    uint32_t offset;
    uint32_t requested;
    uint8_t order;
    uint8_t header_bytes;
} SlimPoolBlock;

typedef enum {
    SLIM_POOL_OK,
    SLIM_POOL_EXHAUSTED,
    SLIM_POOL_INVALID
} SlimPoolResult;

size_t slim_pool_index_bytes(uint64_t capacity);
bool slim_pool_init(SlimPool *pool, uint64_t capacity, uint64_t failure_at);
SlimPoolResult slim_pool_allocate(SlimPool *pool, uint64_t bytes,
                                  unsigned alignment, bool recursive,
                                  SlimPoolBlock *output);
bool slim_pool_release(SlimPool *pool, SlimPoolBlock block);
bool slim_pool_is_free(const SlimPool *pool, unsigned order, uint64_t offset);
bool slim_pool_destroy(SlimPool *pool);
#endif
