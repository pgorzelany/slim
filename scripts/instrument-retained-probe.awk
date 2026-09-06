# Fixed native observation anchors; no C parsing or semantic relocation.
BEGIN {
    if (scope != "" && scope != "project") exit 1
    root = scope == "project" ? "slim_fn_project_95prepare_95project_95retained" : "slim_fn_check_95check_95source_95retained"
}
$0 ~ "^static .*" root "\\(.*\\) \\{$" {
    phase = 1; begins++; print; print "slim_retained_probe_begin();"; next
}
/^static .*slim_fn_typing_95check_95function\(.*\) \{$/ {
    phase = 0; checks++; print; print "slim_retained_probe_check();"; next
}
/^static .*\) \{$/ { phase = 0 }
phase && /^return slim_result;$/ { ends++; print "slim_retained_probe_end();" }
/^int main\(int argc, char \*\*argv\) \{$/ {
    mains++; print; print "slim_retained_probe_init();"; next
}
{ print }
END {
    if (begins != 1 || checks != 1 || ends != 1 || mains != 1) exit 1
    # RFC-0131 durable fixed-payload budgets, excluding vector/index storage.
    print "_Static_assert(sizeof(Slim_type_retained_95Saved) <= 64, \"retained saved-row storage budget\");"
    print "_Static_assert(sizeof(Slim_type_retained_95StoredLink) <= 8, \"retained temporary-link storage budget\");"
}
