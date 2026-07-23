#include "slim_rt.h"

#include <stdint.h>
#include <stdio.h>

int main(void) {
    SlimAllocStatus status;
    slim_alloc_status_init(&status);
    SlimRegion root;
    slim_rt_init(&root, &status);
    SlimRegion child;
    slim_region_init(&child, &root);

    uint8_t *bytes = slim_rt_alloc(&child, 4);
    if (bytes == NULL) {
        return 1;
    }
    bytes[0] = 42;
    bytes[3] = 7;
    slim_region_adopt(&root, &child);
    slim_region_destroy(&child);

    bytes = slim_rt_realloc(&root, bytes, 4, 8);
    if (bytes == NULL || bytes[0] != 42 || bytes[3] != 7 ||
        slim_region_failed(&root)) {
        return 2;
    }
    puts("OK");
    slim_rt_shutdown();
    return 0;
}
