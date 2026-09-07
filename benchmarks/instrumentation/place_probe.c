/* Native observation only; no counter participates in place resolution. */
#include "place_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#define PLACE_PHASES 256
#define PLACE_CAP UINT64_C(1000000000)
static uint64_t visits[PLACE_PHASES];
static unsigned phases;
static int active, bounded;
void slim_place_probe_begin(void) {
    if (active) abort();
    active = 1;
    if (phases >= PLACE_PHASES) bounded = 1;
}
void slim_place_probe_visit(void) {
    if (!active) abort();
    if (phases >= PLACE_PHASES) return;
    if (visits[phases] == PLACE_CAP) bounded = 1;
    else ++visits[phases];
}
void slim_place_probe_end(void) {
    if (!active) abort();
    active = 0;
    if (phases < PLACE_PHASES) ++phases;
}
static void report(void) {
    const char *path = getenv("SLIM_PLACE_REPORT");
    if (path == NULL) return;
    FILE *output = fopen(path, "wb");
    if (output == NULL) return;
    fprintf(output, "slim-place\t1\t%s\t%" PRIu64 "\n", active || bounded ? "bounded" : "exact", PLACE_CAP);
    for (unsigned i = 0; i < phases; ++i) fprintf(output, "%u\t%" PRIu64 "\n", i, visits[i]);
    int error = ferror(output);
    if (fclose(output) != 0) error = 1;
    if (error) remove(path);
}
void slim_place_probe_init(void) { if (atexit(report) != 0) abort(); }
