# RFC-0050: Bounded project module validation

Status: accepted
Implementation: complete
Process: legacy
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

Project loading previously appended each source file to one shared token vector
and indexed declaration names before validating that module's token slice. A
missing closing delimiter could therefore make name indexing advance beyond
the available tokens indefinitely. Standalone inputs already passed through
structural validation, but the project path placed semantic work before that
boundary.

Every loaded module is now structurally validated over its exact half-open
token slice before declaration linking or retention. Invalid source reports
`E0102` against the declared module and its complete source byte range. Name
indexing also stops at the token-vector length independently, so this internal
utility remains bounded even if a future caller violates the validation
ordering.

## Alternatives

Validating only the final flattened translation unit was rejected because
flattening and name linking already assume well-formed module boundaries. A
special project lexer or a second parser would duplicate the canonical syntax
rules and create another behavior to keep synchronized. Making the linker
silently accept malformed input would prevent nontermination but could retain
partial declarations and defer the real error.

A new token-slice type was not introduced. The existing `root` and `end`
indices are sufficient, remain internal to the compiler, and do not add a
source feature or runtime representation.

## Costs

Project preparation performs one additional linear structural validation per
module before linking. The same source is structurally validated again after
flattening, where executable shape and the unique entry point are checked;
that later check is retained as defense at the composed-program boundary. The
existing project geometric benchmark and exponent budget cover this added
work. Standalone compilation and generated-program runtime are unchanged.

The structural slice is capped at 1,000,000 tokens, matching the established
module bound. Diagnostics deliberately use the complete source range because
the shallow lexer does not retain an unmatched-delimiter stack from which a
smaller trustworthy primary interval could be derived.

## Evidence

`project-malformed-module` omits its module closing delimiter and must produce
exactly `E0102@app@0:59`. It runs through the same bounded conformance harness
as every other project fixture. Governance pins the project-to-validator call,
the slice entry point, the independent name-index exhaustion guard, the
manifest dependency, and the semantic ledger row.

The first accepted conformance run passes 98 fixtures and 2,000 deterministic
malformed-input mutations. Self-validation remains about 0.10 seconds. The
three-generation refresh reaches a byte-identical 1,622,171-byte C fixed point
with SHA-256
`7678b680cf808a216bc9b1077f28802434f04bc03b810cde5b9b1f39a213441f`.
The complete release gate also passes the project geometric and same-host
performance budgets, sanitizers, allocation-failure injection, native
challenges, and deterministic bootstrap verification.

## Removal

Replace this boundary only with a project loader that proves each source slice
well formed before any semantic consumer can observe it, preserves bounded
behavior on malformed input, and retains stable module-local diagnostics. Do
not remove the independent linker bound unless its interface makes an invalid
cursor unrepresentable. Do not add a second project grammar or permissive
recovery path.
