from pathlib import Path
import re
import sys

source_path = Path(sys.argv[1])
encode = lambda name: "slim_fn_" + name.replace("_", "_95")
transfer = encode("retained_input_transfer")
start = encode("retained_input_start_history")
discover = encode("retained_input_discover_calls")
collect = encode("ranges_collect_function_invariants")
target = encode("ranges_parameter_call_target")
import_structure = encode("retained_input_import_arguments")
query = encode("retained_input_query_functions")
find = encode("retained_input_find_queries")
apply = encode("retained_input_import_results")
names = {encode("ranges_merge_parameter_fact"): "scalar_merges",
         encode("ranges_scan_parameter_calls"): "fallback_scans",
         encode("retained_input_merge_current"): "query_producers"}
names[encode("retained_input_result_keys_match")] = "query_versions"
names[collect] = "invariant_producers"
names[target] = "discovery_node_visits"
copies = {encode("retained_input_copy_previous_keys"): "copied_keys",
          encode("retained_input_copy_previous_results"): "copied_results",
          encode("retained_input_copy_previous_entries"): "copied_entries",
          encode("retained_input_owner_maps"): "owner_map_rows"}
copies[encode("retained_input_import_invariants")] = "imported_invariants"
copies[import_structure] = "imported_arguments"
counters = ["key_rows", "adjacency_rows", "topology_producers", "structure_imports", "transfer_calls", "current_lookups", "previous_lookups", "current_imports", "previous_imports", *names.values(), *copies.values()]
# Six independent optional-history limits per previous-snapshot invocation.
# Only work inside the probe's input API window is counted; ordinary oracle
# scans and the surrounding checker setup never enter these counters.
output = ["#include <stdio.h>", "#include <stdlib.h>",
          "static int input_active, input_saturated, input_start_active, input_discovery_active;",
          "static unsigned input_phase; static int input_phase_active;",
          "static unsigned long long input_counts[6][" + str(len(counters)) + "];",
          "static void input_count(unsigned long long *v) { if (!input_phase_active) return; if (*v < 1000000000ULL) ++*v; else input_saturated = 1; }"]
for at, name in enumerate(counters):
    output.append(f"#define input_{name} input_counts[input_phase][{at}]")
output += ["static void input_begin(void) { if (input_phase_active || input_phase >= 6) abort(); input_phase_active = 1; }",
           "static void input_end(void) { if (!input_phase_active || input_active || input_start_active || input_discovery_active) abort(); input_phase_active = 0; ++input_phase; }",
           "static void input_report(void) {",
           'const char *path = getenv("SLIM_INPUT_REPORT"); if (!path) return;',
           'FILE *out = fopen(path, "wb"); if (!out) abort();',
           'fprintf(out, "slim-inputs\\t1\\t%s\\t1000000000\\n", input_saturated || input_phase_active ? "bounded" : "exact");',
           'fputs("phase\\t' + "\\t".join(counters) + '\\n", out);',
           'for (unsigned i = 0; i < input_phase; ++i) { fprintf(out, "%u", i);',
           f'for (unsigned j = 0; j < {len(counters)}; ++j) fprintf(out, "\\t%llu", input_counts[i][j]);',
           'fputc(10, out); } if (fclose(out)) abort();', '}']
inside = ""
found = imported = 0
seen = set()
pushes = {}
returns = 0
phase_entries = phase_returns = 0
key_rows = adjacency_writes = 0
for line in source_path.read_text().splitlines():
    match = re.match(r"^static .* (slim_fn_\w+)\(.*\) \{$", line)
    if match:
        inside = match[1]
    if inside == encode("zzprobe_compare_previous_limit") and line.lstrip().startswith("return "):
        output.append("input_end();")
        phase_returns += 1
    if inside == start and line.lstrip().startswith("return "):
        output.append("input_start_active = 0;")
    if inside == discover and line.lstrip().startswith("return "):
        output.append("input_discovery_active = 0;")
    if inside == transfer and line.lstrip().startswith("return "):
        output.append("input_active = 0;")
        returns += 1
    if inside == query and " = " + find + "(" in line:
        counter = ("current_lookups", "previous_lookups")[found]
        output.append(f"if (input_phase_active && input_active) input_count(&input_{counter});")
        found += 1
    if inside == query and " = " + apply + "(" in line:
        counter = ("current_imports", "previous_imports")[imported]
        output.append(f"if (input_phase_active && input_active) input_count(&input_{counter});")
        imported += 1
    if inside in copies and "slim_vec_push(" in line:
        push_index = pushes.get(inside, 0)
        if inside != import_structure or push_index == 0:
            output.append(f"if (input_phase_active) input_count(&input_{copies[inside]});")
        pushes[inside] = push_index + 1
    if inside == encode("retained_input_contribution_keys_match") and " = " + encode("ranges_fact_at") + "(" in line:
        output.append("if (input_phase_active && input_active) input_count(&input_key_rows);")
        key_rows += 1
    if inside == encode("retained_input_adjacency") and line.startswith("((") and "slim_vec_check_index" in line and "] = " in line:
        if adjacency_writes == 0:
            output.append("if (input_phase_active) input_count(&input_adjacency_rows);")
        adjacency_writes += 1
    output.append(line)
    if match and inside == encode("zzprobe_compare_previous_limit"):
        output.append("input_begin();")
        phase_entries += 1
    if match and inside in names:
        seen.add(inside)
        condition = "input_start_active" if inside == collect else ("input_discovery_active" if inside == target else "input_active")
        output.append(f"if (input_phase_active && {condition}) input_count(&input_{names[inside]});")
    elif match and inside == transfer:
        output += ["input_active = 1;", "if (input_phase_active) input_count(&input_transfer_calls);"]
    if match and inside == start:
        output.append("input_start_active = 1;")
    if match and inside == discover:
        output += ["input_discovery_active = 1;", "if (input_phase_active) input_count(&input_topology_producers);"]
    if match and inside == import_structure:
        output.append("if (input_phase_active) input_count(&input_structure_imports);")
    if line.startswith("int main("):
        output.append("if (atexit(input_report) != 0) abort();")
assert key_rows == 1 and adjacency_writes == 2
assert phase_entries == phase_returns == 1
assert found == imported == 2 and returns > 0
assert seen == set(names)
assert pushes == {name: (2 if copies[name] in ("owner_map_rows", "imported_arguments") else 1) for name in copies}, pushes
sys.stdout.write("\n".join(output) + "\n")
