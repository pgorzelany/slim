#include "slim_pool.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
extern uint64_t pool_test_allocations, pool_test_frees;
extern bool pool_test_fail_allocation;
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"pool check failed: line %d: %s\n",__LINE__,#value); exit(1); } } while(0)

#ifdef SLIM_POOL_INSTRUMENT
static SlimPoolWork maximum_work;
#endif

static void reset_work(SlimPool *pool) {
#ifdef SLIM_POOL_INSTRUMENT
    pool->work = (SlimPoolWork){0};
#else
    (void)pool;
#endif
}
static void check_work(SlimPool *pool) {
#ifdef SLIM_POOL_INSTRUMENT
    CHECK(pool->work.hierarchy_reads <= 4);
    CHECK(pool->work.hierarchy_writes <= 100);
    CHECK(pool->work.availability_writes <= 25);
    CHECK(pool->work.bit_steps <= 320);
    CHECK(pool->work.splits <= 24);
    CHECK(pool->work.buddy_tests <= 24);
    CHECK(pool->work.merges <= 24);
#define MAXIMUM(field) if(pool->work.field>maximum_work.field) maximum_work.field=pool->work.field
    MAXIMUM(hierarchy_reads); MAXIMUM(hierarchy_writes); MAXIMUM(availability_writes);
    MAXIMUM(bit_steps); MAXIMUM(splits); MAXIMUM(buddy_tests); MAXIMUM(merges);
#undef MAXIMUM
#else
    (void)pool;
#endif
}
static SlimPoolResult allocate(SlimPool *pool, uint64_t bytes, unsigned alignment,
                                bool recursive, SlimPoolBlock *output) {
    uint64_t host_alloc = pool_test_allocations, host_free = pool_test_frees;
    reset_work(pool);
    SlimPoolResult result=slim_pool_allocate(pool,bytes,alignment,recursive,output);
    check_work(pool);
    CHECK(host_alloc == pool_test_allocations && host_free == pool_test_frees);
    if(result==SLIM_POOL_OK && bytes!=0) {
        CHECK(((uintptr_t)output->data % alignment)==0);
        CHECK(output->usable >= bytes);
        if(recursive) for(unsigned i=16;i<64;i++) CHECK(pool->base[output->offset+i]==0);
        output->data[0]=17; output->data[bytes-1]=29;
    }
    return result;
}
static bool release(SlimPool *pool, SlimPoolBlock block) {
    uint64_t host_alloc = pool_test_allocations, host_free = pool_test_frees;
    reset_work(pool);
    bool result=slim_pool_release(pool,block);
    check_work(pool);
    CHECK(host_alloc == pool_test_allocations && host_free == pool_test_frees);
    return result;
}
static void selftest(void) {
    SlimPool pool={0};
    uint64_t invalid[]={0,1,65535,65537,SLIM_POOL_MAX_CAPACITY+1,UINT64_MAX};
    for(size_t i=0;i<sizeof(invalid)/sizeof(*invalid);i++) CHECK(!slim_pool_init(&pool,invalid[i],0));
    CHECK(pool_test_allocations==0 && pool_test_frees==0);
    pool_test_fail_allocation=true;
    CHECK(!slim_pool_init(&pool,65536,0)); CHECK(pool.reservation==NULL);
    CHECK(slim_pool_init(&pool,65536,0));
    uint64_t host=pool_test_allocations;
    CHECK(!slim_pool_init(&pool,65536,0)); CHECK(pool_test_allocations==host);
    CHECK(slim_pool_index_bytes(SLIM_POOL_MAX_CAPACITY)==4261024);
    SlimPoolBlock block;
    memset(&block,0xA5,sizeof(block));
    unsigned char before[sizeof(block)]; memcpy(before,&block,sizeof(block));
    for(unsigned alignment=0;alignment<=32;alignment++) {
        if(alignment && alignment<=16 && (alignment&(alignment-1))==0) continue;
        CHECK(allocate(&pool,1,alignment,false,&block)==SLIM_POOL_INVALID);
        CHECK(memcmp(before,&block,sizeof(block))==0);
    }
    CHECK(allocate(&pool,UINT64_MAX,1,false,&block)==SLIM_POOL_EXHAUSTED);
    CHECK(memcmp(before,&block,sizeof(block))==0);
    pool.failure_at=pool.attempts+1;
    CHECK(allocate(&pool,0,16,true,&block)==SLIM_POOL_OK && block.data==NULL);
    CHECK(release(&pool,block));
    CHECK(allocate(&pool,1,16,false,&block)==SLIM_POOL_EXHAUSTED);
    CHECK(allocate(&pool,1,16,false,&block)==SLIM_POOL_OK);
    CHECK(!slim_pool_destroy(&pool));
    SlimPool other={0}; CHECK(slim_pool_init(&other,65536,0));
    CHECK(!release(&other,block)); CHECK(slim_pool_destroy(&other));
    SlimPoolBlock bad=block; bad.generation++; CHECK(!release(&pool,bad));
    bad=block; bad.requested++; CHECK(!release(&pool,bad));
    bad=block; bad.offset=UINT32_MAX; CHECK(!release(&pool,bad));
    bad=block; bad.order=UINT8_MAX; CHECK(!release(&pool,bad));
    bad=block; bad.header_bytes=0; CHECK(!release(&pool,bad));
    bad=block; bad.usable++; CHECK(!release(&pool,bad));
    bad=block; bad.data++; CHECK(!release(&pool,bad));
    unsigned fields[]={0,8,12,13,14};
    for(size_t i=0;i<sizeof(fields)/sizeof(*fields);i++) {
        pool.base[block.offset+fields[i]]^=1;
        CHECK(!release(&pool,block));
        pool.base[block.offset+fields[i]]^=1;
    }
    CHECK(release(&pool,block)); CHECK(!release(&pool,block));
    SlimPoolBlock fresh={0}; CHECK(allocate(&pool,1,16,false,&fresh)==SLIM_POOL_OK);
    CHECK(fresh.offset==block.offset && fresh.generation!=block.generation);
    CHECK(!release(&pool,block)); CHECK(release(&pool,fresh));
    pool.attempts=UINT64_MAX-1; pool.failure_at=0;
    CHECK(allocate(&pool,1,1,false,&block)==SLIM_POOL_OK && block.generation==UINT64_MAX);
    CHECK(release(&pool,block));
    CHECK(allocate(&pool,1,1,false,&fresh)==SLIM_POOL_EXHAUSTED && pool.attempts==UINT64_MAX);
    CHECK(allocate(&pool,0,16,true,&fresh)==SLIM_POOL_OK && pool.attempts==UINT64_MAX);
    CHECK(release(&pool,fresh));
    CHECK(slim_pool_destroy(&pool)); CHECK(slim_pool_destroy(&pool));

    for(unsigned recursive=0;recursive<2;recursive++) {
        CHECK(slim_pool_init(&pool,65536,0));
        SlimPoolBlock blocks[1024]; size_t count=0;
        while(count<1024 && allocate(&pool,1,16,recursive!=0,&blocks[count])==SLIM_POOL_OK) count++;
        CHECK(count==(recursive?512:1024));
        CHECK(allocate(&pool,1,16,recursive!=0,&fresh)==SLIM_POOL_EXHAUSTED);
        for(size_t i=0;i<count;i+=2) CHECK(release(&pool,blocks[i]));
        CHECK(allocate(&pool,8192,16,recursive!=0,&fresh)==SLIM_POOL_EXHAUSTED);
        CHECK(allocate(&pool,1,16,recursive!=0,&fresh)==SLIM_POOL_OK && fresh.offset==0);
        CHECK(release(&pool,fresh));
        for(size_t i=1;i<count;i+=2) { CHECK(blocks[i].data[0]==29); CHECK(release(&pool,blocks[i])); }
        CHECK(pool.live_blocks==0 && slim_pool_is_free(&pool,pool.max_order,0));
        CHECK(slim_pool_destroy(&pool));
    }
    /* Every allocation position in the named 16-allocation burst, plus control. */
    for(uint64_t failure=0;failure<=16;failure++) {
        CHECK(slim_pool_init(&pool,65536,failure));
        SlimPoolBlock burst[16]; bool live[16]={false}; unsigned failures=0;
        for(unsigned i=0;i<16;i++) {
            memset(&burst[i],0xA5,sizeof(burst[i]));
            unsigned char saved[sizeof(burst[i])]; memcpy(saved,&burst[i],sizeof(burst[i]));
            SlimPoolResult result=allocate(&pool,(i%3==2)?49:1,16,i%2!=0,&burst[i]);
            CHECK(result==((failure==i+1)?SLIM_POOL_EXHAUSTED:SLIM_POOL_OK));
            live[i]=result==SLIM_POOL_OK;
            if(!live[i]) { failures++; CHECK(memcmp(saved,&burst[i],sizeof(burst[i]))==0); }
        }
        CHECK(failures==(failure?1:0));
        for(unsigned i=16;i>0;i--) if(live[i-1]) CHECK(release(&pool,burst[i-1]));
        CHECK(slim_pool_destroy(&pool));
    }
    uint64_t capacities[]={65536,1048576,SLIM_POOL_DEFAULT_CAPACITY};
    for(size_t c=0;c<3;c++) {
        CHECK(slim_pool_init(&pool,capacities[c],0));
        for(unsigned recursive=0;recursive<2;recursive++) {
            unsigned header=recursive?64:16;
            for(uint64_t power=64;power<=capacities[c];power*=2) {
                for(int delta=-1;delta<=1;delta++) {
                    int64_t signed_bytes=(int64_t)power-(int64_t)header+delta;
                    if(signed_bytes<0) continue;
                    uint64_t bytes=(uint64_t)signed_bytes;
                    for(unsigned alignment=1;alignment<=16;alignment*=2) {
                        SlimPoolResult result=allocate(&pool,bytes,alignment,recursive!=0,&block);
                        CHECK(result==((bytes>capacities[c]-header)?SLIM_POOL_EXHAUSTED:SLIM_POOL_OK));
                        if(result==SLIM_POOL_OK) CHECK(release(&pool,block));
                    }
                }
            }
        }
        CHECK(slim_pool_destroy(&pool));
    }
    CHECK(slim_pool_init(&pool,SLIM_POOL_MAX_CAPACITY,0));
    CHECK(allocate(&pool,1,16,false,&block)==SLIM_POOL_OK); CHECK(release(&pool,block));
    uint32_t boundaries[]={63,64,65,4095,4096,4097,262143,262144,262145,16777215};
    for(size_t c=0;c<sizeof(boundaries)/sizeof(*boundaries);c++) {
        SlimPoolBlock prefix[25]; unsigned count=0; uint64_t offset=0;
        uint64_t bytes=(uint64_t)boundaries[c]*64;
        for(int bit=23;bit>=0;bit--) {
            uint64_t width=UINT64_C(64)<<bit;
            if((bytes&width)==0) continue;
            CHECK(allocate(&pool,width-16,16,false,&prefix[count])==SLIM_POOL_OK);
            CHECK(prefix[count].offset==offset); count++; offset+=width;
        }
        CHECK(allocate(&pool,1,16,false,&block)==SLIM_POOL_OK && block.offset==bytes);
        CHECK(release(&pool,block));
        while(count) CHECK(release(&pool,prefix[--count]));
        CHECK(slim_pool_is_free(&pool,pool.max_order,0));
    }
    CHECK(slim_pool_destroy(&pool));
    CHECK(pool_test_allocations==pool_test_frees+1); /* one injected startup failure */
    printf("{\"selftest\":\"passed\",\"control_bytes\":%zu,\"minimum_index_bytes\":%zu,\"default_index_bytes\":%zu,\"maximum_index_bytes\":%zu,\"host_attempts\":%" PRIu64 ",\"host_frees\":%" PRIu64, sizeof(SlimPool),slim_pool_index_bytes(65536),slim_pool_index_bytes(SLIM_POOL_DEFAULT_CAPACITY),slim_pool_index_bytes(SLIM_POOL_MAX_CAPACITY),pool_test_allocations,pool_test_frees);
#ifdef SLIM_POOL_INSTRUMENT
    printf(",\"maximum_work\":{\"hierarchy_reads\":%" PRIu64 ",\"hierarchy_writes\":%" PRIu64 ",\"availability_writes\":%" PRIu64 ",\"bit_steps\":%" PRIu64 ",\"splits\":%" PRIu64 ",\"buddy_tests\":%" PRIu64 ",\"merges\":%" PRIu64 "}",maximum_work.hierarchy_reads,maximum_work.hierarchy_writes,maximum_work.availability_writes,maximum_work.bit_steps,maximum_work.splits,maximum_work.buddy_tests,maximum_work.merges);
#endif
    printf("}\n");
}

static void protocol(void) {
    SlimPool pool={0}; SlimPoolBlock slots[16]={{0}}; bool used[16]={false};
    char command; unsigned slot=0,recursive=0,fail=0; uint64_t bytes=0;
    while(scanf(" %c",&command)==1) {
        if(command=='R') {
            for(unsigned i=0;i<16;i++) if(used[i]) { CHECK(release(&pool,slots[i])); used[i]=false; }
            CHECK(slim_pool_destroy(&pool)); CHECK(slim_pool_init(&pool,65536,0));
        } else if(command=='A') {
            CHECK(scanf("%u %" SCNu64 " %u %u",&slot,&bytes,&recursive,&fail)==4 && slot<16 && !used[slot]);
            pool.failure_at=fail?pool.attempts+1:0;
            SlimPoolResult result=allocate(&pool,bytes,16,recursive!=0,&slots[slot]);
            CHECK(result!=SLIM_POOL_INVALID);
            used[slot]=result==SLIM_POOL_OK && bytes!=0;
            printf("A %u %" PRIu64 "\n",(unsigned)result,used[slot]?(uint64_t)slots[slot].offset:UINT64_MAX);
        } else if(command=='F') {
            CHECK(scanf("%u",&slot)==1 && slot<16);
            bool result=used[slot] && release(&pool,slots[slot]); used[slot]=false;
            printf("F %u\n",result?1:0);
        } else if(command=='S') {
            printf("S %" PRIu64 " %" PRIu64 " %" PRIu64, pool.live_blocks,pool.live_requested,pool.live_rounded);
            for(unsigned order=0;order<=pool.max_order;order++) {
                uint64_t width=UINT64_C(64)<<order;
                for(uint64_t offset=0;offset<pool.capacity;offset+=width)
                    if(slim_pool_is_free(&pool,order,offset)) printf(" %u:%" PRIu64,order,offset);
            }
            printf("\n");
        } else CHECK(false);
    }
    for(unsigned i=0;i<16;i++) if(used[i]) CHECK(release(&pool,slots[i]));
    CHECK(slim_pool_destroy(&pool));
}
static double now(void) { struct timespec ts; CHECK(timespec_get(&ts,TIME_UTC)==TIME_UTC); return ts.tv_sec+ts.tv_nsec/1e9; }
static void benchmark(void) {
    uint64_t sizes[]={1,48,49,4096};
    for(size_t s=0;s<4;s++) for(int run=0;run<6;run++) {
        SlimPool pool={0}; double begin=now(); CHECK(slim_pool_init(&pool,SLIM_POOL_DEFAULT_CAPACITY,0));
        double initialized=now(); SlimPoolBlock block={0};
        for(unsigned i=0;i<10000;i++) { CHECK(slim_pool_allocate(&pool,sizes[s],16,false,&block)==SLIM_POOL_OK); block.data[0]=(uint8_t)i; block.data[sizes[s]-1]=(uint8_t)(i+1); CHECK(slim_pool_release(&pool,block)); }
        double finished=now(); CHECK(slim_pool_destroy(&pool));
        printf("%" PRIu64 "\t%d\t%.9f\t%.9f\n",sizes[s],run,initialized-begin,finished-initialized);
    }
}
int main(int argc,char **argv) {
    CHECK(argc==2);
    if(strcmp(argv[1],"selftest")==0) selftest();
    else if(strcmp(argv[1],"protocol")==0) protocol();
    else if(strcmp(argv[1],"benchmark")==0) benchmark();
    else if(strcmp(argv[1],"uaf")==0 || strcmp(argv[1],"overrun")==0) {
        SlimPool pool={0}; SlimPoolBlock block={0}; CHECK(slim_pool_init(&pool,65536,0));
        CHECK(slim_pool_allocate(&pool,1,16,false,&block)==SLIM_POOL_OK);
        volatile uint8_t *data=block.data;
        if(strcmp(argv[1],"uaf")==0) { CHECK(slim_pool_release(&pool,block)); data[0]=7; }
        else data[1]=7;
        CHECK(slim_pool_destroy(&pool));
    } else CHECK(false);
    return 0;
}
