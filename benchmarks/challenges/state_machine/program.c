#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    bool a;
    bool b;
    bool c;
    bool d;
} State;

static State run(int64_t remaining, State state) {
    while (remaining > 0) {
        State next = {
            state.b,
            state.c,
            state.d,
            !state.a,
        };
        remaining -= 1;
        state = next;
    }
    return state;
}

static bool same(State left, State right) {
    return left.a == right.a && left.b == right.b && left.c == right.c && left.d == right.d;
}

int main(int argc, char **argv) {
    (void)argv;
    bool seed = argc > 1;
    State left = run(2000000, (State){seed, false, true, false});
    State right = run(2000000, (State){false, seed, false, true});
    printf("%d\n", same(left, right) ? 1 : 0);
    return 0;
}
