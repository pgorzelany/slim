#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t capacity = 2048;
    int64_t *values = calloc((size_t)capacity + 1, sizeof(*values));
    if (values == NULL) return 1;
    for (int64_t item = 1; item <= 200; ++item) {
        int64_t weight = 1 + (item * 17) % 97;
        int64_t value = 1 + (item * 29) % 211;
        for (int64_t current = capacity; current >= weight; --current) {
            int64_t candidate = values[current - weight] + value;
            if (candidate > values[current]) values[current] = candidate;
        }
    }
    printf("%lld\n", (long long)values[capacity]);
    free(values);
    return 0;
}
