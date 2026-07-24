fn minimum_three(first: i64, second: i64, third: i64) -> i64 {
    first.min(second).min(third)
}

fn main() {
    let left =
        b"thequickbrownfoxjumpsoverthelazydogpackmyboxwithfivedozenliquorjugsphinxofblackquartzjudgemyvow";
    let right =
        b"thequickbluefoxjumpedoverthehazydogpacktheboxwithsixdozenliquorjugssphinxofbrightquartzjudgeourvow";
    let mut previous: Vec<i64> = (0..=right.len() as i64).collect();
    let mut current = vec![0i64; right.len() + 1];
    let rounds = 200 * std::env::args().count();
    let mut total = 0i64;
    for _ in 0..rounds {
        for (index, value) in previous.iter_mut().enumerate() {
            *value = index as i64;
        }
        for row in 1..=left.len() {
            current[0] = row as i64;
            for column in 1..=right.len() {
                let cost = i64::from(left[row - 1] != right[column - 1]);
                let deletion = previous[column] + 1;
                let insertion = current[column - 1] + 1;
                let substitution = previous[column - 1] + cost;
                current[column] = minimum_three(deletion, insertion, substitution);
            }
            previous.copy_from_slice(&current);
        }
        total += previous[right.len()];
    }
    println!("{total}");
}
