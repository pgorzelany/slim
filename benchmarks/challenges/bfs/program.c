#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t limit = 100000;
    int64_t *queue = malloc((size_t)(limit + 1) * sizeof(int64_t));
    if (queue == NULL) {
        return 70;
    }
    int64_t length = 1;
    queue[0] = 0;
    int64_t total = 0;
    for (int64_t index = 0; index < length; ++index) {
        int64_t node = queue[index];
        total += node;
        int64_t left = node * 2 + 1;
        int64_t right = left + 1;
        if (left <= limit) {
            queue[length++] = left;
        }
        if (right <= limit) {
            queue[length++] = right;
        }
    }
    printf("%lld\n", (long long)total);
    free(queue);
    return 0;
}

