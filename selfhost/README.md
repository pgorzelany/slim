# Self-host bootstrap

`slim.project` is the self-hosting input. Its twenty-two explicit modules cover
syntax/token utilities, byte-text emission, typing, checking, typed memory planning,
bounded semantic analysis, direct reduction, project handling, C generation,
coordination, and the minimal executable driver. The checked-in portable C11
seed compiles the project to the next
executable, which emits one further generation. The bootstrap succeeds only
when the seed reproduces from this project, the successive generated C files
are byte-for-byte identical, and a program compiled by the resulting SLIM
compiler runs successfully. No Rust compiler participates.

The implementation is deliberately developed in valid Core rather than through
a privileged bootstrap dialect.

The `ir` module begins the D0023 typed-query migration. Syntax indexing creates
one structured declaration vector per standalone check. All top-level checker
passes share that vector, and governance requires exactly one file read and one
lex operation in the self-host checker. D0031 adds the `typing` module: it
derives linked lexical bindings, validates all declared and nested expression
types, and records structured type facts and issues. Expression facts form one
dense token-indexed table with a guarded constant-time query. The checker
consumes that view now, standalone emission retains the checked artifact, and
effect and ownership call resolution reuse its declaration links. Ordinary C
emission consumes the same table for computed call, recursive-transfer,
aggregate, variant-payload, and Boolean-match temporaries. Explicit let types,
match destinations, and pattern bindings remain source or caller contracts.
Boolean/variant match selection and arbitrary computed scrutinee materialization
also consume the same expression fact; codegen no longer decodes the older
packed binding-type link.
User-call allocation failure boundaries now come from the retained memory plan
through a sparse binary site query rather than a backend effect-list read.
Core 1D can be frozen after its final acceptance audit and complete release
gate.

The `memory` module owns recursive storage classification, bounded value
liveness and escape summaries, allocation-site plans, and reverse destruction
plans. The checker executes it from the structured declaration vector, while
the backend uses its function summary to select caller or child regions and to
emit recurrence boundaries without rescanning bodies. The generated ABI also
propagates D0026's typed allocation-effect status and destroys child regions at
the single function exit.

The `analysis` and `reduce` modules implement D0028 without a second program
representation. Analysis derives at most 64 binding facts per function from
stable canonical token indices. Reduction traverses the checked SLIM tree
directly, uses at most eight closure passes and 64-token dead-binding scans,
and emits canonical SLIM. These opt-in tools are not part of ordinary C
generation.

The `quality`, `proof`, `equivalence`, and `edit` modules implement D0029.
They classify bounded evidence, record and independently replay reduction,
exhaust the documented finite Boolean domain, and apply one checked canonical
node replacement. Their reports and patches are non-executable tooling data;
ordinary source checking and C generation retain the same authority and Core
surface.

The self-hosted path accepts the subset needed to compile itself plus the Core
standalone corpus: records, variants, `make`, `get`, `case`, variant `match`,
`set`, vector update, arenas, canonical formatting, and structured checking.
Its lexer dogfoods this support with a `Token` record and a single `Vec Token`.
The checker diagnoses malformed forms, scalar/effect errors, non-exhaustive or
duplicate Boolean arms, use-after-move, and invalid `inout`/`recur` ownership.
The compiler also resolves canonical project manifests, reads explicit
manifest-relative modules, lowers qualified references into one deterministic
translation unit, and checks the original project visibility/cycle/schema
corpus. It emits canonical path-free public interface artifacts. The complete
accepted project ledger, including manifest loading, resolution, interfaces,
incremental sessions, and corruption-safe caches, now runs through this
compiler with no semantic fallback.

New compiler capability is implemented in this SLIM compiler. Rust remains
only as bootstrap-independent conformance, benchmark, and governance
orchestration; it does not decide language behavior.

Run the complete proof with:

    ./bootstrap.sh

Inspect the Core 1A/Core 1B tools with:

    ./slimc analyze examples/vector_sum.slim
    ./slimc reduce conformance/pass/reduction.slim
    ./slimc prove-reduction conformance/pass/reduction.slim
    ./slimc equivalent conformance/evidence/equivalent-left.slim conformance/evidence/equivalent-right.slim
    ./slimc edit conformance/evidence/equivalent-left.slim conformance/evidence/edit.patch

Generated compiler and C files are placed in `build/toolchain/`, which is
ignored by Git.
