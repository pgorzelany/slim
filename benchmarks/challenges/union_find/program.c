#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int64_t find_root(const int64_t *parents, int64_t node) {
    while (parents[node] != node) node = parents[node];
    return node;
}

int main(void) {
    const int64_t count = 200000;
    int64_t *parents = malloc((size_t)count * sizeof(*parents));
    if (parents == NULL) return 1;
    for (int64_t index = 0; index < count; ++index) parents[index] = index;
    for (int64_t index = 0; index < count; ++index) {
        int64_t partner = (index * 17 + 23) % count;
        int64_t left_root = find_root(parents, index);
        int64_t right_root = find_root(parents, partner);
        if (left_root != right_root) parents[right_root] = left_root;
    }
    int64_t total = 0;
    for (int64_t round = 0; round < 10; ++round) {
        for (int64_t index = 0; index < count; ++index) {
            total += find_root(parents, index);
        }
    }
    printf("%lld\n", (long long)total);
    free(parents);
    return 0;
}
