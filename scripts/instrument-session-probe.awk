# host=1 observes the public root owner; the SLIM fixture owner stays required by default.
BEGIN { if (host != 0 && host != 1) exit 1 }
/^static .*\) \{$/ { proof = 0; assembled = 0 }
# Fixed generated-C anchors. Counts production phase entry, never estimates.
/^static .*slim_fn_psession_95_95update\(.*\) \{$/ {
    phase = 1; epoch = 0; capture = 0; begins++; print; print "slim_session_probe_begin();"; next
}
/^static .*slim_fn_pzzprobe_95_95run_950epoch\(.*\) \{$/ {
    phase = 0; epoch = 1; capture = 0; owners++; print; print "slim_session_probe_epoch_begin(slim_region);"; next
}
/^static .*slim_fn_psyntax_95_95lex_950program\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; parses++; print; print "slim_session_probe_count(0);"; next
}
/^static .*slim_fn_ptyping_95_95check_950function\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; checks++; print; print "slim_session_probe_count(1);"; next
}
/^static .*slim_fn_pcodegen_95_95emit_950program_950prefix\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; generates++; print; print "slim_session_probe_count(2);"; next
}
/^static .*slim_fn_pproject_95_95capture_950project_950input\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 1; captures++; print; next
}
/^static .*slim_fn_psyntax_95_95parse_950familiar_950item\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; grammars++; print; print "slim_session_probe_count(3);"; next
}
/^static .*slim_fn_psyntax_95_95import_950parse_950tokens\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 1; imports++; print; next
}
/^static .*slim_fn_pmemory_95_95build_950function_950plan\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; plans++; print; print "slim_session_probe_count(5);"; next
}
/^static .*slim_fn_pretained_95_95import_950plan\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; plan_imports++; print; print "slim_session_probe_count(6);"; next
}
/^static .*slim_fn_pranges_95_95analyze\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_analyzes++; print; print "slim_session_probe_count(7);"; next
}
/^static .*slim_fn_pretained_95_95range_950checked\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_queries++; print; print "slim_session_probe_count(8);"; next
}
/^static .*slim_fn_pranges_95_95analyze_950function\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_functions++; print; print "slim_session_probe_count(9);"; next
}
/^static .*slim_fn_pretained_95_95range_950import\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; range_imports++; print; print "slim_session_probe_count(10);"; next
}
/^static .*slim_fn_pranges_95_95analyze_950functions\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; ordinary_passes++; print; print "slim_session_probe_count(11);"; next
}
/^static .*slim_fn_pretained_95_95range_950functions\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; retained_passes++; print; print "slim_session_probe_count(11);"; next
}
/^static .*slim_fn_pranges_95_95scan_950parameter_950calls\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; parameter_scans++; print; print "slim_session_probe_count(12);"; next
}
/^static .*slim_fn_pretained_95_95input_950transfer\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; input_transfers++; print; print "slim_session_probe_count(22);"; next
}
/^static .*slim_fn_pparallel_95_95analyze\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; parallel_analyzes++; print; print "slim_session_probe_count(13);"; next
}
/^static .*slim_fn_pranges_95_95initialize_950facts\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; fact_initializations++; print; print "slim_session_probe_count(14);"; next
}
/^static .*slim_fn_pretained_95_95range_950clear_950facts\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; fact_resets++; print; print "slim_session_probe_count(15);"; next
}
/^static .*slim_fn_pcodegen_95_95emit_950prototype\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; prototypes++; print; print "slim_session_probe_count(16);"; next
}
/^static .*slim_fn_pcodegen_95_95emit_950function\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; bodies++; print; print "slim_session_probe_count(17);"; next
}
/^static .*slim_fn_pcodegen_95_95emit_950parallel_950call_950wrapper\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; wrappers++; print; print "slim_session_probe_count(18);"; next
}
/^static .*slim_fn_pfragments_95_95append_950fragment\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; fragments++;
    argument = $0; sub(/^.*Slim_type_pfragments_95_95Span /, "", argument); sub(/,.*$/, "", argument);
    if (argument !~ /^slim_v_span_n[0-9]+$/) exit 1;
    print; print "slim_session_probe_import(" argument ".slim_field_start, " argument ".slim_field_end);"; next
}
/^static .*slim_fn_pcodegen_95_95counted_950record_950index\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; counted_lookups++; print; print "slim_session_probe_count(21);"; next
}
# Independent checksum oracle uses unsigned arithmetic on the bounded final bytes.
/^static .*slim_fn_pfragments_95_95generate\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; assembled = 1; assemblies++; print; next
}
assembled && /^return slim_result;$/ {
    assembly_returns++;
    print "if (!slim_region_failed(slim_allocation_region) && slim_result.slim_field_checksum >= 0) {";
    print "SlimBytes slim_assembled = slim_result.slim_field_code; uint64_t slim_assembled_sum = 0;";
    print "if (slim_assembled.len <= 0 || slim_assembled.len > 67108864) abort();";
    print "for (int64_t slim_assembled_at = 0; slim_assembled_at < slim_assembled.len; ++slim_assembled_at) slim_assembled_sum += (uint64_t)slim_assembled.data[slim_assembled_at] * ((uint64_t)slim_assembled_at + 1);";
    print "if (slim_assembled_sum != (uint64_t)slim_result.slim_field_checksum) abort();";
    print "}";
}
# Independent test-only witness: every positive fast eligibility result must
# equal a fresh complete checked-row comparison. Its result never enables reuse.
/^static .*slim_fn_pretained_95_95same_950checked_950function\(.*\) \{$/ {
    phase = 0; epoch = 0; capture = 0; importing = 0; proof = 1; proof_functions++;
    split($0, proof_args, ", ");
    for (proof_i = 1; proof_i <= 4; ++proof_i) sub(/^.* /, "", proof_args[proof_i]);
    if (proof_args[1] !~ /^slim_v_old_n[0-9]+$/ || proof_args[2] !~ /^slim_v_current_n[0-9]+$/ || proof_args[3] !~ /^slim_v_mapping_n[0-9]+$/ || proof_args[4] !~ /^slim_v_at_n[0-9]+$/) exit 1;
    print; next
}
/^static .*\) \{$/ { phase = 0; epoch = 0; capture = 0; importing = 0 }
importing && / = slim_fn_psyntax_95_95push_950tagged_950token\(/ { copies++; print "slim_session_probe_count(4);" }
proof && /^return slim_result;$/ {
    proof_returns++;
    print "if (slim_result) {";
    print "if (!slim_fn_pretained_95_95same_950checked_950function_950rows(" proof_args[1] ", " proof_args[2] ", " proof_args[3] ", " proof_args[4] ", slim_region)) abort();";
    print "}";
}
capture && /^return slim_result;$/ { captured++; print "slim_session_probe_captured();" }
phase && /^return slim_result;$/ { ends++; print "slim_session_probe_end();" }
epoch && /^return slim_result;$/ { cleanups++; print "slim_session_probe_epoch_end(slim_region, &slim_function_region);" }
/^int main\(int argc, char \*\*argv\) \{$/ {
    mains++; print; print "slim_session_probe_init();"; next
}
{ print }
END {
    if (input_transfers != 1 || assemblies != 1 || assembly_returns != 1 || proof_functions != 1 || proof_returns != 1 || counted_lookups != 1 || prototypes != 1 || bodies != 1 || wrappers != 1 || fragments != 1 || fact_initializations != 1 || fact_resets != 1 || range_analyzes != 1 || range_queries != 1 || range_functions != 1 || range_imports != 1 || ordinary_passes != 1 || retained_passes != 1 || parameter_scans != 1 || parallel_analyzes != 1 || plans != 1 || plan_imports != 1 || grammars != 1 || imports != 1 || copies != 1 || captures != 1 || captured != 1 || begins != 1 || parses != 1 || checks != 1 || generates != 1 || ends != 1 || mains != 1 || owners != (host ? 0 : 1) || cleanups != (host ? 0 : 1)) exit 1
    print "_Static_assert(sizeof(Slim_type_pretained_95_95Saved) <= 64, \"retained saved-row storage budget\");"
    print "_Static_assert(sizeof(Slim_type_pretained_95_95StoredLink) <= 8, \"retained temporary-link storage budget\");"
}
