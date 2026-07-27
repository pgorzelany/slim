# RFC-0012: Canonical program arguments

Status: accepted
Implementation: complete
Process: legacy
Kind: language
Primitive: program-arguments
Safety: 2
Compile: 1
Runtime: 1
Minimal: 2
Analysis: 2
Dogfood: 2
Score: 80

## Need

The self-hosted compiler and ordinary tools must receive input paths without
global state or platform-specific foreign calls.

## Alternatives

Zero-argument entry points require hard-coded paths; implicit global argument
access hides an I/O capability; multiple accepted entry signatures duplicate
the program interface.

## Costs

Every executable receives one owned vector of borrowed argument byte strings.

## Evidence

The compiler consumes its source and output arguments through the same checked
vector and byte operations available to programs.

## Removal

Replace only with a smaller single explicit process-input capability; never
support multiple `main` signatures concurrently.

