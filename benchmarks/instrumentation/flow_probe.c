/* Never linked into the installed compiler or runtime. */
#include "flow_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define FLOW_CAP UINT64_C(1000000000)
static uint64_t calls, walks, headers, steps, complete, bounded, invalid, failed;
static int saturated;
static void add(uint64_t *value, uint64_t amount) {
    if (amount > FLOW_CAP - *value) { *value = FLOW_CAP; saturated = 1; }
    else { *value += amount; }
}
void slim_flow_probe_enter(void) { add(&calls, 1); }
void slim_flow_probe_walk(void) { add(&walks, 1); }
void slim_flow_probe_step(void) { add(&headers, 1); }
void slim_flow_probe_end(int32_t status, int64_t count, int failure) {
    if (failure) { add(&failed, 1); return; }
    if (count < 0 || count > 1000000) abort();
    add(&steps, (uint64_t)count);
    switch (status) {
        case 0: add(&complete, 1); break;
        case 1: add(&bounded, 1); break;
        case 2: case 3: add(&invalid, 1); break;
        default: abort();
    }
}
static void report(void) {
    const char *path = getenv("SLIM_FLOW_REPORT");
    if (path == NULL) return;
    FILE *output = fopen(path, "wb");
    if (output == NULL) return;
    fprintf(output, "slim-flow\t1\t%s\t%" PRIu64 "\n", saturated ? "bounded" : "exact", FLOW_CAP);
    fprintf(output, "%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\n",
            calls, walks, headers, steps, complete, bounded, invalid, failed);
    int error = ferror(output);
    if (fclose(output) != 0) error = 1;
    if (error) remove(path);
}
void slim_flow_probe_init(void) { if (atexit(report) != 0) abort(); }
