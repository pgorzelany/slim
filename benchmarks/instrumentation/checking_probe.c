/* Explicit test observation only; never linked into the production compiler. */
#include "checking_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SLIM_CHECKING_PROBE_CAP
#define SLIM_CHECKING_PROBE_CAP UINT64_C(1000000000)
#endif
#define CHECKING_CAP ((uint64_t)SLIM_CHECKING_PROBE_CAP)
_Static_assert(CHECKING_CAP > 0 && CHECKING_CAP <= INT64_MAX,
               "checking observation cap must fit signed source counts");
static uint64_t entries[2], ends[2], invalid[2], failures[2];
static uint64_t active, native_depth, steps, requests, pushes, finishes;
static uint64_t depth_max, branch_max, argument_max, builtin_max;
static uint64_t resumes[18], builtin_phases[13][6];
static int saturated;

static void add(uint64_t *value) {
    if (*value == CHECKING_CAP) saturated = 1;
    else ++*value;
}
static void maximum(uint64_t *value, int64_t candidate) {
    if (candidate < 0) abort();
    uint64_t count = (uint64_t)candidate;
    if (count > CHECKING_CAP) { count = CHECKING_CAP; saturated = 1; }
    if (count > *value) *value = count;
}
void slim_checking_probe_enter(int machine) {
    if (machine < 0 || machine > 1) abort();
    add(&entries[machine]);
    if (machine) {
        add(&active);
        maximum(&native_depth, (int64_t)active);
    }
}
void slim_checking_probe_end(int machine, int64_t kind, int failed) {
    if (machine < 0 || machine > 1) abort();
    add(&ends[machine]);
    if (failed) add(&failures[machine]);
    else if (kind == -2) add(&invalid[machine]);
    if (machine) {
        if (active == 0) abort();
        --active;
    }
}
void slim_checking_probe_step(int returning, int ready, int64_t depth, int64_t limit,
                             int64_t phase, int64_t branches,
                             int64_t arguments, int64_t builtins) {
    if (depth < 0 || depth > limit || branches < 0 || branches > depth ||
        arguments < 0 || arguments > depth || builtins < 0 || builtins > depth)
        abort();
    add(&steps);
    maximum(&depth_max, depth);
    maximum(&branch_max, branches);
    maximum(&argument_max, arguments);
    maximum(&builtin_max, builtins);
    if (!returning) add(&requests);
    if (ready && depth > 0) {
        if (phase < 0 || phase >= 18) abort();
        add(&resumes[phase]);
    }
}
void slim_checking_probe_push(void) { add(&pushes); }
void slim_checking_probe_finish(void) { add(&finishes); }
void slim_checking_probe_builtin(int64_t operation, int64_t phase) {
    if (operation < 1 || operation > 12 || phase < 0 || phase >= 6) abort();
    add(&builtin_phases[operation][phase]);
}
void slim_checking_probe_scalar(int64_t phase) {
    if (phase < 13 || phase > 17) abort();
    slim_checking_probe_builtin(phase == 15 || phase == 17 ? 2 : 1,
                               phase >= 16 ? 1 : 0);
}
static void row(FILE *out, const char *name, uint64_t value) {
    fprintf(out, "%s\t%" PRIu64 "\n", name, value);
}
static void report(void) {
    const char *path = getenv("SLIM_CHECKING_REPORT");
    if (path == NULL) return;
    FILE *out = fopen(path, "wb");
    if (out == NULL) return;
    fprintf(out, "slim-checking\t2\t%s\t%" PRIu64 "\n",
            saturated ? "bounded" : "exact", CHECKING_CAP);
    row(out, "inference_entries", entries[0]);
    row(out, "inference_ends", ends[0]);
    row(out, "inference_invalid", invalid[0]);
    row(out, "inference_failed", failures[0]);
    row(out, "machine_entries", entries[1]);
    row(out, "machine_ends", ends[1]);
    row(out, "machine_invalid", invalid[1]);
    row(out, "machine_failed", failures[1]);
    row(out, "machine_active_at_exit", active);
    row(out, "machine_native_depth", native_depth);
    row(out, "steps", steps);
    row(out, "requests", requests);
    row(out, "pushes", pushes);
    row(out, "finishes", finishes);
    row(out, "pending_depth", depth_max);
    row(out, "branch_depth", branch_max);
    row(out, "argument_depth", argument_max);
    row(out, "builtin_depth", builtin_max);
    for (int i = 0; i < 18; ++i) fprintf(out, "resume_%d\t%" PRIu64 "\n", i, resumes[i]);
    for (int op = 1; op <= 12; ++op)
        for (int phase = 0; phase < 6; ++phase)
            fprintf(out, "builtin_%d_%d\t%" PRIu64 "\n", op, phase, builtin_phases[op][phase]);
    int error = ferror(out);
    if (fclose(out) != 0) error = 1;
    if (error) remove(path);
}
void slim_checking_probe_init(void) { if (atexit(report) != 0) abort(); }
