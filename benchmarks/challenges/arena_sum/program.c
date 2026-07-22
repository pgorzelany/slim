#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t count = 100000;
    int64_t *arena = malloc((size_t)count * sizeof(*arena));
    int64_t *ids = malloc((size_t)count * sizeof(*ids));
    if (arena == NULL || ids == NULL) { free(arena); free(ids); return 1; }
    for (int64_t i = 0; i < count; ++i) {
        arena[i] = (i * 37) % 101;
        ids[i] = i;
    }
    int64_t total = 0;
    for (int64_t i = 0; i < count; ++i) total += arena[ids[i]];
    printf("%lld\n", (long long)total);
    free(ids);
    free(arena);
    return 0;
}
