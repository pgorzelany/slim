#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
int host_live_fds, host_opens, host_closes;
size_t host_max_read,host_max_recv;
static unsigned ordinal;
static int injected(const char *operation) {
    const char *name=getenv("HOST_FAULT");
    if(!name||strcmp(name,operation))return 0;
    const char *at=getenv("HOST_FAULT_AT");
    if(++ordinal!=(unsigned)(at?atoi(at):1))return 0;
    const char *kind=getenv("HOST_ERROR");
    errno=kind&&!strcmp(kind,"eintr")?EINTR:EIO; return 1;
}
static size_t short_count(size_t n) { return getenv("HOST_SHORT")&&n>3?3:n; }
int host_probe_open(const char *path,int flags,...) { host_opens++; if(injected("open"))return -1; int fd=open(path,flags); if(fd>=0)host_live_fds++; return fd; }
int host_probe_close(int fd) { host_closes++; int r=close(fd); if(!r)host_live_fds--; if(injected("close"))return -1; return r; }
ssize_t host_probe_read(int fd,void *out,size_t n) {
    if(n>host_max_read)host_max_read=n;
    static unsigned reads;
    const char *mutation=getenv("HOST_MUTATE"),*path=getenv("HOST_MUTATE_PATH");
    if(++reads==2&&mutation&&path){
        int writer=open(path,O_WRONLY|O_APPEND);
        if(writer<0)_Exit(97);
        if(!strcmp(mutation,"grow")){if(write(writer,"XYZ",3)!=3)_Exit(97);}
        else if(ftruncate(writer,2))_Exit(97);
        if(close(writer))_Exit(97);
    }
    if(injected("read"))return -1;
    return read(fd,out,short_count(n));
}
ssize_t host_probe_write(int fd,const void *in,size_t n) { if(injected("write"))return -1; if(getenv("HOST_ZERO_WRITE"))return 0; return write(fd,in,short_count(n)); }
int host_probe_socket(int family,int type,int protocol) { if(injected("socket"))return -1; int fd=socket(family,type,protocol);if(fd>=0)host_live_fds++;return fd; }
int host_probe_connect(int fd,const struct sockaddr *a,socklen_t n) { if(injected("connect"))return -1;int result=connect(fd,a,n);if(!result&&getenv("HOST_CONNECT_PENDING")){errno=EINPROGRESS;return -1;}return result; }
int host_probe_shutdown(int fd,int how) { if(injected("shutdown"))return -1;return shutdown(fd,how); }
ssize_t host_probe_send(int fd,const void *p,size_t n,int flags) {
    const char *fault=getenv("HOST_FAULT");
    if(fault&&!strcmp(fault,"send-epipe")){
        int pair[2];if(socketpair(AF_UNIX,SOCK_STREAM,0,pair))_Exit(96);
        if(close(pair[1]))_Exit(96);
        ssize_t result=send(pair[0],p,n,flags);int saved=errno;
        if(close(pair[0]))_Exit(96);errno=saved;return result;
    }
    if(injected("send"))return -1;
    return send(fd,p,short_count(n),flags);
}
ssize_t host_probe_recv(int fd,void *p,size_t n,int flags) { if(n>host_max_recv)host_max_recv=n;if(injected("recv"))return -1;return recv(fd,p,short_count(n),flags); }
int host_probe_poll(struct pollfd *p,nfds_t n,int timeout) { if(injected("poll"))return -1;return poll(p,n,timeout); }
int host_probe_fcntl(int fd,int operation,...) { va_list args;va_start(args,operation);int value=va_arg(args,int);va_end(args);if(injected("fcntl"))return -1;return fcntl(fd,operation,value); }
int host_probe_setsockopt(int fd,int level,int option,const void *value,socklen_t n) { if(injected("setsockopt"))return -1;return setsockopt(fd,level,option,value,n); }
int host_probe_getsockopt(int fd,int level,int option,void *value,socklen_t *n) { if(injected("getsockopt"))return -1;return getsockopt(fd,level,option,value,n); }
