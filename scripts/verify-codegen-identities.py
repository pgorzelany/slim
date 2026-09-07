#!/usr/bin/env python3
"""Test-only C lexical/relocation oracle; never used to accept or compile SLIM."""
import argparse
from collections import Counter
import os
from pathlib import Path
import re
import subprocess
import tempfile

# Quoted tokens and comments are opaque: generated-looking text inside them must
# remain byte-identical. The emitter uses ordinary C strings, not raw strings.
TOKEN = re.compile(r'/\*.*?\*/|//[^\n]*|(?:u8|[LuU])?"(?:\\.|[^"\\])*"|'
                   r"(?:[LuU])?'(?:\\.|[^'\\])*'|[A-Za-z_][A-Za-z_0-9]*|"
                   r'[0-9][A-Za-z_0-9.]*|[^\s]', re.S)


def category(token):
    match = re.fullmatch(r'slim_v_(.*)_n[0-9]+', token)
    if match:
        return ('local', 'source', match[1])
    if re.fullmatch(r'slim_t_[0-9]+', token):
        return ('local', 'temporary')
    if re.fullmatch(r'SlimParallel_[A-Za-z_0-9]+', token):
        return ('global', 'context')
    if re.fullmatch(r'slim_parallel_run_[A-Za-z_0-9]+', token):
        return ('global', 'runner')
    return None


def identity_only(before, after):
    left = TOKEN.findall(before)
    right = TOKEN.findall(after)
    assert len(left) == len(right), ('token count', len(left), len(right))
    maps = {'local': ({}, {}), 'global': ({}, {})}
    depth = 0
    renamed = 0
    for index, (a, b) in enumerate(zip(left, right)):
        kind = category(a)
        if kind:
            assert category(b) == kind, ('identifier class', index, a, b)
            forward, inverse = maps[kind[0]]
            assert forward.setdefault(a, b) == b, ('inconsistent', index, a, b)
            assert inverse.setdefault(b, a) == a, ('not injective', index, a, b)
            renamed += a != b
        else:
            assert a == b, ('non-identifier token', index, a, b)
        if a == '{':
            depth += 1
        elif a == '}':
            depth -= 1
            assert depth >= 0
        if depth == 0 and a in (';', '}'):
            # Prototype / complete function scopes, not nested blocks. Global
            # helper mappings remain live across every translation-unit item.
            maps['local'] = ({}, {})
    assert depth == 0
    return renamed


def units(source):
    # Header is fixed runtime plumbing; emitted declarations follow this marker.
    start = source.index('#endif\n\n') + len('#endif\n\n')
    depth = 0
    first = None
    result = []
    for match in TOKEN.finditer(source, start):
        value = match[0]
        if first is None:
            first = value
            start = match.start()
        if value == '{':
            depth += 1
        elif value == '}':
            depth -= 1
        if depth == 0 and (value == ';' or (value == '}' and first in ('static', 'int'))):
            result.append(source[start:match.end()])
            first = None
    assert depth == 0 and first is None, 'unframed C declaration'
    functions = [unit for unit in result if unit.startswith('static SLIM_UNUSED_FUNCTION ')]
    prototypes = {unit[:-1] for unit in functions if unit.endswith(';')}
    definitions = {unit.partition(' {\n')[0] for unit in functions if not unit.endswith(';')}
    assert prototypes == definitions, 'prototype and definition parameter identities disagree'
    return Counter(result)


def run(*args, env=None):
    return subprocess.run(list(map(str, args)), capture_output=True, env=env)


def emit(compiler, path):
    result = run(compiler, path)
    assert result.returncode == 0 and not result.stderr, (path, result.returncode, result.stdout[:1000], result.stderr)
    return result.stdout.decode()


def same_result(a, b, context):
    assert (a.returncode, a.stdout, a.stderr) == (b.returncode, b.stdout, b.stderr), context


def oracle_checks():
    before = 'static int f(int slim_v_a_n12) { int slim_t_23 = slim_v_a_n12; return slim_t_23; }'
    after = before.replace('_n12', '_n2').replace('slim_t_23', 'slim_t_13')
    assert identity_only(before, after) == 4
    opaque = ' "slim_v_a_n12 { \\\"" /* slim_t_23 } */ \'x\''
    identity_only(before + opaque, after + opaque)
    bad = [after.replace('return slim_t_13', 'return slim_t_14'),
           after.replace('= slim_v_a_n2', '= slim_t_13'),
           after + opaque.replace('slim_t_23', 'slim_t_24')]
    for candidate in bad:
        try:
            identity_only(before + (opaque if candidate.endswith("'x'") else ''), candidate)
        except AssertionError:
            pass
        else:
            raise AssertionError('oracle accepted a semantic change')
    try:
        identity_only('int f(){int slim_v_temporary_n1;}', 'int f(){int slim_t_1;}')
    except AssertionError:
        pass
    else:
        raise AssertionError('oracle accepted a changed private identifier class')
    try:
        identity_only('SlimParallel_1 x; SlimParallel_2 y;',
                      'SlimParallel_f_s1_0 x; SlimParallel_f_s1_0 y;')
    except AssertionError:
        pass
    else:
        raise AssertionError('oracle accepted colliding global helpers')
    try:
        identity_only('int f(){int slim_t_1; int slim_t_2;}',
                      'int f(){int slim_t_1; int slim_t_1;}')
    except AssertionError:
        pass
    else:
        raise AssertionError('oracle accepted colliding locals')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('compiler')
    parser.add_argument('--baseline')
    parser.add_argument('--report')
    args = parser.parse_args()
    compiler = Path(args.compiler).resolve()
    oracle_checks()
    fixtures = sorted(Path('conformance/pass').glob('*.slim'))
    native = sorted(Path('benchmarks/challenges').glob('*/program.slim'))
    fixtures += native + [Path('tests/fixtures/stable_codegen.slim')]
    rows = []
    with tempfile.TemporaryDirectory(prefix='slim-codegen-identities-') as temp:
        directory = Path(temp)
        path = directory / 'program.slim'
        noise = '\nfn relocation_noise(value: I64) -> I64:\n  value\n'
        grown = '\nfn relocation_noise(value: I64) -> I64 effects[partial]:\n  let copy: I64 = value + 7\n  copy + 9\n'
        for fixture in fixtures:
            source = fixture.read_text()
            split = source.index('\n')
            header, body = source[:split + 1], source[split + 1:]
            original = emit(compiler, fixture)
            original_units = units(original)
            # Addition, preceding body growth and moving that declaration to the
            # end. Returning to the original tests deletion deterministically.
            for edited in (header + noise + body, header + grown + body, source + grown, source):
                path.write_text(edited)
                actual = units(emit(compiler, path))
                assert not original_units - actual, (fixture, 'relocated fragment changed', list((original_units-actual))[:1])
            if args.baseline:
                previous = emit(args.baseline, fixture)
                renamed = identity_only(previous, original)
                same_result(run(args.baseline, 'analyze', fixture), run(compiler, 'analyze', fixture), (fixture, 'analysis'))
                rows.append((str(fixture), len(previous.encode()), len(original.encode()), renamed, 'exact'))
        # Relocate the actual module file and insert a preceding module. Qualified
        # identities must remain stable, including private helpers.
        fixture = Path('tests/fixtures/stable_codegen.slim')
        source = fixture.read_text()
        (directory / 'owner.slim').write_text(source)
        (directory / 'noise.slim').write_text('module padding\n\nfn helper() -> I64:\n  7\n')
        project = directory / 'slim.project'
        owner = '(module stable_codegen "owner.slim" (imports) (exports))'
        padding = '(module padding "noise.slim" (imports) (exports helper))'
        project.write_text(f'(project 1 (entry stable_codegen) {owner})\n')
        before = units(emit(compiler, project))
        (directory / 'relocated').mkdir()
        (directory / 'relocated' / 'owner.slim').write_text(source)
        owner = owner.replace('owner.slim', 'relocated/owner.slim')
        project.write_text(f'(project 1 (entry stable_codegen) {padding} {owner})\n')
        assert not before - units(emit(compiler, project)), 'module relocation'
        # Reverse every declaration in a fixture with equal lowering inputs.
        chunks = re.split(r'(?=^(?:fn|struct|enum) )', source, flags=re.M)
        path.write_text(chunks[0] + '\n'.join(reversed(chunks[1:])))
        assert units(emit(compiler, fixture)) == units(emit(compiler, path)), 'declaration reorder'
        path.write_text(source.replace('Some(41)', 'Some(40)'))
        assert units(emit(compiler, fixture)) != units(emit(compiler, path)), 'changed body must change C'
        path.write_text(chunks[0] + '\n'.join(reversed(chunks[1:])))
        cfile = directory / 'program.c'
        cfile.write_text(emit(compiler, path))
        assert 'SlimParallel_exercise_95s42_s' in cfile.read_text()
        assert 'SlimParallel_exercise_95s420_s' in cfile.read_text()
        assert 'slim_v_value_n42 slim_t_123 SlimParallel_42_0' in cfile.read_text()
        executable = directory / 'program'
        for flags in ([], ['-DSLIM_POSIX_WORKERS=1', '-pthread']):
            built = run(os.environ.get('CC', 'cc'), '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', *flags,
                        '-DSLIM_PARALLEL=1', '-I', 'runtime', cfile, 'runtime/slim_rt.c', '-o', executable)
            assert built.returncode == 0, built.stderr
            for fail in ('0', '1'):
                env = dict(os.environ, SLIM_TASK_FAIL_AT=fail)
                result = run(executable, env=env)
                assert result.returncode == 0 and result.stdout == b'' and result.stderr == b'', result
            if flags:
                joined = run(executable, env=dict(os.environ, SLIM_TASK_JOIN_FAIL_AT='1'))
                assert joined.returncode == 70 and joined.stderr == b'SLIM runtime trap: injected structured task join failure\n', joined
    if args.baseline:
        rejected = sorted(Path('conformance/fail').glob('*.slim'))
        for path in rejected:
            same_result(run(args.baseline, 'check', path), run(compiler, 'check', path), path)
        print(f'{len(rejected)} rejected fixtures: exact status/stdout/stderr')
    if args.report:
        Path(args.report).write_text('# All consumed analysis reports exact; C differences are injective private identifier renamings only.\n'
                                    'fixture\tbaseline_C_bytes\tcandidate_C_bytes\trenamed_tokens\tanalysis\n' +
                                    ''.join('\t'.join(map(str, row)) + '\n' for row in rows))
    print(f'codegen identities: {len(fixtures)} fixtures, four relocation edits each; module relocation/reordering; native serial/worker/fallback checks')


if __name__ == '__main__':
    main()
