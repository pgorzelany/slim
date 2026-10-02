#ifndef SLIM_HOST_H
#define SLIM_HOST_H
/* Private RFC-0157 component; not linked into current generated programs. */
#include "slim_pool.h"
typedef struct { const uint8_t *data; int64_t len; } SlimHostBytes;
typedef struct { uint8_t *data; int64_t len, capacity; } SlimHostBuffer;
typedef struct {
    SlimPool *domain;
    SlimPoolBlock storage;
    const SlimHostBytes *values;
    int64_t count;
    bool prepared;
} SlimHostArguments;

SlimPoolResult slim_host_arguments(SlimPool *domain, int argc, char *const *argv,
                                   SlimHostArguments *output);
bool slim_host_arguments_release(SlimHostArguments *arguments);
bool slim_host_read_file(SlimHostBytes path, int64_t limit, SlimHostBuffer *output);
bool slim_host_tcp_exchange(SlimHostBytes address, int64_t port, SlimHostBytes request,
                            int64_t limit, int64_t timeout_ms, SlimHostBuffer *output);
bool slim_host_write(int descriptor, SlimHostBytes bytes);
bool slim_host_write_i64(int descriptor, int64_t value);
bool slim_host_write_line(int descriptor, SlimHostBytes bytes);
_Noreturn void slim_host_trap(SlimHostBytes message);
#endif
