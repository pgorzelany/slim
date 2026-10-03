# Measurement and verification evidence

[Current milestone status](../../docs/STATUS.md) is the authoritative status
summary. The [earlier progress ledger](2026-09-05-slim-next-progress.md) retains
its dated observations. Dated results apply only to their named
compiler, configuration and test domain. A passing historical checkpoint does
not certify the current working tree.

Fresh source8b snapshot `9d01a96a0c2705b7cbaf25ed330e7221ff30bb43` passed all
repository, release and website gates in 44m05s over 1,932 files, with fresh Rust
binaries whose embedded repository root was checked. The
[current integration identities](development-operation-cost-current.json) bind
receipt `40062efe…5432e`. Historical checkpoint
[112f6ed](https://github.com/pgorzelany/slim/commit/112f6ed4eb957dcd4909a0ab595b71d634ecc20d)
passed its invocation with copied-cache provenance unresolved. Source8b excludes
RFC172 and subsequent metadata; combined source9 verification remains pending.
General model benefit remains unknown.

| Checkpoint | Record |
| --- | --- |
| Current project contracts | [Concise source-bound namespace evidence](project-namespace-current.json); [complete original record](archive/2026-10-02-project-namespace-scoped-evidence.json.gz) |
| Current duplicate-path validation | [Work, behavior, resources and same-host comparisons](manifest-validation-current.json) |
| Current declared-import impact | [Exact finite relations and source-bound candidate scope](project-impact-current.json) |
| Current checked project inputs | [Finite producer/consumer domains and compiler inventory](project-input-current.json) |
| Current operation costs | [Separate observed tool phases, coverage and missing evidence](development-operation-cost-current.json) |
| Checked impact context export | [Current source, native authority, costs and limits](project-impact-context-current.json) |
| Ledger transaction overflow | [Original failure, current native contract and retained partiality](ledger-transaction-current.json) |
| Bounded source-record selection | [Exact selected bytes, producer authority and acceptance scope](source-context-current.json) |
| Current ordinary development summary | [Finite native/adapter domains, identities and incomplete cohort](development-summary-current.json) |
| Current development evaluator | [Protocol and budgets](../development/PROTOCOL.md); [configured versus completed trials](../development/current.json) |
| Bounded early agent loop, 2026-10-02 | [Self-hosted context, native costs, six frozen trials and release closure](2026-10-02-agent-development-loop.md) |
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
| M2 dependency plan | [Dependency slices, child RFC prerequisites and unchanged gates](2026-09-08-m2-plan.md) |
| M2 prerequisite preparation | [Approved experiment policy, remaining proposals and verification](2026-09-08-m2-prerequisites.md) |
| M2 native block component | [Native oracle, sanitizer checks, measured costs and remaining integration](2026-09-08-m2-native-pool.md) |
| M2 host and worker contracts | [Frozen host boundaries, argv storage and native reserved-buffer task evidence](2026-09-08-m2-host-contract.md) |
| M1 release candidate | [Release results, startup failure and measured native costs](2026-09-08-m1-release-candidate.md) |
| Every archived measurement file | [Manifest: original filename, SHA-256, bytes, lines and compressed size](archive/manifest.tsv) |

## Archived evidence

Historical payloads remain byte-for-byte in individually named gzip archives.
The [manifest](archive/manifest.tsv) records original filenames, SHA-256, sizes
and lines; it is an inventory, not a pass ledger. It preserves intermediate
attempts, raw timings, fault ordinals and contrary results. Later archives are
listed separately. Compression does not alter Git history or permanent gates.

The archived development history ends during interrupted verification, not a
complete pass. Current source-bound records identify accepted scopes. Extract
related TSVs beside it to preserve relative links. Reproduction artifacts retain
patches, scripts and identities; stored hashes alone do not establish acceptance.

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

Maintain concise current status and result records with source/compiler
identities, commands, exact or bounded domains, gate outcomes, costs and limits.
Link supporting raw evidence; preserve significant decisions and permanent
benchmarks. Routine attempts and coordination stay in ignored build storage,
without another dated prose journal. Keep interrupted and failed outcomes
explicit; a partial run is never a complete pass.

Routine reruns stay in ignored `build/` or `benchmarks/results/raw/`. Archive
measurements supporting decisions, baseline controls, significant failures,
contrary results and final acceptance; avoid repeated successful counter dumps.
For a completed report:

```sh
python3 scripts/archive-results.py pack YYYY-MM-DD-checkpoint-work.tsv
```

Add the compressed file and updated manifest to the checkpoint. Existing archive
names are immutable. Preserve enough provenance and the exact reproduction
command in the dated report; a hash alone does not establish a result's scope.
No performance budget or safety gate is changed by this storage policy.
