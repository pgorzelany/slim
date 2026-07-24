#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t size = 128;
    const int64_t count = size * size;
    uint8_t *current = malloc((size_t)count);
    uint8_t *next = malloc((size_t)count);
    if (current == NULL || next == NULL) {
        free(current);
        free(next);
        return 1;
    }
    for (int64_t index = 0; index < count; ++index) {
        current[index] = (uint8_t)(((index * 17 + 3) % 3) == 0);
        next[index] = 0;
    }
    for (int64_t generation = 0; generation < 100; ++generation) {
        for (int64_t index = 0; index < count; ++index) {
            int64_t row = index / size;
            int64_t column = index % size;
            int64_t neighbors = 0;
            for (int64_t offset = 0; offset < 9; ++offset) {
                if (offset == 4) continue;
                int64_t row_delta = offset / 3 - 1;
                int64_t column_delta = offset % 3 - 1;
                int64_t neighbor_row = (row + row_delta + size) % size;
                int64_t neighbor_column = (column + column_delta + size) % size;
                neighbors +=
                    current[neighbor_row * size + neighbor_column] != 0;
            }
            next[index] =
                (uint8_t)(neighbors == 3 || (current[index] && neighbors == 2));
        }
        for (int64_t index = 0; index < count; ++index) {
            current[index] = next[index];
        }
    }
    int64_t checksum = 0;
    for (int64_t index = 0; index < count; ++index) {
        checksum += current[index] != 0;
    }
    printf("%lld\n", (long long)checksum);
    free(next);
    free(current);
    return 0;
}
