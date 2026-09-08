# M1 release candidate verification — 2026-09-08

M1 remains in progress. At clean checkpoint
`b9dc7568d2833e49ee7a1e82acb65ac3ca9c8878`, `scripts/verify-0.9.sh` passed the entire
repository verification and reproducible package/clean-install checks, then failed
website generation: maintained documentation contained 17,664 words against the
existing 16,000-word limit. This invocation exited 1; it is not a full release pass.

The incremental guide now describes the current session and links its detailed
RFC contracts, replacing repeated historical implementation paragraphs. Website
generation reports 15,870 maintained words, and the complete website build and all
18 tests pass. No documentation limit, compiler behavior or performance gate changed.

A completion audit also found that the substantive native-selection oracle was
absent from the main verifier. It now runs beside native-cache/platform checks.
The unchanged oracle passes ordinary and ASan/UBSan variants using the current
sanitized production compiler. It verifies stale/extreme handles, rejected updates
and explicit last-good selection, worker profiles, corrupt snapshot/C metadata,
reset and zero allocation during selection. It ran after the full invocation's
latency stages finished; it did not overlap those measurements. Shell syntax and
governance pass. A complete clean release invocation including the added hook and
shortened documentation is still required by RFC-0124.

## Current evidence

| Obligation | Result in the identified invocation |
| --- | --- |
| Production authority and baseline | Bootstrap fixed point, formatting, Clippy, 10 unit/75 integration tests, governance, 338 conformance fixtures/2,000 mutations and library corpus passed. |
| Permanent performance contracts | All benchmark-prefix gates passed, including observed work, reduction, complete native analysis/resource baselines, generated parallel runtime, comparisons and agent-tool measurements. |
| Identities and checking | Continuation, ownership, checked-state, literal, identity/map/flow, retained typing/input and complete analysis-consumer campaigns passed. |
| Public sessions | Ordinary/sanitized edit, identity, transport, limit, resource, failure/recovery and concurrent host-publication checks passed. |
| Internal sessions | All 6,144 fault ordinals passed: cold 555 injected failures/1,493 successes; update 1,033/1,015; fragment 995/1,053. |
| Parallel/native queries | 323 complete parallel comparisons and import faults passed; native ordinary/sanitized 38-case, ownership, fault, produced-artifact and exact byte-boundary checks passed. |
| Native host | Process/copy/input capture, 77 driver cases, original-path removal independence, both 20-application/80-row corpora, observed exec counts, faults, recovery, corruption, record/byte limits, concurrency and physical cleanup passed. |
| Parsing and places | Extended place campaign passed. Parser corpus covered 1,720 full token/diagnostic comparisons and 2,000 mutations; exact source/token/lexeme boundaries and 2,048 fault ordinals passed. |
| Integer/runtime suffix | Independent decimal-output/rejection checks and 512 fault ordinals passed, followed by source-index and final runtime allocation checks. |
| Packaging | Two source archives were byte-identical; source hashes, clean bootstrap, installed frontend/native behavior and ABI rejection passed. Archive SHA-256: `09ff22c7fffbf06678f110180db05dd2ba5507f91db4c30aba8e8352762a9385`. |
| Website | Failed the prose limit in the full invocation. The documentation repair subsequently passed generation, build and all 18 tests. |

The seed is `b418ef1cd9983e82bac293ffb50d76e144a765c319fe201d23b025340e93d2d0`;
the host identity is `749c2213fa11cb06d4a81fd6a1af539326c88e37307c0d96d5aff81806800447`.

## Public native costs

Five paired samples per case follow warmup. Timings include complete update/build
framing and fresh-inode executable publication; build-plus-run also executes the
result. Ordinary controls alternate with the retained sequence, and both sides
preserve their own stable artifact bytes. Reset time is excluded. Native setup is
measured separately over five fresh connections. Ratios below are medians of paired
ratios, not ratios calculated from rounded displayed medians.

| Case | Ordinary build ms | Edited build ms | Unchanged ms | Build time saved | Build + run time saved | Setup recovery: edits |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Helpers 125 | 155.935 | 71.922 | 0.875 | 53.88% | 31.22% | 63 |
| Helpers 250 | 156.189 | 86.193 | 1.027 | 44.35% | 26.77% | 78 |
| Helpers 500 | 163.050 | 98.965 | 1.253 | 42.05% | 24.92% | 78 |
| Helpers 1000 | 177.759 | 108.849 | 1.838 | 38.77% | 24.83% | 77 |
| Helpers 2000 | 206.047 | 145.175 | 2.919 | 29.22% | 19.45% | 89 |
| Helpers 4000 | 265.400 | 197.041 | 5.387 | 25.53% | 16.84% | 79 |
| State machine, serial | 164.042 | 84.433 | 0.784 | 47.87% | 31.64% | 69 |
| State machine, POSIX | 167.034 | 92.455 | 0.803 | 47.16% | 28.15% | 68 |
| Signal network, serial | 169.389 | 84.511 | 0.802 | 49.31% | 31.91% | 65 |
| Signal network, POSIX | 173.082 | 96.859 | 0.807 | 44.46% | 30.28% | 69 |

Median setup is 5.290 seconds. Estimated body-build recovery is 63–89 edits in
these workloads; short sessions can still lose overall. This full run used
authorized local-loopback execution, while the earlier public baseline was
sandboxed. The absolute setup change cannot be attributed solely to manifest
batching; its separate matched capture profile remains the optimization evidence.

At 4,000 helpers, frontend-only unchanged updates take 2.027 ms versus 26.169 ms
cold, but body updates take 31.867 ms versus 26.410 ms one-shot: 20.66% slower.
Cold/unchanged/body exponents are 0.90947/0.84439/0.97576, each below the unchanged
1.25 gate; warm/cold is 0.07746 against 0.25. Native body edits still recompile the
whole generated-C translation unit while reusing eligible runtime objects.

At the native byte boundary, optimized peak RSS is 209,354,752 bytes and sanitizer
peak RSS is 512,540,672 bytes. Both observe 63,464,207 charged bytes and 125,988,576
allocated native-region bytes including headers. Reset frees the entire owned
region; unchanged native builds clone no native-query bytes. Captured inputs remain
connection-owned, and file block accounting is not unique physical storage.

[Compressed evidence](archive/2026-09-08-m1-release-candidate.json.gz) preserves the
full failed invocation, successful targeted repairs, original guide/status text,
raw timing summaries, source identities and requirement audit. These bounded
verification domains and same-host timings do not prove universal safety or a
controlled improvement in agent productivity. M2-M7 remain outside M1.
