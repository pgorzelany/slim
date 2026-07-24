fn main() {
    let size = 128usize;
    let count = size * size;
    let mut current = vec![false; count];
    let mut next = vec![false; count];
    for (index, value) in current.iter_mut().enumerate() {
        *value = (index * 17 + 3) % 3 == 0;
    }
    for _ in 0..100 {
        for index in 0..count {
            let row = index / size;
            let column = index % size;
            let mut neighbors = 0usize;
            for offset in 0..9 {
                if offset == 4 {
                    continue;
                }
                let row_delta = offset / 3;
                let column_delta = offset % 3;
                let neighbor_row = (row + row_delta + size - 1) % size;
                let neighbor_column = (column + column_delta + size - 1) % size;
                neighbors += usize::from(current[neighbor_row * size + neighbor_column]);
            }
            next[index] = neighbors == 3 || (current[index] && neighbors == 2);
        }
        current.copy_from_slice(&next);
    }
    let checksum = current.iter().filter(|value| **value).count();
    println!("{checksum}");
}
