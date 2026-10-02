#!/bin/sh
# Compiler-specific capture for RFC-0146. The caller owns the private directory.
set -eu
export LC_ALL=C
export PATH=/usr/bin:/bin
fail() { echo "native-context: $*" >&2; exit 2; }
test "$#" -eq 1 || fail arguments
cd "$1"
capture_root=$(pwd -P)
safe_path() { case "$1" in ''|*[!A-Za-z0-9_./+-]*|*/../*|*/./*) return 1 ;; *) return 0 ;; esac; }
safe_path "$capture_root" || fail unsupported-private-path
test -f slim_rt.c && test -f slim_rt.h || fail missing-paired-runtime
mkdir tmp
TMPDIR="$capture_root/tmp"
export TMPDIR
paired_bytes=$(( $(wc -c < slim_rt.c) + $(wc -c < slim_rt.h) ))
test "$paired_bytes" -le 1048576 || fail paired-runtime-capacity
test "$(uname -s)" = Darwin && test "$(uname -m)" = arm64 || fail unsupported-host
native_clang=$(xcrun --find clang)
native_ld=$(xcrun --find ld)
safe_path "$native_clang" && safe_path "$native_ld" || fail unsupported-tool-path
tool_origin=$(CDPATH= cd "$(dirname "$native_clang")/.." && pwd -P)
test "$native_ld" = "$tool_origin/bin/ld" || fail split-toolchain
sdk_origin=$(CDPATH= cd "$(xcrun --show-sdk-path)" && pwd -P)
safe_path "$sdk_origin" && safe_path "$tool_origin" || fail unsupported-root
"$native_clang" --no-default-config --version > compiler-version.txt
sed -n '1p' compiler-version.txt | grep -q '^Apple clang version 21\.' || fail unsupported-compiler
"$native_clang" --no-default-config -dumpmachine > target.txt
grep -q '^arm64-apple-darwin[0-9.]*$' target.txt || fail unsupported-target
resource_origin=$("$native_clang" --no-default-config -print-resource-dir)
test "$resource_origin" = "$tool_origin/lib/clang/21" || fail unsupported-resource-root
mkdir toolchain sdk
: > files.plan
capture_rows=0
copy_file() {
    source_file=$1
    relative_file=$2
    safe_path "$source_file" && safe_path "$relative_file" || fail unsupported-input-path
    test "${#source_file}" -le 4096 && test "${#relative_file}" -le 4096 || fail input-path-capacity
    case "$relative_file" in toolchain/*|sdk/*) ;; *) fail destination-root ;; esac
    capture_rows=$((capture_rows+1))
    test "$capture_rows" -le 8192 || fail discovery-capacity
    printf '%s\t%s\n' "$source_file" "$relative_file" >> files.plan
}
copy_input() {
    case "$1" in
        "$sdk_origin"/*) copy_file "$1" "sdk/${1#"$sdk_origin"/}" ;;
        "$tool_origin"/*) copy_file "$1" "toolchain/${1#"$tool_origin"/}" ;;
        "$capture_root"/program.o|"$capture_root"/runtime.o|program.o|runtime.o) ;;
        *) fail outside-input ;;
    esac
}
: > tools.done
printf '%s\n' "$native_clang" "$native_ld" > tools.queue
while IFS= read -r tool_image; do
    relative="toolchain/${tool_image#"$tool_origin"/}"
    if grep -Fxq "$tool_image" tools.done; then continue; fi
    test "$(wc -l < tools.done)" -lt 512 || fail loader-capacity
    printf '%s\n' "$tool_image" >> tools.done
    copy_file "$tool_image" "$relative"
    otool -l "$tool_image" > tool-load.txt
    awk '$1 == "cmd" && $2 == "LC_RPATH" {r=1; next} r && $1 == "path" {print $2; r=0}' tool-load.txt > rpaths.txt
    while IFS= read -r rpath; do
        case "$rpath" in '@executable_path/../lib/'|'@executable_path/../lib'|'@loader_path/../lib/'|'@loader_path/../lib'|/usr/lib/swift) ;; *) fail unsupported-loader-search ;; esac
    done < rpaths.txt
    otool -L "$tool_image" > tool-deps.raw
    sed '1d' tool-deps.raw | awk '{print $1}' > tool-deps.txt
    while IFS= read -r dependency; do
        case "$dependency" in
            /usr/lib/*|/System/Library/*) ;;
            @rpath/*) printf '%s\n' "$tool_origin/lib/${dependency#@rpath/}" >> tools.queue ;;
            @loader_path/*)
                resolved="$(dirname "$tool_image")/${dependency#@loader_path/}"
                resolved="$(CDPATH= cd "$(dirname "$resolved")" && pwd -P)/$(basename "$resolved")"
                case "$resolved" in "$tool_origin"/lib/*) printf '%s\n' "$resolved" >> tools.queue ;; *) fail outside-loader-input ;; esac
                ;;
            *) fail unsupported-loader-input ;;
        esac
    done < tool-deps.txt
done < tools.queue
for file in SDKSettings.json SDKSettings.plist; do
    test ! -f "$sdk_origin/$file" || copy_file "$sdk_origin/$file" "sdk/$file"
done
cat > prefix.c <<'C'
#include "slim_rt.h"
#include <string.h>
C
cat prefix.c > program.c
printf 'int main(void) { return 0; }\n' >> program.c
# All split fields below are fixed recipe literals, never request text.
mode_flags=
original_cc() {
    "$native_clang" --no-default-config -std=c11 -O3 -DNDEBUG -Wall -Wextra -Werror $mode_flags \
        -isysroot "$sdk_origin" -I. "$@"
}
captured_cc() {
    "$capture_root/toolchain/bin/clang" --no-default-config -std=c11 -O3 -DNDEBUG -Wall -Wextra -Werror $mode_flags \
        -resource-dir "$capture_root/toolchain/lib/clang/21" -isysroot "$capture_root/sdk" \
        -nostdinc -isystem "$capture_root/toolchain/lib/clang/21/include" \
        -isystem "$capture_root/sdk/usr/include" -I. "$@"
}
captured_link() {
    "$capture_root/toolchain/bin/clang" --no-default-config -O3 -isysroot "$capture_root/sdk" \
        -nostdlib program.o runtime.o "$capture_root/sdk/usr/lib/libSystem.tbd" \
        "$capture_root/toolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a" \
        -Wl,-dependency_info,link.bin -o program "$@"
}
check_driver_job() {
    test "$(wc -c < "$4")" -le 1048576 || fail driver-report-capacity
    awk -v root="$1" -v role="$2" -v profile="$3" '
        function die() {bad=1; exit 2}
        function take(value) {if(++i>n || a[i]!=value) die()}
        function number(value) {return value ~ /^[0-9]+([.][0-9]+)*$/}
        BEGIN {
            source=role=="runtime" ? "slim_rt.c" : "program.c"
            split("-cc1 -O3 -Wundef-prefix=TARGET_OS_ -Wdeprecated-objc-isa-usage -Werror=deprecated-objc-isa-usage -Werror=implicit-function-declaration -emit-obj -disable-free -clear-ast-before-backend -disable-llvm-verifier -discard-value-names -mframe-pointer=non-leaf -debugger-tuning=lldb -fno-strict-return -ffp-contract=on -fno-rounding-math -funwind-tables=1 -fobjc-msgsend-selector-stubs -fno-sized-deallocation -fvisibility-inlines-hidden-static-local-var -fno-modulemap-allow-subdirectory-search -fdefine-target-os-macros -fno-assume-unique-vtables -enable-tlsdesc -nostdsysteminc -nobuiltininc -Wall -Wextra -Werror -Wno-elaborated-enum-base -std=c11 -fstack-check -mdarwin-stkchk-strong-link -fblocks -fencode-extended-block-signature -fregister-global-dtors-with-atexit -fgnuc-version=4.2.1 -fskip-odr-check-in-gmf -fmax-type-align=16 -fcommon -vectorize-loops -vectorize-slp -fno-odr-hash-protocols -Wno-error=allocator-wrappers -fdwarf2-cfi-asm", words, " ")
            for(j in words) flags[words[j]]=1
            split("-cc1 -O3 -emit-obj -nostdsysteminc -nobuiltininc -Wall -Wextra -Werror -std=c11 -triple -resource-dir -isysroot -I -o -x -main-file-name -fdebug-compilation-dir -fcoverage-compilation-dir", required, " ")
            definitions[1]="NDEBUG"; definitions[2]="SLIM_PARALLEL=1"; definitions[3]="SLIM_POSIX_WORKERS=1"
            if(role!="program" && role!="runtime" && role!="link") die()
            if(profile !~ /^[012]$/) die()
        }
        FILENAME==ARGV[1] {
            if(split($0,record,/\t/)==2 && record[2]=="sdk/SDKSettings.json") sdk_settings=1
            next
        }
        FNR==1 {if($0 !~ /^Apple clang version 21\.[0-9.]+ [(]clang-[A-Za-z0-9.]+[)]$/) die(); next}
        FNR==2 {if($0 !~ /^Target: arm64-apple-darwin[0-9.]+$/) die(); next}
        FNR==3 {if($0!="Thread model: posix") die(); next}
        FNR==4 {if($0!="InstalledDir: " root "/toolchain/bin") die(); next}
        /^[ \t]*$/ {next}
        {
            if(++jobs!=1 || $0 !~ /^ "[^"]*"( "[^"]*")*$/) die()
            n=split(substr($0,3,length($0)-3),a,/" "/)
            if(n>512) die()
            for(i=1;i<=n;++i) if(!length(a[i]) || length(a[i])>4096 || a[i] ~ /[\\ \t\r]/) die()
            if(role=="link") {
                expected=root "/toolchain/bin/ld|-demangle|-lto_library|" root "/toolchain/lib/libLTO.dylib|-dynamic|-arch|arm64|-platform_version|macos|VERSION|SDK|-syslibroot|" root "/sdk|-O3|-mllvm|-enable-linkonceodr-outlining|-o|program|program.o|runtime.o|" root "/sdk/usr/lib/libSystem.tbd|" root "/toolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a|-dependency_info|link.bin"
                count=split(expected,want,/\|/)
                if(n!=count) die()
                for(i=1;i<=n;++i) {
                    if(i==10 || i==11) {if(!number(a[i])) die()}
                    else if(a[i]!=want[i]) die()
                }
                next
            }
            if(a[1]!=root "/toolchain/bin/clang" || a[2]!="-cc1") die()
            for(i=2;i<=n;++i) {
                option=a[i]; seen[option]++
                if(option in flags) continue
                if(option=="-main-file-name") take(source)
                else if(option=="-mrelocation-model") take("pic")
                else if(option=="-pic-level") take("2")
                else if(option=="-mllvm") take("-extra-vectorizer-passes")
                else if(option=="-target-abi") take("darwinpcs")
                else if(option=="-resource-dir") take(root "/toolchain/lib/clang/21")
                else if(option=="-isysroot") take(root "/sdk")
                # Clang records the already captured SDK settings as an explicit
                # dependency. No other depfile-entry path is an admitted input.
                else if(option=="-fdepfile-entry=" root "/sdk/SDKSettings.json") {
                    if(!sdk_settings || ++settings_inputs!=1) die()
                }
                else if(option=="-isystem") {
                    if(++includes>2) die()
                    take(root (includes==1 ? "/toolchain/lib/clang/21/include" : "/sdk/usr/include"))
                }
                else if(option=="-D") {if(++defines>profile+1) die(); take(definitions[defines])}
                else if(option=="-I") take(".")
                else if(option=="-o") take(role ".o")
                else if(option=="-x") take("c")
                else if(option=="-ferror-limit") take("19")
                else if(option=="-stack-protector") take("1")
                else if(option=="-pthread") {if(profile!=2) die()}
                else if(option=="-triple") {if(++i>n || a[i] !~ /^arm64-apple-macosx[0-9]+([.][0-9]+)*$/) die()}
                else if(option=="-target-cpu") {if(++i>n || a[i] !~ /^[A-Za-z0-9_.+-]+$/) die()}
                else if(option=="-target-feature") {if(++i>n || a[i] !~ /^[+-][A-Za-z0-9_.+-]+$/) die()}
                else if(option=="-target-linker-version") {if(++i>n || !number(a[i])) die()}
                else if(option ~ /^-target-sdk-version=/) {if(!number(substr(option,21))) die()}
                else if(option=="-fdebug-compilation-dir=" root) seen["-fdebug-compilation-dir"]++
                else if(option=="-fcoverage-compilation-dir=" root) seen["-fcoverage-compilation-dir"]++
                else if(option ~ /^-clang-vendor-feature=[+][A-Za-z0-9]+$/) continue
                else if(option==source && i==n) inputs++
                else die()
            }
        }
        END {
            if(bad || jobs!=1) exit 2
            if(role!="link") {
                if(inputs!=1 || includes!=2 || defines!=profile+1 || (profile==2 && seen["-pthread"]!=1)) exit 2
                for(j in required) if(!seen[required[j]]) exit 2
            }
        }' "$1/captured-sha256.tsv" "$4" || fail unsupported-driver-job
}
link_inputs() {
    test "$(wc -c < "$1")" -le 1048576 || fail link-report-capacity
    od -An -v -tu1 "$1" | awk '
        BEGIN {tag=-1; records=0; versions=0; outputs=0; n=0; bad=0}
        {for(i=1;i<=NF;i++) {
            b=$i+0
            if(tag<0) {
                tag=b; text=""; n=0
                if(tag!=0 && tag!=16 && tag!=17 && tag!=64) bad=1
            } else if(b==0) {
                records++
                if(tag==0) {versions++; if(records!=1) bad=1}
                if(tag==64) outputs++
                if(tag==16) {if(n==0) bad=1; print text}
                if(records>4096) bad=1
                tag=-1
            } else {
                n++; if(n>4096) bad=1
                if(n<=4096) text=text sprintf("%c",b)
            }
        }}
        END {if(bad || tag>=0 || versions!=1 || outputs!=1) exit 1}'
}
check_captured_inputs() {
    # One exact manifest index per report; preserve lexical first-error order and
    # role-specific local inputs. FILENAME also handles an empty first file.
    input_failure=$(awk -F '\t' -v root="$capture_root/" -v role="$1" '
        function reject(reason) { print reason; exit 1 }
        FILENAME==ARGV[1] { recorded[$2]=1; next }
        {
            input=$0
            if(index(input,root "sdk/")==1 || index(input,root "toolchain/")==1) {
                relative=substr(input,length(root)+1)
                if(!(relative in recorded)) reject("unrecorded-captured-input")
            } else if(input=="program.o" || input=="runtime.o" ||
                      input==root "program.o" || input==root "runtime.o") {
                if(role!="link") reject("unexpected-header-input")
            } else if(input=="prefix.c" || input=="slim_rt.c" || input=="./slim_rt.h" || input=="slim_rt.h") {
                if(role!="headers") reject("unexpected-link-input")
            } else reject("outside-captured-input")
        }' captured-sha256.tsv "$2") || fail "${input_failure:-captured-input-report}"
}
for profile in 0 1 2; do
    case "$profile" in 0) mode_flags= ;; 1) mode_flags=-DSLIM_PARALLEL=1 ;; 2) mode_flags='-DSLIM_PARALLEL=1 -DSLIM_POSIX_WORKERS=1 -pthread' ;; esac
    for unit in prefix.c slim_rt.c; do
        original_cc -M -MT object "$unit" > headers.make
        test "$(wc -c < headers.make)" -le 1048576 || fail header-report-capacity
        # This provider accepts only unambiguous whitespace-free dependency paths.
        tr '\n' ' ' < headers.make | sed 's/\\/ /g' | awk '{if($1!="object:") exit 1; for(i=2;i<=NF;i++) print $i}' > headers.txt
        while IFS= read -r header; do
            case "$header" in prefix.c|slim_rt.c|./slim_rt.h|slim_rt.h) ;; *) copy_input "$header" ;; esac
        done < headers.txt
    done
    original_cc -c program.c -o program.o
    original_cc -c slim_rt.c -o runtime.o
    original_cc program.o runtime.o -Wl,-dependency_info,original-link.bin -o original-program
    link_inputs original-link.bin > inputs.txt
    while IFS= read -r input; do copy_input "$input"; done < inputs.txt
done
sort -u files.plan > files.sorted
./copy-inputs "$paired_bytes" < files.sorted > files.tsv || fail input-capture
# The captured copier compares source/copy bytes and hashes each copied image.
# Its implementation is embedded in this loaded host, not a mutable sidecar.
awk -F '\t' 'NF!=4 {exit 1} {printf "%s\t%s\n", $4, $2}' files.tsv > captured-sha256.tsv

for profile in 0 1 2; do
    case "$profile" in 0) mode_flags= ;; 1) mode_flags=-DSLIM_PARALLEL=1 ;; 2) mode_flags='-DSLIM_PARALLEL=1 -DSLIM_POSIX_WORKERS=1 -pthread' ;; esac
    for unit in prefix.c slim_rt.c; do
        captured_cc -M -MT object "$unit" > "captured-headers-$profile-$unit.make"
        test "$(wc -c < "captured-headers-$profile-$unit.make")" -le 1048576 || fail captured-header-report-capacity
        tr '\n' ' ' < "captured-headers-$profile-$unit.make" | sed 's/\\/ /g' | awk '{if($1!="object:") exit 1; for(i=2;i<=NF;i++) print $i}' > headers.txt
        check_captured_inputs headers headers.txt
        for view in tokens macros; do
            case "$view" in tokens) view_flags='-E -P' ;; macros) view_flags='-E -dM' ;; esac
            original_cc $view_flags "$unit" > "profile-$profile-$unit-$view"
            captured_cc $view_flags "$unit" > captured-profile
            test "$(wc -c < captured-profile)" -le 1048576 || fail profile-capacity
            cmp "profile-$profile-$unit-$view" captured-profile || fail preprocessing-difference
        done
    done
    captured_cc -c program.c -o program.o
    captured_cc -c slim_rt.c -o runtime.o
    captured_link
    link_inputs link.bin > inputs.txt
    check_captured_inputs link inputs.txt
    for role in program runtime; do
        case "$role" in program) unit=program.c ;; runtime) unit=slim_rt.c ;; esac
        captured_cc -### -c "$unit" -o "$role.o" 2> "compile-job-$profile-$role.txt"
        check_driver_job "$capture_root" "$role" "$profile" "compile-job-$profile-$role.txt"
    done
done
captured_link -### 2> link-job.txt
check_driver_job "$capture_root" link 0 link-job.txt
cat > options.txt <<'OPTIONS'
apple-clang-arm64-v1
driver=--no-default-config; one validated command; 512 arguments; 4096 bytes per argument; 1MiB report
compile=-std=c11 -O3 -DNDEBUG -Wall -Wextra -Werror
includes=-nostdinc; captured resource and SDK roots; paired runtime directory
workers=0:none;1:SLIM_PARALLEL;2:SLIM_PARALLEL,SLIM_POSIX_WORKERS,-pthread
link=-O3 -nostdlib; ordered program/runtime objects; complete captured libSystem.tbd and libclang_rt.osx.a
LC_ALL=C
PATH=/usr/bin:/bin
TMPDIR=private-context/tmp
OPTIONS
shasum -a 256 slim_rt.c slim_rt.h options.txt compiler-version.txt target.txt profile-* captured-headers-* compile-job-* link-job.txt > context-inputs.sha256
cat captured-sha256.tsv context-inputs.sha256 | shasum -a 256 | awk '{print $1}' > context.sha256
awk -F '\t' -v paired="$paired_bytes" '{bytes += $3; count++} END {printf "%d\t%.0f\n", count+2, bytes+paired}' files.tsv > capture-size.tsv
chmod a-w slim_rt.c slim_rt.h
rm -f program program.o runtime.o original-program captured-profile tool-load.txt rpaths.txt tool-deps.txt headers.make headers.txt inputs.txt
printf 'native-context\tapple-clang-arm64-v1\t%s\n' "$(cat context.sha256)"
