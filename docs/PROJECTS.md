# SLIM deterministic projects

Status: implemented in Core 0.2

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

Module identities and declared names are ASCII identifiers without `/`.
Module identities may contain `.` for flat organizational names. `/` has one
meaning in Core 0.2: it separates a module identity from an exported
declaration in a qualified reference.

A path is UTF-8, relative to the directory containing the manifest, uses `/`
separators, ends in `.slim`, and contains no empty, `.` or `..` segment. It
must remain beneath the manifest directory after filesystem resolution.
Absolute paths, backslashes, symlink escapes, duplicate paths, implicit source
search, environment search paths, and filesystem enumeration are rejected.
The module name in each source file must equal its manifest identity.

## Imports, exports, and names

An import grants access to one module namespace, not to all transitive imports.
It does not copy declarations into the local namespace. Imported declarations
are referenced only as `module/name` in every position that currently accepts
a function or named type:

```text
(call math/add left right)
(let number math/Number value body)
(make math/Pair (left 1) (right 2))
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

Existing compiler operations accept either a `(module ...)` source or a
`(project ...)` manifest. The first form explicitly selects the input kind;
file extensions and directory search do not.

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
oracle and remains the default. CLI project operations use `.slim-cache/v1`
beside the manifest; the library's `compile`/`compile_with_jobs` entry points
remain cache-free clean oracles. The compiler never searches upward for a
manifest.

## Canonical interface artifact

Each module has one UTF-8 interface artifact:

```text
(interface 1 math
  (record Number ((value I64)))
  (fn add ((owned math/Number) (owned math/Number)) math/Number (effects)))
```

The grammar is:

```text
interface = (interface 1 MODULE declaration*)
declaration = record | variant | function
record   = (record NAME ((NAME TYPE)*))
variant  = (variant NAME ((NAME TYPE*)*))
function = (fn NAME ((MODE TYPE)*) TYPE (effects EFFECT*))
MODE     = owned | inout
```

Artifacts contain only exported declarations. Declarations are sorted by
name. Record fields and variant cases preserve semantic layout order. Effects
use the fixed `alloc`, `io`, `partial` order. Every named type is fully
qualified, including a type from the artifact's own module. Artifacts contain
no source bodies, spans, paths, timestamps, host data, target data, hashes, or
compiler scheduling data. Their stable fingerprint is computed over these
exact bytes.

Interface schema `1` is rejected rather than guessed when unsupported. A
schema change requires a compatibility decision and a compiler-version change;
there is no permissive reader for unknown fields.

## Incremental cache

The in-memory project session retains per-declaration parsed, lowered, checked,
interface, dependency, and generated results. Its invalidation unit remains the
Core 0.1 declaration identity `(module, kind, name)`.

The optional persistent cache stores enough validated evidence to skip a
module entirely across compiler processes: compiler/schema version, module
identity, normalized source fingerprint, direct dependency interface
fingerprints, canonical interface bytes, and generated C fragment. It never
stores an absolute path or trusts modification time.

Persistent entries use a length-prefixed binary format with the magic
`SLIMCACHE\0`, a big-endian schema number, bounded UTF-8 fields, bounded byte
fields, a bounded declaration count, sorted dependency pairs, and a trailing
stable checksum over all prior bytes. Source evidence combines the normalized
token fingerprint with module identity, entry role, and exports; dependency
interface fingerprints independently cover imports. Generated output is a
self-contained module C fragment assembled in topological order. The file name
is the hexadecimal stable fingerprint of the module identity. Readers reject
truncation, excess bytes, invalid UTF-8, malformed or noncanonical interfaces,
duplicate or unsorted dependencies, length overflow, version mismatch,
identity mismatch, and checksum mismatch. A rejected entry is ignored and
rebuilt from source; it can never make an invalid program pass. Cache writes
use a temporary file and atomic rename after successful checking.

The default cache directory is `.slim-cache/v1` beside the manifest. Cache
contents affect work performed, never diagnostics, interfaces, generated C, or
native behavior. A fresh in-memory session with an empty cache is the clean
oracle used by tests.

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

`ProjectSession` genuinely retains declaration-level lowered and checked ASTs,
dependency edges, fingerprints, and C fragments. A no-change update performs
zero declaration parse/lower/check/generation work. A private body edit does
one of each. A public layout edit checks the deterministic reverse module
closure. Failed edits leave the last good state untouched, and every successful
incremental output is compared with a clean compilation.

The remaining latency is not constant: every update still reads all module
files, lexically indexes them, rebuilds global lookup/graph structures, and
assembles the translation unit. Persistent reuse is currently an all-valid
fast path; one missing or rejected entry causes a safe clean rebuild rather
than partial cross-process reconstruction. The committed project benchmark
records these costs explicitly in
`benchmarks/results/2026-07-21-project.tsv`.

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
cache misses consume that same checked artifact. Structured type diagnostics
are projected back to module-local byte spans; the remaining legacy effect and
ownership reporters are being migrated to the same structured channel.

## Non-goals

- Packages, registries, dependency downloading, version solving, or lockfiles.
- Directory modules, implicit prelude modules, include files, or build scripts.
- Import aliases, glob imports, selective symbol imports, or re-exports.
- Cyclic modules, recursive interfaces, separate compilation ABI stability, or
  dynamic linking.
- Public/private syntax on declarations.
- Parallel execution in compiled SLIM programs.
