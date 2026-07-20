fn main() {
    let limit = 100_000i64;
    let mut queue = Vec::with_capacity((limit + 1) as usize);
    queue.push(0);
    let mut total = 0i64;
    let mut index = 0;
    while index < queue.len() {
        let node = queue[index];
        total += node;
        let left = node * 2 + 1;
        let right = left + 1;
        if left <= limit {
            queue.push(left);
        }
        if right <= limit {
            queue.push(right);
        }
        index += 1;
    }
    println!("{total}");
}

