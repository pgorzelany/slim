#include <stdint.h>
#include <stdio.h>

int main(void) {
    int64_t value = 0;
    for (int64_t index = 0; index < 2000000; ++index) {
        switch (index % 3) {
            case 0: value += index % 17; break;
            case 1: value -= index % 13; break;
            default: break;
        }
    }
    printf("%lld\n", (long long)value);
    return 0;
}
