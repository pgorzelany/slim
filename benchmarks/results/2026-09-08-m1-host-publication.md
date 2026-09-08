# M1 concurrent host publication repair — 2026-09-08

Full M1 closure remains pending. The clean release gate at
`6f50b8bc096bcd357f302f2c2c065bd46aa3c94b` found a real concurrent-build failure:
one of two builders could fail copying `native.c` into their shared output
directory. The executable already used private staging and atomic rename; the
new auxiliary files were copied directly to shared names.

The builder now copies the executable and every auxiliary file into one private
staging directory. Only after preparation succeeds does it rename complete files
into place, publishing the executable last. Cleanup removes the owned staging
directory. Simultaneous identical-input builders cannot race their copy operations.
This does not promise one atomic multi-file snapshot across builders with different
inputs: the loaded executable remains the runtime identity and semantic authority;
the auxiliary files support build verification.

The complete identity suite passes with host identity
`749c2213fa11cb06d4a81fd6a1af539326c88e37307c0d96d5aff81806800447`.
It checks concurrent publication, relocation, adapter/runtime/options changes,
an actual runnable alternate native target, loaded-worker stability and corrupt
seed rejection. A verification-only copy wrapper forces preparation to fail and
proves that the old executable and every auxiliary file remain byte-identical,
with no staging files left behind. The concurrency fixture now collects both
builder outcomes before reporting an ordinary failure and compares every auxiliary
file against its identical-input reference. That strengthened case passes separately.

The seed and frontend compiler bytes are unchanged. The new builder recipe changes
the host implementation identity. All required checkpoint commands, additional
benchmark-prefix gates and the current public native smoke pass in one invocation
after the repair. The next complete clean release run remains pending; prior passing
stages do not make the failed full invocation a pass.

The failed clean run had passed bootstrap, format/clippy/Cargo, governance,
conformance/library checks, all benchmark-prefix stages, continuation/ownership/
checked-state/literal/identity/flow/retained-typing campaigns and the complete
ordinary/sanitized public-session lifecycle checks. It stopped at concurrent build
publication, before the remaining identity, timing, retained-session/native and
release/website stages. [Repair evidence](archive/2026-09-08-m1-host-publication.json.gz)
preserves its exact log, the repair's commands and source identities, and the
targeted and checkpoint results. During the checkpoint, two Rust test executables
were slow to start. A process sample observed the conformance test at `_dyld_start`
with a 96 KB footprint before test output; it subsequently passed. The cause of
the startup delay is unknown, and it is not attributed to SLIM checking work.
