/* Appended to the production host with only its entry point renamed. */
#include <assert.h>

static SlimBytes link_fixture_bytes(const char *text) {
    return slim_bytes_static((const uint8_t *)text, (int64_t)strlen(text));
}
static void link_fixture_row(char *buffer, size_t *size, uint8_t tag, const char *text) {
    buffer[(*size)++] = (char)tag;
    size_t count = strlen(text) + 1;
    memcpy(buffer + *size, text, count);
    *size += count;
}
static void link_fixture_base(char *report, size_t *size) {
    *size = 0;
    link_fixture_row(report, size, 0, "ld-test");
    link_fixture_row(report, size, 16, "program.o");
    link_fixture_row(report, size, 16, "runtime.o");
    link_fixture_row(report, size, 16, "/private/tmp/slim-native.test/sdk/usr/lib/libSystem.tbd");
    link_fixture_row(report, size, 16, "/private/tmp/slim-native.test/toolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a");
}
static bool link_fixture_report(const char *input, const char *output) {
    char report[16384]; size_t size = 0;
    link_fixture_base(report, &size);
    link_fixture_row(report, &size, 16, input);
    link_fixture_row(report, &size, 64, output);
    return native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size));
}
int main(void) {
    const char *hash = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
    char manifest[8192];
    snprintf(manifest, sizeof(manifest), "%s\tsdk/usr/lib/libSystem.tbd\n%s\ttoolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a\n", hash, hash);
    assert(native_manifest_valid(link_fixture_bytes(manifest)));
    native_inputs.bytes = link_fixture_bytes(manifest);
    strcpy(native_directory, "/private/tmp/slim-native.test");
    assert(link_fixture_report("program.o", "program"));
    assert(link_fixture_report("runtime.o", "program"));
    assert(link_fixture_report("/private/tmp/slim-native.test/program.o", "program"));
    assert(link_fixture_report("/private/tmp/slim-native.test/sdk/usr/lib/libSystem.tbd", "program"));
    assert(link_fixture_report("/private/tmp/slim-native.test/toolchain/lib/clang/21/lib/darwin/libclang_rt.osx.a", "/private/tmp/slim-native.test/program"));
    assert(!link_fixture_report("/private/tmp/slim-native.test/sdk/unrecorded.tbd", "program"));
    assert(!link_fixture_report("/private/tmp/slim-native.test/toolchain/unrecorded.a", "program"));
    assert(!link_fixture_report("/private/tmp/slim-native.test2/sdk/usr/lib/libSystem.tbd", "program"));
    assert(!link_fixture_report("/private/tmp/slim-native.test/sdk/usr/lib/../lib/libSystem.tbd", "program"));
    assert(!link_fixture_report("/usr/lib/libSystem.tbd", "program"));
    assert(!link_fixture_report("program.o", "other"));
    assert(!link_fixture_report("program.o", "/private/tmp/slim-native.test2/program"));
    assert(!link_fixture_report("", "program"));
    assert(!native_link_report_known(slim_bytes_static(NULL, 0)));
    assert(!native_link_report_known(slim_bytes_static(NULL, 1048577)));
    char report[32768]; size_t size = 0;
    link_fixture_row(report, &size, 0, "ld-test");
    link_fixture_row(report, &size, 64, "program");
    assert(!native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    link_fixture_base(report, &size);
    link_fixture_row(report, &size, 64, "program");
    assert(native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    assert(!native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size-1)));
    link_fixture_row(report, &size, 64, "program");
    assert(!native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    size = 0;
    link_fixture_row(report, &size, 64, "program");
    link_fixture_row(report, &size, 0, "ld-test");
    assert(!native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    link_fixture_base(report, &size);
    link_fixture_row(report, &size, 64, "program");
    for (unsigned i = 6; i < 4096; ++i) link_fixture_row(report, &size, 17, "x");
    assert(native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    link_fixture_row(report, &size, 17, "x");
    assert(!native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    link_fixture_base(report, &size);
    link_fixture_row(report, &size, 64, "program");
    link_fixture_row(report, &size, 18, "x");
    assert(!native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    char name[4098]; memset(name, 'x', sizeof(name)); name[4096] = 0;
    link_fixture_base(report, &size);
    link_fixture_row(report, &size, 64, "program");
    link_fixture_row(report, &size, 17, name);
    assert(native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));
    report[size-1] = 'x'; report[size++] = 0;
    assert(!native_link_report_known(slim_bytes_static((uint8_t *)report, (int64_t)size)));

    assert(!native_manifest_valid(slim_bytes_static(NULL, 0)));
    assert(!native_manifest_valid(slim_bytes_static(NULL, NATIVE_MANIFEST_LIMIT+1)));
    const char *bad[] = {"sdk/../x", "sdk/./x", "sdk//x", "sdk/.", "sdk/..", "sdk/", "/sdk/x", "sdk/x\ty", "sdk/x y", "other/x"};
    for (unsigned i = 0; i < sizeof(bad)/sizeof(bad[0]); ++i) {
        snprintf(manifest, sizeof(manifest), "%s\t%s\n", hash, bad[i]);
        assert(!native_manifest_valid(link_fixture_bytes(manifest)));
    }
    snprintf(manifest, sizeof(manifest), "%s\tsdk/x\n", hash);
    assert(!native_manifest_valid(slim_bytes_static((uint8_t *)manifest, (int64_t)strlen(manifest)-1)));
    manifest[0] = 'g'; assert(!native_manifest_valid(link_fixture_bytes(manifest))); manifest[0] = '0';
    manifest[64] = ' '; assert(!native_manifest_valid(link_fixture_bytes(manifest))); manifest[64] = '\t';
    manifest[66] = 0; assert(!native_manifest_valid(slim_bytes_static((uint8_t *)manifest, 71)));
    memset(name, 'x', sizeof(name)); memcpy(name, "sdk/", 4); name[4096] = 0;
    snprintf(manifest, sizeof(manifest), "%s\t%s\n", hash, name);
    assert(native_manifest_valid(link_fixture_bytes(manifest)));
    name[4096] = 'x'; name[4097] = 0;
    snprintf(manifest, sizeof(manifest), "%s\t%s\n", hash, name);
    assert(!native_manifest_valid(link_fixture_bytes(manifest)));
    char many[51200]; size = 0;
    for (unsigned i = 0; i < 512; ++i)
        size += (size_t)snprintf(many+size, sizeof(many)-size, "%s\tsdk/file-%u\n", hash, i);
    assert(native_manifest_valid(slim_bytes_static((uint8_t *)many, (int64_t)size)));
    size += (size_t)snprintf(many+size, sizeof(many)-size, "%s\tsdk/extra\n", hash);
    assert(!native_manifest_valid(slim_bytes_static((uint8_t *)many, (int64_t)size)));
    native_inputs = (NativeBytes){{NULL, 0}, NULL};
    *native_directory = 0;
    assert(native_allocations == 0 && host_allocations == 0);
    puts("native link inputs: exact captured membership; strict framing and bounds; no allocation");
    return 0;
}
