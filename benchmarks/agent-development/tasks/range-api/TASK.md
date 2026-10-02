# Range decision API migration

Migrate this four-module application from a Boolean range predicate to one typed
decision API. Edit only the five supplied project/source files; retain module
boundaries and exercise every dependent caller.

Keep `rules.Limits` fields `lower`, `upper`, in order, both I64. Replace
`rules.contains` entirely with `rules.Decision`, cases in this exact order:
`Below`, `Inside(I64)`, `Above`. Implement
`rules.classify(value: I64, limits: Limits) -> Decision` with no effect clause.
Return Below when value < lower, otherwise Above when value > upper, otherwise
Inside(value). This ordering also defines behavior for reversed limits.
Remove the contains function/export rather than retaining a compatibility alias.

Replace the scalar `stats.summarize` result with `stats.Summary`, fields in order
`accepted`, `total`, `rejected`, all I64. Its shared-vector signature is
`summarize(values: Vec[I64], limits: rules.Limits) -> Summary effects[partial]`.
Use classify and exhaustive enum matching: accepted counts Inside values, total
sums their payloads, rejected counts Below/Above values. No allocation or IO.
Do not read the limit fields directly in stats; classify owns that decision.
Input vectors have at most 32 elements, values/limits in [-100,100].

`gateway.score(values: Vec[I64], limits: rules.Limits) -> I64 effects[partial]`
returns accepted*10 + total from the new summary. Update imports/exports,
signatures, matches and all callers. The application retains [-4,0,5,11] and
limits [0,5], prints `25\n`, and returns zero. The vector remains shared and
usable by callers. No second predicate alias or additional effect is permitted.
