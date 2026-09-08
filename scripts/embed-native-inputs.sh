#!/bin/sh
# Embed captured inputs in a private host-build directory; no live sidecar use.
set -eu
input_dir=$1
emit_array() {
    name=$1
    path=$2
    printf 'static const unsigned char %s[] = {\n' "$name"
    od -An -v -tu1 "$path" | awk '{for(i=1;i<=NF;i++) printf "%s,", $i; print ""}'
    printf '};\n'
}
{
    emit_array native_copy_program "$input_dir/copy-inputs"
    emit_array native_runtime_c "$input_dir/slim_rt.c"
    emit_array native_runtime_h "$input_dir/slim_rt.h"
    emit_array native_capture_recipe "$input_dir/native-context.sh"
} > "$input_dir/native-inputs.h"
