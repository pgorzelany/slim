#include <stdbool.h>
#include <pthread.h>
#include <stddef.h>
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

typedef struct {
    int64_t remaining;
    State input;
    State output;
} Task;

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

static void *run_task(void *opaque) {
    Task *task = opaque;
    task->output = run(task->remaining, task->input);
    return NULL;
}

static bool same(State left, State right) {
    return left.a == right.a && left.b == right.b && left.c == right.c && left.d == right.d;
}

int main(int argc, char **argv) {
    (void)argv;
    bool seed = argc > 1;
    State left_state = {seed, false, true, false};
    State right_state = {false, seed, false, true};
    for (int64_t repetition = 0; repetition < STATE_MACHINE_REPETITIONS; repetition += 1) {
        Task left = {
            STATE_MACHINE_ITERATIONS,
            left_state,
            {false, false, false, false},
        };
        pthread_t worker;
        int created = pthread_create(&worker, NULL, run_task, &left);
        right_state = run(STATE_MACHINE_ITERATIONS, right_state);
        if (created == 0) {
            void *worker_status = NULL;
            if (pthread_join(worker, &worker_status) != 0 || worker_status != NULL) {
                return 70;
            }
        } else {
            left.output = run(left.remaining, left.input);
        }
        left_state = left.output;
    }
    printf("%d\n", same(left_state, right_state) ? 1 : 0);
    return 0;
}
