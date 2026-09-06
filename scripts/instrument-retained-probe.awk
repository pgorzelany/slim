# Fixed native observation anchors; no C parsing or semantic relocation.
/^static .*slim_fn_check_95check_95source_95retained\(.*\) \{$/ {
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
END { if (begins != 1 || checks != 1 || ends != 1 || mains != 1) exit 1 }
