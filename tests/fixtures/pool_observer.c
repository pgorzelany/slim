#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
uint64_t pool_test_allocations;
uint64_t pool_test_frees;
bool pool_test_fail_allocation;
void *pool_test_malloc(size_t size) {
    pool_test_allocations++;
    if (pool_test_fail_allocation) { pool_test_fail_allocation = false; return NULL; }
    return malloc(size);
}
void pool_test_free(void *pointer) { pool_test_frees++; free(pointer); }
