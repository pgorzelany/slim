#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int64_t minimum_three(int64_t first, int64_t second, int64_t third) {
    int64_t result = first < second ? first : second;
    return result < third ? result : third;
}

int main(int argc, char **argv) {
    (void)argv;
    static const char left[] =
        "thequickbrownfoxjumpsoverthelazydogpackmyboxwithfivedozenliquorjugs"
        "phinxofblackquartzjudgemyvow";
    static const char right[] =
        "thequickbluefoxjumpedoverthehazydogpacktheboxwithsixdozenliquorjugs"
        "sphinxofbrightquartzjudgeourvow";
    const int64_t left_length = (int64_t)strlen(left);
    const int64_t right_length = (int64_t)strlen(right);
    int64_t *previous =
        malloc((size_t)(right_length + 1) * sizeof(*previous));
    int64_t *current =
        malloc((size_t)(right_length + 1) * sizeof(*current));
    if (previous == NULL || current == NULL) {
        free(previous);
        free(current);
        return 1;
    }
    for (int64_t index = 0; index <= right_length; ++index) {
        previous[index] = index;
        current[index] = 0;
    }
    int64_t total = 0;
    const int64_t rounds = 200 * argc;
    for (int64_t round = 0; round < rounds; ++round) {
        for (int64_t index = 0; index <= right_length; ++index) {
            previous[index] = index;
        }
        for (int64_t row = 1; row <= left_length; ++row) {
            current[0] = row;
            for (int64_t column = 1; column <= right_length; ++column) {
                int64_t cost = left[row - 1] == right[column - 1] ? 0 : 1;
                int64_t deletion = previous[column] + 1;
                int64_t insertion = current[column - 1] + 1;
                int64_t substitution = previous[column - 1] + cost;
                current[column] =
                    minimum_three(deletion, insertion, substitution);
            }
            for (int64_t index = 0; index <= right_length; ++index) {
                previous[index] = current[index];
            }
        }
        total += previous[right_length];
    }
    printf("%lld\n", (long long)total);
    free(current);
    free(previous);
    return 0;
}
