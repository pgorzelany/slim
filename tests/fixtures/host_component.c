#define _POSIX_C_SOURCE 200809L
#include "slim_host.h"
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
extern int host_live_fds,host_opens,host_closes;
extern size_t host_max_read,host_max_recv;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"host check %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static SlimHostBytes text(const char *s){return (SlimHostBytes){(const uint8_t *)s,(int64_t)strlen(s)};}
static void arguments(void) {
    for(unsigned empty=0;empty<2;++empty)for(unsigned fail=0;fail<2;++fail){
        SlimPool pool={0};CHECK(slim_pool_init(&pool,65536,fail));
        char a[]="program",b[]="",c[]={ (char)0xff, 'x', 0 };char *argv[]={a,b,c};
        SlimHostArguments args={0};
        SlimPoolResult result=slim_host_arguments(&pool,empty?0:3,argv,&args);
        if(fail&&!empty){CHECK(result==SLIM_POOL_EXHAUSTED&&!args.prepared&&!pool.live_blocks);}
        else {CHECK(result==SLIM_POOL_OK&&args.prepared&&args.domain==&pool&&args.count==(empty?0:3));
            for(int i=0;i<args.count;++i){CHECK(args.values[i].data==(uint8_t *)argv[i]);CHECK(args.values[i].len==(int64_t)strlen(argv[i]));}
            CHECK(pool.attempts==(empty?0:1)); CHECK(slim_host_arguments(&pool,0,NULL,&args)==SLIM_POOL_INVALID);
            CHECK(slim_host_arguments_release(&args)&&!args.prepared);CHECK(!slim_host_arguments_release(&args));}
        SlimHostArguments invalid={0};CHECK(slim_host_arguments(&pool,-1,NULL,&invalid)==SLIM_POOL_INVALID);
        CHECK(slim_host_arguments(&pool,1,NULL,&invalid)==SLIM_POOL_INVALID);
        char *bad[]={NULL};CHECK(slim_host_arguments(&pool,1,bad,&invalid)==SLIM_POOL_INVALID);
        CHECK(slim_pool_destroy(&pool));
    }
    char empty[]=""; char *many[4096];for(unsigned i=0;i<4096;++i)many[i]=empty;
    for(int count=4094;count<=4096;++count){
        SlimPool pool={0};CHECK(slim_pool_init(&pool,65536,0));SlimHostArguments args={0};
        SlimPoolResult result=slim_host_arguments(&pool,count,many,&args);
        CHECK(pool.attempts==1);
        if(count==4096)CHECK(result==SLIM_POOL_EXHAUSTED&&!args.prepared);
        else {CHECK(result==SLIM_POOL_OK&&!slim_pool_destroy(&pool));CHECK(slim_host_arguments_release(&args));}
        CHECK(slim_pool_destroy(&pool));
    }
    puts("arguments passed");
}
static void invalid(void){
    uint8_t byte=0;SlimHostBuffer buffers[]={{NULL,-1,0},{NULL,0,-1},{&byte,2,1},{NULL,0,1}};
    for(unsigned i=0;i<4;++i){CHECK(!slim_host_read_file(text("x"),0,&buffers[i]));CHECK(!slim_host_tcp_exchange(text("127.0.0.1"),80,text(""),0,1,&buffers[i]));}
    SlimHostBuffer out={NULL,0,0};SlimHostBytes bad[]={{NULL,-1},{NULL,1}};
    for(unsigned i=0;i<2;++i){CHECK(!slim_host_read_file(bad[i],0,&out));CHECK(!slim_host_write(1,bad[i]));CHECK(!slim_host_tcp_exchange(bad[i],80,text(""),0,1,&out));}
    CHECK(!slim_host_read_file(text("x"),0,NULL));CHECK(!slim_host_read_file(text("x"),INT64_MAX,&out));
    for(int i=0;i<2;++i){CHECK(!slim_host_tcp_exchange(text("127.0.0.1"),i?65536:0,text(""),0,1,&out));CHECK(!slim_host_tcp_exchange(text("127.0.0.1"),80,text(""),0,i?-1:0,&out));}
    CHECK(!slim_host_tcp_exchange(text("local.host"),80,text(""),0,1,&out));
    CHECK(!slim_host_tcp_exchange((SlimHostBytes){(uint8_t*)"127\0.0.1",8},80,text(""),0,1,&out));
    CHECK(host_live_fds==0&&host_opens==0);puts("invalid passed");
}
static atomic_bool worker_started;
static void *writer(void *p) {uint8_t *bytes=p;atomic_store(&worker_started,true);for(;;)for(unsigned i=0;i<64;++i)((volatile uint8_t*)bytes)[i]=(uint8_t)i;return NULL;}
static void live_trap(bool broken) {SlimPool pool={0};CHECK(slim_pool_init(&pool,65536,0));SlimPoolBlock block={0};CHECK(slim_pool_allocate(&pool,64,1,false,&block)==SLIM_POOL_OK);pthread_t thread;atomic_init(&worker_started,false);CHECK(!pthread_create(&thread,NULL,writer,block.data));while(!atomic_load(&worker_started)){}if(broken){int fds[2];CHECK(!pipe(fds)&&!close(fds[0]));CHECK(dup2(fds[1],2)==2);CHECK(!close(fds[1]));}slim_host_trap(text("live worker witness"));}
static void broken_pipe(void) {
    sigset_t set,old,before,after;CHECK(!sigemptyset(&set)&&!sigaddset(&set,SIGPIPE));
    CHECK(!pthread_sigmask(SIG_BLOCK,&set,&old));CHECK(!raise(SIGPIPE));CHECK(!sigpending(&before));
    int fds[2];CHECK(!pipe(fds));CHECK(!close(fds[0]));CHECK(!slim_host_write(fds[1],text("x")));CHECK(!sigpending(&after));
    CHECK(sigismember(&before,SIGPIPE)==1&&sigismember(&after,SIGPIPE)==1);
    int number=0;CHECK(!sigwait(&set,&number)&&number==SIGPIPE);CHECK(!pthread_sigmask(SIG_SETMASK,&old,NULL));
    CHECK(!slim_host_write(fds[1],text("x")));CHECK(!sigpending(&after));CHECK(sigismember(&after,SIGPIPE)==0);CHECK(!close(fds[1]));puts("pipe passed");
}
int main(int argc,char **argv) {
    CHECK(argc>=2);
    if(!strcmp(argv[1],"arguments")){arguments();return 0;}
    if(!strcmp(argv[1],"invalid")){invalid();return 0;}
    if(!strcmp(argv[1],"binary")){uint8_t data[256];for(unsigned i=0;i<256;++i)data[i]=(uint8_t)i;return slim_host_write(1,(SlimHostBytes){data,256})?0:2;}
    if(!strcmp(argv[1],"large-file")){
        CHECK(argc==4);size_t n=(size_t)strtoull(argv[2],NULL,10);CHECK(n>=1048575&&n<=1048577);
        uint8_t *data=malloc(n+16);CHECK(data);memset(data,0xa5,n+16);SlimHostBuffer out={data,0,(int64_t)n};
        CHECK(slim_host_read_file(text(argv[3]),(int64_t)n,&out)&&out.len==(int64_t)n);
        CHECK(host_max_read<=1048576&&host_max_read==(n>1048576?1048576:n));
        for(size_t i=0;i<n;++i)if(data[i]!=(uint8_t)i){fprintf(stderr,"large byte %zu actual=%u expected=%u\n",i,data[i],(uint8_t)i);return 1;}
        for(size_t i=n;i<n+16;++i)CHECK(data[i]==0xa5);
        CHECK(host_live_fds==0);free(data);puts("large passed");return 0;
    }
    if(!strcmp(argv[1],"live-trap"))live_trap(false);
    if(!strcmp(argv[1],"live-trap-pipe"))live_trap(true);
    if(!strcmp(argv[1],"pipe")){broken_pipe();return 0;}
    if(!strcmp(argv[1],"trap-pipe")){int fds[2];CHECK(!pipe(fds));CHECK(!close(fds[0]));CHECK(dup2(fds[1],2)==2);CHECK(!close(fds[1]));slim_host_trap(text("broken stderr"));}
    if(!strcmp(argv[1],"number")){CHECK(argc==3);return slim_host_write_i64(1,strtoll(argv[2],NULL,10))?0:2;}
    if(!strcmp(argv[1],"line")){return slim_host_write_line(1,text(argc==3?argv[2]:""))?0:2;}
    CHECK(argc>=7);
    int64_t limit=strtoll(argv[2],NULL,10),prefix=strtoll(argv[3],NULL,10),capacity=strtoll(argv[4],NULL,10);
    CHECK(prefix>=0&&prefix<=3&&capacity>=prefix&&capacity<=8192);
    uint8_t data[8200];memset(data,0xa5,sizeof(data));for(int64_t i=0;i<prefix;++i)data[i]=(uint8_t)(240+i);
    SlimHostBuffer buffer={capacity?data:NULL,prefix,capacity};uint8_t *identity=buffer.data;
    bool result;
    SlimHostBytes name=text(argv[5]);
    if(name.len>0){if(!strcmp(argv[6],"nul"))argv[5][name.len/2]=0;else if(!strcmp(argv[6],"nul-first"))argv[5][0]=0;else if(!strcmp(argv[6],"nul-last"))argv[5][name.len-1]=0;}
    if(!strcmp(argv[1],"file"))result=slim_host_read_file(name,limit,&buffer);
    else {CHECK(argc==9);uint8_t binary[]={0,255,'X'};SlimHostBytes request=!strcmp(argv[1],"tcp-binary")?(SlimHostBytes){binary,3}:text(argv[7]);result=slim_host_tcp_exchange(name,strtoll(argv[6],NULL,10),request,limit,strtoll(argv[8],NULL,10),&buffer);}
    CHECK(buffer.capacity==capacity&&buffer.data==identity);CHECK(buffer.len>=prefix&&buffer.len<=capacity);
    if(!result)CHECK(buffer.len==prefix);
    for(int64_t i=0;i<prefix;++i)CHECK(data[i]==240+i);
    for(int64_t i=capacity;i<(int64_t)sizeof(data);++i)CHECK(data[i]==0xa5);
    CHECK(host_live_fds==0);
    printf("%u %lld ",(unsigned)result,(long long)buffer.len);for(int64_t i=0;i<buffer.len;++i)printf("%02x",data[i]);printf("\n");
    fprintf(stderr,"opens=%d closes=%d live=%d\n",host_opens,host_closes,host_live_fds);
    return 0;
}
