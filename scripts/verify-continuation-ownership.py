#!/usr/bin/env python3
"""Run the durable finite ownership path domains against explicit compiler builds.

This is the same 486/15,552-program domain as the independent Rust integration
oracles, with optional exact baseline diagnostics for an uninstalled candidate.
The path model never parses source or supplies facts to either compiler.
"""
import argparse
import hashlib
from pathlib import Path
import subprocess
import tempfile


SEQUENCES = ((), (1,), (2,), (3,), (2, 3), (3, 2))


def expected(actions):
    for leaf in (1, 2, 3):
        available = True
        for position in (0, leaf, 4):
            for action in SEQUENCES[actions[position]]:
                if action in (1, 2) and not available:
                    return False
                if action == 2:
                    available = False
                elif action == 3:
                    available = True
    return True


def fixture(base, orientation, pattern):
    digits = pattern
    actions = []
    for _ in range(5):
        actions.append(digits % base)
        digits //= base

    def action(position, indent):
        prefix = ' ' * indent
        if base == 3 and actions[position] == 0:
            return prefix + 'void\n'
        result = ''
        for step in SEQUENCES[actions[position]]:
            if step == 1:
                result += f'{prefix}let read_{position}: I64 = vec.len(values)\n'
            elif step == 2 and (pattern + position) % 2 == 0:
                result += f'{prefix}consume(^values)\n'
            elif step == 2:
                result += f'{prefix}let moved_{position}: Vec[I64] = values\n'
            else:
                result += f'{prefix}values = vec.new()\n'
        return result

    def leaf(position, indent):
        return action(position, indent) + ' ' * indent + 'void\n'

    binding = 'let' if base == 3 else 'var'
    source = (
        'module branch_domain\n\n'
        'fn consume(value: ^Vec[I64]) -> Void:\n  void\n\n'
        'fn exercise(flag: Bool, other: Bool) -> Void effects[alloc, partial]:\n'
        f'  {binding} values: Vec[I64] = vec.new()\n'
    )
    source += action(0, 2) + '  if flag:\n'
    if orientation == 0:
        source += '    if other:\n' + leaf(1, 6)
        source += '    else:\n' + leaf(2, 6)
        source += '  else:\n' + leaf(3, 4)
    else:
        source += leaf(1, 4) + '  else:\n    if other:\n' + leaf(2, 6)
        source += '    else:\n' + leaf(3, 6)
    source += action(4, 2)
    source += '  void\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
    return source, expected(actions)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', required=True, type=Path)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--report', required=True, type=Path)
    args = parser.parse_args()
    compiler = args.compiler.resolve()
    baseline = args.baseline.resolve() if args.baseline else None
    args.report.parent.mkdir(parents=True, exist_ok=True)
    passed = accepted = 0
    with tempfile.TemporaryDirectory(prefix='slim-continuation-domain-') as tmp:
        path = Path(tmp) / 'program.slim'
        with args.report.open('w') as report:
            report.write('domain\torientation\tpattern\texpected\tstatus\tdiagnostic_sha256\n')
            for base in (3, 6):
                for orientation in (0, 1):
                    for pattern in range(base ** 5):
                        source, valid = fixture(base, orientation, pattern)
                        path.write_text(source)
                        result = subprocess.run([str(compiler), 'check', str(path)],
                                                capture_output=True, timeout=30)
                        outputs = result.returncode, result.stdout, result.stderr
                        matches = result.returncode == (0 if valid else 1)
                        matches &= not result.stderr
                        matches &= not result.stdout if valid else b'E0315@' in result.stdout
                        if baseline:
                            prior = subprocess.run([str(baseline), 'check', str(path)],
                                                   capture_output=True, timeout=30)
                            matches &= outputs == (prior.returncode, prior.stdout, prior.stderr)
                        if not matches:
                            prefix = args.report.with_suffix('')
                            prefix.with_suffix('.mismatch.slim').write_text(source)
                            prefix.with_suffix('.mismatch.stdout').write_bytes(result.stdout)
                            prefix.with_suffix('.mismatch.stderr').write_bytes(result.stderr)
                            raise AssertionError((base, orientation, pattern, valid, outputs))
                        digest = hashlib.sha256(result.stdout).hexdigest()
                        report.write(f'{base}\t{orientation}\t{pattern}\t{int(valid)}\t'
                                     f'{result.returncode}\t{digest}\n')
                        passed += 1
                        accepted += valid
                        if passed % 1024 == 0:
                            report.flush()
                            print(f'ownership domain: {passed} checked', flush=True)
    assert passed == 16038
    print(f'ownership domain: {passed} exact cases; {accepted} accepted; '
          f'{passed - accepted} rejected', flush=True)


if __name__ == '__main__':
    main()
