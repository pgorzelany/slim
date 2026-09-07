#include "slim_rt.h"
#include <stdio.h>
#include <stdlib.h>

static const size_t widths[] = {1, 2, 3, 4, 8, 16, 32, 64};

static uint8_t expected_byte(int64_t index, size_t byte, size_t width) {
    return (uint8_t)(((uint64_t)index * 17 + byte * 29 + width) % 256);
}

static void verify(const SlimVec *vectors) {
    for (size_t at = 0; at < sizeof(widths) / sizeof(widths[0]); ++at) {
        const SlimVec *vector = &vectors[at];
        if (vector->element_size != widths[at] || vector->len < 0 ||
            vector->len > 257 || vector->capacity < vector->len) abort();
        for (int64_t index = 0; index < vector->len; ++index) {
            uint8_t value[64];
            slim_vec_get(vector, index, value);
            for (size_t byte = 0; byte < widths[at]; ++byte) {
                if (value[byte] != expected_byte(index, byte, widths[at])) abort();
            }
        }
    }
}

int main(void) {
    SlimAllocStatus status;
    slim_alloc_status_init(&status);
    SlimRegion root;
    slim_rt_init(&root, &status);
    SlimVec vectors[sizeof(widths) / sizeof(widths[0])];
    for (size_t at = 0; at < sizeof(widths) / sizeof(widths[0]); ++at) {
        vectors[at] = slim_vec_new(widths[at], &root);
    }
    for (size_t at = 0; at < sizeof(widths) / sizeof(widths[0]); ++at) {
        for (int64_t index = 0; index < 257; ++index) {
            uint8_t value[64];
            for (size_t byte = 0; byte < widths[at]; ++byte) {
                value[byte] = expected_byte(index, byte, widths[at]);
            }
            if (!slim_vec_push(&vectors[at], value)) {
                if (vectors[at].len != index || !slim_region_failed(&root)) abort();
                verify(vectors);
                slim_alloc_report(&status);
                slim_rt_shutdown();
                return 71;
            }
        }
    }
    verify(vectors);
    slim_rt_shutdown();
    puts("vector append: 8 widths, 257 values");
    return 0;
}
