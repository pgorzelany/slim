#include <stdint.h>
#include <stdio.h>

typedef struct { int64_t x, y, vx, vy; } State;

int main(void) {
    State state = {0, 0, 11, 17};
    for (int64_t i = 0; i < 1000000; ++i) {
        state.x += state.vx;
        state.y += state.vy;
        state.vx += 3;
        state.vy -= 2;
    }
    printf("%lld\n", (long long)(state.x + state.y));
    return 0;
}
