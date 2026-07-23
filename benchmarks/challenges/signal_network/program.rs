#[derive(Clone, Copy)]
struct Signals {
    a: bool,
    b: bool,
    c: bool,
    d: bool,
    e: bool,
    f: bool,
    g: bool,
    h: bool,
}

fn run(mut remaining: i64, mut signals: Signals) -> Signals {
    while remaining > 0 {
        signals = Signals {
            a: !signals.h,
            b: signals.a,
            c: signals.b && signals.h,
            d: signals.c || signals.a,
            e: signals.d,
            f: !signals.e,
            g: signals.f && signals.c,
            h: signals.g || signals.b,
        };
        remaining -= 1;
    }
    signals
}

fn same(left: Signals, right: Signals) -> bool {
    left.a == right.a
        && left.b == right.b
        && left.c == right.c
        && left.d == right.d
        && left.e == right.e
        && left.f == right.f
        && left.g == right.g
        && left.h == right.h
}

fn main() {
    let seed = std::env::args_os().len() > 1;
    let left = run(
        2_000_000,
        Signals {
            a: seed,
            b: false,
            c: true,
            d: false,
            e: true,
            f: false,
            g: true,
            h: false,
        },
    );
    let right = run(
        2_000_000,
        Signals {
            a: false,
            b: seed,
            c: false,
            d: true,
            e: false,
            f: true,
            g: false,
            h: true,
        },
    );
    println!("{}", if same(left, right) { 1 } else { 0 });
}
