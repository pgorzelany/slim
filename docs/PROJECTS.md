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
strictly sorted and unique. A module must not import itself. Reordering these
semantic sets is rejected with a canonical replacement instead of becoming a
second accepted representation.

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

`interfaces` is the only additional operation. It materializes the same
canonical interface bytes used by checking and caching; it is not another way
to compile. Project operations accept `--jobs N`; `--jobs 1` is the serial
oracle and remains the default. CLI project operations use `.slim-cache/v3`
beside the manifest; the library's `compile`/`compile_with_jobs` entry points
remain cache-free clean oracles. The compiler never searches upward for a
manifest.

## Canonical interface artifact

Each module has one UTF-8 interface artifact:

```text
(interface 3 math
  (struct Number ((value I64)))
  (fn add ((owned math.Number) (owned math.Number)) math.Number (effects)))
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
bodies and C fragments across framed updates. RFC-0143 integration and full M1
closure remain pending. Historical invalidation estimates have moved to a
measurement fixture compiled by the production compiler. See [incremental compilation status](INCREMENTAL.md) for the current
measurement boundary and the SLIM Next implementation requirements.

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
compiler changes. A versioned, compiler-identified cache is M1 work.

## Deterministic parallel checking

The coordinator computes stable topological layers. Modules in one layer have
no dependency edges between them and may be checked concurrently. Each worker
owns its parser, checker state, source, and output and receives immutable copies
of already completed dependency interfaces. Workers never wait for other
workers and share no mutable compiler state. The coordinator joins the finite
layer and merges results in module-identity order.

Worker count is bounded by the layer size, `--jobs`, and available hardware.
Diagnostics, interfaces, caches, generated C, and work counts must be identical
for one worker and every tested higher worker count. Parallel checking becomes
the default only if repeated geometric benchmarks show a benefit outside the
recorded noise band; otherwise the implementation remains available and the
default stays serial.

The committed geometric measurements did not justify a parallel default.
Across the tested wide and deep graphs through 129 declarations, two and four
workers were slower than the serial oracle because each owned worker currently
rebuilds declaration lookup state. The worker path remains useful correctness
infrastructure and opt-in experimentation; it is not presented as a speedup.

## Measured implementation boundary

The production compiler is the SLIM implementation and portable C seed.
Historical `ProjectSession` descriptions referred to a retained compilation
API that is absent from this production path. Current session counts describe
selected invalidations; they do not demonstrate cached checking or emission.
The [historical project measurements](../benchmarks/results/2026-07-21-project.tsv)
remain available, but must not establish a present-day reuse claim.

Whole-project artifact hits and dependency-invalidation estimates are separate
mechanisms. True declaration-local incremental compilation, transactional
last-good state, and actual operation counters are pending RFC-0112 M1.

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
- Parallel execution in compiled SLIM programs.
