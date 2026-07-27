# RFC-0082: Freeze the SLIM 1.0 contracts

Status: accepted
Implementation: complete
Process: legacy
Kind: compatibility
Primitive: none
Safety: 2
Compile: 1
Runtime: 1
Minimal: 2
Analysis: 2
Dogfood: 0
Score: 75

## Need

Core 1K has a self-hosted compiler and enough demonstrated language surface
for maintained applications, but a version number alone would not make that
surface dependable. Source forms, project artifacts, diagnostics, tool
reports, generated C, and the runtime need explicit compatibility boundaries
before downstream programs can rely on them.

SLIM 1.0 therefore freezes the exact accepted surface and project ledgers,
names each machine-readable schema, gives the C runtime an exact ABI number,
and derives every displayed compiler version from one `VERSION` file. This
decision adds no language operation.

## Alternatives

Freezing only source syntax leaves generated programs, diagnostics, projects,
and agent tools vulnerable to silent drift. Freezing every byte forever would
prevent compatible diagnostics and implementation improvements. Maintaining a
second copied list of all language forms would itself create drift.

The accepted policy applies one compatibility class to each canonical
artifact. The existing ledgers remain the sole surface inventories;
`design/release-contract.tsv` records versions and policies rather than
duplicating them.

## Costs

Within major version 1, accepted source remains accepted with the same defined
behavior and stable diagnostic identities. Additive tooling-report changes
require a new schema version. Cache artifacts may be rejected and rebuilt.
Generated C and runtime headers must have the same ABI number and fail during
C compilation when they do not.

These are release and governance checks. Normal parsing and checking gain no
pass, generated programs gain no runtime branch, and the language gains no
alias, compatibility syntax, unsafe escape, dependency, or second IR.

## Evidence

Governance maps every accepted surface and project behavior to an accepted
decision and conformance tag, validates the release-contract ledger, and
checks one canonical compiler version. Runtime tests compile matching
artifacts and deliberately reject a mismatched header. Diagnostic tests pin
codes, byte spans, ordering, and JSON schema. The complete bootstrap,
conformance, malformed-input, sanitizer, fault, application, and performance
gates remain mandatory.

## Removal

Before 1.0, remove a contract only if its artifact is removed from the release.
After 1.0, an incompatible source, project, diagnostic, or ABI change requires
a new major version and migration documentation. A rebuildable cache or
versioned tooling schema may change under its recorded policy; it must never
silently reinterpret old bytes.
