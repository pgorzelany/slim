#include "slim_pool.h"

#include <stdlib.h>
#include <string.h>

#ifdef SLIM_POOL_ASAN
#include <sanitizer/asan_interface.h>
#define POOL_POISON(address, bytes) __asan_poison_memory_region((address), (bytes))
#define POOL_UNPOISON(address, bytes) __asan_unpoison_memory_region((address), (bytes))
#else
#define POOL_POISON(address, bytes) ((void)0)
#define POOL_UNPOISON(address, bytes) ((void)0)
#endif

#define SLIM_POOL_MAGIC UINT16_C(0x534c)

typedef struct {
    uint64_t generation;
    uint32_t requested;
    uint8_t order;
    uint8_t kind;
    uint16_t magic;
} SlimPoolHeader;

_Static_assert(sizeof(SlimPoolHeader) == 16, "pool header must be 16 bytes");
_Static_assert(_Alignof(SlimPoolHeader) <= 16, "unsupported pool header alignment");

#ifdef SLIM_POOL_INSTRUMENT
#define POOL_COUNT(pool, field) ((pool)->work.field += 1)
#else
#define POOL_COUNT(pool, field) ((void)0)
#endif

/* These checks protect internal derived indexes, not untrusted native pointers. */
static void pool_require(bool condition) {
    if (!condition) abort();
}

static bool valid_capacity(uint64_t capacity) {
    return capacity >= SLIM_POOL_MIN_CAPACITY &&
           capacity <= SLIM_POOL_MAX_CAPACITY &&
           (capacity & (capacity - 1)) == 0;
}

static bool active(const SlimPool *pool) {
    return pool != NULL && pool->reservation != NULL && pool->base != NULL &&
           pool->availability != NULL && valid_capacity(pool->capacity) &&
           pool->max_order < SLIM_POOL_ORDERS;
}

static uint64_t block_bytes(unsigned order) {
    pool_require(order < SLIM_POOL_ORDERS);
    return UINT64_C(64) << order;
}

static unsigned lowest_bit(SlimPool *pool, uint64_t word) {
    pool_require(word != 0);
    unsigned bit = 0;
    while ((word & UINT64_C(1)) == 0) {
        POOL_COUNT(pool, bit_steps);
        word >>= 1;
        bit += 1;
    }
    POOL_COUNT(pool, bit_steps);
    (void)pool;
    return bit;
}

size_t slim_pool_index_bytes(uint64_t capacity) {
    if (!valid_capacity(capacity)) return 0;
    uint64_t total_words = 1; /* availability */
    for (uint64_t bits = capacity / 64; bits != 0; bits /= 2) {
        uint64_t count = bits;
        do {
            count = (count + 63) / 64;
            total_words += count;
        } while (count != 1);
    }
    if (total_words > SIZE_MAX / sizeof(uint64_t)) return 0;
    return (size_t)total_words * sizeof(uint64_t);
}

static bool index_has(const SlimPool *pool, unsigned order, uint32_t bit) {
    pool_require(order <= pool->max_order);
    const SlimPoolIndex *index = &pool->index[order];
    pool_require(bit < index->bits && bit / 64 < index->counts[0]);
    return (index->words[0][bit / 64] & (UINT64_C(1) << (bit % 64))) != 0;
}

static void index_update(SlimPool *pool, unsigned order, uint32_t bit, bool present) {
    pool_require(order <= pool->max_order);
    SlimPoolIndex *index = &pool->index[order];
    pool_require(bit < index->bits && index->levels >= 1 && index->levels <= SLIM_POOL_LEVELS);
    for (unsigned level = 0; level < index->levels; level++) {
        uint32_t word_index = bit / 64;
        pool_require(word_index < index->counts[level]);
        uint64_t prior = index->words[level][word_index];
        uint64_t mask = UINT64_C(1) << (bit % 64);
        uint64_t current = present ? prior | mask : prior & ~mask;
        index->words[level][word_index] = current;
        POOL_COUNT(pool, hierarchy_writes);
        if ((prior != 0) == (current != 0)) break;
        present = current != 0;
        bit = word_index;
    }
    uint64_t mask = UINT64_C(1) << order;
    bool nonempty = index->words[index->levels - 1][0] != 0;
    *pool->availability = nonempty ? *pool->availability | mask : *pool->availability & ~mask;
    POOL_COUNT(pool, availability_writes);
}

static uint32_t index_minimum(SlimPool *pool, unsigned order) {
    pool_require(order <= pool->max_order);
    SlimPoolIndex *index = &pool->index[order];
    pool_require(index->levels >= 1 && index->levels <= SLIM_POOL_LEVELS);
    uint32_t position = 0;
    for (unsigned level = index->levels; level != 0; level--) {
        pool_require(position < index->counts[level - 1]);
        uint64_t word = index->words[level - 1][position];
        POOL_COUNT(pool, hierarchy_reads);
        position = position * 64 + lowest_bit(pool, word);
    }
    pool_require(position < index->bits);
    return position;
}

bool slim_pool_init(SlimPool *pool, uint64_t capacity, uint64_t failure_at) {
    if (pool == NULL || pool->reservation != NULL || pool->capacity != 0) return false;
    size_t index_bytes = slim_pool_index_bytes(capacity);
    if (index_bytes == 0 || capacity > SIZE_MAX - 15 ||
        index_bytes > SIZE_MAX - 15 - (size_t)capacity) return false;
    size_t total = index_bytes + 15 + (size_t)capacity;
    void *reservation = malloc(total);
    if (reservation == NULL) return false;
    memset(reservation, 0, index_bytes);
    SlimPool built = {0};
    built.reservation = reservation;
    built.availability = reservation;
    uint8_t *candidate = (uint8_t *)reservation + index_bytes;
    size_t padding = (16 - ((uintptr_t)candidate & 15)) & 15;
    built.base = candidate + padding;
    built.capacity = capacity;
    built.index_bytes = index_bytes;
    built.reservation_bytes = total;
    built.failure_at = failure_at;
    uint64_t *cursor = (uint64_t *)reservation + 1;
    unsigned order = 0;
    for (uint64_t bits = capacity / 64; bits != 0; bits /= 2, order++) {
        pool_require(order < SLIM_POOL_ORDERS);
        SlimPoolIndex *index = &built.index[order];
        index->bits = (uint32_t)bits;
        uint64_t count = bits;
        do {
            count = (count + 63) / 64;
            unsigned level = index->levels++;
            pool_require(level < SLIM_POOL_LEVELS);
            index->words[level] = cursor;
            index->counts[level] = (uint32_t)count;
            cursor += count;
        } while (count != 1);
    }
    pool_require((uint8_t *)cursor == (uint8_t *)reservation + index_bytes);
    built.max_order = order - 1;
    index_update(&built, built.max_order, 0, true);
    POOL_POISON(built.base, (size_t)capacity);
    *pool = built;
    return true;
}

SlimPoolResult slim_pool_allocate(SlimPool *pool, uint64_t bytes,
                                  unsigned alignment, bool recursive,
                                  SlimPoolBlock *output) {
    if (!active(pool) || output == NULL || alignment == 0 || alignment > 16 ||
        (alignment & (alignment - 1)) != 0) return SLIM_POOL_INVALID;
    if (bytes == 0) {
        *output = (SlimPoolBlock){0};
        return SLIM_POOL_OK;
    }
    if (pool->attempts == UINT64_MAX) return SLIM_POOL_EXHAUSTED;
    pool->attempts += 1;
    if (pool->failure_at != 0 && pool->attempts == pool->failure_at) return SLIM_POOL_EXHAUSTED;
    unsigned header_bytes = recursive ? 64 : 16;
    if (bytes > pool->capacity - header_bytes) return SLIM_POOL_EXHAUSTED;
    uint64_t required = bytes + header_bytes;
    unsigned order = 0;
    while (block_bytes(order) < required) order += 1;
    pool_require(order <= pool->max_order);
    uint64_t allowed = (UINT64_C(1) << (pool->max_order + 1)) - 1;
    pool_require((*pool->availability & ~allowed) == 0);
    uint64_t eligible = *pool->availability & (UINT64_MAX << order);
    if (eligible == 0) return SLIM_POOL_EXHAUSTED;
    unsigned current = lowest_bit(pool, eligible);
    uint32_t bit = index_minimum(pool, current);
    uint64_t offset = (uint64_t)bit * block_bytes(current);
    pool_require(offset <= pool->capacity - block_bytes(current));
    index_update(pool, current, bit, false);
    while (current > order) {
        current -= 1;
        uint64_t buddy = offset + block_bytes(current);
        index_update(pool, current, (uint32_t)(buddy / block_bytes(current)), true);
        POOL_COUNT(pool, splits);
    }
    uint64_t rounded = block_bytes(order);
    pool_require(pool->live_rounded <= pool->capacity - rounded);
    pool_require(pool->live_requested <= pool->capacity - bytes);
    pool_require(pool->live_blocks < pool->capacity / 64);
    SlimPoolHeader *header = (void *)(pool->base + offset);
    POOL_UNPOISON(header, header_bytes + (size_t)bytes);
    memset(header, 0, header_bytes);
    *header = (SlimPoolHeader){pool->attempts, (uint32_t)bytes, (uint8_t)order,
                               recursive ? 1 : 0, SLIM_POOL_MAGIC};
    pool->live_blocks += 1;
    pool->live_requested += bytes;
    pool->live_rounded += rounded;
    *output = (SlimPoolBlock){pool->base + offset + header_bytes, pool->attempts,
                              rounded - header_bytes, (uint32_t)offset,
                              (uint32_t)bytes, (uint8_t)order, (uint8_t)header_bytes};
    return SLIM_POOL_OK;
}

bool slim_pool_release(SlimPool *pool, SlimPoolBlock block) {
    if (!active(pool)) return false;
    if (block.data == NULL) {
        return block.generation == 0 && block.usable == 0 && block.offset == 0 &&
               block.requested == 0 && block.order == 0 && block.header_bytes == 0;
    }
    if (block.order > pool->max_order ||
        (block.header_bytes != 16 && block.header_bytes != 64) ||
        block.generation == 0 || block.requested == 0) return false;
    uint64_t rounded = block_bytes(block.order);
    if (block.offset % rounded != 0 || block.offset > pool->capacity - rounded ||
        block.header_bytes >= rounded || block.requested > rounded - block.header_bytes ||
        block.usable != rounded - block.header_bytes ||
        block.data != pool->base + block.offset + block.header_bytes) return false;
    SlimPoolHeader *header = (void *)(pool->base + block.offset);
    if (header->magic != SLIM_POOL_MAGIC || header->generation != block.generation ||
        header->requested != block.requested || header->order != block.order ||
        header->kind != (block.header_bytes == 64 ? 1 : 0) ||
        pool->live_blocks == 0 || pool->live_rounded < rounded ||
        pool->live_requested < block.requested ||
        index_has(pool, block.order, block.offset / (uint32_t)rounded)) return false;
    header->magic = 0;
    POOL_POISON(header, (size_t)rounded);
    /* Retain only the initialized header for stale/duplicate-handle checks. */
    POOL_UNPOISON(header, sizeof(*header));
    pool->live_blocks -= 1;
    pool->live_rounded -= rounded;
    pool->live_requested -= block.requested;
    unsigned order = block.order;
    uint64_t offset = block.offset;
    while (order < pool->max_order) {
        uint64_t buddy = offset ^ block_bytes(order);
        POOL_COUNT(pool, buddy_tests);
        if (!index_has(pool, order, (uint32_t)(buddy / block_bytes(order)))) break;
        index_update(pool, order, (uint32_t)(buddy / block_bytes(order)), false);
        if (buddy < offset) offset = buddy;
        order += 1;
        POOL_COUNT(pool, merges);
    }
    index_update(pool, order, (uint32_t)(offset / block_bytes(order)), true);
    return true;
}

bool slim_pool_is_free(const SlimPool *pool, unsigned order, uint64_t offset) {
    if (!active(pool) || order > pool->max_order) return false;
    uint64_t width = block_bytes(order);
    if (offset % width != 0 || offset > pool->capacity - width) return false;
    return index_has(pool, order, (uint32_t)(offset / width));
}

bool slim_pool_destroy(SlimPool *pool) {
    if (pool == NULL) return false;
    if (pool->reservation == NULL) return pool->capacity == 0;
    if (!active(pool) || pool->live_blocks != 0 || pool->live_requested != 0 ||
        pool->live_rounded != 0 || *pool->availability != (UINT64_C(1) << pool->max_order) ||
        !index_has(pool, pool->max_order, 0)) return false;
    free(pool->reservation);
    *pool = (SlimPool){0};
    return true;
}
