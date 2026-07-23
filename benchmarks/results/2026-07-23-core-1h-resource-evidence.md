# Core 1H bounded resource evidence

Date: 2026-07-23
Decision: D0073
Status: accepted first slice

## Boundary

Analysis schema 6 publishes canonical tail-recurrence profiles and profiled
call workloads from the existing checked range view. It retains 16 profiles,
prints 64 call sites, counts truncation explicitly, and reports a nonliteral
controller as unknown. Check, C generation, runtime semantics, and Core syntax
are unchanged.

## Application evidence

Command:

    cargo run --release --quiet --bin slim-bench -- resources

Every maintained application is analyzed twice, must produce byte-identical
balanced reports, and must match `benchmarks/resource-baseline.tsv`.

| corpus | source bytes | profiles | profiled calls | exact calls | unknown calls | allocation sites | trap sites | owned bindings | exact total functions |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 14 applications | 20,282 | 2 | 4 | 4 | 0 | 24 | 142 | 27 | 5 |

`state_machine` and `signal_network` each expose one profile and two exact
2,000,000-iteration calls. The other 12 applications expose no recurrence in
the supported canonical shape.

That negative result matters: zero retained profiles does not mean zero work,
and a structural allocation site is not an allocation-volume or peak-byte
bound. This slice therefore does not justify source resource syntax.

## Permanent evidence

- A fixture proves positive, zero, and unknown call workloads.
- Boundary fixtures prove 16-profile and 64-call reporting limits, including
  the truncation guarantee.
- Governance requires the decision, implementation, fixtures, baseline,
  documentation, and verification command.
- The existing geometric analysis-scaling budget, full conformance,
  malformed-input, sanitizer, allocation-failure, comparison, runtime, and
  fixed-point bootstrap gates remain mandatory.

The portable self-hosted compiler reproduces a byte-identical 2,008,342-byte
C11 seed.
