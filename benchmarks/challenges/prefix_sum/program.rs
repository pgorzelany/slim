fn main() {
    let mut values: Vec<i64> = (0..250000i64).map(|index| (index * 31) % 97).collect();
    let mut total = 0i64;
    for value in &mut values {
        total += *value;
        *value = total;
    }
    println!("{total}");
}
