fn find_root(parents: &[usize], mut node: usize) -> usize {
    while parents[node] != node {
        node = parents[node];
    }
    node
}

fn main() {
    let count = 200000usize;
    let mut parents: Vec<usize> = (0..count).collect();
    for index in 0..count {
        let partner = (index * 17 + 23) % count;
        let left_root = find_root(&parents, index);
        let right_root = find_root(&parents, partner);
        if left_root != right_root {
            parents[right_root] = left_root;
        }
    }
    let mut total = 0usize;
    for _ in 0..10 {
        for index in 0..count {
            total += find_root(&parents, index);
        }
    }
    println!("{total}");
}
