/* Observation only; counters and the read-boundary assertion do not accept nodes. */
#include "parse_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#define PARSE_PHASES 256
#define PARSE_CAP UINT64_C(1000000000)
static uint64_t grammar[PARSE_PHASES], imports[PARSE_PHASES], successful;
static unsigned phases;
static int active, item_active, bounded;
static int64_t first_read, last_read;
static void increment(uint64_t *value) {
    if (*value < PARSE_CAP) ++*value;
    else bounded = 1;
}
void slim_parse_probe_begin(void) {
    if (active) abort();
    active = 1;
    if (phases >= PARSE_PHASES) bounded = 1;
}
void slim_parse_probe_end(void) {
    if (!active || item_active) abort();
    active = 0;
    if (phases < PARSE_PHASES) ++phases;
}
void slim_parse_probe_item(int64_t index) {
    if (item_active) abort();
    item_active = 1;
    first_read = last_read = index;
    if (active && phases < PARSE_PHASES) increment(&grammar[phases]);
}
void slim_parse_probe_read(int64_t index) {
    if (item_active) {
        if (index < first_read) abort();
        if (index > last_read) last_read = index;
    }
}
void slim_parse_probe_item_end(bool valid, int64_t next) {
    if (!item_active) abort();
    if (valid) {
        if (last_read > next) abort();
        increment(&successful);
    }
    item_active = 0;
}
void slim_parse_probe_import(void) {
    if (!active) abort();
    if (phases < PARSE_PHASES) increment(&imports[phases]);
}
static void report(void) {
    const char *path = getenv("SLIM_PARSE_REPORT");
    if (path == NULL) return;
    FILE *out = fopen(path, "wb");
    if (out == NULL) return;
    fprintf(out, "slim-parse\t1\t%s\t%" PRIu64 "\t%" PRIu64 "\n",
            active || item_active || bounded ? "bounded" : "exact", PARSE_CAP, successful);
    for (unsigned i = 0; i < phases; ++i)
        fprintf(out, "%u\t%" PRIu64 "\t%" PRIu64 "\n", i, grammar[i], imports[i]);
    int error = ferror(out);
    if (fclose(out) != 0) error = 1;
    if (error) remove(path);
}
void slim_parse_probe_init(void) { if (atexit(report) != 0) abort(); }
