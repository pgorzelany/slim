# Measurement and verification evidence

[Current milestone status and measured limits](2026-09-05-slim-next-progress.md)
is the authoritative status summary. Dated results apply only to their named
compiler, configuration and test domain. A passing historical checkpoint does
not certify the current working tree.

| Checkpoint | Record |
| --- | --- |
| M0 repairs and M1 development through 2026-09-07 | [Complete development history](archive/2026-09-07-slim-next-development-history.md.gz) |
| Validated literal-storage checkpoint `0f19fbc` | [Scope, costs and verification](2026-09-07-m1-literal-storage.md) |
| Validated byte-literal checkpoint (composite verification) | [Scope, costs and pending checks](2026-09-07-m1-byte-literals.md) |
| Validated RFC-0142 input-query checkpoint | [Verified domains, costs and remaining M1 work](2026-09-07-m1-input-queries.md) |
| Working RFC-0143 public-session checkpoint (closure pending) | [Protocol, verified domains, costs and remaining work](2026-09-08-m1-host-session.md) |
| RFC-0143 public acceptance | [Edit, capacity, fault, resource and latency evidence](2026-09-08-m1-host-closure.md) |
| RFC-0144 retained bounded parallel analysis | [Complete-result reuse, costs and verification](2026-09-08-m1-parallel-query.md) |
| RFC-0145 validated native query foundation | [Byte preservation, work, costs and remaining integration](2026-09-08-m1-native-query.md) |
| RFC-0146 default native integration (release closure pending) | [Public build costs, startup optimization and verification](2026-09-08-m1-native-session.md) |
| Concurrent host publication repair | [Failure found by the clean gate and verified repair scope](2026-09-08-m1-host-publication.md) |
| M1 complete at `4bdb675` | [Exit criteria, complete release evidence, costs and limits](2026-09-08-m1-closure.md) |
| M2 planning only | [Dependency slices, child RFC prerequisites and unchanged gates](2026-09-08-m2-plan.md) |
| M2 prerequisite preparation | [Approved experiment policy, remaining proposals and verification](2026-09-08-m2-prerequisites.md) |
| M1 release candidate | [Release results, startup failure and measured native costs](2026-09-08-m1-release-candidate.md) |
| Every archived measurement file | [Manifest: original filename, SHA-256, bytes, lines and compressed size](archive/manifest.tsv) |

## Archived evidence

The September consolidation preserves all 257 raw TSV files and the full
development history byte-for-byte in individual gzip files. Intermediate runs,
raw timing samples, zero-valued counters, fault ordinals, failures, regressions
and caveats remain available. The manifest is an inventory, not a pass ledger.
Each original filename maps to `archive/<filename>.gz`; gzip timestamps are zero.
The history preserves its original relative links. Its TSV links resolve when
extracted alongside the original TSV filenames.

The original consolidation captures 270,342 text lines (16,274,889 bytes) in
2,636,037 compressed bytes. All 257 TSVs were compared byte-for-byte against
`010be59`; the history includes the working tree's pending byte-literal notes.
Later checkpoint archives are listed separately in the same manifest. Existing
Git history is preserved, so this reduces the current textual diff rather than
removing historical blobs from the repository.

The input-query checkpoint retains measurements for all three implementation
candidates, including rejected costs. Its compressed reproduction record contains
scripts and hash-verified source patches; its verification record contains the
individual log payloads, hashes and completed command/source identities.

The history is a snapshot of the development log before consolidation. Its last
entry says verification was running. That run was subsequently interrupted at the
maintainer's request with exit 143 during place fault ordinal 1,850. It is not a
complete verification pass. The subsequent resumed suffix passes every remaining
stage on the same seed; current status and the byte-literal report supersede that
entry. The isolated RFC-0142 draft is not production implementation evidence.

From the repository root, verify every archived payload against its original hash:

```sh
python3 scripts/archive-results.py verify
```

Restore the original filenames to an ignored working directory for analysis:

```sh
python3 scripts/archive-results.py extract build/evidence
```

The extractor refuses to overwrite different contents. To read one file, use
`gzip -dc benchmarks/results/archive/<filename>.gz`. Compression changes storage
only; it does not summarize, discard or reinterpret observations.

## Recording future checkpoints

Keep fixtures, metric definitions, executable checks and regression baselines in
their existing source locations. Verification generates fresh results; it does
not consume these historical September TSVs. Older reports used by governance
remain at their original paths.

For each meaningful checkpoint, commit a concise dated report with source and
compiler identities, commands, exact or bounded test domains, gate outcomes,
before/after costs and remaining limitations. Link its supporting raw evidence.
Update the current status file instead of appending another chronological report
to it. Keep interrupted and failed outcomes explicit; a partial run is never a
complete pass.

Use ignored `build/` or `benchmarks/results/raw/` for routine successful reruns.
Preserve raw timings used for decisions, baseline controls, important failures,
contrary results and final checkpoint evidence in uniquely named archives. Do
not repeatedly commit every successful per-fixture counter row as plain text.
Archive a completed report from the results directory with:

```sh
python3 scripts/archive-results.py pack YYYY-MM-DD-checkpoint-work.tsv
```

Add the compressed file and updated manifest to the checkpoint. Existing archive
names are immutable. Preserve enough provenance and the exact reproduction
command in the dated report; a hash alone does not establish a result's scope.
No performance budget or safety gate is changed by this storage policy.
