# D0060: Allocation-free region elision

Status: accepted
Kind: architecture
Primitive: none
Safety: 2
Compile: 1
Runtime: 2
Minimal: 2
Analysis: 0
Dogfood: 2
Score: 75

## Need

The Core 0.4 region rule selected a child region for functions whose result and
`inout` outputs could not carry storage. Code generation initialized and
destroyed that child even when the retained function plan proved the function
and all called operations performed no allocation. Small pure helpers therefore
made two external region calls, blocked useful C inlining, and dominated tight
record and variant loops.

Code generation now creates the child only when both the retained placement
decision and `function_plan_allocates` are true. An allocation-free function
forwards its caller region as the otherwise-unused ABI argument. Functions with
any direct or transitive `alloc` call retain their existing child or caller
region, failure label, and reverse destruction path.

## Alternatives

Deleting regions from all pure-looking source functions would repeat effect
analysis in the backend and miss transitive allocation. Removing the region ABI
parameter would require call-graph specialization and a second calling
convention. Relying on interprocedural C optimization cannot erase calls to the
stateful out-of-line region API. A source annotation would expose an
implementation fact already proved by the checked plan.

The retained allocation-site vector is exact for accepted calls and already
controls failure propagation. Combining its nonempty fact with the retained
placement Boolean adds no analysis pass, plan field, syntax, or runtime mode.

## Costs

Code generation computes one Boolean conjunction per function. Allocation-free
generated functions still accept and forward `SlimRegion *` so ABI and call
lowering stay uniform. The optimization relies on the invariant that every
allocating built-in or user call is present in the retained plan; existing
effect, allocation-failure, and planned-call fixtures govern that producer.

Generated C changes for allocation-free functions, so the portable bootstrap
seed advances. Allocation-capable functions remain structurally unchanged.

## Evidence

Both profiled gaps were dominated by allocation-free helper calls. Same-host
quick results reduce the records workload from 5.080 to 1.844 milliseconds
(2.75x) and variants from 17.280 to 3.387 milliseconds (5.10x). Candidate C
oracles measure 1.283 and 3.265 milliseconds respectively, giving SLIM/C ratios
of 1.437 and 1.037. Their permanent limits tighten to 3.0 and 2.0.

The generated challenge C contains no `slim_function_region` for its
allocation-free functions, while allocating fixtures retain child-region
initialization, failure propagation, and destruction. Five warm self-host
checks each consume 0.03 seconds of user CPU, down from D0059's 0.05 seconds.
The authoritative release run measures records at 2.104 versus 1.369
milliseconds for C and variants at 3.421 versus 3.542 milliseconds for C. All
109 fixtures, 2,000 mutations, tightened budgets, sanitizers,
allocation-failure injection, native challenges, and agent feedback checks
pass.
The fixed point is 1,622,580 generated C bytes with SHA-256
`a22b64129cd5e4808c4aa4bc256218e894589933452474a5aa3b2fde82365796`.

## Removal

Restore an allocation-free child region only if the retained plan can no longer
prove the absence of direct and transitive allocation. Any replacement must
keep one ABI, preserve all allocation-failure behavior, and pass the tightened
records/variants ratios. Do not add a source region hint or backend effect scan.
