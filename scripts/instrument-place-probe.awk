# Count actual operand/root visits after the checked budget test, not loop entries.
/^static .*slim_fn_retained_95place_95fact\(.*\) \{$/ {
    phase = 1; walk = 0; begins++; print; print "slim_place_probe_begin();"; next
}
/^static .*slim_fn_retained_95place_95walk\(.*\) \{$/ {
    phase = 0; walk = 1; walks++; print; next
}
/^static .*\) \{$/ { phase = 0; walk = 0 }
walk && / = slim_fn_retained_95place_95node_95position\(/ { visits++; print "slim_place_probe_visit();" }
phase && /^return slim_result;$/ { ends++; print "slim_place_probe_end();" }
/^int main\(int argc, char \*\*argv\) \{$/ {
    mains++; print; print "slim_place_probe_init();"; next
}
{ print }
END {
    if (begins != 1 || walks != 1 || visits != 1 || ends != 1 || mains != 1) exit 1
    print "_Static_assert(sizeof(Slim_type_retained_95Saved) <= 64, \"retained saved-row storage budget\");"
    print "_Static_assert(sizeof(Slim_type_retained_95StoredLink) <= 8, \"retained stored-link storage budget\");"
}
