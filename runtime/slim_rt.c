#include "slim_rt.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SlimAllocation {
    struct SlimAllocation *next;
    struct SlimAllocation *previous;
    SlimRegion *region;
    size_t size;
    max_align_t alignment;
    uint8_t data[];
};

static _Thread_local SlimRegion *slim_root_region = NULL;
static _Thread_local SlimRegion *slim_active_region = NULL;

void slim_region_init(SlimRegion *region, SlimRegion *parent) {
    if (region == NULL) {
        slim_rt_trap("cannot initialize a null region");
    }
    region->newest = NULL;
    region->parent = parent;
    region->status = parent == NULL ? NULL : parent->status;
    slim_active_region = region;
}

void slim_region_destroy(SlimRegion *region) {
    if (region == NULL) {
        return;
    }
    SlimAllocation *allocation = region->newest;
    region->newest = NULL;
    while (allocation != NULL) {
        SlimAllocation *next = allocation->next;
        free(allocation);
        allocation = next;
    }
    if (slim_active_region == region) {
        slim_active_region = region->parent;
    }
}

void slim_alloc_status_init(SlimAllocStatus *status) {
    *status = (SlimAllocStatus){.code = SLIM_ALLOC_OK};
    const char *setting = getenv("SLIM_ALLOC_FAIL_AT");
    if (setting == NULL || setting[0] == 0) {
        return;
    }
    errno = 0;
    char *end = NULL;
    unsigned long long ordinal = strtoull(setting, &end, 10);
    if (errno == 0 && end != setting && *end == 0 && ordinal > 0) {
        status->fail_at = (uint64_t)ordinal;
    }
}

bool slim_region_failed(const SlimRegion *region) {
    return region->status->code != SLIM_ALLOC_OK;
}

void slim_alloc_report(const SlimAllocStatus *status) {
    fprintf(stderr,
            "SLIM allocation failure: exhausted at allocation %" PRIu64 "\n",
            status->failure_at);
}

void slim_rt_init(SlimRegion *root, SlimAllocStatus *status) {
    if (slim_root_region != NULL) {
        slim_rt_trap("runtime initialized twice");
    }
    slim_region_init(root, NULL);
    root->status = status;
    slim_root_region = root;
}

void slim_rt_shutdown(void) {
    if (slim_root_region != NULL) {
        SlimRegion *root = slim_root_region;
        slim_root_region = NULL;
        slim_region_destroy(root);
    }
}

_Noreturn void slim_rt_trap(const char *message) {
    fprintf(stderr, "SLIM runtime trap: %s\n", message);
    while (slim_active_region != NULL) {
        slim_region_destroy(slim_active_region);
    }
    slim_root_region = NULL;
    exit(70);
}

static SlimAllocation *slim_allocation_from_data(void *pointer) {
    return (SlimAllocation *)((uint8_t *)pointer - offsetof(SlimAllocation, data));
}

void *slim_rt_alloc(SlimRegion *region, size_t size) {
    if (region == NULL) {
        slim_rt_trap("allocation requires a region");
    }
    SlimAllocStatus *status = region->status;
    if (status == NULL) {
        slim_rt_trap("allocation region has no status");
    }
    if (status->code != SLIM_ALLOC_OK) {
        return NULL;
    }
    status->attempts += 1;
    if (status->attempts == 0 || status->attempts == status->fail_at) {
        status->code = SLIM_ALLOC_EXHAUSTED;
        status->failure_at = status->attempts;
        return NULL;
    }
    if (size == 0) {
        size = 1;
    }
    if (size > SIZE_MAX - offsetof(SlimAllocation, data)) {
        slim_rt_trap("allocation size overflow");
    }
    SlimAllocation *allocation = calloc(1, offsetof(SlimAllocation, data) + size);
    if (allocation == NULL) {
        status->code = SLIM_ALLOC_EXHAUSTED;
        status->failure_at = status->attempts;
        return NULL;
    }
    allocation->next = region->newest;
    allocation->previous = NULL;
    allocation->region = region;
    allocation->size = size;
    if (region->newest != NULL) {
        region->newest->previous = allocation;
    }
    region->newest = allocation;
    return allocation->data;
}

void *slim_rt_realloc(SlimRegion *region, void *pointer, size_t old_size, size_t new_size) {
    if (pointer == NULL) {
        return slim_rt_alloc(region, new_size);
    }
    SlimAllocation *allocation = slim_allocation_from_data(pointer);
    if (allocation->region != region) {
        slim_rt_trap("attempted to resize memory through the wrong region");
    }
    if (old_size > allocation->size) {
        slim_rt_trap("attempted to resize memory with an invalid old size");
    }
    void *new_pointer = slim_rt_alloc(region, new_size);
    if (new_pointer == NULL) {
        return NULL;
    }
    size_t copied = old_size < new_size ? old_size : new_size;
    if (copied > 0) {
        memcpy(new_pointer, pointer, copied);
    }
    if (allocation->previous != NULL) {
        allocation->previous->next = allocation->next;
    } else {
        region->newest = allocation->next;
    }
    if (allocation->next != NULL) {
        allocation->next->previous = allocation->previous;
    }
    free(allocation);
    return new_pointer;
}

int64_t slim_i64_add(int64_t left, int64_t right) {
    int64_t result;
    if (__builtin_add_overflow(left, right, &result)) {
        slim_rt_trap("I64 addition overflow");
    }
    return result;
}

int64_t slim_i64_sub(int64_t left, int64_t right) {
    int64_t result;
    if (__builtin_sub_overflow(left, right, &result)) {
        slim_rt_trap("I64 subtraction overflow");
    }
    return result;
}

int64_t slim_i64_mul(int64_t left, int64_t right) {
    int64_t result;
    if (__builtin_mul_overflow(left, right, &result)) {
        slim_rt_trap("I64 multiplication overflow");
    }
    return result;
}

int64_t slim_i64_div(int64_t left, int64_t right) {
    if (right == 0) {
        slim_rt_trap("I64 division by zero");
    }
    if (left == INT64_MIN && right == -1) {
        slim_rt_trap("I64 division overflow");
    }
    return left / right;
}

int64_t slim_i64_rem(int64_t left, int64_t right) {
    if (right == 0) {
        slim_rt_trap("I64 remainder by zero");
    }
    if (left == INT64_MIN && right == -1) {
        return 0;
    }
    return left % right;
}

uint8_t slim_i64_to_u8(int64_t value) {
    if (value < 0 || value > UINT8_MAX) {
        slim_rt_trap("I64 value does not fit U8");
    }
    return (uint8_t)value;
}

SlimBytes slim_bytes_static(const uint8_t *data, int64_t len) {
    if (len < 0) {
        slim_rt_trap("negative byte length");
    }
    return (SlimBytes){.data = data, .len = len};
}

int64_t slim_bytes_len(SlimBytes bytes) {
    return bytes.len;
}

uint8_t slim_bytes_get(SlimBytes bytes, int64_t index) {
    if (index < 0 || index >= bytes.len) {
        slim_rt_trap("byte index out of bounds");
    }
    return bytes.data[index];
}

SlimBytes slim_bytes_freeze(SlimVec bytes) {
    if (bytes.element_size != sizeof(uint8_t)) {
        slim_rt_trap("bytes.freeze requires a U8 vector");
    }
    return (SlimBytes){.data = bytes.data, .len = bytes.len};
}

bool slim_read_file(SlimBytes path, SlimVec *output) {
    if (output->element_size != sizeof(uint8_t)) {
        slim_rt_trap("io.read-file requires a U8 vector");
    }
    if (memchr(path.data, 0, (size_t)path.len) != NULL) {
        return false;
    }
    SlimRegion scratch;
    slim_region_init(&scratch, output->region);
    char *path_string = slim_rt_alloc(&scratch, (size_t)path.len + 1);
    if (path_string == NULL) {
        slim_region_destroy(&scratch);
        return false;
    }
    memcpy(path_string, path.data, (size_t)path.len);
    path_string[path.len] = 0;
    FILE *file = fopen(path_string, "rb");
    slim_region_destroy(&scratch);
    if (file == NULL) {
        return false;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return false;
    }
    long length = ftell(file);
    if (length < 0 || (uintmax_t)length > (uintmax_t)(INT64_MAX - output->len)) {
        fclose(file);
        return false;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return false;
    }
    int64_t required = output->len + (int64_t)length;
    if (required > output->capacity) {
        size_t old_size = (size_t)output->capacity;
        size_t new_size = (size_t)required;
        uint8_t *resized = slim_rt_realloc(output->region, output->data, old_size, new_size);
        if (resized == NULL) {
            fclose(file);
            return false;
        }
        output->data = resized;
        output->capacity = required;
    }
    size_t read = fread(output->data + output->len, 1, (size_t)length, file);
    if (read != (size_t)length || ferror(file)) {
        fclose(file);
        return false;
    }
    if (fclose(file) != 0) {
        return false;
    }
    output->len = required;
    return true;
}

SlimUnit slim_print_i64(int64_t value) {
    if (printf("%" PRId64, value) < 0) {
        slim_rt_trap("cannot write output");
    }
    return (SlimUnit){0};
}

SlimUnit slim_print_bytes(SlimBytes value) {
    if (value.len > 0 && fwrite(value.data, 1, (size_t)value.len, stdout) != (size_t)value.len) {
        slim_rt_trap("cannot write output");
    }
    return (SlimUnit){0};
}

SlimUnit slim_println(SlimBytes value) {
    slim_print_bytes(value);
    if (fputc('\n', stdout) == EOF) {
        slim_rt_trap("cannot write output");
    }
    return (SlimUnit){0};
}

SlimVec slim_vec_new(size_t element_size, SlimRegion *region) {
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

int64_t slim_vec_len(SlimVec vector) {
    return vector.len;
}

size_t slim_vec_check_index(const SlimVec *vector, int64_t index) {
    if (index < 0 || index >= vector->len) {
        slim_rt_trap("vector index out of bounds");
    }
    return (size_t)index;
}

static size_t slim_vec_offset(const SlimVec *vector, int64_t index) {
    size_t checked_index = slim_vec_check_index(vector, index);
    if ((uint64_t)index > SIZE_MAX / vector->element_size) {
        slim_rt_trap("vector offset overflow");
    }
    return checked_index * vector->element_size;
}

void slim_vec_get(const SlimVec *vector, int64_t index, void *output) {
    memcpy(output, vector->data + slim_vec_offset(vector, index), vector->element_size);
}

bool slim_vec_push(SlimVec *vector, const void *value) {
    if (vector->len == vector->capacity) {
        int64_t new_capacity = vector->capacity == 0 ? 8 : vector->capacity * 2;
        if (new_capacity < vector->capacity ||
            (uint64_t)new_capacity > SIZE_MAX / vector->element_size) {
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

void slim_vec_set(SlimVec *vector, int64_t index, const void *value) {
    memcpy(vector->data + slim_vec_offset(vector, index), value, vector->element_size);
}

bool slim_arena_add(SlimVec *arena, const void *value, SlimId *output) {
    SlimId id = arena->len;
    if (!slim_vec_push(arena, value)) {
        return false;
    }
    *output = id;
    return true;
}
