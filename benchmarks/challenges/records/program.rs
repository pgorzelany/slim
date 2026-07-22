#[derive(Clone, Copy)]
struct State {
    x: i64,
    y: i64,
    vx: i64,
    vy: i64,
}

fn main() {
    let mut state = State { x: 0, y: 0, vx: 11, vy: 17 };
    for _ in 0..1000000 {
        state.x += state.vx;
        state.y += state.vy;
        state.vx += 3;
        state.vy -= 2;
    }
    println!("{}", state.x + state.y);
}
