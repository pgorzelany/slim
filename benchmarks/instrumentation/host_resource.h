#ifndef SLIM_HOST_RESOURCE_H
#define SLIM_HOST_RESOURCE_H
#include <stddef.h>
#include <stdint.h>
void host_resource_init(void);
void host_resource_attempt(size_t size);
void host_resource_add(size_t size, size_t header);
void host_resource_free(size_t size, size_t header);
void host_resource_copy(size_t size);
void host_resource_host_attempt(size_t size);
void host_resource_resize(size_t before, size_t after);
void host_resource_snapshot(unsigned event, int64_t epoch, int64_t serial);
#endif
