#ifndef SLIM_RT_H
#define SLIM_RT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
    SlimAllocCode code;
    uint64_t attempts;
    uint64_t failure_at;
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

void slim_alloc_status_init(SlimAllocStatus *status);
bool slim_region_failed(const SlimRegion *region);
void slim_alloc_report(const SlimAllocStatus *status);
void slim_rt_init(SlimRegion *root, SlimAllocStatus *status);
void slim_rt_shutdown(void);
_Noreturn void slim_rt_trap(const char *message);

void slim_region_init(SlimRegion *region, SlimRegion *parent);
void slim_region_destroy(SlimRegion *region);
void *slim_rt_alloc(SlimRegion *region, size_t size);
void *slim_rt_realloc(SlimRegion *region, void *pointer, size_t old_size, size_t new_size);

int64_t slim_i64_add(int64_t left, int64_t right);
int64_t slim_i64_sub(int64_t left, int64_t right);
int64_t slim_i64_mul(int64_t left, int64_t right);
int64_t slim_i64_div(int64_t left, int64_t right);
int64_t slim_i64_rem(int64_t left, int64_t right);
uint8_t slim_i64_to_u8(int64_t value);

SlimBytes slim_bytes_static(const uint8_t *data, int64_t len);
int64_t slim_bytes_len(SlimBytes bytes);
uint8_t slim_bytes_get(SlimBytes bytes, int64_t index);
SlimBytes slim_bytes_freeze(SlimVec bytes);
bool slim_read_file(SlimBytes path, SlimVec *output);

SlimUnit slim_print_i64(int64_t value);
SlimUnit slim_print_bytes(SlimBytes value);
SlimUnit slim_println(SlimBytes value);

SlimVec slim_vec_new(size_t element_size, SlimRegion *region);
int64_t slim_vec_len(SlimVec vector);
size_t slim_vec_check_index(const SlimVec *vector, int64_t index);
void slim_vec_get(const SlimVec *vector, int64_t index, void *output);
bool slim_vec_push(SlimVec *vector, const void *value);
void slim_vec_set(SlimVec *vector, int64_t index, const void *value);
bool slim_arena_add(SlimVec *arena, const void *value, SlimId *output);

#endif
