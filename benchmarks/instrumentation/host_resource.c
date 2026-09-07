/* Measurement only: requested payload/header storage excludes libc bookkeeping.
 * Runtime realloc copies exclude other source-level or transport copying. */
#include "host_resource.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#define HOST_RESOURCE_ROWS 256
#define HOST_RESOURCE_FIELDS 11
static uint64_t values[HOST_RESOURCE_FIELDS];
static uint64_t snapshots[HOST_RESOURCE_ROWS][HOST_RESOURCE_FIELDS];
static int64_t identities[HOST_RESOURCE_ROWS][3];
static unsigned rows, bounded;
static void add(unsigned field, uint64_t value) {
    if (value > UINT64_MAX - values[field]) abort();
    values[field] += value;
}
static void subtract(unsigned field, uint64_t value) {
    if (value > values[field]) abort();
    values[field] -= value;
}
void host_resource_attempt(size_t size) { add(0, 1); add(1, size); }
void host_resource_add(size_t size, size_t header) {
    add(2, size); add(3, size); add(3, header);
    if (values[2] > values[4]) values[4] = values[2];
    if (values[3] > values[5]) values[5] = values[3];
}
void host_resource_free(size_t size, size_t header) {
    subtract(2, size); subtract(3, size); subtract(3, header);
}
void host_resource_copy(size_t size) { add(6, size); }
void host_resource_host_attempt(size_t size) { add(7, 1); add(8, size); }
void host_resource_resize(size_t before, size_t after) {
    subtract(9, before); add(9, after);
    if (values[9] > values[10]) values[10] = values[9];
}
void host_resource_snapshot(unsigned event, int64_t epoch, int64_t serial) {
    if (event > 2 || epoch < 0 || serial < 0) abort();
    /* Both reset and final shutdown must physically free every observed owner. */
    if (event != 0 && (values[2] || values[3] || values[9])) abort();
    if (rows == HOST_RESOURCE_ROWS) { bounded = 1; return; }
    identities[rows][0] = event;
    identities[rows][1] = epoch;
    identities[rows][2] = serial;
    for (unsigned field = 0; field < HOST_RESOURCE_FIELDS; ++field)
        snapshots[rows][field] = values[field];
    ++rows;
}
static void report(void) {
    if (values[2] || values[3] || values[9]) abort();
    const char *path = getenv("SLIM_HOST_RESOURCE_REPORT");
    if (!path) return;
    FILE *out = fopen(path, "wb");
    if (!out) abort();
    fprintf(out, "slim-host-resources\t1\t%s\t256\n", bounded ? "bounded" : "exact");
    fprintf(out, "event\tepoch\tserial\truntime_attempts\trequested_payload\tlive_payload\tlive_with_headers\tpeak_payload\tpeak_with_headers\truntime_realloc_copies\thost_attempts\thost_requested\thost_live\thost_peak\n");
    for (unsigned row = 0; row < rows; ++row) {
        fprintf(out, "%" PRId64 "\t%" PRId64 "\t%" PRId64, identities[row][0], identities[row][1], identities[row][2]);
        for (unsigned field = 0; field < HOST_RESOURCE_FIELDS; ++field)
            fprintf(out, "\t%" PRIu64, snapshots[row][field]);
        fputc('\n', out);
    }
    int error = ferror(out);
    if (fclose(out) != 0) error = 1;
    if (error) abort();
}
void host_resource_init(void) { if (atexit(report) != 0) abort(); }
