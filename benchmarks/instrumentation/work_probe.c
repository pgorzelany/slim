/* Measurement-only support. Never linked into the installed compiler/runtime. */
#include "work_probe.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

_Static_assert(SLIM_WORK_LIMIT > 0 && SLIM_WORK_LIMIT <= UINT64_C(1000000000),
               "work counter cap must be in 1..1000000000");
static uint64_t slim_work_values[SLIM_WORK_COUNT];
static unsigned char slim_work_bounded[SLIM_WORK_COUNT];

void slim_work_add(unsigned index, uint64_t amount) {
    if (index >= SLIM_WORK_COUNT) abort();
    if (amount > SLIM_WORK_LIMIT - slim_work_values[index]) {
        slim_work_values[index] = SLIM_WORK_LIMIT;
        slim_work_bounded[index] = 1;
    } else {
        slim_work_values[index] += amount;
    }
}

static void slim_work_report(void) {
    const char *path = getenv("SLIM_WORK_REPORT");
    if (path == NULL) return;
    FILE *output = fopen(path, "wb");
    if (output == NULL) return;
    fprintf(output, "slim-work\t1\t%" PRIu64 "\n", SLIM_WORK_LIMIT);
    for (unsigned index = 0; index < SLIM_WORK_COUNT; ++index) {
        fprintf(output, "%s\t%s\t%" PRIu64 "\n", slim_work_names[index],
                slim_work_bounded[index] ? "bounded" : "exact",
                slim_work_values[index]);
    }
    fputs("end\n", output);
    int failed = ferror(output);
    if (fclose(output) != 0) failed = 1;
    if (failed) remove(path);
}

void slim_work_init(void) {
    if (atexit(slim_work_report) != 0) abort();
}
