# Metrics report API repair

Migrate the scalar metrics API in this four-module application.
Edit only the five supplied project/source files; preserve the module boundaries.

`metrics.Summary` must have fields, in order, `count`, `total`, `minimum`,
`maximum`, all `I64`. `metrics.summarize(values: Vec[I64]) -> Summary` reads its
shared vector and returns its length, sum, minimum and maximum. For an empty
vector all four fields are zero. Intended inputs have at most 32 elements, each
in [-100,100], so sums fit. Its exact declared ceiling is `effects[partial]`;
it must perform no IO or allocation. Internal recurrence helpers are permitted.

`render.render(summary: metrics.Summary) -> Void effects[io]` prints one line
`count,total,minimum,maximum` with decimal numbers and no spaces. `report.send`
accepts a shared `Vec[I64]`, summarizes it once and renders that result once;
its exact ceiling is `effects[io, partial]`. Preserve the original application
input [6,-2,9] and return code zero; its output becomes `3,13,-2,9\n`.
Keep send as straight-line composition of those two calls; no conditional,
recurrence or additional helper call in send is needed or permitted.

Update signatures, fields, imports/exports and all dependent callers. Do not keep
the old scalar public API, add another output path, allocate/copy in summarize,
or suppress the compiler's ownership/effect checks. The vector must remain usable
after summarization and sending. Check/build the complete project and exercise
the application before submission. The independent evaluator also uses other
clients and inputs; participant-authored tests are feedback, not acceptance.
