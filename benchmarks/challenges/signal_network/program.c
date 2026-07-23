#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    bool a;
    bool b;
    bool c;
    bool d;
    bool e;
    bool f;
    bool g;
    bool h;
} Signals;

static Signals run(int64_t remaining, Signals signals) {
    while (remaining > 0) {
        Signals next = {
            !signals.h,
            signals.a,
            signals.b && signals.h,
            signals.c || signals.a,
            signals.d,
            !signals.e,
            signals.f && signals.c,
            signals.g || signals.b,
        };
        remaining -= 1;
        signals = next;
    }
    return signals;
}

static bool same(Signals left, Signals right) {
    return left.a == right.a && left.b == right.b && left.c == right.c &&
           left.d == right.d && left.e == right.e && left.f == right.f &&
           left.g == right.g && left.h == right.h;
}

int main(int argc, char **argv) {
    (void)argv;
    bool seed = argc > 1;
    Signals first = {seed, false, true, false, true, false, true, false};
    Signals second = {false, seed, false, true, false, true, false, true};
    Signals left = run(2000000, first);
    Signals right = run(2000000, second);
    printf("%d\n", same(left, right) ? 1 : 0);
    return 0;
}
