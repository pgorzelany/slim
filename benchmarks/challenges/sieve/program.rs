fn main() {
    let limit = 1_000_000usize;
    let mut flags = vec![true; limit + 1];
    flags[0] = false;
    flags[1] = false;
    let mut prime = 2;
    while prime * prime <= limit {
        if flags[prime] {
            let mut multiple = prime * prime;
            while multiple <= limit {
                flags[multiple] = false;
                multiple += prime;
            }
        }
        prime += 1;
    }
    println!("{}", flags.into_iter().filter(|value| *value).count());
}

