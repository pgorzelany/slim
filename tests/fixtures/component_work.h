#ifndef SLIM_COMPONENT_WORK_H
#define SLIM_COMPONENT_WORK_H

/* Test-only observation of direct operations in production-emitted C.
 * Compile the runtime separately without this header. No production ABI or
 * source semantics change; instrumented stdout must match the independent
 * oracle before these counters are accepted. This is not native timing. */
#include "slim_rt.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define COMPONENT_WORK_CAP UINT64_C(10000000000)
static uint64_t component_bytes_get;
static uint64_t component_vector_access;
static uint64_t component_vector_push;

static inline void component_work_increment(uint64_t *counter) {
    if (*counter >= COMPONENT_WORK_CAP) {
        fputs("component-work-v2 unknown counter-saturated\n", stderr);
        _Exit(72);
    }
    *counter += 1;
}

static inline uint8_t component_observe_bytes_get(SlimBytes bytes, int64_t index) {
    component_work_increment(&component_bytes_get);
    return slim_bytes_get(bytes, index);
}

static inline void component_observe_vector_get(const SlimVec *vector, int64_t index, void *output) {
    component_work_increment(&component_vector_access);
    slim_vec_get(vector, index, output);
}

static inline void component_observe_vector_set(SlimVec *vector, int64_t index, const void *value) {
    component_work_increment(&component_vector_access);
    slim_vec_set(vector, index, value);
}

static inline size_t component_observe_vector_index(const SlimVec *vector, int64_t index) {
    component_work_increment(&component_vector_access);
    return slim_vec_check_index(vector, index);
}

static inline bool component_observe_vector_push(SlimVec *vector, const void *value) {
    component_work_increment(&component_vector_push);
    return slim_vec_push(vector, value);
}

static void component_work_report(void) {
    fprintf(stderr, "component-work-v2 bytes_get=%" PRIu64 " vector_access=%" PRIu64
            " vector_push=%" PRIu64 "\n",
            component_bytes_get, component_vector_access, component_vector_push);
}

__attribute__((constructor)) static void component_work_start(void) {
    if (atexit(component_work_report) != 0) {
        fputs("component-work-v2 unknown observer-unavailable\n", stderr);
        _Exit(72);
    }
}

#define slim_bytes_get component_observe_bytes_get
#define slim_vec_check_index component_observe_vector_index
#define slim_vec_get component_observe_vector_get
#define slim_vec_set component_observe_vector_set
#define slim_vec_push component_observe_vector_push
#endif
