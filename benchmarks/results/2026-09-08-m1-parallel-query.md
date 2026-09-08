# M1 retained parallel-analysis checkpoint

Status: RFC-0144 implemented and verified; M1 remains incomplete.
Baseline: `bc32e478866a6a3578fa3e18e78121a791563e99`.
Candidate seed: `9959671c67bdce89f1826230aa90827a722319dbf5fb787bb7ad2a23180df963`.
Candidate host: `c0bdf15e31cb86d412d01d53715ebe91abd762699e2bded94e96b85943ff150a`.

RFC-0144 retains the complete existing bounded parallel-analysis result in the
successful production SLIM snapshot. It preserves all blocker bits, site fields,
scheduling counters and uncertainty. No source-language or native execution
semantics change. Changes to consumed checked/range/recurrence inputs recompute
normal analysis; eligible source relocation imports current canonical positions.
Native object/link caching and full M1 release closure remain required.

## Correctness and observed work

The complete-view oracle passes 323 comparisons in ordinary and ASan/UBSan
builds: 221 independently observed imports and 102 producer recomputations.
Coverage includes the 96-source corpus, accepted public edit pairs, selected
prefix insertion/removal, 63/64/65 functions and reported sites, 4,095/4,096/4,097
call edges, work thresholds, caller-input changes and individual damaged fields.
The oracle compares every field, rather than accepting matching checksums.

For the scalar outside-prefix edit at 125 through 4,000 helpers, each reuse
checks 64 function keys and 704 corresponding range nodes, validates/imports
64 facts and no sites. Canonical declaration selection reads respectively 252,
502, 1,002, 2,002, 4,002 and 8,002 metadata rows across both revisions. These
linear scans are real work; unrelated function bodies are not queried.

Every allocation failure in the deterministic two-query fixture is exercised:
333 terminal failures, then complete controls at ordinals 334 and 2,048. Ordinary
and sanitized builds agree; four failures occur after result import begins.
The full query command already includes this campaign. A redundant separate
fault-mode rerun is recorded independently, not counted as extra coverage.

The internal session matrix and complete range/quality/parallel analysis
comparisons pass. Governance and all 10 unit / 75 integration tests pass after
correcting three RFC section headings. Bootstrap reaches a fixed point at
4,808,588 generated C bytes, versus 4,716,052 at the baseline (+92,536 bytes,
approximately 1.96%). The full public-session campaign also passes, including all capacity crossings,
failed-update recovery, partial output and exact cleanup. All required performance/reduction/parallelism/compare/agent commands and the
complete resource baseline pass. Native analysis/resource rows are unchanged.

## Implementation corrections

The first implementation kept graph history inside the frequently passed
fragment-cache record. It was moved alongside that record in the successful
snapshot to reduce metadata copying in fragment loops.

That separation exposed a real bug in the session corruption matrix: function
selection used optional memory-plan indices, and a damaged plan caused an
out-of-bounds trap. The final implementation derives function membership and
order from checked canonical declarations. The regression now passes; optional
memory-planning metadata does not define parallel-query identities.

Initial observation hooks counted helper entries instead of actual loop rows.
The corrected permanent hooks count only nonterminal loop bodies and separately
report declaration selection. Initial logs remain exploratory evidence.
An earlier fault-test wrapper exited 127 because its shell script was edited
while it was running; its child results do not make that wrapper a passing run.
The corrected standalone campaign and final full command supersede it.

## Performance and remaining work

Two identical baseline workers and the candidate run 18 measured groups after
two warmups, rotating all six execution orders at six geometric sizes. Every C
artifact equals both the baseline and current clean production output. Timing
includes session response copying and excludes external C compilation.

| Helpers | Scalar outside edit ratio | Call-heavy outside edit ratio |
| --- | --- | --- |
| 125 | 1.0212 | 0.8916 |
| 250 | 1.0209 | 0.8976 |
| 500 | 1.0080 | 0.9203 |
| 1,000 | 1.0031 | 0.9427 |
| 2,000 | 0.9975 | 0.9607 |
| 4,000 | 0.9996 | 0.9864 |

Ratios compare the candidate with the mean of its matched baseline controls.
The call-heavy fixture has 63 selected functions making 16 calls each. Its
outside-prefix edits measure about 10.8% faster at 125 helpers and 1.4% faster
at 4,000. The simple fixture shows small overhead at smaller sizes and little
change at 4,000. Other cold/inside/unchanged median ratios range from 0.9770 to
1.0291. Raw samples and control variability are preserved; control quartiles
are descriptive, not confidence intervals. No general speedup is inferred.

All four existing public latency gates pass: cold/unchanged/body exponents
0.919998/0.851260/0.973187 against 1.25, and unchanged/cold 0.078008 against 0.25.
At 4,000 helpers this separate nine-sample run measures cold 25.662 ms, unchanged
2.002 ms, body edit 32.311 ms and one-shot 26.035 ms. Body edits remain 24.1% slower
than fresh frontend compilation. The matched scalar body-edit ratio is 1.0022;
the separate historical 22.7% figure is not a matched regression comparison.

The exact resource history at 4,000 helpers retains 52,121,126 payload bytes
after cold publication and 128,706,345 after change/rejection/recovery. Peak
payload plus headers reaches 133,566,347 bytes; cumulative realloc copies reach
117,730,144 bytes. Against the previous checkpoint these are respectively
+1,152 / +3,328 / +3,488 / +896 bytes. Reset and exit leave zero measured live
storage. These counters exclude libc bookkeeping and arbitrary source copies.

Earlier exploratory timings apply only to their identified intermediate builds
and are superseded by the final measurements. Neither a cache hit nor these
frontend timings establish significant agent-productivity improvement.

Native artifact caching, integrated locality/cost evaluation and complete
`scripts/verify-0.9.sh` release closure remain required before M1 completion.

## Evidence and next checkpoint

[Full command logs, raw timing samples and source identities](archive/2026-09-08-m1-parallel-query-verification.json.gz)
retain successful and failed development runs with separate final exit statuses.
[Source reproduction data](archive/2026-09-08-m1-parallel-query-reproduction.json.gz)
and the [intermediate inline-history seed](archive/2026-09-08-m1-parallel-query-inline-seed.c.gz)
preserve the measured development artifacts. The intermediate separated-history
measurements identify their binary hashes but are superseded by the reproducible
final source. No final conclusion depends on those provisional measurements.

All 296 archives verify against their byte/hash manifest. These three new
archives preserve 5,512,222 original bytes without adding their raw text to the
source diff. The checkpoint commit introducing this report identifies the final
source. Next work is native object/link caching, followed by integrated M1
locality/performance and complete release closure.
