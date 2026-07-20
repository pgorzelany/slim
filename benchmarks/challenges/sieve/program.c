#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const int64_t limit = 1000000;
    bool *flags = malloc((size_t)(limit + 1) * sizeof(bool));
    if (flags == NULL) {
        return 70;
    }
    for (int64_t index = 0; index <= limit; ++index) {
        flags[index] = true;
    }
    flags[0] = false;
    flags[1] = false;
    for (int64_t prime = 2; prime * prime <= limit; ++prime) {
        if (flags[prime]) {
            for (int64_t multiple = prime * prime; multiple <= limit; multiple += prime) {
                flags[multiple] = false;
            }
        }
    }
    int64_t count = 0;
    for (int64_t index = 2; index <= limit; ++index) {
        count += flags[index] ? 1 : 0;
    }
    printf("%lld\n", (long long)count);
    free(flags);
    return 0;
}

