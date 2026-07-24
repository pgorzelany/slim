fn main() {
    let size = 192usize;
    let count = size * size;
    let mut distances = vec![0i64; count];
    for (index, distance) in distances.iter_mut().enumerate() {
        let row = index / size;
        let column = index % size;
        *distance = if row == column {
            0
        } else {
            1 + ((row * 17 + column * 31) % 97) as i64
        };
    }
    for pivot in 0..size {
        for row in 0..size {
            for column in 0..size {
                let target = row * size + column;
                let candidate =
                    distances[row * size + pivot] + distances[pivot * size + column];
                if candidate < distances[target] {
                    distances[target] = candidate;
                }
            }
        }
    }
    let checksum: i64 = distances.iter().sum();
    println!("{checksum}");
}
