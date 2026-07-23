#ifndef SLIM_RT_H
#define SLIM_RT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>
#include <string.h>

#if defined(SLIM_PARALLEL) && defined(SLIM_POSIX_WORKERS)
#include <pthread.h>
#endif

typedef struct {
    uint8_t value;
} SlimUnit;

typedef struct {
    const uint8_t *data;
    int64_t len;
} SlimBytes;

typedef struct SlimAllocation SlimAllocation;

typedef enum {
    SLIM_ALLOC_OK = 0,
    SLIM_ALLOC_EXHAUSTED = 1,
} SlimAllocCode;

typedef struct {
    _Atomic SlimAllocCode code;
    _Atomic uint64_t attempts;
    _Atomic uint64_t failure_at;
    uint64_t fail_at;
} SlimAllocStatus;

typedef struct SlimRegion {
    SlimAllocation *newest;
    struct SlimRegion *parent;
    SlimAllocStatus *status;
} SlimRegion;

typedef struct {
    uint8_t *data;
    int64_t len;
    int64_t capacity;
    size_t element_size;
    SlimRegion *region;
} SlimVec;

typedef int64_t SlimId;

#if defined(SLIM_PARALLEL)
typedef void (*SlimTaskFn)(void *context);

typedef struct {
    SlimTaskFn function;
    void *context;
    bool active;
#if defined(SLIM_POSIX_WORKERS)
    pthread_t worker;
#endif
} SlimTask;
#endif

void slim_alloc_status_init(SlimAllocStatus *status);
void slim_alloc_report(const SlimAllocStatus *status);
void slim_rt_init(SlimRegion *root, SlimAllocStatus *status);
void slim_rt_shutdown(void);
_Noreturn void slim_rt_trap(const char *message);

void slim_region_init(SlimRegion *region, SlimRegion *parent);
void slim_region_adopt(SlimRegion *parent, SlimRegion *child);
void slim_region_destroy(SlimRegion *region);
void *slim_rt_alloc(SlimRegion *region, size_t size);
void *slim_rt_realloc(SlimRegion *region, void *pointer, size_t old_size, size_t new_size);

bool slim_read_file(SlimBytes path, SlimVec *output);
int64_t slim_monotonic_ms(void);
bool slim_tcp_exchange(
    SlimBytes address,
    int64_t port,
    SlimBytes request,
    int64_t response_limit,
    int64_t timeout_ms,
    SlimVec *output
);

#if defined(SLIM_PARALLEL)
bool slim_task_spawn(SlimTask *task, SlimTaskFn function, void *context);
void slim_task_run_inline(SlimTaskFn function, void *context);
void slim_task_join(SlimTask *task);
#endif

SlimUnit slim_print_i64(int64_t value);
SlimUnit slim_print_bytes(SlimBytes value);
SlimUnit slim_println(SlimBytes value);

static inline bool slim_region_failed(const SlimRegion *region) {
    return atomic_load(&region->status->code) != SLIM_ALLOC_OK;
}

static inline int64_t slim_i64_add(int64_t left, int64_t right) {
    int64_t result;
    if (__builtin_add_overflow(left, right, &result)) {
        slim_rt_trap("I64 addition overflow");
    }
    return result;
}

static inline int64_t slim_i64_sub(int64_t left, int64_t right) {
    int64_t result;
    if (__builtin_sub_overflow(left, right, &result)) {
        slim_rt_trap("I64 subtraction overflow");
    }
    return result;
}

static inline int64_t slim_i64_mul(int64_t left, int64_t right) {
    int64_t result;
    if (__builtin_mul_overflow(left, right, &result)) {
        slim_rt_trap("I64 multiplication overflow");
    }
    return result;
}

static inline int64_t slim_i64_div(int64_t left, int64_t right) {
    if (right == 0) {
        slim_rt_trap("I64 division by zero");
    }
    if (left == INT64_MIN && right == -1) {
        slim_rt_trap("I64 division overflow");
    }
    return left / right;
}

static inline int64_t slim_i64_rem(int64_t left, int64_t right) {
    if (right == 0) {
        slim_rt_trap("I64 remainder by zero");
    }
    if (left == INT64_MIN && right == -1) {
        return 0;
    }
    return left % right;
}

static inline uint8_t slim_i64_to_u8(int64_t value) {
    if (value < 0 || value > UINT8_MAX) {
        slim_rt_trap("I64 value does not fit U8");
    }
    return (uint8_t)value;
}

static inline SlimBytes slim_bytes_static(const uint8_t *data, int64_t len) {
    if (len < 0) {
        slim_rt_trap("negative byte length");
    }
    return (SlimBytes){.data = data, .len = len};
}

static inline int64_t slim_bytes_len(SlimBytes bytes) {
    return bytes.len;
}

static inline uint8_t slim_bytes_get(SlimBytes bytes, int64_t index) {
    if (index < 0 || index >= bytes.len) {
        slim_rt_trap("byte index out of bounds");
    }
    return bytes.data[index];
}

static inline SlimBytes slim_bytes_freeze(SlimVec bytes) {
    if (bytes.element_size != sizeof(uint8_t)) {
        slim_rt_trap("bytes.freeze requires a U8 vector");
    }
    return (SlimBytes){.data = bytes.data, .len = bytes.len};
}

static inline SlimVec slim_vec_new(size_t element_size, SlimRegion *region) {
    if (element_size == 0) {
        slim_rt_trap("zero-sized vector element");
    }
    if (region == NULL) {
        slim_rt_trap("vector requires a region");
    }
    return (SlimVec){
        .data = NULL,
        .len = 0,
        .capacity = 0,
        .element_size = element_size,
        .region = region,
    };
}

static inline int64_t slim_vec_len(SlimVec vector) {
    return vector.len;
}

static inline size_t slim_vec_check_index(const SlimVec *vector, int64_t index) {
    if (index < 0 || index >= vector->len) {
        slim_rt_trap("vector index out of bounds");
    }
    return (size_t)index;
}

static inline size_t slim_vec_offset(const SlimVec *vector, int64_t index) {
    size_t checked_index = slim_vec_check_index(vector, index);
    if (checked_index > SIZE_MAX / vector->element_size) {
        slim_rt_trap("vector offset overflow");
    }
    return checked_index * vector->element_size;
}

static inline void slim_vec_get(const SlimVec *vector, int64_t index, void *output) {
    memcpy(output, vector->data + slim_vec_offset(vector, index), vector->element_size);
}

static inline bool slim_vec_push(SlimVec *vector, const void *value) {
    if (vector->len == vector->capacity) {
        int64_t new_capacity = 8;
        if (vector->capacity != 0) {
            if (vector->capacity > INT64_MAX / 2) {
                slim_rt_trap("vector capacity overflow");
            }
            new_capacity = vector->capacity * 2;
        }
        if ((uint64_t)new_capacity > SIZE_MAX / vector->element_size) {
            slim_rt_trap("vector capacity overflow");
        }
        size_t old_size = (size_t)vector->capacity * vector->element_size;
        size_t new_size = (size_t)new_capacity * vector->element_size;
        uint8_t *resized = slim_rt_realloc(vector->region, vector->data, old_size, new_size);
        if (resized == NULL) {
            return false;
        }
        vector->data = resized;
        vector->capacity = new_capacity;
    }
    memcpy(vector->data + (size_t)vector->len * vector->element_size,
           value,
           vector->element_size);
    vector->len += 1;
    return true;
}

static inline void slim_vec_set(SlimVec *vector, int64_t index, const void *value) {
    memcpy(vector->data + slim_vec_offset(vector, index), value, vector->element_size);
}

static inline bool slim_arena_add(SlimVec *arena, const void *value, SlimId *output) {
    SlimId id = arena->len;
    if (!slim_vec_push(arena, value)) {
        return false;
    }
    *output = id;
    return true;
}

#endif
