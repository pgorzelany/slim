enum Command { Keep, Add(i64), Remove(i64) }

fn main() {
    let mut value = 0i64;
    for index in 0..2000000i64 {
        let command = match index % 3 {
            0 => Command::Add(index % 17),
            1 => Command::Remove(index % 13),
            _ => Command::Keep,
        };
        value = match command {
            Command::Keep => value,
            Command::Add(amount) => value + amount,
            Command::Remove(amount) => value - amount,
        };
    }
    println!("{value}");
}
