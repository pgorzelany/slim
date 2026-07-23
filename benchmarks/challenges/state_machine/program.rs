#[derive(Clone, Copy)]
struct State {
    a: bool,
    b: bool,
    c: bool,
    d: bool,
}

fn run(mut remaining: i64, mut state: State) -> State {
    while remaining > 0 {
        state = State {
            a: state.b,
            b: state.c,
            c: state.d,
            d: !state.a,
        };
        remaining -= 1;
    }
    state
}

fn same(left: State, right: State) -> bool {
    left.a == right.a && left.b == right.b && left.c == right.c && left.d == right.d
}

fn main() {
    let seed = std::env::args_os().len() > 1;
    let left = run(
        2_000_000,
        State {
            a: seed,
            b: false,
            c: true,
            d: false,
        },
    );
    let right = run(
        2_000_000,
        State {
            a: false,
            b: seed,
            c: false,
            d: true,
        },
    );
    println!("{}", if same(left, right) { 1 } else { 0 });
}
