# Prospective protocol 2 corpus

The manifest fixes twelve task pairs before participant dispatch. Six pairs run
baseline first and six context first, alternating by task order. Each condition
receives a fresh candidate containing identical initial bytes, the task text and
the same public documentation. The context condition additionally admits the
production context operation. Budgets are 900 seconds, 40 admitted operations,
and 60 seconds per subprocess. These are trial bounds, not a completion estimate.

| Task | Existing substrate | Initial check | Independent finite domain |
| --- | --- | --- | --- |
| text-budgeted-quote | selfhost/text | rejected | 1,297 byte, span, encoding and resource probes |
| identity-span-intersection | selfhost/identity | accepted | 1,312 provenance and half-open interval probes |
| format-call-modes | selfhost/format | accepted | 12 canonical/whitespace/multiline inputs, checked before/after formatting |
| effects-call-ceilings | selfhost/effects and ordinary checker | accepted | 21 capability classifications and 24 complete checker sources |
| netstring-batch-atomic | std_netstring/std_bytes/std_text | rejected | 66 batch validation, order, atomicity and count-bound probes |
| byte-index-range | std_byte_index | accepted | 196 complete-key endpoint pairs, binary/prefix/order/ordinal controls |
| decimal-exact | std_decimal/std_ascii | accepted | 283 canonical signed I64, malformed, absolute offset and overflow probes |
| hex-atomic-codec | std_ascii and original library project | rejected | 1,054 codec, output-budget, source-limit and atomicity probes |
| catalog-ordinal-sum | catalog loader/index/emitter | accepted | 76 catalog selections, input permutations and checked aggregate states |
| workplan-critical-ties | workplan loader/scheduler/emitter | accepted | 54 permuted DAGs, tie/critical/cycle/overflow controls |
| http-header-window | existing HTTP parser/model | accepted | 26 token-name, exact header-window and preserved protocol controls |
| ledger-overflow-transaction | existing ledger state/model/parser | rejected | 143 commands observing every field of both account poststates |

Every task also includes one independently specified negative caller diagnostic
and a separate accepted positive caller. Complete original submitted projects
are always checked first. Task entry tests cannot confer acceptance. Clients
replace only the admitted entry source and call the same submitted target module;
fixed manifests preserve imports, exports, original modules and their ordinary
checker authority. Four compiler tasks retain the complete actual selfhost
project, rather than implementing another parser/checker in Python.

`base` mappings freeze existing repository source once per task. Initial and
reference directories override only changed source and the fixed task manifest;
there are no durable copies of whole compilers. The workplan base is the original
scan scheduler, before the separately coordinated heap optimization. References
establish feasibility only. They are never imported, executed or read to produce
the expected bytes. No task repeats protocol 1 or merely reorders a manifest.

`generate_expected.py` checks the checked-in expected bytes from independently
authored finite byte/integer/DAG/transaction models and explicit canonical/truth
table goldens in `fixtures/domain.json`. It does not parse general SLIM or invoke
a compiler/reference. Acceptance JSON contains only fixed operation enums,
source overlays, exact expected byte paths and diagnostics; the evaluator never
executes this authoring Python or data-provided commands. `--write` is an explicit
prospective authoring operation and must not be used after freeze/dispatch.

Evidence is bounded to these domains. Native compilation of a reference is
feasibility evidence, not participant success. Finite correctness does not prove
universal behavior, asymptotic cost, unobserved model tokens/calls, or isolation
against arbitrary same-filesystem reads. Participant outcomes, wrapper/native
times and leakage/intervention observations remain separate facts. Optional
later production use of a repaired utility requires its own review and gates;
it cannot change the frozen task or its measured outcome.
