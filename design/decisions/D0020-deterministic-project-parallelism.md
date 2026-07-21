# D0020: Deterministic project parallelism

Status: accepted
Kind: architecture
Primitive: none
Safety: 1
Compile: 2
Runtime: 0
Minimal: 0
Analysis: 2
Dogfood: 2
Score: 50

## Need

Independent modules are a natural unit for reducing clean project-check latency
on machines with multiple cores, provided scheduling cannot alter observable
compiler behavior.

## Alternatives

A global concurrent query graph adds shared-state and deadlock complexity
before the module model is proven. Unbounded thread-per-module execution wastes
resources. Serial-only checking leaves demonstrated independent work unused.
Bounded topological layers require no worker-to-worker coordination.

## Costs

The coordinator must copy immutable dependency interfaces, bound worker count,
join every finite layer, sort all results, and retain a serial oracle. Small
projects may be slower, so default parallelism depends on repeated benchmark
evidence rather than assumption.

## Evidence

Stress tests must vary worker counts and injected completion order while
comparing diagnostics, interfaces, work counts, generated C, and native output.
Benchmarks must report serial and parallel medians and may claim a benefit only
outside the recorded noise band.

## Removal

Remove if measurements show no useful benefit or if a simpler ownership-safe
scheduler proves equal determinism and lower complexity. No language or runtime
compatibility surface depends on this implementation strategy.
