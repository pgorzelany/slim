# D0022: SLIM-first self-hosting

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 60

## Need

The restricted SLIM compiler has proved the bootstrap, but new compiler
capabilities have continued to land primarily in Rust. That weakens the
evidence that Core is sufficient for serious implementation work and delays
finding deficiencies in SLIM itself.

Core 0.3 makes the self-hosted compiler the default implementation site for
compiler capabilities. Rust stage 0 remains the trusted bootstrap seed and
independent behavioral oracle.

## Alternatives

Continuing to complete every feature in Rust before porting it to SLIM makes
early implementation easier, but increases the porting backlog and permits the
two compilers to acquire different architectures. Rewriting stage 0 at once
would discard the differential oracle and make regressions harder to isolate.

## Costs

During the transition the repository carries two implementations. The SLIM
implementation may initially be less polished and must pass a bootstrap fixed
point plus differential conformance. Production Rust is held to the byte
ceilings in `design/rust-budget.tsv`; increasing a ceiling requires an explicit
accepted architecture decision.

Rust remains allowed for the bootstrap harness, conformance and benchmark
runners, governance, operating-system integration, and corrections required to
keep stage 0 a faithful oracle. Language capability must not silently fall back
to stage 0.

## Evidence

The first Core 0.3 slice teaches the SLIM compiler to emit records, variants,
`make`, `get`, `case`, variant `match`, `set`, storage operations, and canonical
formatting. Its lexer replaces three parallel vectors with one `Vec Token`, so
the compiler directly uses the record feature. The corresponding conformance
fixtures pass through the self-hosted compiler, and stage 2 and stage 3
generated C remain byte-identical.

## Removal

Remove the Rust ceiling only if SLIM-first implementation prevents a required
safety or bootstrap property and a replacement policy provides stronger
measurable dogfooding evidence. Retire stage 0 as an active implementation only
after the SLIM compiler reaches complete differential parity.
