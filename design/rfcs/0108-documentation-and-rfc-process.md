# RFC-0108: Documentation and RFC process

Status: accepted
Implementation: complete
Process: 1
Audience: both
Author: SLIM project
Created: 2026-07-27
DecisionDate: 2026-07-27
Approver: project-maintainer
Kind: process
Primitive: none
Safety: 1
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 1
Dogfood: 1
Score: 50

## Summary

SLIM maintains one progressively disclosed Handbook, one canonical owner for
each current fact, a searchable algorithm gallery backed by benchmark sources,
and web access to development contracts, RFCs, and dated evidence. Durable
proposals use numbered RFCs with an explicit maintainer decision and a separate
implementation state.

## Motivation

The former documentation divided the programming model between a learner guide,
a language reference, focused contracts, project documents, decision records,
and benchmark reports. The material was accurate but repeated several concepts,
made implementation history look like required reading, and left useful
algorithm programs hidden inside the benchmark tree.

The earlier decision-record format also combined proposal disposition with
implementation evidence and did not give new proposals a complete guide-level
and reference-level review shape. SLIM needs a process in which user-visible
features are discussed before they become permanent language cost, while small
behavior-preserving engineering work remains lightweight.

## Guide-level explanation

Readers enter through one Handbook. Its foundations introduce source, values,
bindings, functions, control flow, structs, and enums. Systems chapters then
explain ownership, storage, host failure, and projects. Parallelism and compiler
tools are advanced chapters. Exact grammar, built-ins, diagnostics,
compatibility, and migration remain appendices rather than a second book.

Complete examples are embedded from repository-controlled fixtures. An
algorithm gallery publishes the twenty comparative benchmark programs. Binary
search, merge sort, breadth-first search, edit distance, N-Queens, and Game of
Life receive detailed walkthroughs; the other programs receive concise,
metadata-backed pages.

## Reference-level specification

Every tracked Markdown file is classified as current Handbook material,
normative contract, development documentation, RFC, dated evidence, or an
explicitly excluded operational file. Each published source has exactly one
canonical route. The website fails generation for unclassified Markdown.

Current maintained prose is budgeted separately from RFC and evidence history.
The Handbook may contain at most 10,000 prose words, and all current maintained
documentation may contain at most 16,000 prose words. Generated source
listings, RFCs, and dated benchmark reports do not count toward those limits.

RFC files live under `design/rfcs/`, use four-digit filenames, and carry the
canonical identifier `RFC-NNNN`. A current-process RFC records status,
implementation, audience, authorship, dates, approver, kind, primitive rating,
the six weighted ratings, and the calculated score.

Status is one of `proposed`, `accepted`, `rejected`, `withdrawn`, or
`superseded`. Implementation is one of `pending`, `complete`, or
`not-planned`. Only explicit project-maintainer approval changes a proposal to
accepted. An accepted RFC and its implementation may merge in the same change.

## Compiler and runtime design

This process adds no language form, executable representation, compiler
semantic path, runtime branch, or dependency. The Rust governance tool parses
and validates RFC metadata because governance remains bootstrap-independent
infrastructure. The static website derives pages from canonical repository
Markdown, ledgers, manifests, and source programs.

## Compatibility and migration

The 106 records formerly named `DNNNN` retain their numbers, bodies, ratings,
and dispositions. The 98 accepted records become accepted and complete legacy
RFCs. The eight rejected records become rejected and not-planned legacy RFCs.
Historical numbering gaps remain valid.

Existing website language and reference routes become static redirects to the
Handbook or Development destination. The machine-readable
`/reference/surface.json` route remains stable.

## Diagnostics and failure cases

Governance rejects malformed filenames, identifier/title disagreement, unknown
statuses or implementation states, invalid status/implementation combinations,
unknown kinds or audiences, missing process fields or sections, score
miscalculation, disposition-count drift, legacy identifiers, and active
surface rows that cite an RFC without accepted and complete implementation.

Website generation rejects unclassified Markdown, duplicate canonical routes,
missing example metadata, broken internal Markdown links, malformed challenge
metadata, and fixture references outside the repository.

## Performance and complexity

RFC validation and documentation classification are linear in the number and
size of repository text files. Search remains a static client-side index with
separate current, development, RFC, and evidence scopes. The compiler and
generated programs perform no additional runtime work.

## Alternatives and drawbacks

Keeping separate guide and reference books preserves familiar documentation
categories but repeats the same semantic facts. Publishing only current user
material keeps the website smaller but leaves compiler developers searching the
repository. Creating separate educational copies of benchmark algorithms makes
examples easier to edit but introduces source drift.

The accepted design makes the static build and governance checker more
responsible for classification and metadata. That cost is bounded, testable,
and preferable to manual publication lists or duplicated language text.

## Test and acceptance plan

Governance must validate all legacy and current RFCs, retain the historical
98/8 disposition counts, reject inactive surface owners, and find no legacy
identifier or directory spelling. Website tests must cover all Markdown,
canonical routes, redirects, search scopes, word budgets, algorithm metadata,
fixture-backed examples, both supported base paths, sitemap output, and
machine-readable surface output.

The complete bootstrap, governance, conformance, Cargo, performance, reduction,
parallelism, native comparison, agent, and website gates must pass.

## Ratings and evidence

The process improves safety by requiring explicit review of ownership and
safety changes, protects compile-time constraints through permanent feature
ratings, adds no runtime work, substantially reduces duplicated documentation,
improves analyzability of accepted surface ownership, and publishes the
self-hosting evidence used to justify changes.

The weighted score is
`(1*20 + 1*20 + 0*20 + 2*20 + 1*15 + 1*5) / 2 = 50`.

## Decision

Accepted by the project maintainer on 2026-07-27. Ratings may be proposed by an
assistant or other author, but the recorded disposition remains a maintainer
decision.

## Implementation

The accepting change migrates all records and references, extends governance,
consolidates the Handbook, publishes development and historical material,
builds the algorithm gallery, adds publication and word-budget gates, and
updates the static website routes and search scopes.

## Removal and supersession

This RFC can be removed only with a replacement accepted process that preserves
explicit maintainer approval, durable rejected-proposal rationale, canonical
fact ownership, and complete documentation publication coverage. It replaces
the former decision-record format and the separate Guide/Language Reference
publication shape without erasing their historical evidence.
