fn main() {
    let count = 100000usize;
    let mut arena = Vec::with_capacity(count);
    let mut ids = Vec::with_capacity(count);
    for index in 0..count {
        arena.push((index as i64 * 37) % 101);
        ids.push(index);
    }
    let total: i64 = ids.into_iter().map(|identity| arena[identity]).sum();
    println!("{total}");
}
