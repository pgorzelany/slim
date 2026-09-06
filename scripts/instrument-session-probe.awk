# Fixed generated-C anchors. Counts production phase entry, never estimates.
/^static .*slim_fn_session_95update\(.*\) \{$/ {
    phase = 1; epoch = 0; capture = 0; begins++; print; print "slim_session_probe_begin();"; next
}
/^static .*slim_fn_zzprobe_95run_95epoch\(.*\) \{$/ {
    phase = 0; epoch = 1; capture = 0; owners++; print; print "slim_session_probe_epoch_begin(slim_region);"; next
}
/^static .*slim_fn_syntax_95parse_95program_95result\(.*\) \{$/ {
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
/^static .*\) \{$/ { phase = 0; epoch = 0; capture = 0 }
capture && /^return slim_result;$/ { captured++; print "slim_session_probe_captured();" }
phase && /^return slim_result;$/ { ends++; print "slim_session_probe_end();" }
epoch && /^return slim_result;$/ { cleanups++; print "slim_session_probe_epoch_end(slim_region, &slim_function_region);" }
/^int main\(int argc, char \*\*argv\) \{$/ {
    mains++; print; print "slim_session_probe_init();"; next
}
{ print }
END {
    if (captures != 1 || captured != 1 || begins != 1 || parses != 1 || checks != 1 || generates != 1 || ends != 1 || mains != 1 || owners != 1 || cleanups != 1) exit 1
    print "_Static_assert(sizeof(Slim_type_retained_95Saved) <= 64, \"retained saved-row storage budget\");"
    print "_Static_assert(sizeof(Slim_type_retained_95StoredLink) <= 8, \"retained temporary-link storage budget\");"
}
