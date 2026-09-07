#ifndef SLIM_CHECKING_PROBE_H
#define SLIM_CHECKING_PROBE_H
#include <stdint.h>
void slim_checking_probe_init(void);
void slim_checking_probe_enter(int machine);
void slim_checking_probe_end(int machine, int64_t kind, int failed);
void slim_checking_probe_step(int returning, int ready, int64_t depth, int64_t limit,
                             int64_t phase, int64_t branches,
                             int64_t arguments, int64_t builtins);
void slim_checking_probe_push(void);
void slim_checking_probe_finish(void);
void slim_checking_probe_builtin(int64_t operation, int64_t phase);
void slim_checking_probe_scalar(int64_t phase);
#endif
