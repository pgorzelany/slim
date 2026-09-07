/* Measurement only. No observed value participates in compiler acceptance.
 * Schema 7: program lexings, function checks, C generations, declaration grammar
 * executions, parsed canonical-node imports, memory-plan constructions and
 * memory-plan imports, then fresh range analyses, retained range queries, range
 * function production, range imports, range passes, parameter scans and parallel
 * analyses, full fact-vector initializations and complete output-vector resets,
 * then prototype/body/wrapper producers, fragment imports and imported C bytes,
 * then counted-record cursor lookups and retained parameter-input transfers,
 * in that order. Parameter scans continue to count only actual ordinary scans. */
#include "session_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#define SESSION_PHASES 256
#define SESSION_CAP UINT64_C(1000000000)
static uint64_t counts[SESSION_PHASES][23];
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
    if (kind >= 23) abort();
    if (!active || phases >= SESSION_PHASES) return;
    if (counts[phases][kind] == SESSION_CAP) bounded = 1;
    else ++counts[phases][kind];
}
void slim_session_probe_import(int64_t start, int64_t end) {
    if (start < 0 || end < start || end > INT64_C(67108864)) abort();
    slim_session_probe_count(19);
    if (!active || phases >= SESSION_PHASES) return;
    uint64_t bytes = (uint64_t)(end - start);
    if (bytes > SESSION_CAP - counts[phases][20]) {
        bounded = 1;
        counts[phases][20] = SESSION_CAP;
    } else counts[phases][20] += bytes;
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
/* The public adapter physically owns a root, rather than a SLIM child region. */
void slim_session_probe_host_begin(SlimRegion *root) {
    if (epoch_active || root == NULL || root->parent != NULL || root->newest != NULL) abort();
    epoch_active = 1;
    epoch_parent = root;
}
void slim_session_probe_host_end(SlimRegion *root) {
    if (!epoch_active || root != epoch_parent || root->parent != NULL || root->newest != NULL) abort();
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
    fprintf(output, "slim-session\t7\t%s\t%" PRIu64 "\t%u\n", bounded || active || epoch_active ? "bounded" : "exact", SESSION_CAP, epochs);
    for (unsigned i = 0; i < phases; ++i) {
        fprintf(output, "%u", i);
        for (unsigned j = 0; j < 23; ++j)
            fprintf(output, "\t%" PRIu64, counts[i][j]);
        fputc('\n', output);
    }
    int error = ferror(output);
    if (fclose(output) != 0) error = 1;
    if (error) remove(path);
}
void slim_session_probe_init(void) { if (atexit(report) != 0) abort(); }
