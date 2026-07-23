# D0051: Retained recurrence boundaries

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 2
Runtime: 0
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 70

## Need

Function emission scanned every body for the atom `recur` to decide whether to
emit the tail-recursion label. That was the last function-level control-flow
fact inferred privately by the backend after checked memory planning had
already traversed the same function. It also classified any identifier spelled
`recur`, not specifically a recurrence form.

`FunctionPlan` now retains one `recursive` Boolean. Planning recognizes an
actual form whose head is `recur`; emission reads the retained fact and no
longer searches the body. The originating function-token identity check still
guards every plan consumer.

## Alternatives

The earlier combined allocation-and-recurrence summary widened and reshaped the
planner enough to increase self-validation from about 0.11 seconds to more than
eight seconds. A packed replacement for an existing Boolean still exceeded six
seconds. Both candidates were rejected without being retained.

Encoding recurrence as a token-link sentinel would give one field multiple
meanings and collide with the checked structural-link contract. Reusing an
allocation or destruction entry would similarly obscure the plan schema.
Leaving the scan in code generation would preserve two semantic authorities.

The accepted isolated field keeps the fact explicit and measurable. It adds no
source form, user-visible alternative, serialized artifact, or runtime ABI.

## Costs

Each planned function stores one Boolean. Planning performs one linear scan of
the bounded body while emission removes its former linear scan, so total
asymptotic compiler work is unchanged and the semantic decision moves to the
checked artifact boundary. A later unified expression walk may produce the
same fact without the dedicated planning scan, but must preserve this plan
interface and its evidence before replacing it.

The recognizer advances by token and checks only opening forms, making the fact
more precise than the former atom search. Generated functions still contain a
label exactly when their body contains a `recur` expression.

## Evidence

The executable `recur` fixture exercises label emission and prints `55`; the
non-recursive corpus continues to compile under `-Wall -Wextra -Werror`.
Governance requires the retained field, structural producer, and backend
consumer and forbids restoration of the body-wide backend scan.

The isolated source shape validates the self-host project in about 0.10
seconds, reaches a byte-identical candidate fixed point, and passes 98 fixtures
plus 2,000 deterministic malformed-input mutations. The portable fixed point
is 1,622,521 generated C bytes with SHA-256
`65d23caa0a0b70e7b4e5db8892538596f2a63e89a73ba89b273f013254a8025a`.
The complete release gate passes geometric and same-host performance budgets,
sanitizers, allocation-failure injection, native challenges, and deterministic
bootstrap verification.

## Removal

Replace the field only with another checked control-flow summary that identifies
real recurrence forms, remains bounded, and prevents ordinary emission from
rediscovering recurrence. Do not restore a textual backend scan, an overloaded
token sentinel, or a second backend-specific plan.
