#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void merge_sort(int64_t *values, int64_t *scratch, int64_t left, int64_t right) {
    if (right - left <= 1) {
        return;
    }
    int64_t middle = left + (right - left) / 2;
    merge_sort(values, scratch, left, middle);
    merge_sort(values, scratch, middle, right);
    int64_t left_index = left;
    int64_t right_index = middle;
    int64_t output = left;
    while (left_index < middle && right_index < right) {
        if (values[left_index] <= values[right_index]) {
            scratch[output++] = values[left_index++];
        } else {
            scratch[output++] = values[right_index++];
        }
    }
    while (left_index < middle) {
        scratch[output++] = values[left_index++];
    }
    while (right_index < right) {
        scratch[output++] = values[right_index++];
    }
    for (int64_t index = left; index < right; ++index) {
        values[index] = scratch[index];
    }
}

int main(void) {
    const int64_t count = 20000;
    int64_t *values = malloc((size_t)count * sizeof(int64_t));
    int64_t *scratch = malloc((size_t)count * sizeof(int64_t));
    if (values == NULL || scratch == NULL) {
        free(values);
        free(scratch);
        return 70;
    }
    for (int64_t index = 0; index < count; ++index) {
        values[index] = count - index;
    }
    merge_sort(values, scratch, 0, count);
    int64_t total = 0;
    for (int64_t index = 0; index < count; ++index) {
        total += values[index] * (index + 1);
    }
    printf("%" PRId64 "\n", total);
    free(values);
    free(scratch);
    return 0;
}

