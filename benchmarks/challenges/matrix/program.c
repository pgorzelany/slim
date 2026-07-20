#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t size = 128;
    const int64_t count = size * size;
    int64_t *left = malloc((size_t)count * sizeof(int64_t));
    int64_t *right = malloc((size_t)count * sizeof(int64_t));
    if (left == NULL || right == NULL) {
        free(left);
        free(right);
        return 70;
    }
    for (int64_t index = 0; index < count; ++index) {
        left[index] = (index * 17 + 1) % 97;
        right[index] = (index * 31 + 1) % 97;
    }
    int64_t total = 0;
    for (int64_t row = 0; row < size; ++row) {
        for (int64_t column = 0; column < size; ++column) {
            for (int64_t index = 0; index < size; ++index) {
                total += left[row * size + index] * right[index * size + column];
            }
        }
    }
    printf("%lld\n", (long long)total);
    free(left);
    free(right);
    return 0;
}

