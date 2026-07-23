#include "slim_rt.h"

#include <inttypes.h>
#include <stdio.h>

typedef struct {
    int64_t value;
    bool nested_spawned;
} Probe;

static void increment(void *opaque) {
    Probe *probe = opaque;
    probe->value += 1;
}

static void outer(void *opaque) {
    Probe *probe = opaque;
    SlimTask nested;
    probe->nested_spawned = slim_task_spawn(&nested, increment, probe);
    if (probe->nested_spawned) {
        slim_task_join(&nested);
    } else {
        slim_task_run_inline(increment, probe);
    }
}

int main(void) {
    SlimAllocStatus status;
    slim_alloc_status_init(&status);
    SlimRegion root;
    slim_rt_init(&root, &status);

    Probe probe = {.value = 41, .nested_spawned = false};
    SlimTask task;
    bool spawned = slim_task_spawn(&task, outer, &probe);
    if (spawned) {
        slim_task_join(&task);
    } else {
        slim_task_run_inline(outer, &probe);
    }

    printf("%d %d %" PRId64 "\n", spawned ? 1 : 0, probe.nested_spawned ? 1 : 0, probe.value);
    slim_rt_shutdown();
    return 0;
}
