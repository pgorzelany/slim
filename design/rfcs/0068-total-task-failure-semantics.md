# RFC-0068: Total-task failure semantics

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

Parallel execution cannot be considered until trap order, allocation failure,
ownership transfer, cancellation, race freedom, and deadlock freedom have one
precise meaning. Arbitrary effectful tasks make those questions expensive and
often impossible to answer statically.

Core 1F instead defines an intentionally narrow total-task model. A selected
task is eligible only when the complete checked view proves termination, no
defined trap, no allocation or I/O, no lexical mutation, no exclusive borrow,
and no unsafe or unknown callee. The ordinary ownership checker has already
rejected two expressions that move the same owned value. Therefore the two
initializers may be evaluated concurrently without an observable task failure
or shared mutable location.

If execution is implemented later, the parent prepares owned inputs, attempts
one structured child, evaluates the other task, joins the child, and installs
both results into their original immutable bindings before the continuation.
Worker-creation failure executes the same work inline. A task cannot require
cancellation because no accepted task can fail. A child cannot wait or spawn
through source surface, and the parent owns the only join handle, so the model
contains no wait cycle.

## Alternatives

Preserving source trap order between partial tasks would require buffering,
cancellation, and a policy for simultaneous failures. Parallel allocation would
need deterministic allocation-failure arbitration. Locks, channels, detached
tasks, futures, and user-visible cancellation would each add language and
runtime surface. Treating external worker failure as a program effect would
make a proven-total expression platform-dependent.

The accepted subset avoids all of those semantics. Worker creation is an
optimization attempt with serial fallback, not a source-visible operation.

## Costs

This decision changes no generated code and adds no runtime. The existing
blocker proof already computes every premise. Reports retain exact
`race-free` and `deadlock-free` facts only for the total-task subset.

Any future backend must move each owned input exactly once into either a worker
context or its inline fallback, join every successfully created worker exactly
once, destroy task-local storage on both paths, and expose no lock handle.

## Evidence

Permanent fixtures reject allocation, I/O, checked traps, unsupported
recurrence, mutation, exclusive borrows, unsafe callees, cycles, and bounded-away
facts. The positive `state_machine` site consumes distinct owned records,
performs two proven-total computations, and joins before comparison and output.
The overlap policy ensures selected intervals do not nest or cross.

The report makes the production status explicit: execution is disabled, so the
accepted semantics add zero runtime and preserve current source-order behavior.

## Removal

Never admit a partial, allocating, I/O, mutating, exclusively borrowing, or
unknown task into this model. Never drop a successful join or make worker
creation failure observable. Adding task failure or cancellation requires a new
scored language/runtime decision and adversarial ordering tests.
