#include "component_work.h"
#include <string.h>

int main(int argc, char **argv) {
    uint64_t value = COMPONENT_WORK_CAP - 1;
    component_work_increment(&value);
    if (value != COMPONENT_WORK_CAP) abort();
    if (argc == 2 && strcmp(argv[1], "saturate") == 0) {
        component_work_increment(&value);
        abort();
    }
    return 0;
}
