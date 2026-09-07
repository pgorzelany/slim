/* Measurement only. No observed value participates in compiler acceptance.
 * Schema 3: program lexings, function checks, C generations, declaration grammar
 * executions, parsed canonical-node imports, memory-plan constructions and
 * memory-plan imports, in that order. */
#include "session_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#define SESSION_PHASES 256
#define SESSION_CAP UINT64_C(1000000000)
static uint64_t counts[SESSION_PHASES][7];
static unsigned phases, epochs;
static int active, bounded, epoch_active;
static SlimRegion *epoch_parent;
static SlimAllocation *parent_allocation;
void slim_session_probe_begin(void) {
    if (active) abort();
    active = 1;
    if (phases >= SESSION_PHASES) bounded = 1;
}
void slim_session_probe_count(unsigned kind) {
    if (kind >= 7) abort();
    if (!active || phases >= SESSION_PHASES) return;
    if (counts[phases][kind] == SESSION_CAP) bounded = 1;
    else ++counts[phases][kind];
}
void slim_session_probe_end(void) {
    if (!active) abort();
    active = 0;
    if (phases < SESSION_PHASES) ++phases;
}
void slim_session_probe_epoch_begin(SlimRegion *parent) {
    if (epoch_active || parent == NULL) abort();
    epoch_active = 1;
    epoch_parent = parent;
    parent_allocation = parent->newest;
}
void slim_session_probe_epoch_end(SlimRegion *parent, SlimRegion *child) {
    if (!epoch_active || parent != epoch_parent || child->parent != parent ||
        child->newest != NULL || parent->newest != parent_allocation) abort();
    epoch_active = 0;
    if (epochs == SESSION_PHASES) bounded = 1;
    else ++epochs;
}
/* Test-only fault: replace a file after its first complete capture. */
void slim_session_probe_captured(void) {
    static unsigned captures;
    if (captures != 0) return;
    ++captures;
    const char *destination = getenv("SLIM_SESSION_CAPTURE_REPLACE");
    const char *source = getenv("SLIM_SESSION_CAPTURE_SOURCE");
    if (destination == NULL && source == NULL) return;
    if (destination == NULL || source == NULL) abort();
    FILE *input = fopen(source, "rb");
    if (input == NULL) abort();
    FILE *output = fopen(destination, "wb");
    if (output == NULL) abort();
    int byte;
    while ((byte = fgetc(input)) != EOF) {
        if (fputc(byte, output) == EOF) abort();
    }
    int error = ferror(input) || ferror(output);
    if (fclose(input) != 0) error = 1;
    if (fclose(output) != 0) error = 1;
    if (error) abort();
}
static void report(void) {
    const char *path = getenv("SLIM_SESSION_REPORT");
    if (path == NULL) return;
    FILE *output = fopen(path, "wb");
    if (output == NULL) return;
    fprintf(output, "slim-session\t3\t%s\t%" PRIu64 "\t%u\n", bounded || active || epoch_active ? "bounded" : "exact", SESSION_CAP, epochs);
    for (unsigned i = 0; i < phases; ++i)
        fprintf(output, "%u\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\t%" PRIu64 "\n", i, counts[i][0], counts[i][1], counts[i][2], counts[i][3], counts[i][4], counts[i][5], counts[i][6]);
    int error = ferror(output);
    if (fclose(output) != 0) error = 1;
    if (error) remove(path);
}
void slim_session_probe_init(void) { if (atexit(report) != 0) abort(); }
