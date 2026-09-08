/* Bounded native-context file capture; no compiler or query semantics. */
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE 1
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#if defined(__APPLE__)
#include <sys/clonefile.h>
#include <CommonCrypto/CommonDigest.h>
#endif
static bool safe_path(const char *path) {
    const char *allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_./+-";
    size_t length = strlen(path);
    return length > 0 && length <= 4096 && strspn(path, allowed) == length &&
        !strstr(path, "/../") && !strstr(path, "/./");
}
static bool parents(char *path) {
    for (char *cursor = strchr(path, '/'); cursor; cursor = strchr(cursor+1, '/')) {
        *cursor = 0;
        struct stat info;
        if (mkdir(path, 0700) != 0 && errno != EEXIST) { *cursor='/'; return false; }
        bool okay = lstat(path, &info) == 0 && S_ISDIR(info.st_mode);
        *cursor = '/';
        if (!okay) return false;
    }
    return true;
}
static bool copy_bytes(int source, int output, uint64_t size) {
    uint64_t copied = 0;
    while (copied < size) {
        unsigned char buffer[65536];
        size_t wanted = size - copied < sizeof(buffer) ? (size_t)(size-copied) : sizeof(buffer);
        ssize_t n = read(source, buffer, wanted);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return false;
        size_t at = 0;
        while (at < (size_t)n) {
            ssize_t written = write(output, buffer+at, (size_t)n-at);
            if (written < 0 && errno == EINTR) continue;
            if (written <= 0) return false;
            at += (size_t)written;
        }
        copied += (uint64_t)n;
    }
    unsigned char extra;
    ssize_t n;
    do { n=read(source, &extra, 1); } while (n<0 && errno==EINTR);
    return n==0;
}
#if defined(__APPLE__)
static bool read_exact(int fd, unsigned char *buffer, size_t size) {
    size_t at=0;
    while(at<size) {
        ssize_t n=read(fd,buffer+at,size-at);
        if(n<0 && errno==EINTR) continue;
        if(n<=0) return false;
        at+=(size_t)n;
    }
    return true;
}
#endif
static bool digest_equal(int source, const char *path, uint64_t size, char digest[65]) {
#if defined(__APPLE__)
    if(lseek(source,0,SEEK_SET)<0) return false;
    int copy=open(path,O_RDONLY|O_NOFOLLOW);
    if(copy<0) return false;
    CC_SHA256_CTX context;
    bool okay=CC_SHA256_Init(&context)==1;
    while(okay && size) {
        unsigned char left[65536],right[65536];
        size_t count=size<sizeof(left)?(size_t)size:sizeof(left);
        okay=read_exact(source,left,count) && read_exact(copy,right,count) &&
            memcmp(left,right,count)==0 && CC_SHA256_Update(&context,right,(CC_LONG)count)==1;
        size-=count;
    }
    unsigned char left,right,hash[CC_SHA256_DIGEST_LENGTH];
    if(okay) okay=read(source,&left,1)==0 && read(copy,&right,1)==0 && CC_SHA256_Final(hash,&context)==1;
    if(close(copy)!=0) okay=false;
    if(okay) for(unsigned i=0;i<CC_SHA256_DIGEST_LENGTH;++i) (void)snprintf(digest+2*i,3,"%02x",hash[i]);
    return okay;
#else
    (void)source; (void)path; (void)size; (void)digest;
    return false;
#endif
}
int main(int argc, char **argv) {
    if (argc != 2) return 64;
    char *end;
    errno = 0;
    uint64_t total = strtoull(argv[1], &end, 10);
    if (errno || !*argv[1] || *end || total > 1048576) return 64;
    unsigned count = 2;
    char line[8195];
    while (fgets(line, sizeof(line), stdin)) {
        char *newline = strchr(line, '\n'), *separator = strchr(line, '\t');
        if (!newline || !separator || separator > newline || ++count > 512) return 2;
        *newline = 0; *separator++ = 0;
        if (!safe_path(line) || line[0] != '/' || !safe_path(separator) ||
            (strncmp(separator,"toolchain/",10) && strncmp(separator,"sdk/",4)) || !parents(separator)) return 2;
        int source = open(line, O_RDONLY);
        if (source < 0) return 2;
        struct stat before;
        if (fstat(source,&before) != 0 || !S_ISREG(before.st_mode) || before.st_size < 0 ||
            (uint64_t)before.st_size > UINT64_C(536870912)-total) { close(source); return 2; }
        uint64_t size = (uint64_t)before.st_size;
        bool copied = false;
#if defined(__APPLE__)
        copied = fclonefileat(source, AT_FDCWD, separator, CLONE_NOOWNERCOPY) == 0;
#endif
        if (!copied) {
            int output = open(separator,O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW,0600);
            if (output < 0) { close(source); return 2; }
            copied = copy_bytes(source,output,size);
            if (close(output) != 0) copied=false;
        }
        char digest[65];
        if(copied) copied=digest_equal(source,separator,size,digest);
        (void)close(source);
        struct stat after;
        if (!copied || lstat(separator,&after) != 0 || !S_ISREG(after.st_mode) || after.st_size != before.st_size ||
            chmod(separator,(before.st_mode & 0111) ? 0500 : 0400) != 0) return 2;
        total += size;
        if (printf("%s\t%s\t%" PRIu64 "\t%s\n", line,separator,size,digest)<0) return 2;
    }
    return ferror(stdin) || fflush(stdout) != 0 ? 2 : 0;
}
