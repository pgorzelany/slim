#include "slim_rt.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct SlimAllocation {
    struct SlimAllocation *next;
    struct SlimAllocation *previous;
    SlimRegion *region;
    size_t size;
    max_align_t alignment;
    uint8_t data[];
};

static _Thread_local SlimRegion *slim_root_region = NULL;
static _Thread_local SlimRegion *slim_active_region = NULL;
static _Thread_local int64_t slim_last_monotonic_ms = 0;

#if defined(SLIM_PARALLEL)
static _Thread_local bool slim_task_worker = false;
static uint64_t slim_task_spawn_attempts = 0;
static uint64_t slim_task_spawn_fail_at = 0;
static uint64_t slim_task_join_attempts = 0;
static uint64_t slim_task_join_fail_at = 0;
static bool slim_task_disabled = false;

static uint64_t slim_positive_environment_ordinal(const char *name) {
    const char *setting = getenv(name);
    if (setting == NULL || setting[0] == 0) {
        return 0;
    }
    errno = 0;
    char *end = NULL;
    unsigned long long ordinal = strtoull(setting, &end, 10);
    if (errno != 0 || end == setting || *end != 0 || ordinal == 0) {
        return 0;
    }
    return (uint64_t)ordinal;
}

#if defined(SLIM_POSIX_WORKERS)
static void *slim_task_entry(void *opaque) {
    SlimTask *task = opaque;
    slim_task_worker = true;
    task->function(task->context);
    slim_task_worker = false;
    return NULL;
}
#endif
#endif

void slim_region_init(SlimRegion *region, SlimRegion *parent) {
    if (region == NULL) {
        slim_rt_trap("cannot initialize a null region");
    }
    region->newest = NULL;
    region->parent = parent;
    region->status = parent == NULL ? NULL : parent->status;
    slim_active_region = region;
}

void slim_region_destroy(SlimRegion *region) {
    if (region == NULL) {
        return;
    }
    SlimAllocation *allocation = region->newest;
    region->newest = NULL;
    while (allocation != NULL) {
        SlimAllocation *next = allocation->next;
        free(allocation);
        allocation = next;
    }
    if (slim_active_region == region) {
        slim_active_region = region->parent;
    }
}

void slim_alloc_status_init(SlimAllocStatus *status) {
    *status = (SlimAllocStatus){.code = SLIM_ALLOC_OK};
    const char *setting = getenv("SLIM_ALLOC_FAIL_AT");
    if (setting == NULL || setting[0] == 0) {
        return;
    }
    errno = 0;
    char *end = NULL;
    unsigned long long ordinal = strtoull(setting, &end, 10);
    if (errno == 0 && end != setting && *end == 0 && ordinal > 0) {
        status->fail_at = (uint64_t)ordinal;
    }
}

void slim_alloc_report(const SlimAllocStatus *status) {
    fprintf(stderr,
            "SLIM allocation failure: exhausted at allocation %" PRIu64 "\n",
            status->failure_at);
}

void slim_rt_init(SlimRegion *root, SlimAllocStatus *status) {
    if (slim_root_region != NULL) {
        slim_rt_trap("runtime initialized twice");
    }
    slim_region_init(root, NULL);
    root->status = status;
    slim_root_region = root;
#if defined(SLIM_PARALLEL)
    slim_task_spawn_attempts = 0;
    slim_task_spawn_fail_at = slim_positive_environment_ordinal("SLIM_TASK_FAIL_AT");
    slim_task_join_attempts = 0;
    slim_task_join_fail_at = slim_positive_environment_ordinal("SLIM_TASK_JOIN_FAIL_AT");
    slim_task_disabled = slim_positive_environment_ordinal("SLIM_TASK_DISABLE") != 0;
#endif
}

void slim_rt_shutdown(void) {
    if (slim_root_region != NULL) {
        SlimRegion *root = slim_root_region;
        slim_root_region = NULL;
        slim_region_destroy(root);
    }
}

_Noreturn void slim_rt_trap(const char *message) {
    fprintf(stderr, "SLIM runtime trap: %s\n", message);
    while (slim_active_region != NULL) {
        slim_region_destroy(slim_active_region);
    }
    slim_root_region = NULL;
    exit(70);
}

#if defined(SLIM_PARALLEL)
bool slim_task_spawn(SlimTask *task, SlimTaskFn function, void *context) {
    if (task == NULL || function == NULL) {
        slim_rt_trap("invalid structured task");
    }
    *task = (SlimTask){
        .function = function,
        .context = context,
        .active = false,
    };
    if (slim_task_disabled || slim_task_worker) {
        return false;
    }
    slim_task_spawn_attempts += 1;
    if (slim_task_spawn_attempts == 0 ||
        slim_task_spawn_attempts == slim_task_spawn_fail_at) {
        return false;
    }
#if defined(SLIM_POSIX_WORKERS)
    int created = pthread_create(&task->worker, NULL, slim_task_entry, task);
    if (created != 0) {
        return false;
    }
    task->active = true;
    return true;
#else
    return false;
#endif
}

void slim_task_run_inline(SlimTaskFn function, void *context) {
    if (function == NULL) {
        slim_rt_trap("invalid inline structured task");
    }
    bool prior_worker = slim_task_worker;
    slim_task_worker = true;
    function(context);
    slim_task_worker = prior_worker;
}

void slim_task_join(SlimTask *task) {
    if (task == NULL || !task->active) {
        slim_rt_trap("invalid structured task join");
    }
#if defined(SLIM_POSIX_WORKERS)
    int joined = pthread_join(task->worker, NULL);
    task->active = false;
    if (joined != 0) {
        slim_rt_trap("structured task join failed");
    }
    slim_task_join_attempts += 1;
    if (slim_task_join_attempts == 0 ||
        slim_task_join_attempts == slim_task_join_fail_at) {
        slim_rt_trap("injected structured task join failure");
    }
#else
    slim_rt_trap("structured task joined without a worker backend");
#endif
}
#endif

static SlimAllocation *slim_allocation_from_data(void *pointer) {
    return (SlimAllocation *)((uint8_t *)pointer - offsetof(SlimAllocation, data));
}

void *slim_rt_alloc(SlimRegion *region, size_t size) {
    if (region == NULL) {
        slim_rt_trap("allocation requires a region");
    }
    SlimAllocStatus *status = region->status;
    if (status == NULL) {
        slim_rt_trap("allocation region has no status");
    }
    if (status->code != SLIM_ALLOC_OK) {
        return NULL;
    }
    status->attempts += 1;
    if (status->attempts == 0 || status->attempts == status->fail_at) {
        status->code = SLIM_ALLOC_EXHAUSTED;
        status->failure_at = status->attempts;
        return NULL;
    }
    if (size == 0) {
        size = 1;
    }
    if (size > SIZE_MAX - offsetof(SlimAllocation, data)) {
        slim_rt_trap("allocation size overflow");
    }
    SlimAllocation *allocation = calloc(1, offsetof(SlimAllocation, data) + size);
    if (allocation == NULL) {
        status->code = SLIM_ALLOC_EXHAUSTED;
        status->failure_at = status->attempts;
        return NULL;
    }
    allocation->next = region->newest;
    allocation->previous = NULL;
    allocation->region = region;
    allocation->size = size;
    if (region->newest != NULL) {
        region->newest->previous = allocation;
    }
    region->newest = allocation;
    return allocation->data;
}

void *slim_rt_realloc(SlimRegion *region, void *pointer, size_t old_size, size_t new_size) {
    if (pointer == NULL) {
        return slim_rt_alloc(region, new_size);
    }
    SlimAllocation *allocation = slim_allocation_from_data(pointer);
    if (allocation->region != region) {
        slim_rt_trap("attempted to resize memory through the wrong region");
    }
    if (old_size > allocation->size) {
        slim_rt_trap("attempted to resize memory with an invalid old size");
    }
    void *new_pointer = slim_rt_alloc(region, new_size);
    if (new_pointer == NULL) {
        return NULL;
    }
    size_t copied = old_size < new_size ? old_size : new_size;
    if (copied > 0) {
        memcpy(new_pointer, pointer, copied);
    }
    if (allocation->previous != NULL) {
        allocation->previous->next = allocation->next;
    } else {
        region->newest = allocation->next;
    }
    if (allocation->next != NULL) {
        allocation->next->previous = allocation->previous;
    }
    free(allocation);
    return new_pointer;
}

bool slim_read_file(SlimBytes path, SlimVec *output) {
    if (output->element_size != sizeof(uint8_t)) {
        slim_rt_trap("io.read-file requires a U8 vector");
    }
    if (memchr(path.data, 0, (size_t)path.len) != NULL) {
        return false;
    }
    SlimRegion scratch;
    slim_region_init(&scratch, output->region);
    char *path_string = slim_rt_alloc(&scratch, (size_t)path.len + 1);
    if (path_string == NULL) {
        slim_region_destroy(&scratch);
        return false;
    }
    memcpy(path_string, path.data, (size_t)path.len);
    path_string[path.len] = 0;
    FILE *file = fopen(path_string, "rb");
    slim_region_destroy(&scratch);
    if (file == NULL) {
        return false;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return false;
    }
    long length = ftell(file);
    if (length < 0 || (uintmax_t)length > (uintmax_t)(INT64_MAX - output->len)) {
        fclose(file);
        return false;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return false;
    }
    int64_t required = output->len + (int64_t)length;
    if (required > output->capacity) {
        size_t old_size = (size_t)output->capacity;
        size_t new_size = (size_t)required;
        uint8_t *resized = slim_rt_realloc(output->region, output->data, old_size, new_size);
        if (resized == NULL) {
            fclose(file);
            return false;
        }
        output->data = resized;
        output->capacity = required;
    }
    size_t read = fread(output->data + output->len, 1, (size_t)length, file);
    if (read != (size_t)length || ferror(file)) {
        fclose(file);
        return false;
    }
    if (fclose(file) != 0) {
        return false;
    }
    output->len = required;
    return true;
}

int64_t slim_monotonic_ms(void) {
    struct timespec now = {0, 0};
#if defined(CLOCK_MONOTONIC)
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        return slim_last_monotonic_ms;
    }
#else
    if (timespec_get(&now, TIME_UTC) != TIME_UTC) {
        return slim_last_monotonic_ms;
    }
#endif
    if (now.tv_sec < 0) {
        return slim_last_monotonic_ms;
    }
    uint64_t seconds = (uint64_t)now.tv_sec;
    if (seconds > (uint64_t)INT64_MAX / 1000) {
        slim_last_monotonic_ms = INT64_MAX;
        return slim_last_monotonic_ms;
    }
    int64_t milliseconds = (int64_t)(seconds * 1000);
    if (now.tv_nsec > 0) {
        milliseconds += (int64_t)((uint64_t)now.tv_nsec / 1000000);
    }
    if (milliseconds > slim_last_monotonic_ms) {
        slim_last_monotonic_ms = milliseconds;
    }
    return slim_last_monotonic_ms;
}

SlimUnit slim_print_i64(int64_t value) {
    if (printf("%" PRId64, value) < 0) {
        slim_rt_trap("cannot write output");
    }
    return (SlimUnit){0};
}

SlimUnit slim_print_bytes(SlimBytes value) {
    if (value.len > 0 && fwrite(value.data, 1, (size_t)value.len, stdout) != (size_t)value.len) {
        slim_rt_trap("cannot write output");
    }
    return (SlimUnit){0};
}

SlimUnit slim_println(SlimBytes value) {
    slim_print_bytes(value);
    if (fputc('\n', stdout) == EOF) {
        slim_rt_trap("cannot write output");
    }
    return (SlimUnit){0};
}
