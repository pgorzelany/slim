#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int64_t search(const int64_t *values, int64_t target, int64_t left, int64_t right) {
    while (left < right) {
        int64_t middle = left + (right - left) / 2;
        if (values[middle] == target) return middle;
        if (values[middle] < target) left = middle + 1;
        else right = middle;
    }
    return -1;
}

int main(void) {
    const int64_t count = 200000;
    int64_t *values = malloc((size_t)count * sizeof(*values));
    if (values == NULL) return 1;
    for (int64_t i = 0; i < count; ++i) values[i] = i * 2;
    int64_t total = 0;
    for (int64_t i = 0; i < 20000; ++i) {
        int64_t target = (i * 7919) % count;
        total += search(values, target * 2, 0, count);
    }
    printf("%lld\n", (long long)total);
    free(values);
    return 0;
}
