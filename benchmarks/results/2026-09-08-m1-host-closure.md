# M1 public-session acceptance

Status: RFC-0143 implemented and verified; M1 remains incomplete.
Baseline source: `2c8999f063bf754ec2980264fd98f37a3ff432d5`.
Seed: `f074c6f23d33ffe6389584c9dfb0567ac3323afdfacefdf0a93e6f261dd170c6`.
Host: `8c9d351818c59a836209334e005b09ae72b1eeb7bc103d88e50db343d64f50f8`.
Production compiler, host adapter and runtime bytes are unchanged from the
[working-session checkpoint](2026-09-08-m1-host-session.md). This change completes
public verification infrastructure and adds four durable latency gates.

## Acceptance domain

| Check | Result |
| --- | --- |
| Public source edits | 32 pairs cover body, interface, effect, layout, ownership mode, caller-input facts, insertion/deletion/reordering/rename, manifest/import/export and source spans. Every pair runs in-place edit, repeat, reversal and relocation; C and diagnostics match fresh production runs. Ordinary and ASan/UBSan. |
| Existing corpus | 96 sources across cold/unchanged/insertion and 30 projects with rejection/recovery remain exact, ordinary and ASan/UBSan. |
| History allocation faults | All 1,024 ordinals across cold/change/rejection/recovery/reverse: respectively 205/203/149/14/203 terminal allocation failures and 250 complete histories. Ordinary and ASan/UBSan agree. Every accepted prefix retains its exact revision; errors never return stale C. |
| Existing cold faults | The independent 1,024-ordinal campaign still gives 159 terminal failures and 865 complete results. |
| Input capacity | Exactly 32 admitted 2 MiB captures fill 64 MiB. Two subsequent requests fail S0004; reset permits a cold update. |
| Canonical-node capacity | 4,000-helper alternating edits admit 22 updates; request 23 fails S0005. Repeated failure, last-good artifact reuse and reset pass. |
| Generated-C capacity | Alternating 1 MiB-literal edits admit 31 updates totaling 65,063,296 C bytes; request 32 crosses 64 MiB and fails S0006. Repeated failure, last-good reuse and reset pass. |
| Other limits | Existing 64-attempt/no-further-file-read test, exact 1 MiB diagnostic boundary and all 13 host allocation-growth faults remain passing. |
| Partial output | A complete initial result is followed by only 128 bytes of a large second response. The client rejects the incomplete frame; closing its read pipe causes status 65 and physical cleanup under sanitizers. |
| Actual build contexts | Adapter, runtime and flags change through real builds. A runnable `arm64-apple-macos15.0` build differs from `arm64-apple-darwin25.6.0`; both report their actual target and match clean C. Other target families remain untested. Relocation, concurrent publication, loaded-worker stability and corrupt-seed rejection still pass. |
| Actual work | All 23 observed producer/import entries remain zero on unchanged updates; body changes execute the relevant checker. Detailed internal locality/corruption gates are preserved. |

The new input/node/C crossings all pass ordinary and ASan/UBSan. Node and code
limits describe admitted work, not peak physical storage. The node-limit fixture
reaches approximately 820 MiB maximum resident storage in the ordinary child
process campaign and 1,226 MiB sanitized. This is an observed cost, not a memory
budget or a portable bound. The process metric includes the clean oracle and
worker children; it does not isolate worker RSS.

## Physical storage and copying

Test-only hooks count requested runtime payload, allocation-header storage,
runtime realloc copies and separate diagnostic-buffer growth. Three histories
at each of 125/1,000/4,000 helpers repeat all counters exactly. The history is
cold, unchanged, body edit, rejected edit, recovery, reset, cold, quit.
Reset and quit leave zero live measured payload, headers and diagnostic buffers.
Counters do not include libc bookkeeping, arbitrary source copies or pipe storage.
The 256-row report cap remains explicit; above-cap records report bounded.

At 4,000 helpers the first cold result retains 52,119,974 payload bytes and
53,563,774 bytes including headers. The unchanged request performs 14 additional
allocation attempts and retains another 127,830 payload bytes. After body edit,
rejection and recovery, live payload reaches 128,703,017 bytes; peak payload plus
headers is 133,562,859 bytes. Cumulative runtime realloc copies reach 117,729,248
bytes before reset. A 256-byte diagnostic buffer survives recovery until reset.
These measurements expose retained failed-attempt and copying costs; cleanup does
not imply immediate RSS release by the system allocator.

## Quiet same-host latency

Nine samples after warmup, six geometric sizes and complete C comparisons pass
all new RFC-0030 gates. Endpoint exponents are 0.911744 cold, 0.829476 unchanged
and 0.968382 body edit, each below 1.25. At 4,000 helpers, cold takes 25.706 ms,
unchanged 1.947 ms, body edit 31.813 ms and direct one-shot 25.932 ms. The
unchanged/cold ratio is 0.075744 against a 0.25 ceiling. Body edits are still
22.7% slower than fresh compilation. No significant productivity gain is claimed.
All raw samples, including warmups and startup/reset costs, are preserved.

## Checkpoint and remaining M1 work

Bootstrap fixed point, governance, all 10 unit and 75 integration tests, and
required performance/reduction/parallelism/compare/agent benchmarks pass. The
resource observer also crosses 255/256/257 report rows under sanitizers, verifying
its explicit saturation boundary. The unchanged production
implementation already passed reproducible packaging and clean installation at
`2ffc37b`; that historical check remains identified separately. Full integrated
`scripts/verify-0.9.sh` closure is required when the complete M1 implementation
is ready.

Global analysis retention, native object/link caching with complete dependencies,
locality and improved real edit costs remain M1 work. This acceptance checkpoint
adds no production semantics and does not close M1.

The initial governance run rejected citing the score-15 host architecture RFC
for performance rows: its checker requires an accepted score-60 performance
contract for every row. The additive gates now belong explicitly to RFC-0030's
existing incremental performance contract, with their exact fixture and limits
recorded there. No governance rule, prior limit or RFC score was changed.

## Evidence

The [command ledger](2026-09-08-m1-host-closure-gates.tsv) records the checkpoint.
[Full logs and source hashes](archive/2026-09-08-m1-host-closure-verification.json.gz)
and [raw timing samples](archive/2026-09-08-m1-host-closure-latency.tsv.gz) preserve
the evidence without expanding the source diff. The full host campaign precedes
the added partial-response case and report-cap boundary test; those additions
pass separately under ordinary/sanitized protocol and isolated sanitizer checks.
No single uninterrupted full-script run including all later additions is claimed.
The latency run used the same numeric gates before correcting their RFC owner.
Source checkpoint identity is the commit introducing this report.

The two new archives retain 74,615 original bytes in 20,270 compressed
bytes. All 293 archived files verify against their byte/hash manifest.
