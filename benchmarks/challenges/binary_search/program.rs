fn search(values: &[i64], target: i64) -> i64 {
    let mut left = 0usize;
    let mut right = values.len();
    while left < right {
        let middle = left + (right - left) / 2;
        if values[middle] == target {
            return middle as i64;
        }
        if values[middle] < target {
            left = middle + 1;
        } else {
            right = middle;
        }
    }
    -1
}

fn main() {
    let values: Vec<i64> = (0..200000).map(|value| value * 2).collect();
    let mut total = 0i64;
    for index in 0..20000i64 {
        let target = (index * 7919) % values.len() as i64;
        total += search(&values, target * 2);
    }
    println!("{total}");
}
