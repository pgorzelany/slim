#ifndef SLIM_SESSION_PROBE_H
#define SLIM_SESSION_PROBE_H
#include "slim_rt.h"
void slim_session_probe_init(void);
void slim_session_probe_begin(void);
void slim_session_probe_captured(void);
void slim_session_probe_count(unsigned kind);
void slim_session_probe_end(void);
void slim_session_probe_epoch_begin(SlimRegion *parent);
void slim_session_probe_epoch_end(SlimRegion *parent, SlimRegion *child);
#endif
