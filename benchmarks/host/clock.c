#include "slim_rt.h"

#include <stdint.h>

int main(void) {
    int64_t previous = slim_monotonic_ms();
    for (int64_t remaining = 100000; remaining > 0; --remaining) {
        int64_t current = slim_monotonic_ms();
        if (current < previous) {
            return 1;
        }
        previous = current;
    }
    return 0;
}
