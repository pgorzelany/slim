#include "slim_rt.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SlimAllocation {
    void *pointer;
    struct SlimAllocation *next;
} SlimAllocation;

static SlimAllocation *slim_allocations = NULL;

void slim_rt_init(void) {
    if (slim_allocations != NULL) {
        slim_rt_trap("runtime initialized twice");
    }
}

void slim_rt_shutdown(void) {
    SlimAllocation *allocation = slim_allocations;
    slim_allocations = NULL;
    while (allocation != NULL) {
        SlimAllocation *next = allocation->next;
        free(allocation->pointer);
        free(allocation);
        allocation = next;
    }
}

_Noreturn void slim_rt_trap(const char *message) {
    fprintf(stderr, "SLIM runtime trap: %s\n", message);
    slim_rt_shutdown();
    exit(70);
}

static SlimAllocation *slim_find_allocation(void *pointer) {
    for (SlimAllocation *item = slim_allocations; item != NULL; item = item->next) {
        if (item->pointer == pointer) {
            return item;
        }
    }
    return NULL;
}

void *slim_rt_alloc(size_t size) {
    if (size == 0) {
        size = 1;
    }
    void *pointer = calloc(1, size);
    SlimAllocation *record = malloc(sizeof(SlimAllocation));
    if (pointer == NULL || record == NULL) {
        free(pointer);
        free(record);
        slim_rt_trap("out of memory");
    }
    record->pointer = pointer;
    record->next = slim_allocations;
    slim_allocations = record;
    return pointer;
}

void *slim_rt_realloc(void *pointer, size_t old_size, size_t new_size) {
    if (pointer == NULL) {
        return slim_rt_alloc(new_size);
    }
    SlimAllocation *record = slim_find_allocation(pointer);
    if (record == NULL) {
        slim_rt_trap("attempted to resize unmanaged memory");
    }
    if (new_size == 0) {
        new_size = 1;
    }
    void *new_pointer = realloc(pointer, new_size);
    if (new_pointer == NULL) {
        slim_rt_trap("out of memory");
    }
    if (new_size > old_size) {
        memset((uint8_t *)new_pointer + old_size, 0, new_size - old_size);
    }
    record->pointer = new_pointer;
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

SlimBytes slim_read_file(SlimBytes path) {
    if (memchr(path.data, 0, (size_t)path.len) != NULL) {
        slim_rt_trap("file path contains a zero byte");
    }
    char *path_string = slim_rt_alloc((size_t)path.len + 1);
    memcpy(path_string, path.data, (size_t)path.len);
    path_string[path.len] = 0;
    FILE *file = fopen(path_string, "rb");
    if (file == NULL) {
        slim_rt_trap("cannot open input file");
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        slim_rt_trap("cannot seek input file");
    }
    long length = ftell(file);
    if (length < 0 || (uintmax_t)length > (uintmax_t)INT64_MAX) {
        fclose(file);
        slim_rt_trap("input file is too large");
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        slim_rt_trap("cannot rewind input file");
    }
    uint8_t *data = slim_rt_alloc((size_t)length);
    size_t read = fread(data, 1, (size_t)length, file);
    if (read != (size_t)length || ferror(file)) {
        fclose(file);
        slim_rt_trap("cannot read input file");
    }
    if (fclose(file) != 0) {
        slim_rt_trap("cannot close input file");
    }
    return (SlimBytes){.data = data, .len = (int64_t)length};
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

SlimVec slim_vec_new(size_t element_size) {
    if (element_size == 0) {
        slim_rt_trap("zero-sized vector element");
    }
    return (SlimVec){.data = NULL, .len = 0, .capacity = 0, .element_size = element_size};
}

int64_t slim_vec_len(SlimVec vector) {
    return vector.len;
}

static size_t slim_vec_offset(const SlimVec *vector, int64_t index) {
    if (index < 0 || index >= vector->len) {
        slim_rt_trap("vector index out of bounds");
    }
    if ((uint64_t)index > SIZE_MAX / vector->element_size) {
        slim_rt_trap("vector offset overflow");
    }
    return (size_t)index * vector->element_size;
}

void slim_vec_get(const SlimVec *vector, int64_t index, void *output) {
    memcpy(output, vector->data + slim_vec_offset(vector, index), vector->element_size);
}

void slim_vec_push(SlimVec *vector, const void *value) {
    if (vector->len == vector->capacity) {
        int64_t new_capacity = vector->capacity == 0 ? 8 : vector->capacity * 2;
        if (new_capacity < vector->capacity ||
            (uint64_t)new_capacity > SIZE_MAX / vector->element_size) {
            slim_rt_trap("vector capacity overflow");
        }
        size_t old_size = (size_t)vector->capacity * vector->element_size;
        size_t new_size = (size_t)new_capacity * vector->element_size;
        vector->data = slim_rt_realloc(vector->data, old_size, new_size);
        vector->capacity = new_capacity;
    }
    memcpy(vector->data + (size_t)vector->len * vector->element_size,
           value,
           vector->element_size);
    vector->len += 1;
}

void slim_vec_set(SlimVec *vector, int64_t index, const void *value) {
    memcpy(vector->data + slim_vec_offset(vector, index), value, vector->element_size);
}

SlimId slim_arena_add(SlimVec *arena, const void *value) {
    SlimId id = arena->len;
    slim_vec_push(arena, value);
    return id;
}

