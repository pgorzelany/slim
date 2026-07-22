#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t count = 250000;
    int64_t *values = malloc((size_t)count * sizeof(*values));
    if (values == NULL) return 1;
    for (int64_t i = 0; i < count; ++i) values[i] = (i * 31) % 97;
    int64_t total = 0;
    for (int64_t i = 0; i < count; ++i) {
        total += values[i];
        values[i] = total;
    }
    printf("%lld\n", (long long)total);
    free(values);
    return 0;
}
