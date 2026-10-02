# Native observation of one fixed compiler module, not a C semantic parser.
/^static .*slim_fn_pflow_95_95derive\(.*\) \{$/ {
    derive = 1; walk = 0; enters++; print; print "slim_flow_probe_enter();"; next
}
/^static .*slim_fn_pflow_95_95walk\(.*\) \{$/ {
    derive = 0; walk = 1; walks++; print; print "slim_flow_probe_walk();"; next
}
/^static .*\) \{$/ { derive = 0; walk = 0 }
walk && /^slim_recur: ;$/ { headers++; print; print "slim_flow_probe_step();"; next }
derive && /^return slim_result;$/ {
    ends++;
    print "slim_flow_probe_end(slim_result.slim_field_status.tag, slim_result.slim_field_steps, slim_region_failed(slim_allocation_region));"
}
/^int main\(int argc, char \*\*argv\) \{$/ {
    mains++; print; print "slim_flow_probe_init();"; next
}
{ print }
END { if (enters != 1 || walks != 1 || headers != 1 || ends != 1 || mains != 1) exit 1 }
