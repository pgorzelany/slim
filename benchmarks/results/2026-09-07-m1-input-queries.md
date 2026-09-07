# M1 retained parameter-input queries

Status: validated RFC-0142 checkpoint; M1 incomplete.
Baseline: `67055823aa43b11f3ab49466fea81b76b63d4f72`.
Current seed: `64107135f2677bcada015bfe5cd292bb1c3a8f41216fe2c92ebf7793c9df5f61`.

## Implemented behavior

The retained checker carries complete recurrence-invariant masks, ordered call
arguments and paired parameter-input results between successful snapshots.
Complete typed owner maps, current contributions and all six argument-fact fields
authorize reuse. The existing scalar merge remains the sole producer. Both input
modes, all four transfers and all five function passes remain unchanged.

A lookup compares at most four versions across current and previous history.
The entire previous chain must fit the remaining allowance. Complete empty
incoming adjacency leaves the exact cleared defaults without storing an empty
query. The full entry-invariant mask is initialized only when fallback needs it.
Invalid or exhausted optional history causes ordinary recomputation. A fifth
produced version recomputes exact results and declines incomplete new history.

Integrity seals detect tested accidental corruption of compiler-owned data.
Re-sealed structurally invalid records are tested; seals do not authenticate
forged, structurally valid semantic records. No external facts approve source.

## Verification on the current candidate

| Check | Observed result |
| --- | --- |
| Portable seed and bootstrap | Fixed point, 4,716,564 C bytes. |
| Governance and Cargo | Passed; 10 unit and 75 integration tests. |
| Required benchmark commands | Performance quick, reduction quick, parallelism, compare quick and agent passed with existing gates. |
| Complete input oracle | 96 accepted fixtures, fresh/previous snapshots, six limits, four rounds, both full arrays and all six fact fields; ordinary and ASan/UBSan. |
| Input edits | 19 pairs cover caller/callee bodies, recurrence masks, known/unknown/out-of-domain inputs, parameter names/order/types, effects, borrow modes, insertion/deletion/reordering, relocation and CRLF. |
| Input corruption and boundaries | 45 raw-damage and 42 re-sealed structurally/domain-invalid cases; fallback/recovery, absent/multiple history, exact/beyond lookup bounds and five real producer inputs. |
| Actual input work | Independently predicted counters repeat exactly; fan-in sizes 1, 2, 125, 250, 500, 1,000, 2,000 and 4,000 pass. |
| Internal session matrix | Complete prepared fields, diagnostics, ranges/recurrences, memory plans and byte-exact C; configuration/edit/corruption/recovery cases and existing geometric domains, ordinary and ASan/UBSan. |
| Analysis consumers | Every parallel-view field, including all blocker bits and schedule fields, plus complete range/quality/parallel report sections; 96 fixtures through cold/unchanged/insertion updates and six targeted caller/callee edits; ordinary and ASan/UBSan. |
| Retained typing fault campaign | 95 fixtures; all 1,024 ordinals agree between ordinary and sanitized execution: 581 status-71 failures, 443 successes. |
| Full session fault campaign | All 6,144 ordinals pass: cold 554 failures/1,494 successes; update 1,029/1,019; fragment reuse 991/1,057. Ordinary and sanitized statuses, outputs and errors agree. |
| Native applications | All 20 challenges produce byte-identical C and matching native status/stdout/stderr. |

These are the recorded bounded test domains, not a proof over every accepted
program. The full M1 release gate has not run on this candidate.

The baseline had 572 retained-typing failures and 550/1,008/982 session failures
in the same domains. The increases of 9 and 4/21/9 allocation attempts reflect
the added optional input history. Every campaign still reaches successful
ordinals; no failure is converted to accepted source or stale publication.

## Actual work and current costs

The fixed multi-call fixture has seven functions, ten parameters and 21 argument
contributions. An admitted update imports seven function structures, then performs
four transfers with four previous and twelve current result imports. It compares
16 query versions and 84 key rows, constructs 21 adjacency rows, and performs zero
scalar merges or ordinary scans. Copy work remains visible: 21 keys, eight paired
results, four entries, 18 owner-map rows, ten invariants and 21 arguments.

Invalid/small/below-topology limits execute eight ordinary scans and 168 merges.
The exact topology allowance imports one query before declining further storage,
then completes six ordinary scans and 162 merges. For fan-in N, admitted work
includes 4N key comparisons, N adjacency/key/argument rows and N+2 imported
function structures. Session observer schema 7 appends actual input transfers;
the original scan column still counts actual ordinary scans.

The seed grows from 4,444,746 to 4,716,564 C bytes (+271,818, about 6.12%). Native
row widths are 16 bytes per invariant, 56 per argument, 80 per function/key,
64 per paired result and 72 per query entry, all aligned to eight bytes.
Query/history/state headers are 160/328/728 bytes, excluding backing allocations.
The retained cache header grows from 536 to 576 bytes. Record-count limits are
not total-memory promises.

Three repeats of all 18 retained scenarios have deterministic allocation/live-byte/
copy observations and zero live payload/header bytes after cleanup. Scalar 4,000
body-update attempts rise from 119,386 to 119,507; peak live bytes including runtime
headers rise from 113,265,361 to 114,228,673; vector-growth copies rise from
92,914,000 to 93,862,176 bytes. Dense 512 body-update peak bytes fall from
392,628,665 to 392,387,753. Counted 2,000 interface-update peak bytes rise from
214,712,232 to 217,094,616. These are process-wide costs, not isolated cache sizes.

All 26 clean check/emission timing cells pass their identical-baseline control
comparison, including frozen selfhost source. The 18 retained timing cells flag
three cases; their prescribed 66-group follow-ups all clear:

| Current retained case | Candidate median ratio | Control upper quartile |
| --- | --- | --- |
| Scalar 125, interface | 1.00198 | 1.01604 |
| Scalar 4,000, body | 1.00629 | 1.00942 |
| Counted 125, interface | 1.00100 | 1.01447 |

Timing uses two warmups and all six permutations of two identical baseline runs
and one candidate. Retained measurements are complete cold-plus-update process
totals, including I/O and teardown; they do not isolate warm query latency.
Across 20 applications, five-pair external C-backend median ratios range from
0.98768 to 1.03119; eleven-pair native runtime medians range from 0.97126 to 1.01571.
Byte-identical generated C and these timings do not establish a runtime speedup
or significant agent-productivity gain.

## Earlier candidates and contrary evidence

The first candidate, seed `8ddf3c1a75a55534d72afb700bb20f93b5483ec6d509d9794d13c8a93a073a34`
(4,715,436 C bytes), failed four repeated retained timing comparisons:

| Initial retained case | Candidate median ratio | Control upper quartile |
| --- | --- | --- |
| Scalar 4,000, unchanged | 1.02506 | 1.02014 |
| Scalar 4,000, body | 1.04368 | 1.02408 |
| Scalar 4,000, interface | 1.04696 | 1.03470 |
| Counted 2,000, interface | 1.01190 | 1.01115 |

Avoiding empty queries produced seed
`dbf9077ce05d314ad42f93c2aece13fae56bc9e23608d08777b200ee187c809e`
(4,715,571 C bytes). Its scalar 4,000 interface repeat still failed at 1.00975
against control upper quartile 1.00804. Lazily constructing the fallback mask
produced the current candidate. Scalar 4,000 body peak bytes/copies fell from
116,027,729/95,661,136 initially to 114,359,825/93,993,232 with empty queries
removed, then to the current values above. No performance budget was relaxed.

A test-only import-closure experiment included 13 rather than 31 modules, but
complete verification took 33.66 seconds versus 33.25 seconds with the original
manifest in one sequential comparison. It showed no useful benefit and was removed.

## Evidence and remaining work

Completed raw measurements are losslessly compressed under `archive/`, with
filenames and SHA-256 values in the [manifest](archive/manifest.tsv). The
[reproduction record](archive/2026-09-07-m1-input-reproduction.json.gz) preserves
measurement scripts, fixed source identities and patches for all three candidates.
Each archived patch reconstructs every recorded frozen source hash. Earlier
failures and intermediate measurements remain available. The
[verification record](archive/2026-09-07-m1-input-verification.json.gz) preserves
all 66 experiment/verification logs, individual payload hashes, source identities
and the completed checkpoint commands. Routine working copies remain under
ignored `build/slim-next-m1/input-queries/`.
The compact [gate ledger](2026-09-07-m1-input-gates.tsv) records final governance
after the implementation/documentation update and the remaining M1 obligations.

Public host-bound sessions, retention of global analyses, native
artifact caching and full release closure remain required M1 work. The public
session command still reports estimates; this internal checkpoint does not complete M1.
