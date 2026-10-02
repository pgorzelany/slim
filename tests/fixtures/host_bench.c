#define _POSIX_C_SOURCE 200809L
#include "slim_rt.h"
#ifdef SLIM_HOST_CANDIDATE
#include "slim_host.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
static double now(void) { struct timespec t; if(clock_gettime(CLOCK_MONOTONIC,&t)) exit(2); return (double)t.tv_sec+(double)t.tv_nsec/1e9; }
int main(int argc,char **argv) {
    if(argc!=4) return 2;
    unsigned length=(unsigned)strtoul(argv[2],NULL,10); if(length>4096) return 2;
    int file=argv[1][0]=='f'; uint8_t data[4096]; for(unsigned i=0;i<length;++i)data[i]=(uint8_t)i;
    SlimAllocStatus status; SlimRegion root; slim_alloc_status_init(&status); slim_rt_init(&root,&status);
    for(unsigned sample=0;sample<6;++sample) {
        double start=now();
        for(unsigned operation=0;operation<1000;++operation) {
#ifdef SLIM_HOST_CANDIDATE
            if(file) { SlimHostBuffer out={data,0,length}; SlimHostBytes path={(const uint8_t*)argv[3],(int64_t)strlen(argv[3])}; if(!slim_host_read_file(path,length,&out)||out.len!=length) return 3; }
            else if(!slim_host_write(1,(SlimHostBytes){data,length})) return 3;
#else
            if(file) { SlimRegion child; slim_region_init(&child,&root); SlimVec out=slim_vec_new(1,&child); if(!slim_read_file((SlimBytes){(const uint8_t*)argv[3],(int64_t)strlen(argv[3])},&out)||out.len!=length) return 3; if(length)memcpy(data,out.data,length); slim_region_destroy(&child); }
            else slim_print_bytes((SlimBytes){data,length});
#endif
        }
        if(fflush(stdout))return 3;
        double elapsed=now()-start;
        for(unsigned i=0;i<length;++i)if(data[i]!=(uint8_t)i)return 4;
        fprintf(stderr,"%s,%u,%u,%.9f\n",file?"file":"write",length,sample,elapsed);
    }
    slim_rt_shutdown(); return 0;
}
