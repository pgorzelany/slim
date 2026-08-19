# RFC-0111: Explicit source standard library

Status: proposed
Implementation: pending
Process: 1
Audience: both
Author: SLIM project
Created: 2026-08-18
DecisionDate:
Approver:
Kind: compatibility
Primitive: none
Safety: 1
Compile: 1
Runtime: 1
Minimal: 2
Analysis: 0
Dogfood: 2
Score: 55

## Summary

SLIM should ship a small standard library as ordinary canonical SLIM source.
Programs import every used module explicitly through their project manifest.
The compiler adds no implicit prelude, installed-library search, alternate
module resolution, generated source dialect, or privileged implementation
path.

The first stable candidates are ASCII classification, byte-span operations,
owned text construction, checked decimal parsing, source spans, byte cursors,
integer helpers, and deterministic testing. Type-specific collection modules
and format or protocol libraries graduate only after maintained applications
reuse them without changing their semantic contract.

Until this RFC is accepted, implementations live under an experimental
library namespace and carry no compatibility promise.

## Motivation

The self-hosted compiler and maintained applications repeatedly implement
integer emission, decimal parsing, byte equality, byte-span copying, bounded
search, and test reporting. These copies increase code and proof burden without
providing useful domain separation. At the same time, the rejected generic
experiment in RFC-0109 showed that a broad abstraction mechanism is not
justified by compiler dogfooding alone.

Ordinary source modules can establish which operations are genuinely shared,
make allocation and partiality visible in reusable APIs, and expose concrete
ownership or performance limits before a compiler primitive is proposed. A
standard library is therefore application evidence as well as user software.

## Guide-level explanation

A project vendors the required library source beneath its manifest directory,
declares each module and dependency, and calls exported operations through the
normal qualified module name. Nothing is available merely because a compiler
release contains it.

Library functions follow the same rules as application functions:

- allocation, I/O, and unproven termination appear in effects;
- affine inputs are shared, exclusively borrowed, or transferred through the
  existing parameter modes;
- failure that carries information uses an explicit enum result;
- iteration and output order are deterministic; and
- a function does not merely rename an existing built-in.

Applications may keep local helpers. A candidate graduates only when at least
two unrelated maintained consumers use the same contract, it has complete
boundary and malformed-input tests, and its measured behavior is suitable for
a shared implementation.

## Reference-level specification

This RFC adds no program grammar, type, effect, built-in, runtime ABI, project
manifest clause, interface schema, or cache schema.

The distributed library consists of canonical `.slim` modules. Every module is
listed explicitly in the consuming schema-1 project manifest. Source paths obey
the existing confinement rule and remain beneath that manifest. Imports,
exports, qualification, interfaces, caching, checking, and C emission are the
existing project operations without exceptions.

The stable initial module roles are:

- ASCII byte classification and digit conversion;
- byte equality, range comparison, search, count, and explicit copying;
- an owned byte-text builder with explicit allocation and consuming freeze;
- checked decimal parsing with distinct invalid, overflow, and success cases;
- copyable source spans and sequential byte cursors;
- nonduplicating integer composition helpers; and
- a deterministic test result and reporting model.

Module and declaration names are finalized only by an accepting revision of
this RFC after the experimental application evidence is recorded. Format,
protocol, graph, set, queue, sorting, and bit-set modules are not stable merely
because an experimental implementation exists.

## Compiler and runtime design

The compiler treats library code exactly like vendored application code. It
does not recognize library module identities, declarations, or call sites.
Unused modules are absent from the project and incur no parse, check, generated
code, binary-size, initialization, or runtime cost.

The release archive may contain a canonical library source directory and a
versioned inventory. A vendoring command or packaging script may copy those
exact sources beneath a consumer project, but it cannot resolve versions,
download dependencies, search parent directories, edit source semantics, or
make a module implicit. The normal checker remains the authority over copied
source.

Compiler-adjacent analysis, linting, formatting, interface comparison, and
editing continue to reuse the one retained checked artifact. A library tool
must not introduce a second SLIM parser or executable representation.

## Compatibility and migration

The proposed library has no compatibility promise before acceptance. Existing
programs, project manifests, generated C, caches, interfaces, and runtime ABI
remain unchanged.

If accepted, stable library releases are source-level API contracts tied to a
documented SLIM release series. A breaking stable API change requires a later
compatibility RFC and an atomic migration; removed names are not retained as
aliases. Experimental modules may change or be removed without migration.

An installed library root, implicit prelude, registry, version solver,
dependency download, lockfile, cross-project path escape, or re-export model is
outside this RFC. Any such mechanism requires a separate accepted architecture
or project-semantics decision.

## Diagnostics and failure cases

Library source receives ordinary parser, type, ownership, effect, project, and
runtime diagnostics. There are no library-specific compiler diagnostics.

The library conformance suite must cover empty input, boundary indices,
malformed decimal input, exact `I64` limits, overflow, allocation failure where
the operation allocates, consuming builder freeze, deterministic test order,
and incorrect ownership or effect use at public call boundaries.

Vendoring or release tooling must reject a missing inventory entry, path
escape, content mismatch, duplicate destination, or unsupported inventory
version before publishing a project tree. Such tooling failures are not
language diagnostics and cannot make invalid SLIM source pass.

## Performance and complexity

Default compiler behavior is unchanged. A consumer pays linear frontend work
for the library modules it explicitly lists. No unused library module is read
or generated.

Each general traversal is linear in its named byte or collection range unless
documented otherwise. Decimal parsing is linear in the number of consumed
digits and detects overflow before the overflowing arithmetic operation.
Builder operations make allocation visible and must avoid accidental
quadratic reconstruction. Type-specific collection candidates publish their
own scaling and allocation evidence before graduation.

The permanent application gate records clean check and C-emission work,
incremental private and public edits, runtime, binary size, allocation sites,
checked trap sites, and exact, bounded, or unknown analysis facts. A compiler
feature may claim library benefit only from a same-source before/after result
or an explicit checked source migration.

## Alternatives and drawbacks

Compiler intrinsics make common calls short but permanently expand privileged
surface and prevent ordinary application evidence from separating semantic
need from convenience. An implicit prelude reduces manifest text but creates
hidden dependencies and work. A package manager solves distribution at the
cost of registries, versioning, authority, network behavior, and a second set
of project decisions before SLIM has external packages to manage.

Source vendoring repeats library bytes across independent project trees and
does not by itself provide ecosystem version resolution. Explicit source and
dependencies are nevertheless consistent with the current deterministic
project model and preserve a simple clean-build oracle.

## Test and acceptance plan

Before acceptance:

- implement the experimental foundation modules and deterministic test entry;
- use the same contracts in the self-hosted compiler where the project layout
  permits and in at least two unrelated substantial applications;
- record code removed or centralized from existing helpers;
- add success, boundary, malformed-input, allocation-failure, ownership, and
  effect tests;
- measure clean and incremental compiler work plus library runtime scaling;
- generate and compare canonical public interfaces twice; and
- pass bootstrap, governance, conformance, Cargo, performance, reduction,
  parallelism, comparison, and agent gates.

An accepting revision must name the stable modules and exports, record dated
application evidence, add the library sources and inventory to the release
contract, and document exact vendoring and clean-build steps.

## Ratings and evidence

Safety is `+1` because checked shared implementations replace repeated parsing
and boundary logic without weakening callers. Compile is `+1` because imports
remain explicit and unused modules cost nothing, while shared modules provide
real incremental dependency boundaries. Runtime is `+1` because measured
builders and parsers can replace repeated application implementations without
a runtime abstraction mechanism. Minimality is `+2` because ordinary source
composition directly avoids new primitives, prelude rules, and package
semantics. Analysis is `0` until application measurements show that the shared
contracts improve retained facts. Dogfood is `+2` because the self-hosted
compiler already contains repeated candidate operations and is a required
consumer.

The weighted score is
`(1*20 + 1*20 + 1*20 + 2*20 + 0*15 + 2*5) / 2 = 55`.

## Experimental implementation evidence

As of 2026-08-19, the pre-acceptance implementation exists under `library/`:

- ten ordinary source modules cover the foundation and the two collection
  shapes justified by maintained applications;
- the self-hosted compiler uses one shared signed-integer renderer, removing
  local copies without adding a privileged compiler path;
- NDJSON, transactional ledger, SAT, raster, LZ4, and bounded HTTP programs
  exercise the modules across parsing, state, search, binary output, and host
  I/O;
- canonical schema-3 interface comparison and reference generation are written
  in SLIM;
- a deterministic typed generator checks 12 valid programs and rejects 12
  seeded record, variant, effect, and exhaustiveness mutations per corpus run;
- repeated application C emission is byte-identical, and ten same-host
  emit/check ratios are gated at 2.0 under RFC-0030; and
- bootstrap, governance, conformance, Cargo, scaling, reduction, parallelism,
  comparison, resource, host, incremental, project, sanitizer,
  allocation-failure, and agent gates pass together through
  `scripts/verify.sh`.

This evidence does not accept the RFC or stabilize the experimental module
names. Stable release inclusion, an inventory/vendoring contract, and the
compatibility decision remain blocked on explicit maintainer review.

## Decision

Pending explicit project-maintainer review. Implementation evidence may be
developed experimentally, but this proposal must not be described as accepted
or stable before that decision.

## Implementation

Pending acceptance. The experimental implementation uses ordinary source
projects and adds no compiler recognition while the proposal remains
undecided.

## Removal and supersession

Before acceptance, the experimental library may be removed without changing
the language, project, runtime, or compatibility contract. If accepted, a
replacement must preserve explicit dependencies, canonical checked source,
visible effects and ownership, deterministic behavior, and zero cost for
unused modules unless a later accepted RFC explicitly changes one of those
properties.
