# Self-host bootstrap

`slim.project` is the self-hosting input. It contains explicit modules for
syntax/token utilities, byte-text emission, checking, typed memory planning,
project handling, C generation, coordination, and the minimal executable
driver. The checked-in portable C11 seed compiles the project to the next
executable, which emits one further generation. The bootstrap succeeds only
when the seed reproduces from this project, the successive generated C files
are byte-for-byte identical, and a program compiled by the resulting SLIM
compiler runs successfully. No Rust compiler participates.

The implementation is deliberately developed in valid Core rather than through
a privileged bootstrap dialect.

The `ir` module begins the D0023 typed-query migration. Syntax indexing creates
one structured declaration vector per standalone check. All top-level checker
passes share that vector, and governance requires exactly one file read and one
lex operation in the self-host checker. Expression typing and checked query
state continue to move into this representation incrementally.

The `memory` module owns recursive storage classification, bounded value
liveness and escape summaries, allocation-site plans, and reverse destruction
plans. The checker executes it from the structured declaration vector, while
the backend uses its function summary to select caller or child regions. The
generated ABI also propagates D0026's typed allocation-effect status and
destroys child regions at the single function exit.

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

Generated compiler and C files are placed in `build/toolchain/`, which is
ignored by Git.
