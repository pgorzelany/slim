# SLIM deterministic projects

Status: SLIM 0.9 — experimental, pre-1.0; project manifest schema 1

Core 0.2 combines existing Core modules into one executable without adding a
second module form or source-level import/export declarations. One explicit
project manifest owns paths, dependencies, visibility, and the entry module.

## Project manifest

A manifest is an S-expression read by the existing lexer and parser machinery:

```text
(project 1
  (entry app)
  (module app "src/app.slim" (imports math) (exports))
  (module math "src/math.slim" (imports) (exports Number add)))
```

The grammar is:

```text
project = (project 1 entry module+)
entry   = (entry MODULE)
module  = (module MODULE BYTE_PATH imports exports)
imports = (imports MODULE*)
exports = (exports NAME*)
```

There is exactly one `entry` clause and it precedes every module clause.
Module clauses are sorted by module identity. Import and export names are
strictly sorted and unique in unsigned ASCII byte order. A module must not
import itself. Duplicate or descending import/export names produce `E0406`
at the first offending name; the compiler does not silently sort input.

Module identities and declared names are ASCII identifiers. Module identities
may contain `.` for flat organizational names. `.` separates a module identity
from an exported declaration in a qualified reference.

A path is UTF-8, relative to the directory containing the manifest, uses `/`
separators, ends in `.slim`, and contains no empty, `.` or `..` segment. It
must remain beneath the manifest directory after filesystem resolution.
Absolute paths, backslashes, symlink escapes, duplicate paths, implicit source
search, environment search paths, and filesystem enumeration are rejected.
The module name in each source file must equal its manifest identity.

## Imports, exports, and names

An import grants access to one module namespace, not to all transitive imports.
It does not copy declarations into the local namespace. Imported declarations
are referenced only as `module.name` in every position that currently accepts
a function or named type:

```text
fn main(args: Vec[Bytes]) -> I64:
  let number: math.Number = math.add(20, 22)
  number.value
```

Unqualified references resolve only to local declarations or built-ins.
Aliases, glob imports, selective symbol imports, re-exports, implicit imports,
and self-qualified local references do not exist. A qualified reference is
valid only when the module is a direct manifest import and the declaration is
listed in that module's exports.

Export lists name local declarations. Exporting an absent name or built-in is
an error. An exported signature or data layout must not expose a private local
type or an inaccessible imported type. The entry module contains the sole
Core `main`; `main` need not be exported unless another module imports it.
Every other module is forbidden from declaring `main` to keep one entry model.

Import cycles are rejected. The diagnostic reports the canonical cycle rotated
to start with its lexicographically smallest module. Acyclic imports permit
single-pass interface checking and bounded topological parallelism.

## CLI

Existing compiler operations accept either an indented `module NAME` source or
a versioned `(project ...)` manifest. The first token explicitly selects the
input kind; file extensions and directory search do not.

```text
slimc check PATH [--jobs N]
slimc emit-c PATH [--jobs N] -o FILE
slimc build PATH [--jobs N] [-o FILE]
slimc run PATH [--jobs N] [-- ARGS...]
slimc fmt PATH [--check]
slimc interfaces MANIFEST [--jobs N] -o DIRECTORY
```

`interfaces` materializes canonical exported signatures. The native compiler
writes one interface per stdout line; the launcher writes `.sli` files under
`-o DIRECTORY`. One-shot operations check current source without a default
cache directory. `--jobs N` remains accepted, but current production checking
and emission are serial; a scheduler plan is not executed worker activity.
The compiler never searches upward for a manifest. Bounded semantic queries
are documented in [CONTEXT.md](CONTEXT.md).

## Canonical interface artifact

Each module has one UTF-8 interface artifact:

```text
(interface 3 math
  (struct Number ((value I64)))
  (fn add ((copy I64) (copy I64)) math.Number (effects)))
```

The grammar is:

```text
interface = (interface 3 MODULE declaration*)
declaration = struct | enum | function
struct   = (struct NAME ((NAME TYPE)*))
enum     = (enum NAME ((NAME TYPE*)*))
function = (fn NAME ((MODE TYPE)*) TYPE (effects EFFECT*))
MODE     = copy | shared | exclusive | owned
```

Artifacts contain only exported declarations. Declarations are sorted by
name. Struct fields and enum cases preserve semantic layout order. Effects
use the fixed `alloc`, `io`, `partial` order. Every named type is fully
qualified, including a type from the artifact's own module. Artifacts contain
no source bodies, spans, paths, timestamps, host data, target data, hashes, or
compiler scheduling data. Their stable fingerprint is computed over these
exact bytes.

Interface schemas other than `3` are rejected rather than guessed. A
schema change requires a compatibility decision and a compiler-version change;
there is no permissive reader for unknown fields.

## Incremental cache

The working `slimc session` protocol reuses accepted parsed declarations, checked
bodies, analysis and C fragments across framed updates; native artifacts use
the RFC-0146 session contract. M1 release verification is complete. See
[INCREMENTAL.md](INCREMENTAL.md) for retained identities, work, admission and
last-good boundaries. Historical invalidation estimates remain separate
measurement fixtures, not evidence of executed checking or emission.

The internal `cache PROJECT CACHE_FILE` command in `selfhost/cache.slim`
provides a whole-project generated-C artifact probe. Its key contains the
exact manifest bytes and length-prefixed source bytes for every module. It is
not a normalized source fingerprint or a per-module interface cache.

The binary frame contains the ten-byte magic `SLIMCACHE\0`, one schema byte
(currently 3), two eight-byte big-endian lengths, the key, the C artifact, and
an eight-byte weighted checksum. Key and artifact lengths are each bounded by
64 MiB. The reader validates the complete frame before comparing key bytes;
truncated, oversized, mismatched, and checksum-invalid frames rebuild.

A hit returns the stored complete C artifact. A miss performs a normal project
check and emits a newly framed artifact for the caller to store. The command
does not itself maintain a default cache directory, write atomically, or
reconstruct a project from individual module fragments. The checksum detects
accidental corruption; it is not an authenticity or semantic proof. The frame
also lacks a compiler-build identity, so callers must invalidate it across
compiler changes. The public session captures compiler/runtime/tool identities
through its separate checked contract; this internal probe is not that service.

## Scheduling and measured implementation boundary

The production scheduler derives stable dependency layers and bounded worker
batches, but one-shot project preparation and C emission execute serially.
Historical multi-worker measurements describe their original implementations;
they cannot establish current worker execution or a speedup.

The production compiler is SLIM with the portable C seed. Retained declaration
checking, transactional last-good state and measured producer work exist through
`slimc session`; ordinary project commands do not acquire that retained state.
Whole-project artifact probes, invalidation estimates and public retained work
remain separate mechanisms. [INCREMENTAL.md](INCREMENTAL.md) names their contracts
and current measurement boundaries. Parallel execution within compiled programs
has its own [PARALLELISM.md](PARALLELISM.md) contract.

## Stable project diagnostics

Core 0.2 reserves `E0401` through `E0449` for project structure, resolution,
visibility, interface, and cache errors. Every diagnostic identifies its
manifest or source file and exact primary byte span. Cross-file labels retain
their own file identity. Required cases include malformed manifests, unsupported
versions, invalid or escaping paths, missing source files, duplicate identities
or paths, module-name mismatch, missing or duplicate imports/exports, cycles,
unknown qualifiers, undeclared direct imports, inaccessible declarations,
private interface leaks, multiple/missing entry points, and corrupt artifacts.

Cache corruption recovered by rebuilding may be reported as stable warning
`W0401`; it is never an error when valid source is available. Error ordering is
`(module identity, file-relative primary start, code)` and is independent of
filesystem enumeration and worker completion order.

Project checking prepares one in-memory artifact containing the validated
manifest, flattened source, lexical tokens, typed facts, and one original-source
origin per flattened token. Scheduling, ordinary C emission, and persistent
cache misses consume that same checked artifact. Structured issues retain an
inclusive token interval, allowing point and whole-expression ranges to project
back to module-local byte spans. Missing-effect, exact ownership-mode call,
recursive ownership-mode, nonexhaustive-Boolean, secondary Boolean-recovery,
aggregate-move, and borrowed-return diagnostics use the finalized checked issue
stream. Recovery retains its standalone order while projecting unknown names,
duplicate arms, arm-type mismatches, repeated moves, borrowed transfers, and
borrowed returns to their original module.

Each module source is structurally validated over its exact token slice before
declaration names are linked or the module is admitted to the prepared project.
Malformed source produces `E0102` with that module's identity and complete
source byte range. Name indexing also has its own token-vector exhaustion bound;
the validation order is the primary contract and the local bound prevents an
invalid internal caller from turning malformed input into unbounded compiler
work.

## Non-goals

- Packages, registries, dependency downloading, version solving, or lockfiles.
- Directory modules, implicit prelude modules, include files, or build scripts.
- Import aliases, glob imports, selective symbol imports, or re-exports.
- Cyclic modules, recursive interfaces, separate compilation ABI stability, or
  dynamic linking.
- Public/private syntax on declarations.
- A parallel default for compiler project checking.
