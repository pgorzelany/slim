#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef STATE_MACHINE_ITERATIONS
#define STATE_MACHINE_ITERATIONS 2000000
#endif

#ifndef STATE_MACHINE_REPETITIONS
#define STATE_MACHINE_REPETITIONS 1
#endif

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
    State left = {seed, false, true, false};
    State right = {false, seed, false, true};
    for (int64_t repetition = 0; repetition < STATE_MACHINE_REPETITIONS; repetition += 1) {
        left = run(STATE_MACHINE_ITERATIONS, left);
        right = run(STATE_MACHINE_ITERATIONS, right);
    }
    printf("%d\n", same(left, right) ? 1 : 0);
    return 0;
}
