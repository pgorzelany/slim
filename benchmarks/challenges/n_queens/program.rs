fn safe(columns: &[i64], row: usize, column: i64) -> bool {
    for (prior, occupied) in columns[..row].iter().copied().enumerate() {
        let difference = (occupied - column).abs();
        if occupied == column || row as i64 - prior as i64 == difference {
            return false;
        }
    }
    true
}

fn main() {
    let size = 13usize;
    let mut columns = vec![-1i64; size];
    let mut row = 0usize;
    let mut column = 0i64;
    let mut total = 0i64;

    loop {
        if column >= size as i64 {
            if row == 0 {
                break;
            }
            let previous_row = row - 1;
            let previous_column = columns[previous_row];
            columns[row] = -1;
            row = previous_row;
            column = previous_column + 1;
        } else if !safe(&columns, row, column) {
            column += 1;
        } else {
            columns[row] = column;
            if row == size - 1 {
                total += 1;
                column += 1;
            } else {
                row += 1;
                column = 0;
            }
        }
    }

    println!("{total}");
}
