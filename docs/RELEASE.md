# Building and verifying SLIM 1.0

Version: 1
Status: release contract

SLIM releases are source archives rooted at `slim-VERSION/`. They contain the
self-hosted compiler, checked-in portable C11 seed, runtime, launcher,
canonical design ledgers, specification, tutorial, and maintained examples.
The archive does not contain build output, caches, repository history, or the
website toolchain.

## Requirements

A C11 compiler, POSIX shell, standard utilities, and either `sha256sum` or
`shasum` are required. The validated release target is Darwin/arm64 with Apple
Clang. The generated C is deliberately portable C11; other targets are
supported only after the same clean-package gate passes there. POSIX workers
are selected on Darwin, Linux, and BSD targets; otherwise the deterministic
serial tier is used.

## Clean install

```text
tar -xzf slim-1.0.0.tar.gz
cd slim-1.0.0
./bootstrap.sh
./slimc --version
./slimc check examples/hello.slim
./slimc run examples/hello.slim
```

`bootstrap.sh` verifies the seed digest, compiles it, regenerates the compiler
from `selfhost/slim.project`, proves two successive generations byte-identical,
and runs Hello. No Rust compiler is part of the production bootstrap.

## Reproducing a release

From a clean repository:

```text
./scripts/verify.sh
./scripts/package-release.sh build/release
```

The packager copies only `release/manifest.txt`, normalizes paths, permissions,
timestamps, owner metadata, and gzip headers, then emits the archive and its
SHA-256 file. Packaging the same committed source twice must produce identical
bytes.

The release is accepted only when the normal repository gate, two-archive
reproducibility check, clean extraction/bootstrap/build/run test, runtime ABI
mismatch test, and documentation/website drift checks pass.
