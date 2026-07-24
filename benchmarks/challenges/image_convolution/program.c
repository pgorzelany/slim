#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t size = 256;
    const int64_t count = size * size;
    int64_t *current = malloc((size_t)count * sizeof(*current));
    int64_t *next = malloc((size_t)count * sizeof(*next));
    if (current == NULL || next == NULL) {
        free(current);
        free(next);
        return 1;
    }
    for (int64_t index = 0; index < count; ++index) {
        current[index] = (index * 31 + 7) % 256;
        next[index] = 0;
    }
    for (int64_t round = 0; round < 40; ++round) {
        for (int64_t index = 0; index < count; ++index) {
            int64_t row = index / size;
            int64_t column = index % size;
            int64_t previous_row = (row + size - 1) % size;
            int64_t next_row = (row + 1) % size;
            int64_t previous_column = (column + size - 1) % size;
            int64_t next_column = (column + 1) % size;
            int64_t total =
                current[index] * 4 +
                current[previous_row * size + column] +
                current[next_row * size + column] +
                current[row * size + previous_column] +
                current[row * size + next_column];
            next[index] = total / 8;
        }
        for (int64_t index = 0; index < count; ++index) {
            current[index] = next[index];
        }
    }
    int64_t checksum = 0;
    for (int64_t index = 0; index < count; ++index) checksum += current[index];
    printf("%lld\n", (long long)checksum);
    free(next);
    free(current);
    return 0;
}
