fn main() {
    let capacity = 2048usize;
    let mut values = vec![0i64; capacity + 1];
    for item in 1..=200i64 {
        let weight = (1 + (item * 17) % 97) as usize;
        let value = 1 + (item * 29) % 211;
        for current in (weight..=capacity).rev() {
            let candidate = values[current - weight] + value;
            if candidate > values[current] {
                values[current] = candidate;
            }
        }
    }
    println!("{}", values[capacity]);
}
