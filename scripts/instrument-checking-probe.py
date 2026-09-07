#!/usr/bin/env python3
"""Observe fixed generated checker definitions, never parse or accept SLIM."""
import re
import sys

NAMES = {
    "infer_expr": "entry", "infer_control_walk": "machine",
    "push_pending": "push", "finish_type": "finish",
    "finish_builtin": "builtin",
    "finish_scalar_operand": "scalar",
}
definitions = {name: 0 for name in NAMES.values()}
ends = {"entry": 0, "machine": 0}
headers = decisions = mains = 0
current = None
variables = {}
output = []

for line in sys.stdin:
    match = re.fullmatch(r"static .* slim_fn_typing_95([A-Za-z_0-9]+)\((.*)\) \{\n", line)
    if re.match(r"^static .*\) \{$", line.rstrip()):
        current = None
    output.append(line)
    if match:
        name = match[1].replace("_95", "_")
        current = NAMES.get(name)
        if current:
            definitions[current] += 1
            variables = {name.replace("_95", "_"): full for full, name in
                         re.findall(r"(slim_v_([A-Za-z_0-9]+)_n[0-9]+)", match[2])}
            if current in ends:
                output.append(f"slim_checking_probe_enter({int(current == 'machine')});\n")
            elif current == "push":
                output.append("slim_checking_probe_push();\n")
            elif current == "finish":
                output.append("slim_checking_probe_finish();\n")
            elif current == "builtin":
                state = variables["state"]
                output.append(f"slim_checking_probe_builtin({state}.slim_field_operation, {state}.slim_field_phase);\n")
            elif current == "scalar":
                frame = variables["frame"]
                output.append(f"slim_checking_probe_scalar({frame}.slim_field_phase);\n")
    if current == "machine" and line == "slim_recur: ;\n":
        headers += 1
    decision = re.fullmatch(r"if \((slim_v_ready_n[0-9]+)\) \{\n", line)
    if current == "machine" and decision:
        decisions += 1
        args = [variables["returning"], decision[1]]
        args += [variables[name] for name in ("depth", "limit")]
        args += [variables["top"] + ".slim_field_phase"]
        args += [variables[name] for name in ("branch_depth", "argument_depth", "builtin_depth")]
        output.insert(len(output) - 1, "slim_checking_probe_step(" + ", ".join(args) + ");\n")
    if current in ends and line == "return slim_result;\n":
        ends[current] += 1
        output.insert(len(output) - 1, f"slim_checking_probe_end({int(current == 'machine')}, slim_result.slim_field_kind, slim_region_failed(slim_allocation_region));\n")
    if line == "int main(int argc, char **argv) {\n":
        mains += 1
        output.append("slim_checking_probe_init();\n")

assert all(count == 1 for count in definitions.values()), definitions
assert ends == {"entry": 1, "machine": 1}, ends
assert headers == decisions == mains == 1, (headers, decisions, mains)
sys.stdout.writelines(output)
