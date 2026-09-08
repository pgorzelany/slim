# M1 native session integration and initial public costs — 2026-09-08

M1 remains incomplete. The default compiler now exposes RFC-0146 native builds,
with correct retention and recovery, but connection startup is still expensive.

## Checkpoint after capture batching

The rebuilt host identity is
`42a7af6f32eb89fb380fd7d3f99a73c9c16f2708bcd6d830ba5f2c5d4bd8a403`;
the seed is unchanged. Bootstrap, format, clippy, all 10 unit/75 integration tests,
governance, 338 conformance fixtures and 2,000 malformed-input mutations, library
corpus and all required checkpoint benchmarks pass across the recorded commands.
The current public native smoke passes cold/unchanged/edit/revert/reject/reset.
Full repository/release closure on the committed checkpoint is still required.

The initial full-verification attempt stopped at the owned-transfer normalized
performance ratio: 1.653 against the unchanged 1.30 limit. The compiler binary
is byte-identical to the initial timing baseline. Eighteen alternating diagnostic
pairs yield a 0.964 median ratio; the unchanged performance suite rerun passes at
0.991. No compiler code or gate was changed in response. A subsequent host benchmark
hit the sandbox's loopback restriction; its suffix passes with authorized loopback
access. [Checkpoint evidence](archive/2026-09-08-m1-native-checkpoint.json.gz)
preserves the failed runs, raw diagnostic pairs, successful suffix and identities.
The [final governance record](archive/2026-09-08-m1-native-checkpoint-govern.json.gz)
supersedes its initially incomplete governance log with the observed successful exit.

## Verified state

Seed refresh and bootstrap reach the same 4,877,338-byte fixed point as the private
candidate: `b418ef1cd9983e82bac293ffb50d76e144a765c319fe201d23b025340e93d2d0`.
The default host is byte-identical to that candidate, with identity
`ef953b79eee5a2235b047d4b4e6eb0b1217855dae523de89e8b558b202d1175f`.
The existing public-session quick check and complete expanded native aggregate pass.
This includes 20 applications / 80 corpus rows per ordinary and sanitized host,
input/driver/copy/process checks, native and SLIM allocation faults, state corruption,
real record/byte capacity, physical ownership, concurrent sessions and reset/quit.

The main verification and installed-release scripts now invoke platform-aware
native checks. The reference Darwin/arm64 Apple-clang-21 provider must pass positive
checks; other providers must return explicit unavailability while ordinary builds
still work. The reference smoke passes here. The other-platform branch and packaged
installation have not been executed in this pass. Full required checkpoint commands
and release closure remain outstanding; no checkpoint commit is claimed.

## Public timing method

The uninstrumented default `./slimc session` receives complete U/B exchanges. Timings
include executable extraction and publication to a fresh inode; build-plus-run also
includes execution. Source edits use the same project/file path. Controls invoke
ordinary `./slimc build` through the system `cc` wrapper, with the same emitted C,
worker tier, executable basename and checked output. Both sides produce stable
binary bytes across repeated samples. Five samples per case follow a warmup;
controls alternate before/after the retained sequence. No other agent-driven test
or build job ran concurrently. Absolute times describe this Darwin/arm64 host only.

Each measured retained sequence resets query state, then builds cold, unchanged,
edited and reverted source using the already captured tool context. Reset time is
outside those build measurements. Five separate fresh connections measure setup.
Body builds start program compilation and linking while reusing the runtime object;
unchanged/reverted builds start no backend tools. These are workflow measurements,
not an agent-task success rate or proof of a universal productivity improvement.

## Results

Build times below include U/B framing and publication. Speedups use the median of
paired ratios, so they need not equal the ratio of the displayed rounded medians.

| Case | Ordinary build ms | Edited build ms | Unchanged build ms | Edited build faster | Build + run faster | Estimated setup recovery: edits |
|---|---:|---:|---:|---:|---:|---:|
| Helpers: 125 | 161.62 | 84.55 | 0.98 | 47.5% | 28.7% | 142 |
| Helpers: 250 | 168.15 | 91.13 | 1.12 | 46.1% | 28.0% | 141 |
| Helpers: 500 | 173.84 | 101.36 | 1.35 | 41.9% | 26.2% | 149 |
| Helpers: 1000 | 189.31 | 114.64 | 2.01 | 40.1% | 25.2% | 143 |
| Helpers: 2000 | 227.71 | 148.02 | 3.03 | 33.3% | 21.5% | 143 |
| Helpers: 4000 | 278.01 | 217.13 | 5.76 | 22.1% | 13.2% | 179 |
| State machine, serial | 175.95 | 90.02 | 0.83 | 50.0% | 29.3% | 130 |
| State machine, POSIX | 180.04 | 84.21 | 0.81 | 52.4% | 31.7% | 113 |
| Signal network, serial | 182.49 | 86.97 | 0.85 | 52.0% | 32.0% | 116 |
| Signal network, POSIX | 174.82 | 88.98 | 0.81 | 49.5% | 32.4% | 127 |

Median context setup is **10.84 seconds**. The recovery estimates divide that
setup cost by median paired body-build savings; they exclude reset cost and do not
predict every editing history. Unchanged/reverted builds recover setup faster,
but short sessions can still lose overall. The next performance work should profile
and reduce capture setup without weakening complete-input validation or any gate.
Program C still recompiles after a body edit; these results do not establish native
compilation locality within that translation unit. Earlier frontend-only limitations
and existing performance budgets remain in force.

## Resources and evidence

Subsequent phase profiling identified about 2.2 seconds spent launching one `awk`
per captured dependency. One exact manifest index per report reduces median
standalone capture from 10.799 to 8.774 seconds across three fresh captures per
version (about 19%); membership validation falls from 2.245 seconds to 57 ms.
These timings include verification-only phase markers and are separate from the
public baseline above. All 77 driver/input cases pass, including exact first errors,
empty manifests, 512-member reversal/duplicates and malformed names. A final invalid
unterminated record is now checked instead of silently ignored by shell `read`.
All production-generated reports terminate their lines. Input and report limits
are unchanged. [Profiling and preservation evidence](archive/2026-09-08-m1-native-capture-batching.json.gz)
retains both recipes and measurements. Integrated revalidation is pending.

The optimized resource observer reaches 63,464,207 charged native bytes with
125,988,576 allocated bytes including runtime allocation headers; further entries
are declined while complete artifacts remain usable. Reset observes exactly those
frees, and unchanged B requests clone no native bytes. Optimized peak RSS is about
209 MB in that campaign; it includes frontend and allocator costs and need not fall
after free. Each connection captures 224,339,295 input bytes. Per-file disk blocks
can describe shared clone extents and are not unique physical storage.

[Resource evidence](archive/2026-09-08-m1-native-host-resources.json.gz) preserves
those independent observations. [Default integration and timing evidence](archive/2026-09-08-m1-native-session.json.gz)
contains exact identities, bootstrap/default aggregate output, raw samples and prior
attempts. The first timing attempt incorrectly bypassed SDK selection by invoking
Clang directly. The second mistook successful SDK notices for a failed build. The
third completed, but unique control basenames changed binary identities and affected
execution comparisons; its ratios are not used here. The final run records SDK
notices, uses system cc, and requires stable control artifacts with a common basename.
