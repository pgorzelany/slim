# RFC-0093: Relational benchmark recurrences

Status: rejected
Implementation: not-planned
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 0
Runtime: 2
Minimal: 1
Analysis: 2
Dogfood: 0
Score: 65

## Need

Many native SLIM fixtures express an increasing counted recurrence with an
equality terminal guard, even though their matched C and Rust loops use a
relational bound. Equality leaves every overshoot state live: an out-of-domain
starting controller or future non-unit step continues until checked overflow.
It also withholds the false-branch upper bound that SLIM's existing analyzer can
use to prove the controller increment.

Normalize benchmark recurrence guards, not language semantics. An increasing
controller that advances toward an exclusive upper bound uses `i64.ge`; a
decreasing controller that advances toward zero uses `i64.le`. Ordinary value
equality, search matches, tags, roots, and algorithmic equality remain
unchanged.

## Alternatives

The compiler cannot globally replace equality with order without changing
program behavior. RFC-0089 tested closed-call equality induction, but its complete
corpus result did not justify the extra analyzer surface. Adding a loop form,
range syntax, contract, or lint would add language/tool surface for a fact the
canonical recurrence can already state directly.

Keeping equality preserves the current benchmark outputs but represents more
invalid state and differs from the termination relation used by both reference
implementations.

## Costs

This changes only maintained benchmark SLIM programs. It adds no compiler pass,
syntax, dependency, runtime branch, allocation, ABI, or permanent second
spelling. Closed-program output must remain byte-identical. Every changed guard
must control a monotonic recurrence toward the named bound; a non-controller
equality is outside this decision.

The complete native comparison, application analysis baseline, resource
baseline, and parallel blocker baseline must be regenerated or shown unchanged.
Retention requires an aggregate improvement rather than a single fixture win.

## Evidence

On the dominant Game of Life fixture, changing all six counted terminal guards
to relational bounds preserves output and lowers the median from about 21.3 ms
to 20.0 ms on the recorded host. Generated loop increments become direct
arithmetic only where the existing bounded range view proves them total.

The corpus-wide candidate is accepted only after same-host measurement and the
complete repository gate. Its source normalization is independently valuable:
invalid overshoot inputs terminate at the declared boundary instead of
recurring toward an eventual overflow trap.

The 2026-07-24 quick corpus measured 1.277 against C overall and 1.343 on serial
work, outside the retained RFC-0087 quick band around 1.263 and 1.320. Game of Life
improved by about 6% and byte frequency by about 14%, but image convolution and
the aggregate did not. Benchmark sources must not be normalized toward an idiom
that happens to help selected fixtures, so all sixteen source changes were
removed. The compiler may still infer the same relation when it has a complete
behavior-preservation proof.

## Removal

Restore a guard only if its controller is not monotonic toward the named bound,
the relational form changes a supported closed output, or aggregate native
performance regresses materially. Do not apply this decision to ordinary
equality or add a second loop syntax.
