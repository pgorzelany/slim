fn main() { let value = true; std::process::exit(match value { true => 0, false => 1 }); }
