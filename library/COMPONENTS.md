# Experimental framed components

[RFC-0160](../design/rfcs/0160-bounded-source-data-components.md) adds ordinary
source modules and two applications; it does not stabilize RFC-0111 or change
SLIM semantics. Each manifest explicitly imports only its source dependencies.

`std_netstring.parse(source, start, end, maximum)` returns `End`,
`Invalid(code, byte_position)`, or `Value(Frame(start, end, next))`. Payloads
are arbitrary bytes. Decimal lengths are canonical: `0:,` is empty; `00:,`
is rejected. `append` validates all bounds and the 0..1,048,576 maximum before
mutating its output, then explicitly allocates to append one canonical frame.
Allocation exhaustion retains the current runtime exit-71 contract.

`std_byte_index.build(source, ^entries, maximum)` consumes entries, validates
at most 4,096 source spans of at most 256 bytes and nonnegative ordinals, stably
merge-sorts keys, and rejects duplicates. Its errors are 1 invalid capacity/count
(position is count), 2 invalid entry (position is input-entry index), and 3 duplicate
(position is the second source span in stable lexical order).
`find(index, query, start, end)` returns the original ordinal or -1 for an invalid
query span or absent key. `prefix(index, query)` returns a half-open sorted-entry
range. The catalog manifest also exports the existing `compare` function for
byte lexical comparison of two spans; each span must satisfy
`0 <= start <= end <= bytes.len(source)`. It has checked partiality and no
forged-span totality claim. Search correctness applies to builder-produced, unmodified `Index` values;
public struct construction is not an opaque-type guarantee. Empty prefix selects
all keys. `framed_records` composes three frames and complete canonical signed
I64 parsing, including exact limits and rejection of leading zeros and negative
zero; `addition_fits` supports recoverable checked aggregation.

Run the applications:

```sh
./slimc run library/catalog.project -- library/applications/catalog/fixtures/services.ns prefix api
./slimc run library/workplan.project -- library/applications/workplan/fixtures/release.ns
```

Catalog input is a sequence of key/weight/value triples. Nonempty keys have at
most 256 bytes; weights are signed I64. Reports select an exact key or prefix in
byte lexical order, with checked aggregate sum and canonical framed records.
Workplan input is name/nonnegative-cost/dependency-list triples. Names have at
most 64 ASCII alphanumeric/underscore/dash/dot bytes. Dependencies are comma
separated names; empty means none. At most 4,096 records/tasks, 65,536 edges and
1,048,576 source bytes are admitted. Source admission occurs after ordinary file
read and is not a pre-read physical storage limit.

The planner selects the lexical first ready task, reports start/finish costs,
and returns one critical path. Equal-cost predecessor and endpoint ties select
the lexical first name. Costs are user-supplied units; they are not measured
execution durations. Cycles report the lexical first unfinished task, which
can be downstream of the cycle. Validation, admission, report preparation and
allocation failures before printing publish no successful report prefix. Output
failure retains the runtime's partial-write and trap behavior.
Ready selection uses a private minimum heap of lexical ranks, with N explicitly
allocated I64 slots; each task enters once when its indegree becomes zero.
Complete loading, dependency resolution, scheduling and reporting have a
conservative direct-operation bound linear in source bytes plus
(tasks + edges) times the logarithm of task count, for the admitted key lengths.

[RFC-0163](../design/rfcs/0163-catalog-snapshot-reconciliation.md) adds
`catalog BEFORE diff AFTER`. It completely validates before, then reads/validates
after, and emits `changes N\n` followed by lexical key/before/after frame triples.
A present side is its complete canonical record; empty denotes absence. Weight
or binary value differences produce changes without subtraction or aggregation.
`catalog_reconcile.prepare(before, after, change_limit, output_limit)` admits
0..8,192 changes and 0..8,388,608 output bytes. Its explicit allocation stores
changes and preflights exact report size; reporting explicitly allocates complete
buffers before stdout. Catalogs and diffs must be producer-built and unmodified.
Diff errors print `error CODE in before|after|diff at POSITION\n`, with existing
loader/read exits and offsets. Codes 23/24 reject change/output admission at zero;
25 rejects an emitted-size disagreement at zero before successful output.
Read, validation, admission, append, count and allocation failures before the
single report print publish no successful prefix. Runtime output failure can
write a prefix before trapping; stdout is not transactional.

Every original query/planner failure prints `error CODE at POSITION` and a newline. Exit codes are
64 invocation, 65 invalid data, and 66 file read; positions are source byte
offsets except invocation/read and whole-source capacity failures (zero).

[RFC-0164](../design/rfcs/0164-bounded-development-summary.md) adds
`development-summary INPUT.ns` and an explicit `stats` command. The fixed
repository adapter converts an identity-checked protocol-2 freeze/run set:

```sh
python3 scripts/development-summary-input.py --freeze FREEZE --runs RUNS --output INPUT.ns --receipt RECEIPT.json
./slimc run library/development-summary.project -- INPUT.ns stats
```

Admit 32 task pairs, 64 trials, 131,072 source bytes and 65,536 report bytes.
Counts retain every outcome and unknown observation; no partial-cohort success
rate is emitted. Medians include terminal submitted failures/timeouts and exclude
unresolved/not-run durations. Only two strictly accepted trials contribute paired
elapsed operands, beside the complete denominators and selected subset count.
The explicit statistic checks 1,024 insertion-work units per condition and emits
two middle endpoints without overflow. Sum overflow remains unknown. Model
calls/tokens, active model time, native performance and general effectiveness
remain unknown. Supplied identity fields alone do not establish source authority;
the adapter retains a separate checked receipt. Public summary values must be
validator-built and unmodified. The corpus `observations.ns` is synthetic fixture
data, not a model experiment. Source admission follows ordinary file read, and
output retains the existing partial-write/trap contract.

| Code | Meaning / position |
| --- | --- |
| 1, 2 | File read / invocation |
| 10 | Source or record/task admission / zero or first excess record |
| 11 | Empty/invalid key or task name / offending key byte |
| 12, 13 | Noncanonical/invalid integer, out-of-I64 integer / first offending digit |
| 14, 15 | Duplicate key/task, unexpected index rejection / index failure position |
| 16 | Catalog aggregate overflow / selected key; planner malformed dependency / byte |
| 17, 18, 19, 20 | Planner absent, self, repeated, excess dependency / dependency start |
| 21, 22 | Planner cycle, critical-path overflow / unfinished or overflowing task name |
| 101 | Invalid framing bounds/maximum / requested start |
| 102, 103, 104, 105 | Bad header byte, missing colon, leading zero, excess payload / first byte |
| 106, 107, 108 | Truncated payload, absent/bad comma, incomplete triple / required byte |

`python3 scripts/verify-source-components.py --sanitize --full --faults` compares
native results with an independent finite Python model and retains source/artifact
identities under `build/overnight-components/`. Address/undefined behavior checks
run with leak detection disabled because the macOS sanitizer cannot provide it.
The checked domains and flags in each receipt bound the evidence; they are not
proof for arbitrary inputs or forged library structs.

The reconciliation check covers independent dictionary reports, exact limits,
producer-precondition boundaries and whole-pipeline counters:

```sh
python3 scripts/verify-catalog-diff.py --output build/catalog-diff-check \
  --sanitize --full --faults --work
```

 Both
component checks run permanently in `check-library-corpus.sh`; original budgets
and corpus members remain. Counters exclude host/runtime internals; latency,
physical allocation bounds and leak detection remain separate evidence.
