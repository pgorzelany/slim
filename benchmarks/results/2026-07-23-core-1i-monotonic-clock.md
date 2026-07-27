# Core 1I monotonic clock

Date: 2026-07-23
Decision: RFC-0075
Status: accepted first host-service slice

## Boundary

`io.monotonic-ms() -> I64` is the only clock. It requires the existing `io`
effect, allocates no SLIM memory, adds no `partial` effect, exposes no epoch,
and cannot enter pure reduction or automatic reordering. Successive readings
on one execution thread do not decrease.

The slice adds no host object, raw handle, pointer, foreign declaration, wall
time, calendar, sleep, timer, alias, dependency, or second I/O mechanism.

## Runtime evidence

Command:

    cargo run --release --quiet --bin slim-bench -- host

The permanent probe performs 100,000 samples in generated SLIM and in a C
reference using the same runtime operation. The acceptance run reported:

| calls | SLIM | C | SLIM / C |
| ---: | ---: | ---: | ---: |
| 100,000 | 4,139 us | 4,178 us | 0.991 |

The durable same-host ratio limit is 2.00. It tracks language call and recurrence
overhead without treating host clock latency as compiler performance.

## Safety evidence

- Native conformance samples twice and checks nondecreasing `I64` values.
- A pure caller receives the stable missing-`io` diagnostic.
- Analysis records four effectful calls, zero allocation sites, zero trap
  sites, and unknown totality.
- The runtime clamps failed, overflowing, or regressing samples to the prior
  thread-local value.
- Fixed-point bootstrap and the full correctness, malformed-input, sanitizer,
  allocation-failure, scaling, native comparison, and parallel gates remain
  mandatory.
