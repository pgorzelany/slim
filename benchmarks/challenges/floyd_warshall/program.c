#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t size = 192;
    const int64_t count = size * size;
    int64_t *distances = malloc((size_t)count * sizeof(*distances));
    if (distances == NULL) return 1;
    for (int64_t index = 0; index < count; ++index) {
        int64_t row = index / size;
        int64_t column = index % size;
        distances[index] =
            row == column ? 0 : 1 + (row * 17 + column * 31) % 97;
    }
    for (int64_t pivot = 0; pivot < size; ++pivot) {
        for (int64_t row = 0; row < size; ++row) {
            for (int64_t column = 0; column < size; ++column) {
                int64_t target = row * size + column;
                int64_t candidate =
                    distances[row * size + pivot] +
                    distances[pivot * size + column];
                if (candidate < distances[target]) distances[target] = candidate;
            }
        }
    }
    int64_t checksum = 0;
    for (int64_t index = 0; index < count; ++index) {
        checksum += distances[index];
    }
    printf("%lld\n", (long long)checksum);
    free(distances);
    return 0;
}
