#ifndef SLIM_PARSE_PROBE_H
#define SLIM_PARSE_PROBE_H
#include <stdbool.h>
#include <stdint.h>
void slim_parse_probe_init(void);
void slim_parse_probe_begin(void);
void slim_parse_probe_end(void);
void slim_parse_probe_item(int64_t index);
void slim_parse_probe_item_end(bool valid, int64_t next);
void slim_parse_probe_read(int64_t index);
void slim_parse_probe_import(void);
#endif
