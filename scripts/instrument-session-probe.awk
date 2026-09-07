# Fixed generated-C anchors. Counts production phase entry, never estimates.
/^static .*slim_fn_session_95update\(.*\) \{$/ {
    phase = 1; epoch = 0; capture = 0; begins++; print; print "slim_session_probe_begin();"; next
}
/^static .*slim_fn_zzprobe_95run_95epoch\(.*\) \{$/ {
    phase = 0; epoch = 1; capture = 0; owners++; print; print "slim_session_probe_epoch_begin(slim_region);"; next
}
/^static .*slim_fn_syntax_95lex_95program\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; parses++; print; print "slim_session_probe_count(0);"; next
}
/^static .*slim_fn_typing_95check_95function\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; checks++; print; print "slim_session_probe_count(1);"; next
}
/^static .*slim_fn_project_95generate_95prepared_95project\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; generates++; print; print "slim_session_probe_count(2);"; next
}
/^static .*slim_fn_project_95capture_95project_95input\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 1; captures++; print; next
}
/^static .*slim_fn_syntax_95parse_95familiar_95item\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; grammars++; print; print "slim_session_probe_count(3);"; next
}
/^static .*slim_fn_syntax_95import_95parse_95tokens\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 1; imports++; print; next
}
/^static .*slim_fn_memory_95build_95function_95plan\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; plans++; print; print "slim_session_probe_count(5);"; next
}
/^static .*slim_fn_retained_95import_95plan\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; plan_imports++; print; print "slim_session_probe_count(6);"; next
}
/^static .*slim_fn_ranges_95analyze\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_analyzes++; print; print "slim_session_probe_count(7);"; next
}
/^static .*slim_fn_retained_95range_95checked\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_queries++; print; print "slim_session_probe_count(8);"; next
}
/^static .*slim_fn_ranges_95analyze_95function\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_functions++; print; print "slim_session_probe_count(9);"; next
}
/^static .*slim_fn_retained_95range_95import\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_imports++; print; print "slim_session_probe_count(10);"; next
}
/^static .*slim_fn_ranges_95analyze_95functions\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; ordinary_passes++; print; print "slim_session_probe_count(11);"; next
}
/^static .*slim_fn_retained_95range_95functions\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; retained_passes++; print; print "slim_session_probe_count(11);"; next
}
/^static .*slim_fn_ranges_95scan_95parameter_95calls\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; parameter_scans++; print; print "slim_session_probe_count(12);"; next
}
/^static .*slim_fn_parallel_95analyze\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; parallel_analyzes++; print; print "slim_session_probe_count(13);"; next
}
/^static .*slim_fn_ranges_95initialize_95facts\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; fact_initializations++; print; print "slim_session_probe_count(14);"; next
}
/^static .*slim_fn_retained_95range_95clear_95facts\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; fact_resets++; print; print "slim_session_probe_count(15);"; next
}
/^static .*\) \{$/ { phase = 0; epoch = 0; capture = 0; importing = 0 }
importing && / = slim_fn_syntax_95push_95tagged_95token\(/ { copies++; print "slim_session_probe_count(4);" }
capture && /^return slim_result;$/ { captured++; print "slim_session_probe_captured();" }
phase && /^return slim_result;$/ { ends++; print "slim_session_probe_end();" }
epoch && /^return slim_result;$/ { cleanups++; print "slim_session_probe_epoch_end(slim_region, &slim_function_region);" }
/^int main\(int argc, char \*\*argv\) \{$/ {
    mains++; print; print "slim_session_probe_init();"; next
}
{ print }
END {
    if (fact_initializations != 1 || fact_resets != 1 || range_analyzes != 1 || range_queries != 1 || range_functions != 1 || range_imports != 1 || ordinary_passes != 1 || retained_passes != 1 || parameter_scans != 1 || parallel_analyzes != 1 || plans != 1 || plan_imports != 1 || grammars != 1 || imports != 1 || copies != 1 || captures != 1 || captured != 1 || begins != 1 || parses != 1 || checks != 1 || generates != 1 || ends != 1 || mains != 1 || owners != 1 || cleanups != 1) exit 1
    print "_Static_assert(sizeof(Slim_type_retained_95Saved) <= 64, \"retained saved-row storage budget\");"
    print "_Static_assert(sizeof(Slim_type_retained_95StoredLink) <= 8, \"retained temporary-link storage budget\");"
}
