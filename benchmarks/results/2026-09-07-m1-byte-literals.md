# Byte-literal C lexical-boundary repair

Status: repair validated through composite checkpoint verification; M1 remains in progress
Baseline: 010be59 (production compiler identical to 0f19fbc)
Baseline seed SHA-256: 3f1b6b59620d838d5164de0b974e865b85fdee7397c9c109e4c7fa769eee378d
Candidate seed SHA-256: 614ac65c7350c3c131d5393571344ead9e298f54dcf1576f91a195d31635675a

## Existing defects

The accepted Core byte-string grammar defines a hexadecimal escape with exactly
two digits. The baseline emits the original spelling into C, whose hexadecimal
escape consumes every following hexadecimal digit. Consequently,
`io.print_i64(bytes.len("\x00A"))` prints `1` instead of `2`, and `"\x41B"`
produces a C compilation error with the ordinary strict C11 flags.

Plain `"??/"` is another accepted SLIM Bytes value. In C11, its three characters
form a trigraph that changes the generated token into an unterminated string.
The other eight trigraph spellings also have meanings different from their
literal SLIM byte values. Both defects precede the vector/arena address repair.

## Lowering and local preservation argument

`codegen.emit_string_token` traverses the existing canonical string token.
After an active, valid two-digit hexadecimal escape, it inserts the adjacent C
string boundary `""` only when the following source character is hexadecimal.
It inserts the same boundary after the first question mark of a C trigraph.
For example, `"\x00A"` becomes `"\x00""A"`, and `"??/"` becomes `"?""?/"`.
C concatenates adjacent literals after recognizing escapes and trigraphs.
The runtime's static Bytes pointer and its `sizeof` length use the identical
lowered spelling, so embedded zero bytes retain their full length.

The traversal copies escape pairs together, so an escaped backslash cannot
activate a hexadecimal escape. It reads ahead only after checking the remaining
token extent. Every recurrence consumes at least one source byte, and additions
are within that checked extent. Each source byte incurs a fixed amount of work;
each detected boundary adds two generated C bytes. Literal runtime storage stays
static, and the value expression, evaluation order and runtime operations remain
unchanged. The existing vector/arena address helper uses this same value emitter.
No lexer acceptance rule, source escape, runtime ABI or diagnostic is added.
This restores accepted behavior under the feature policy's bug-fix exception.

## Permanent native test domains

`scripts/verify-hex-literals.py` checks an independent exact byte oracle through
the production SLIM compiler and the native C compiler. Each case prints its
byte length, a separator, the complete byte payload and a newline. A mismatch
reports the first differing output byte and the corresponding source literal.

- All 10,648 case-sensitive hexadecimal boundary spellings: 22 choices for
  each of two hexadecimal digits and 22 for the following hexadecimal character.
- All nine trigraph suffixes, question-mark runs of lengths two through eight,
  repeated sequences, escaped-backslash prefixes and hex-encoded prefixes:
  90 cases.
- 102 controls, including empty values, ordinary escapes, escaped backslashes,
  nonhexadecimal followers, decoded question marks and every printable ASCII
  third character that is not a trigraph suffix. Their C stays baseline-exact.
- Seven composites cross long hex followers, multiple escapes, quotes,
  backslashes and mixed hexadecimal/trigraph boundaries.
- Two exact ordered diagnostics, each through check and emit, cover a wrong
  storage element type and an unterminated source string.

Every generated program is emitted twice and compared byte-for-byte. Each native
program also crosses allocation ordinals 1..8: ordinal one fails argument-vector
allocation with the exact status-71 diagnostic and empty stdout; the remaining
seven preserve successful exact output. The matrix uses small helper functions
within the separately documented recursive name-resolution test domain.

The storage fixture now has twelve values in each of Vec[Bytes] and Arena[Bytes],
including a hexadecimal boundary and a trigraph in each container. Its 24 values
cross both growth boundaries and preserve exact bytes after insertion. All 64
allocation ordinals still give five exact failures and 59 successes, with the
existing four type/mode/effect rejection diagnostics unchanged. Cargo runs both
ordinary oracles; `scripts/verify.sh` runs both with ASan/UBSan on the production
compiler and generated native programs. These domains do not establish every
possible source spelling, arbitrary source size or unsupported escape behavior.

## Differential and cost evidence

The existing C identity differential passes on this seed: 96 accepted fixtures
preserve raw C and full analysis, including four relocation edits per fixture,
module relocation/reordering, and native serial, worker and declined-spawn paths.
All 197 rejected fixtures preserve exact status, stdout and stderr.

The portable seed grows from 4,433,406 to 4,444,746 C bytes: +11,340 bytes
(0.2558%). Seed refresh and independent bootstrap reach the same fixed point.
The full verification command was interrupted at the maintainer's request with
exit 143 during place fault ordinal 1,850. Earlier stages passed, including the
required benchmark commands; this does not count as a complete verification pass.
The resumed suffix exits zero on this same identified seed, completing every
remaining stage. This is composite verification; the original full invocation
remains interrupted.
The [interrupted log](archive/2026-09-07-m1-byte-literals-interrupted.log.gz)
and [resumed suffix command](archive/2026-09-07-m1-byte-literals-remaining.sh.gz)
preserve that distinction. The suffix restarts the complete place campaign,
then runs parsing, integers, source-index allocation and final native runtime checks.
The [completed suffix log](archive/2026-09-07-m1-byte-literals-remaining.log.gz)
and [gate/identity ledger](2026-09-07-m1-byte-literals-gates.tsv) record the outcome.

All eight required checkpoint commands passed in the original prefix. The suffix
passes 2,048 place faults (193 failures), 1,720 parser corpus/edit cases, 2,000
parser mutations, all geometric and capacity boundaries, and 2,048 parser faults
(94 failures). Decimal checks cover 456 values, 16 exact rejection diagnostics
and 512 fault ordinals (117 failures). Source-index checks cover 128 ordinals
(109 failures); the final native runtime and all six allocation failures pass.
These are bounded tested domains, not a universal safety proof.

Quiet same-host measurements use two identical baseline controls and the candidate,
two warmups per executable, 18 groups and all six rotating orders. Ordinary,
builtin and user-call function families cross 125/500/2,000/4,000 declarations;
the frozen baseline self-hosting project is also checked and emitted. All 26
measurements preserve identical output before timing. Candidate median ratios
range from 0.93402 to 1.02628. Every median above one stays below the corresponding
baseline-control upper quartile, so the existing rule triggers no expanded rerun.
Self-hosting check/emission ratios are 0.99811/0.99689. No performance exception
or general speedup is claimed.

The [raw timings](archive/2026-09-07-m1-byte-literals-latency.tsv.gz),
[26-row summary](2026-09-07-m1-byte-literals-latency-summary.tsv) and
[exact C differential](archive/2026-09-07-m1-byte-literals-differential.tsv.gz)
record the measured domains. Reproduction from the repository root:

```sh
python3 scripts/measure-continuations.py \
  build/slim-next-m1/hex-escapes/baseline/slimc build/toolchain/slimc \
  --work build/slim-next-m1/hex-escapes/latency-sources \
  --report build/slim-next-m1/hex-escapes/latency-repeat.tsv \
  --shapes functions builtin_functions call_functions --sizes 125 500 2000 4000 \
  --selfhost build/slim-next-m1/hex-escapes/baseline/selfhost/slim.project --groups 18
```

The baseline directory contains the production compiler and frozen selfhost sources
from `010be59`. Build both compilers with identical C flags and runtime, preserve
the identities recorded in the raw file, and run after other native jobs finish.

M1 remains in progress. RFC-0142 parameter-input queries, public host-bound
sessions and owning-epoch lifecycle, retained global/parallel results, native
backend caching and the final complete release gate remain required.
