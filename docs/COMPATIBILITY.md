# SLIM 0.9 compatibility

Status: SLIM 0.9 — experimental, pre-1.0

`VERSION` is the canonical compiler version. `design/surface.tsv` and
`design/project-semantics.tsv` inventory the accepted language and project
surface. `design/release-contract.tsv` records each machine interface schema.

## Pre-1.0 policy

- A `0.y.0` release may make breaking source or documented-behavior changes
  through an accepted decision and complete migration notes.
- A `0.y.z` patch release preserves accepted source and documented behavior
  within that minor version.
- No long-term source-compatibility promise begins until a future explicit
  1.0 freeze.

SLIM 0.9 deliberately has one canonical spelling for each operation. Removed
spellings do not remain as aliases, and there is no legacy parser in the final
compiler.

Project manifest schema 1 remains current because its structure is unchanged.
Interface and cache schemas are 2 because qualified names and stored source
identities changed. Generated C retains runtime ABI 1 because runtime
semantics and representation did not change.

## Future 1.0 freeze

A future 1.0 requires an accepted compatibility decision, a frozen canonical
surface and project contract, complete conformance coverage, stable diagnostic
and tooling schemas, migration notes for the preceding 0.y release, and
recorded compiler/runtime performance budgets. It is not implied by completing
an internal milestone.

## Historical decisions

D0082 and D0083 remain historical evidence for the earlier internal 1.0
candidate. D0107 supersedes their current release-status and major-version
compatibility promises without rewriting that recorded evidence.

Every later compatibility change still requires the feature policy and an
accepted decision. Tooling consumers must reject unknown schemas rather than
guessing; rebuildable caches may always be rejected and reconstructed.
