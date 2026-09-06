/* Measurement-only hooks for the production flow probe. */
#include <stdint.h>
void slim_flow_probe_init(void);
void slim_flow_probe_enter(void);
void slim_flow_probe_walk(void);
void slim_flow_probe_step(void);
void slim_flow_probe_end(int32_t status, int64_t steps, int failed);
