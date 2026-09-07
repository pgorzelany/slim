# Observe actual grammar execution, imports, and the successful-item read boundary.
/^static .*\) \{$/ {
    phase = 0; item = 0; importing = 0
    if ($0 ~ /slim_fn_syntax_95parse_95program_95retained\(/) {
        phase = 1; begins++; print; print "slim_parse_probe_begin();"; next
    }
    if ($0 ~ /slim_fn_syntax_95parse_95familiar_95item\(/) {
        if (!match($0, /slim_v_index_n[0-9]+/)) exit 1
        argument = substr($0, RSTART, RLENGTH)
        item = 1; items++; print; print "slim_parse_probe_item(" argument ");"; next
    }
    if ($0 ~ /slim_fn_syntax_95lexeme_95index_95valid\(/) {
        if (!match($0, /slim_v_index_n[0-9]+/)) exit 1
        argument = substr($0, RSTART, RLENGTH)
        reads++; print; print "slim_parse_probe_read(" argument ");"; next
    }
    if ($0 ~ /slim_fn_syntax_95import_95parse_95tokens\(/) { importing = 1; imports++ }
}
importing && / = slim_fn_syntax_95push_95tagged_95token\(/ { copies++; print "slim_parse_probe_import();" }
item && /^return slim_result;$/ { item_ends++; print "slim_parse_probe_item_end(slim_result.slim_field_valid, slim_result.slim_field_next);" }
phase && /^return slim_result;$/ { ends++; print "slim_parse_probe_end();" }
/^int main\(int argc, char \*\*argv\) \{$/ {
    mains++; print; print "slim_parse_probe_init();"; next
}
{ print }
END {
    if (begins != 1 || ends != 1 || items != 1 || item_ends != 1 || reads != 1 || imports != 1 || copies != 1 || mains != 1) exit 1
}
