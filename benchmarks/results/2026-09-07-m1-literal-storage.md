# Literal Bytes storage-address repair

Status: address repair validated; M1 remains in progress
Baseline: bdb18d2
Baseline seed SHA-256: 59d895508fa9011dea2d833738e4ea6a7da72829412130de48d790685b001706
Candidate seed SHA-256: 3f1b6b59620d838d5164de0b974e865b85fdee7397c9c109e4c7fa769eee378d

## Existing defect and repair

Both the baseline and candidate checker accept a literal Bytes value as the
element of `vec.push` and `arena.add`. The baseline C emitter instead requests
the address of a nonexistent binding named after the literal. Compiling the
new native fixture with the baseline produces undeclared-identifier errors.
The defect was first recorded during RFC-0141 validation.

`codegen.emit_value_address` now recognizes the canonical string node and emits
the address of element zero in a one-element C11 compound array of `SlimBytes`.
The initializer calls the existing literal-value emitter once. The descriptor
has automatic block lifetime through the synchronous vector/arena operation;
the existing runtime copies it before returning. Its byte payload has static
string-literal storage. This adds no runtime allocation or language operation.
The other address cases retain their previous C text. This is an accepted-
behavior repair under the feature policy's bug-fix exception, not a new RFC.

## Permanent regression domain

`tests/fixtures/literal_storage.slim` inserts ten values in each container,
crossing the initial eight-element growth boundary. Direct literals cover empty
bytes, a builtin-like spelling, Boolean- and integer-like spellings, quotes,
backslashes, newline/carriage-return/tab, embedded zero and `0xff`. Named Bytes
and a function-returned Bytes value cover the existing binding and temporary
paths. Reads occur after insertion and growth, with independent exact expected
output bytes in `scripts/verify-literal-storage.py`.

The script checks deterministic C, exact native output and all allocation-fault
ordinals 1..64. There are exactly five failures: the argument-vector allocation
and two growth allocations each for the tested vector and arena. Every failure
has status 71, empty stdout and the existing exact allocation diagnostic; the
remaining 59 positions succeed. Literal descriptors add no heap allocation in
this measured domain. Four invalid sources preserve the baseline's exact ordered
diagnostics for vector/arena element type, mutation marker and effect ceiling.
Both check and emit reject them. Cargo runs the ordinary native campaign; the
full verification script runs the compiler and generated fixture with ASan/UBSan.

## Differential and costs

The existing identity differential preserves raw C bytes and full analysis for
96 accepted fixtures, including four relocation edits per fixture and module
relocation/reordering. All 197 rejected fixtures retain exact status/stdout/stderr.
Native serial, worker and declined-spawn paths pass. This does not compare the
new positive fixture's native behavior with the baseline: its baseline C does
not compile.

Seed size changes from 4,432,372 to 4,433,406 C bytes: +1,034 bytes (0.0233%).
The seed refresh and independent bootstrap both reach a deterministic fixed
point. No general performance improvement or complete M1 result is claimed.

Quiet same-host measurements use two identical baseline runs and one candidate,
two warmups per executable, 18 groups and all six rotating orders. Three source
families (ordinary, builtin and user-call functions) cross 125/500/2,000/4,000
declarations; the frozen bdb18d2 self-hosting project is also measured. Both
checking and emission are compared, with identical outputs required first.
Across the 26 measurements, candidate medians range from 0.99112 to 1.01689 of
the paired baseline. Every median above one is below its identical-baseline
control's upper quartile. Self-hosting check/emission ratios are 1.00310/0.99981,
inside their respective control intervals. No expanded repeat was triggered by
the recorded above-one/above-control-quartile rule.

The [raw timings](2026-09-07-m1-literal-storage-latency.tsv),
[summaries](2026-09-07-m1-literal-storage-latency-summary.tsv) and
[exact C differential](2026-09-07-m1-literal-storage-differential.tsv) retain
the measured scope. This is compiler process latency, not a native speedup for
the newly repaired fixture, whose baseline C fails to compile.

## Checkpoint validation

The complete `./scripts/verify.sh` invocation exits zero on the identified seed.
This includes all eight required checkpoint commands, 74 integration tests,
338 conformance fixtures and 2,000 malformed-input mutations, library and extended
benchmark gates, all continuation regressions, and the full sanitizer/recovery
suffix. The new ordinary and sanitized native fixture each passes all 64 fault
positions. Existing flow, retained typing/project, session, place, parser,
integer and runtime failure campaigns retain their previous expected outcomes.
The [gate and identity ledger](2026-09-07-m1-literal-storage-gates.tsv) records
the exact domains, source/tool hashes and full-run log identity. No gate or
performance budget is waived.
Public sessions, retained global analyses, native artifact caching and the full
M1 release gate remain separate obligations.

## Newly identified, separate escape defect

The address regression deliberately names its exact tested byte sequences; it
does not establish every string escape boundary. A subsequent boundary check
found that `io.print_i64(bytes.len("\x00A"))` prints `1` on both bdb18d2 and this
candidate. SLIM's fixed two-digit escape denotes a zero byte followed by `A`,
so the required length is `2`. The existing literal-value emitter copies the
source token into C, whose hexadecimal escape consumes the following hex digit.
This affects ordinary literals independently of vector or arena addressing.
It remains an explicit M1 repair obligation. The private baseline/candidate
reproducers are in `build/slim-next-m1/literal-storage/hex-following-*`; this
minimal source expression and the two identified seeds reproduce the result.
