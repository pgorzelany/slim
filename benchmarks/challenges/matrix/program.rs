fn main() {
    let size = 128usize;
    let count = size * size;
    let left: Vec<i64> = (0..count).map(|index| (index as i64 * 17 + 1) % 97).collect();
    let right: Vec<i64> = (0..count).map(|index| (index as i64 * 31 + 1) % 97).collect();
    let mut total = 0i64;
    for row in 0..size {
        for column in 0..size {
            for index in 0..size {
                total += left[row * size + index] * right[index * size + column];
            }
        }
    }
    println!("{total}");
}

