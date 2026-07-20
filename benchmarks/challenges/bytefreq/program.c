#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        return 64;
    }
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        return 70;
    }
    int64_t counts[256] = {0};
    uint8_t buffer[65536];
    size_t length;
    while ((length = fread(buffer, 1, sizeof(buffer), file)) != 0) {
        for (size_t index = 0; index < length; ++index) {
            counts[buffer[index]] += 1;
        }
    }
    if (ferror(file) || fclose(file) != 0) {
        return 70;
    }
    int64_t total = 0;
    for (int64_t index = 0; index < 256; ++index) {
        total += counts[index] * (index + 1);
    }
    printf("%" PRId64 "\n", total);
    return 0;
}

