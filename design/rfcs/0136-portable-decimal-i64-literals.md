# RFC-0136: Portable decimal I64 literals

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: Codex, implementing the approved SLIM Next M1 goal
Created: 2026-09-07
DecisionDate: 2026-09-07
Approver: project-maintainer
Kind: compatibility
Primitive: none
Safety: 1
Compile: 0
Runtime: 0
Minimal: 0
Analysis: 0
Dogfood: 0
Score: 10

## Summary

Restore the documented decimal signed-I64 contract at the checker and C emitter.
Accept every representable decimal value, including minimum I64 and leading zeros.
Reject unrepresentable literals with E0361 before producing executable C.

## Motivation

The M1 source-identity checkpoint recorded that minimum I64 is accepted by SLIM
but emitted as an unsigned C token rejected by strict compilation. Reproduction
at ab7758c also finds decimal `0009` rejected as invalid C octal, and `0010`
interpreted as eight through C's octal rules. Literal widths are not checked at all.
These defects must not become retained generated artifacts.

## Guide-level explanation

Literals remain decimal I64 values with an optional minus sign. Leading zeros
do not change the base. The inclusive range is -9223372036854775808 through
9223372036854775807. A literal outside it receives a diagnostic at its complete
source token. This adds no syntax, conversion, integer type or runtime operation.

## Reference-level specification

The canonical lexer still establishes the optional-sign/nonempty-digit grammar.
A shared source-span helper skips the optional minus and leading zeros, retaining
one digit for zero. It does not change canonical source bytes or token spans.
The normal type checker compares significant decimal digit count with 19, then
lexicographically with the appropriate signed bound. It never accumulates the
literal in machine arithmetic. Comparison reads at most 19 significant digits;
leading-zero traversal is linear in the token and uses the existing source bound.

An out-of-range atom adds E0361 at that exact token. The checker may retain its
I64 type for diagnostic recovery, but the issue prevents accepted checking and
emission. Retained checks consume the same ordinary operation on changed bodies;
old results require the existing compiler-configuration identity boundary.

The emitter uses the same significant-digit span. Ordinary non-minimum decimal
tokens retain the existing INT64_C form after removing leading zeros. Negative
zero may retain its minus. Minimum I64 emits
`(-INT64_C(9223372036854775807) - INT64_C(1))`.
Both constants and both arithmetic results are representable signed I64 values;
the expression is a C constant with no SLIM runtime evaluation, effect, allocation
or trap. Parentheses preserve enclosing precedence. No unsigned literal, cast,
runtime negation or C text post-processing is used.

## Compiler and runtime design

Implement source-span traversal in syntax, range validation in typing, and
literal emission in codegen. The same normal checker remains the sole semantic
authority. No new module, dependency, runtime ABI, primitive or Rust semantics.
Regenerate the portable seed. Default and retained project checking share the fix.

## Compatibility and migration

This explicitly repairs erroneous checker acceptance outside the documented I64
domain. At ab7758c, `9223372036854775808` passes check and emission but fails strict
native compilation; it is not a supported unsigned value. The new source diagnostic
replaces that backend failure. Representable programs preserve decimal meaning,
including previously miscompiled leading-zero literals and minimum I64. Ordinary
non-minimum tokens without leading zeros retain identical generated C. No source
span, formatting, analysis proof domain or arithmetic trap policy changes.

## Diagnostics and failure cases

E0361 means an integer literal is outside the signed I64 range. Its primary span
includes the optional sign and every leading zero. Source grammar errors keep
their existing parser priority. Rejected candidates publish neither C nor a new
successful snapshot. Allocation failure retains the existing status-71 behavior.

## Performance and complexity

No retained table is added. Width checking and output normalization are linear
in literal bytes, with at most 19 significant-digit comparisons. Measure ordinary
checks/emission against ab7758c and retain every existing performance gate. No
speedup or budget relaxation is claimed.

## Alternatives and drawbacks

Special-casing only the exact minimum spelling leaves equivalent leading-zero
spellings broken. Relying on C's token grammar permits octal reinterpretation and
unsigned values. Accumulating positive magnitude can overflow on minimum I64.
Rejecting leading zeros would unnecessarily remove valid decimal source.

## Test and acceptance plan

Exercise minimum/maximum, adjacent values, negative zero, long leading-zero runs,
all decimal digits and both out-of-range boundaries. Compare runtime values with
independently specified decimal expectations through strict C11 and ASan/UBSan.
Pin E0361 and exact spans for both signs, leading zeros and very long magnitudes.
Preserve existing complete diagnostics, native analysis and generated C where
unaffected. Test failed retained updates and recovery with minimum/leading-zero
and out-of-range bodies. Run bootstrap, required compiler checkpoint gates,
conformance, appropriate sanitizer checks and bounded allocation faults.

## Ratings and evidence

Safety +1 repairs incorrect accepted value representation. Other ratings remain
zero. Weighted score 10;
this is a contract repair, not a new language primitive or performance exception.

## Decision

Accepted under RFC-0112's maintainer delegation and the explicit M1 goal. This
records the checker-acceptance correction above; it does not authorize unrelated
source incompatibilities or relax any hard gate. It is not external review.

## Implementation

Implemented in production syntax/typing/codegen and the reproducible portable
seed. The fixed point is 4,153,396 bytes, SHA-256
`c3c8af2d0cc1c3164b068d1fbc1f0d651bdadfcb2eca94fd31915d53e29262a7`.

The permanent literal probe checks 456 spellings against independent decimal
expectations at C11 O0/O2 and under ASan/UBSan. It covers both boundaries and
65,536 leading zeros. Sixteen out-of-domain cases preserve exact E0361 native
and JSON source spans. All 512 allocation-fault ordinals agree between ordinary
and sanitized compilers (116 failures, 396 successes). The full session matrix
covers 95 accepted fixtures, literal edit/rejection/recovery and 2,048 allocation
faults (471 failures, 1,577 successes), with complete prepared-state/C equality.

All required compiler checkpoint gates pass, including 10 unit and 71 integration
tests, 338 conformance fixtures and 2,000 malformed mutations. The prior 74 accepted
conformance sources have identical checking and C; 193 prior rejected sources
retain exact diagnostics, and 20 native applications retain complete analysis/C.
Paired 4,000-helper process medians are 20.178/20.239 ms for baseline/current checks
and 37.877/37.574 ms for C emission. These are observations, not a speedup claim.
The [progress report](../../benchmarks/results/2026-09-05-slim-next-progress.md)
links the dated evidence and retains all larger M1 obligations.

## Removal and supersession

Preserve exact decimal values across the whole I64 domain, source diagnostics
outside it, strict portable emission, current spans and retained-update recovery.
