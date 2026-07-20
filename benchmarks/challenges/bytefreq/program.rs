use std::env;
use std::fs;

fn main() {
    let path = env::args().nth(1).expect("input path");
    let input = fs::read(path).expect("read input");
    let mut counts = [0i64; 256];
    for byte in input {
        counts[byte as usize] += 1;
    }
    let total: i64 = counts
        .iter()
        .copied()
        .enumerate()
        .map(|(index, count)| count * (index as i64 + 1))
        .sum();
    println!("{total}");
}
