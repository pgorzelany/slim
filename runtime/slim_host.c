#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#if defined(__APPLE__) && !defined(_DARWIN_C_SOURCE)
#define _DARWIN_C_SOURCE 1
#endif
#include "slim_host.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if defined(SLIM_HOST_POSIX)
#if !defined(__APPLE__) && !defined(__linux__)
#error "SLIM_HOST_POSIX requires an audited Darwin/Linux descriptor provider"
#endif
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
/* Reuse the accepted clock service, not another host clock implementation. */
extern int64_t slim_monotonic_ms(void);
#endif

static bool extent(int64_t size) {
    return size >= 0 && (uint64_t)size <= SIZE_MAX && (uint64_t)size <= PTRDIFF_MAX;
}
static bool bytes_valid(SlimHostBytes bytes) {
    return extent(bytes.len) && (bytes.len == 0 || bytes.data != NULL);
}
static bool buffer_valid(SlimHostBuffer *buffer, int64_t limit) {
    return buffer && extent(buffer->capacity) && buffer->len >= 0 &&
           buffer->len <= buffer->capacity && limit >= 0 &&
           limit <= buffer->capacity - buffer->len &&
           (buffer->capacity == 0 || buffer->data != NULL);
}

SlimPoolResult slim_host_arguments(SlimPool *domain, int argc, char *const *argv,
                                   SlimHostArguments *output) {
    if (!domain || !domain->reservation || !output || output->prepared || argc < 0 ||
        (argc != 0 && !argv)) return SLIM_POOL_INVALID;
    uint64_t count = (uint64_t)argc;
    if (count > (uint64_t)PTRDIFF_MAX / sizeof(SlimHostBytes) ||
        count > SIZE_MAX / sizeof(SlimHostBytes)) return SLIM_POOL_EXHAUSTED;
    for (int i = 0; i < argc; ++i) {
        if (!argv[i]) return SLIM_POOL_INVALID;
        size_t length = strlen(argv[i]);
        if (length > INT64_MAX || length > PTRDIFF_MAX) return SLIM_POOL_INVALID;
    }
    SlimPoolBlock storage = {0};
    SlimPoolResult result = slim_pool_allocate(domain, count * sizeof(SlimHostBytes),
                                                _Alignof(SlimHostBytes), false, &storage);
    if (result != SLIM_POOL_OK) return result;
    SlimHostBytes *values = (SlimHostBytes *)storage.data;
    for (int i = 0; i < argc; ++i) {
        values[i] = (SlimHostBytes){(const uint8_t *)argv[i], (int64_t)strlen(argv[i])};
    }
    *output = (SlimHostArguments){domain, storage, values, argc, true};
    return SLIM_POOL_OK;
}
bool slim_host_arguments_release(SlimHostArguments *arguments) {
    if (!arguments || !arguments->prepared || !arguments->domain) return false;
    if (arguments->storage.data && !slim_pool_release(arguments->domain, arguments->storage)) return false;
    *arguments = (SlimHostArguments){0};
    return true;
}

#if defined(SLIM_HOST_POSIX)
static size_t transfer_size(int64_t remaining) {
    uint64_t size = (uint64_t)remaining;
    if (size > UINT64_C(1048576)) size = UINT64_C(1048576);
    if (size > (uint64_t)SSIZE_MAX) size = (uint64_t)SSIZE_MAX;
    return (size_t)size;
}
/* The audited targets release ordinary private fds before reporting close errors. */
static bool close_once(int descriptor) { return close(descriptor) == 0; }
#endif

bool slim_host_read_file(SlimHostBytes path, int64_t limit, SlimHostBuffer *output) {
    if (!bytes_valid(path) || path.len > 4095 || !buffer_valid(output, limit) ||
        (path.len && memchr(path.data, 0, (size_t)path.len))) return false;
#if defined(SLIM_HOST_POSIX)
    char name[4096];
    if (path.len) memcpy(name, path.data, (size_t)path.len);
    name[path.len] = 0;
    int descriptor;
    do { descriptor = open(name, O_RDONLY); } while (descriptor < 0 && errno == EINTR);
    if (descriptor < 0) return false;
    int64_t received = 0;
    bool complete = false;
    for (;;) {
        uint8_t extra;
        int64_t remaining = limit - received;
        void *target = remaining ? output->data + output->len + received : &extra;
        size_t requested = remaining ? transfer_size(remaining) : 1;
        ssize_t count = read(descriptor, target, requested);
        if (count == 0) { complete = true; break; }
        if (count < 0) { if (errno == EINTR) continue; break; }
        if (!remaining || (uint64_t)count > requested) break;
        received += count;
    }
    bool closed = close_once(descriptor);
    if (!complete || !closed) return false;
    output->len += received;
    return true;
#else
    return false;
#endif
}

#if defined(SLIM_HOST_POSIX)
static bool wait_socket(int descriptor, short events, int64_t deadline) {
    for (;;) {
        int64_t now = slim_monotonic_ms();
        if (now >= deadline) return false;
        int64_t remaining = deadline - now;
        struct pollfd item = {descriptor, events, 0};
        int result = poll(&item, 1, remaining > INT_MAX ? INT_MAX : (int)remaining);
        if (result > 0) return (item.revents & events) != 0 ||
                             (events == POLLIN && (item.revents & POLLHUP) != 0);
        if (result == 0 || errno != EINTR) return false;
    }
}
static bool connect_socket(int descriptor, const struct sockaddr *address,
                           socklen_t length, int64_t deadline) {
    if (connect(descriptor, address, length) == 0) return true;
    if (errno != EINPROGRESS || !wait_socket(descriptor, POLLOUT, deadline)) return false;
    int error = 0; socklen_t size = sizeof(error);
    return getsockopt(descriptor, SOL_SOCKET, SO_ERROR, &error, &size) == 0 && error == 0;
}
static bool send_request(int descriptor, SlimHostBytes request, int64_t deadline) {
    int64_t sent = 0;
    while (sent < request.len) {
        if (!wait_socket(descriptor, POLLOUT, deadline)) return false;
        int flags = 0;
#if defined(MSG_NOSIGNAL)
        flags = MSG_NOSIGNAL;
#endif
        size_t requested = transfer_size(request.len - sent);
        ssize_t count = send(descriptor, request.data + sent, requested, flags);
        if (count > 0) { if ((uint64_t)count > requested) return false; sent += count; }
        else if (!count || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) return false;
    }
    return true;
}
static bool receive_response(int descriptor, int64_t limit, int64_t deadline,
                              SlimHostBuffer *output, int64_t *received) {
    *received = 0;
    for (;;) {
        if (!wait_socket(descriptor, POLLIN, deadline)) return false;
        uint8_t extra;
        int64_t remaining = limit - *received;
        void *target = remaining ? output->data + output->len + *received : &extra;
        size_t requested = remaining ? transfer_size(remaining) : 1;
        ssize_t count = recv(descriptor, target, requested, 0);
        if (!count) return true;
        if (count < 0) { if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) continue; return false; }
        if (!remaining || (uint64_t)count > requested) return false;
        *received += count;
    }
}
#endif

bool slim_host_tcp_exchange(SlimHostBytes address, int64_t port, SlimHostBytes request,
                            int64_t limit, int64_t timeout_ms, SlimHostBuffer *output) {
    if (!bytes_valid(address) || address.len <= 0 || address.len >= 46 ||
        memchr(address.data, 0, (size_t)address.len) || port <= 0 || port > 65535 ||
        !bytes_valid(request) || timeout_ms <= 0 || !buffer_valid(output, limit)) return false;
#if defined(SLIM_HOST_POSIX)
    char name[46]; memcpy(name, address.data, (size_t)address.len); name[address.len] = 0;
    struct sockaddr_storage storage; memset(&storage, 0, sizeof(storage));
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)&storage;
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&storage;
    int family; socklen_t size;
    if (inet_pton(AF_INET, name, &ipv4->sin_addr) == 1) {
        family = AF_INET; ipv4->sin_family = AF_INET; ipv4->sin_port = htons((uint16_t)port); size = sizeof(*ipv4);
    } else if (inet_pton(AF_INET6, name, &ipv6->sin6_addr) == 1) {
        family = AF_INET6; ipv6->sin6_family = AF_INET6; ipv6->sin6_port = htons((uint16_t)port); size = sizeof(*ipv6);
    } else return false;
    int descriptor = socket(family, SOCK_STREAM, 0);
    if (descriptor < 0) return false;
#if !defined(MSG_NOSIGNAL) && defined(SO_NOSIGPIPE)
    int no_sigpipe = 1;
    if (setsockopt(descriptor, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(no_sigpipe))) {
        (void)close_once(descriptor); return false;
    }
#elif !defined(MSG_NOSIGNAL)
#error "native host TCP requires a target SIGPIPE containment mechanism"
#endif
    int flags = fcntl(descriptor, F_GETFL, 0);
    if (flags < 0 || fcntl(descriptor, F_SETFL, flags | O_NONBLOCK) < 0) {
        (void)close_once(descriptor); return false;
    }
    int64_t now = slim_monotonic_ms();
    int64_t deadline = timeout_ms > INT64_MAX - now ? INT64_MAX : now + timeout_ms;
    int64_t received = 0;
    bool complete = connect_socket(descriptor, (const struct sockaddr *)&storage, size, deadline) &&
                    send_request(descriptor, request, deadline) && shutdown(descriptor, SHUT_WR) == 0 &&
                    receive_response(descriptor, limit, deadline, output, &received);
    bool closed = close_once(descriptor);
    if (!complete || !closed) return false;
    output->len += received;
    return true;
#else
    return false;
#endif
}

bool slim_host_write(int descriptor, SlimHostBytes bytes) {
    if (!bytes_valid(bytes)) return false;
#if defined(SLIM_HOST_POSIX)
    if (!bytes.len) return true;
#if defined(__APPLE__)
    /* Darwin pipe errors signal the process: a thread mask is insufficient.
       Admitted output descriptors retain suppression; never race a restoration. */
    if (fcntl(descriptor, F_SETNOSIGPIPE, 1) < 0) return false;
#else
    sigset_t blocked, previous, pending;
    if (sigemptyset(&blocked) || sigaddset(&blocked, SIGPIPE) ||
        pthread_sigmask(SIG_BLOCK, &blocked, &previous)) return false;
    if (sigpending(&pending)) {
        (void)pthread_sigmask(SIG_SETMASK, &previous, NULL);
        return false;
    }
    bool pending_before = sigismember(&pending, SIGPIPE) == 1;
#endif
    bool success = true, broken_pipe = false;
    int64_t sent = 0;
    while (sent < bytes.len) {
        size_t requested = transfer_size(bytes.len - sent);
        ssize_t count = write(descriptor, bytes.data + sent, requested);
        if (count > 0) { if ((uint64_t)count > requested) { success = false; break; } sent += count; }
        else if (!count || errno != EINTR) {
            broken_pipe = count < 0 && errno == EPIPE;
            success = false; break;
        }
    }
#if defined(__APPLE__)
    (void)broken_pipe;
#else
    /* Consume only the newly generated synchronous SIGPIPE, never an old one. */
    if (broken_pipe && !pending_before) {
        if (sigpending(&pending)) success = false;
        else if (sigismember(&pending, SIGPIPE) == 1) {
            int signal_number = 0;
            if (sigwait(&blocked, &signal_number) || signal_number != SIGPIPE) success = false;
        }
    }
    if (pthread_sigmask(SIG_SETMASK, &previous, NULL)) success = false;
#endif
    return success;
#else
    (void)descriptor;
    return false;
#endif
}
bool slim_host_write_i64(int descriptor, int64_t value) {
    uint8_t digits[20]; size_t start = sizeof(digits);
    uint64_t magnitude = value < 0 ? UINT64_C(0) - (uint64_t)value : (uint64_t)value;
    do { digits[--start] = (uint8_t)('0' + magnitude % 10); magnitude /= 10; } while (magnitude);
    if (value < 0) digits[--start] = '-';
    return slim_host_write(descriptor, (SlimHostBytes){digits + start, (int64_t)(sizeof(digits) - start)});
}
bool slim_host_write_line(int descriptor, SlimHostBytes bytes) {
    return slim_host_write(descriptor, bytes) &&
           slim_host_write(descriptor, (SlimHostBytes){(const uint8_t *)"\n", 1});
}
_Noreturn void slim_host_trap(SlimHostBytes message) {
    (void)slim_host_write(2, (SlimHostBytes){(const uint8_t *)"SLIM runtime trap: ", 19});
    (void)slim_host_write_line(2, message);
    _Exit(70);
}
