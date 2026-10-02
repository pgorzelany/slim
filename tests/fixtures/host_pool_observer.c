#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void *host_pool_malloc(size_t n){return malloc(n);}
void host_pool_free(void *p){if(getenv("HOST_FORBID_FREE")){const char *s="forbidden pool free\n";(void)write(2,s,strlen(s));_Exit(99);}free(p);}
