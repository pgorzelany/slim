# SLIM 1.0 compatibility

Version: 1
Status: stable for major version 1

`VERSION` is the only compiler-version source. `design/surface.tsv` and
`design/project-semantics.tsv` are the complete accepted language and project
inventories. `design/release-contract.tsv` names every frozen machine
interface and its current schema.

## Promise

For supported 1.x compilers:

- accepted 1.0 source remains accepted with the same defined behavior;
- names are not repurposed and no second spelling is added for an operation;
- project manifests and interfaces with schema 1 retain their meaning;
- diagnostic identities, primary byte spans, and JSON schema 1 remain
  machine-readable;
- generated C requires runtime ABI 1 exactly; and
- a schema version is changed before a tooling report changes incompatibly.

Compatible additions still require the feature policy, a decision, a unique
surface role, specification, diagnostics, conformance, and measured costs.
Deprecation does not silently change behavior. Removal or incompatible meaning
requires a new major version and a migration note.

## Deliberately weaker contracts

Persistent caches are validated evidence, not program artifacts. A compiler
may reject and rebuild them. Human diagnostic wording, optimization choices,
C identifier spellings, analysis facts explicitly reported as `unknown`,
performance within recorded budgets, and undocumented implementation details
may change in a 1.x release.

Versioned analysis, equivalence, proof, and edit reports are tooling data, not
executable SLIM. Consumers must reject unknown schema versions rather than
guessing. Generated C and `slim_rt.h` are a matched pair; mixing ABI versions
is unsupported and rejected by the C compiler.

## Compatibility changes

Every compatibility change needs a scored decision. The release gate checks
that the canonical ledgers, specification, conformance tags, diagnostic
contract, runtime ABI, and this policy agree. No compatibility alias is added
automatically: migration is performed by an explicit tool or source edit.
