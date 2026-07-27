# RFC-0085: Proof-directed arithmetic lowering

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 90

## Need

The expanded RFC-0084 corpus retains checked integer helpers in hot generated C
even when the checked `ranges` view has already proved the exact operation
total. This duplicates work and prevents the host optimizer from seeing
ordinary arithmetic. The largest measured effect is in integer stencil code:
the full-run `image_convolution` SLIM/C ratio is 1.596 before this change.

Code generation may emit direct C `+`, `-`, `*`, `/`, or `%` only when the
shared range fact for that exact checked call node is total. Missing facts,
unsupported transfers, saturated bounds, out-of-domain values, and unknown
operands retain the existing checked runtime helper. This is a lowering choice,
not a second safety profile.

Closed-program range analysis also propagates an exact scalar parameter when
every checked user call supplies the same exact total value and every
recurrence passes that parameter unchanged. Four fixed propagation passes are
the complete budget. A changed recurrent argument, conflicting call sites,
unknown argument, or deeper call chain remains unknown.

## Alternatives

Removing arithmetic checks from all release builds would make C undefined
behavior part of SLIM and is rejected. Re-running a separate optimizer range
analysis would duplicate semantic authority. Constant substitution in
generated C was prototyped and removed: it produced cleaner text but no
measured runtime benefit because the host compiler already propagated those
values.

Increasing the propagation budget until a particular application converges is
also rejected. The fixed pass count preserves predictable compiler work and
leaves deeper facts explicitly unknown.

## Costs

Ordinary C emission now constructs the existing bounded range view and threads
its dense fact vector through lowering. Parameter propagation performs five
bounded analyses: the initial pass plus four propagation passes. The analysis
report publishes `parameter-pass-limit 4`.

The 2,000-declaration quick emission/check ratio is 2.404, below the permanent
4.00 budget, and generated-emission scaling remains inside its 1.25 exponent
gate. The compiler and bootstrap seed grow, but the language surface, runtime
ABI, dependency set, source syntax, and default safety rules do not change.

## Evidence

The full twenty-application candidate run measures a 1.359 geometric-mean
SLIM/C ratio and 1.439 across the eighteen serial applications, improving the
expanded baseline of 1.371 and 1.450. `image_convolution` improves from 1.596
to 1.171. The candidate is still short of RFC-0084's parity criterion and is not
reported as C parity.

Positive tests require proven literal and closed-call arithmetic to emit direct
C. Negative tests require unknown arithmetic, conflicting call-site constants,
changed recurrent parameters, and facts beyond the four-pass boundary to retain
the checked helper. Bootstrap fixed point, exact output, trap fixtures,
parallel-blocker evidence, quick compiler-performance gates, and the complete
repository gate remain mandatory.

## Removal

Remove a direct lowering if its fact is not tied to the exact call node, if a
supported input changes its trap or output, or if bounded compiler scaling
regresses. Remove parameter propagation if its whole-program call scan becomes
unbounded or if an incomplete call/recur shape is treated as positive
evidence. Retain the expanded corpus, dated results, and tightened budgets even
if either optimization is reverted.
