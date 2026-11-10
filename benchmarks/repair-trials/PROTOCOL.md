# Paired repair trials

The primary question is whether structured SLIM diagnostics shorten time to an independently accepted repair on four held-out tasks. Register the design and pin all eight broken projects, prompts, editable paths, private clients and expected results before the first participant starts. Keep the held-out contents with their author until development changes are frozen.

Use two fresh GPT-6.1 Sol High sessions per task, one per diagnostic format. Each receives the same broken source, prompt, editing authority, public language documentation and 900-second wall-clock budget. The sole intended difference is the output of the production compiler's `check` versus `check-json`. Both arms in each phase use the same compiler, runtime and acceptance clients. Participants may inspect their assigned candidate, edit declared files, and run the trial's check and submit operations. The restriction on reading sibling runs, references, checkout source and oracles is advisory on the shared host.

Start each clock before revealing the task or initial diagnostic. Capture the initial response, subsequent checks, submitted snapshots and hidden behavior results. An accepted submission ends its run. Record elapsed time through the acceptance result, including tool and host delays. Record unavailable sessions, interruptions and timeouts explicitly. Do not replace participants after seeing an outcome. Count wrapper invocations independently from elapsed time. Model thinking time and off-wrapper activity are unobserved.

Run development pairs d1–d4 first. Dispatch raw first for odd-numbered pairs and structured first for even-numbered pairs. Overlap the arms of a pair when capacity permits. Review development receipts, edits and participant observations. Select at most two diagnostic changes justified by actual repair friction, implement them in the production checker, and freeze the compiler before dispatching h1–h4 under the same protocol. Make any subsequent change a separate experiment.

Report all eight pairs with correctness, elapsed time and number of checks. For the primary decision, use h1–h4 only. Require a complete four-pair campaign, at least as many accepted structured repairs as raw repairs, and at least three jointly accepted pairs. For each jointly accepted pair compute `(raw_elapsed - structured_elapsed) / raw_elapsed`. The median must be at least 0.25. A completed campaign with fewer than three joint successes leaves the timing criterion inconclusive. Missing dispatch or infrastructure failure leaves the campaign incomplete. The outcome applies to these tasks and this model.

## Running a comparison

From the repository root, build the production compiler with `./bootstrap.sh`.
Use a new campaign directory for each comparison. Keep that directory for
receipt inspection and independent replay.

```sh
trial=benchmarks/repair-trials/run.py
campaign=build/repair-trials/comparison
python3 -B "$trial" prepare --campaign "$campaign" --phase development \
  --compiler build/toolchain/slimc --model gpt-6.1-sol --reasoning-effort high
```

Give the participant its `start` command as its first action, with its own task,
arm and unique session identifier. It follows the printed prompt inside that
session. After development, repeat `prepare` with `--phase heldout` and the
reviewed production compiler. Use the following command to calculate the
comparison from the recorded runs.

```sh
python3 -B "$trial" report --campaign "$campaign"
```
