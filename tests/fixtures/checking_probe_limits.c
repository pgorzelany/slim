/* Boundary test for test-only counters, not a second checker. */
#include "checking_probe.h"
#include <string.h>
int main(int argc, char **argv) {
    if (argc != 2) return 64;
    int count;
    if (strcmp(argv[1], "exact") == 0) count = 4;
    else if (strcmp(argv[1], "bounded") == 0) count = 5;
    else return 64;
    slim_checking_probe_init();
    for (int i = 0; i < count; ++i) {
        slim_checking_probe_enter(0);
        slim_checking_probe_enter(1);
        slim_checking_probe_step(0, 1, 4, 4, 17, 1, 2, 3);
        slim_checking_probe_push();
        slim_checking_probe_finish();
        slim_checking_probe_builtin(11, 5);
        slim_checking_probe_scalar(17);
        slim_checking_probe_end(1, -2, 0);
        slim_checking_probe_end(0, -2, 0);
    }
    return 0;
}
