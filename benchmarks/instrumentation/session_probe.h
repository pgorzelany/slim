#ifndef SLIM_SESSION_PROBE_H
#define SLIM_SESSION_PROBE_H
#include "slim_rt.h"
#include <stdlib.h>
void slim_session_probe_init(void);
void slim_session_probe_begin(void);
void slim_session_probe_captured(void);
void slim_session_probe_count(unsigned kind);
void slim_session_probe_import(int64_t start, int64_t end);
void slim_session_probe_end(void);
void slim_session_probe_epoch_begin(SlimRegion *parent);
void slim_session_probe_epoch_end(SlimRegion *parent, SlimRegion *child);
#endif
