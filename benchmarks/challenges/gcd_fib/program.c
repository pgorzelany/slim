#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

static int64_t gcd(int64_t left, int64_t right) {
    while (right != 0) {
        int64_t next = left % right;
        left = right;
        right = next;
    }
    return left;
}

static int64_t fib(int64_t remaining, int64_t left, int64_t right) {
    while (remaining != 0) {
        int64_t next = left + right;
        left = right;
        right = next;
        remaining -= 1;
    }
    return left;
}

int main(void) {
    printf("%" PRId64 "\n", gcd(1071, 462) + fib(40, 0, 1));
    return 0;
}

