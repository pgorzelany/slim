fn gcd(mut left: i64, mut right: i64) -> i64 {
    while right != 0 {
        let next = left % right;
        left = right;
        right = next;
    }
    left
}

fn fib(mut remaining: i64, mut left: i64, mut right: i64) -> i64 {
    while remaining != 0 {
        let next = left + right;
        left = right;
        right = next;
        remaining -= 1;
    }
    left
}

fn main() {
    println!("{}", gcd(1071, 462) + fib(40, 0, 1));
}

