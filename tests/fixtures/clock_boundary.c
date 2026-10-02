#define _POSIX_C_SOURCE 200809L
#include "slim_rt.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static struct timespec reading;
static int failed;
int host_probe_clock(clockid_t id,struct timespec *out){(void)id;*out=reading;return failed?-1:0;}
int host_probe_utc(struct timespec *out,int base){(void)base;*out=reading;return failed?0:TIME_UTC;}
#define CHECK(x) do{if(!(x)){fprintf(stderr,"clock check %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char **argv){
    if(argc!=2)return 2;int scenario=atoi(argv[1]);
    if(scenario==0){reading=(struct timespec){1,999000000};CHECK(slim_monotonic_ms()==1999);reading=(struct timespec){0,0};CHECK(slim_monotonic_ms()==1999);failed=1;CHECK(slim_monotonic_ms()==1999);failed=0;reading.tv_sec=-1;CHECK(slim_monotonic_ms()==1999);}
    else {long nanos=scenario==1?806000000:scenario==2?807000000:scenario==3?808000000:999000000;reading=(struct timespec){INT64_MAX/1000,nanos};CHECK(slim_monotonic_ms()==(scenario==1?INT64_MAX-1:INT64_MAX));reading=(struct timespec){INT64_MAX/1000+1,0};CHECK(slim_monotonic_ms()==INT64_MAX);reading=(struct timespec){0,LONG_MAX};CHECK(slim_monotonic_ms()==INT64_MAX);}
    puts("clock passed");return 0;
}
