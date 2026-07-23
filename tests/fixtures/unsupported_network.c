#include "slim_rt.h"

int main(void) {
    SlimAllocStatus status;
    slim_alloc_status_init(&status);
    SlimRegion root;
    slim_rt_init(&root, &status);
    SlimVec output = slim_vec_new(sizeof(uint8_t), &root);
    bool result = slim_tcp_exchange(
        slim_bytes_static((const uint8_t *)"127.0.0.1", 9),
        1,
        slim_bytes_static((const uint8_t *)"", 0),
        0,
        1,
        &output
    );
    bool valid = !result && output.len == 0 && !slim_region_failed(&root);
    slim_rt_shutdown();
    return valid ? 0 : 1;
}
