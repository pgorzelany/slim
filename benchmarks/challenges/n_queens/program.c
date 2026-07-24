#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int safe(
    const int64_t *columns,
    int64_t row,
    int64_t column
) {
    for (int64_t prior = 0; prior < row; ++prior) {
        int64_t occupied = columns[prior];
        int64_t difference =
            occupied >= column ? occupied - column : column - occupied;
        if (occupied == column || row - prior == difference) return 0;
    }
    return 1;
}

int main(void) {
    const int64_t size = 13;
    int64_t *columns = malloc((size_t)size * sizeof(*columns));
    if (columns == NULL) return 1;
    for (int64_t index = 0; index < size; ++index) columns[index] = -1;

    int64_t row = 0;
    int64_t column = 0;
    int64_t total = 0;
    while (row >= 0) {
        if (column >= size) {
            if (row == 0) break;
            int64_t previous_row = row - 1;
            int64_t previous_column = columns[previous_row];
            columns[row] = -1;
            row = previous_row;
            column = previous_column + 1;
        } else if (!safe(columns, row, column)) {
            column += 1;
        } else {
            columns[row] = column;
            if (row == size - 1) {
                total += 1;
                column += 1;
            } else {
                row += 1;
                column = 0;
            }
        }
    }

    printf("%lld\n", (long long)total);
    free(columns);
    return 0;
}
