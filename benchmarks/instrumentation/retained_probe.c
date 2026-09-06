/* Measurement only; never installed in the compiler/runtime. */
#include "retained_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define RETAINED_CAP UINT64_C(1000000000)
#define RETAINED_PHASES 64
static uint64_t counts[RETAINED_PHASES];
static unsigned phases;
static int active, bounded;
void slim_retained_probe_begin(void) {
    if (active) abort();
    active = 1;
    if (phases >= RETAINED_PHASES) bounded = 1;
}
void slim_retained_probe_check(void) {
    if (!active || phases >= RETAINED_PHASES) return;
    if (counts[phases] == RETAINED_CAP) bounded = 1;
    else ++counts[phases];
}
void slim_retained_probe_end(void) {
    if (!active) abort();
    active = 0;
    if (phases < RETAINED_PHASES) ++phases;
}
static void report(void) {
    const char *path = getenv("SLIM_RETAINED_REPORT");
    if (path == NULL) return;
    FILE *output = fopen(path, "wb");
    if (output == NULL) return;
    fprintf(output, "slim-retained\t1\t%s\t%" PRIu64 "\n", bounded || active ? "bounded" : "exact", RETAINED_CAP);
    for (unsigned i = 0; i < phases; ++i) fprintf(output, "%u\t%" PRIu64 "\n", i, counts[i]);
    int error = ferror(output);
    if (fclose(output) != 0) error = 1;
    if (error) remove(path);
}
void slim_retained_probe_init(void) { if (atexit(report) != 0) abort(); }
