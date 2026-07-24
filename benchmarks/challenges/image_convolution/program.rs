fn main() {
    let size = 256usize;
    let count = size * size;
    let mut current: Vec<i64> = (0..count)
        .map(|index| ((index * 31 + 7) % 256) as i64)
        .collect();
    let mut next = vec![0i64; count];
    for _ in 0..40 {
        for index in 0..count {
            let row = index / size;
            let column = index % size;
            let previous_row = (row + size - 1) % size;
            let next_row = (row + 1) % size;
            let previous_column = (column + size - 1) % size;
            let next_column = (column + 1) % size;
            let total = current[index] * 4
                + current[previous_row * size + column]
                + current[next_row * size + column]
                + current[row * size + previous_column]
                + current[row * size + next_column];
            next[index] = total / 8;
        }
        current.copy_from_slice(&next);
    }
    let checksum: i64 = current.iter().sum();
    println!("{checksum}");
}
