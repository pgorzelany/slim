"""Source edit fixtures for the public retained compiler protocol; no SLIM semantics."""
from pathlib import Path
import re


def edit_cases():
    manifest = '(project 1 (entry hello) (module data "data.slim" (imports) (exports helper)) (module hello "program.slim" (imports data) (exports)))\n'
    main = 'module hello\n\nfn main(args: Vec[Bytes]) -> I64:\n  data.helper(1)\n'
    helper = 'module data\n\nfn helper(value: I64) -> I64:\n  value + 1\n'

    def files(app=main, library=helper, project=manifest):
        return {'slim.project': project, 'program.slim': app, 'data.slim': library}

    base = files()
    yield 'callee-body', base, files(library=helper.replace('+ 1', '+ 2')), True
    yield 'caller-argument', base, files(app=main.replace('helper(1)', 'helper(2)')), True
    yield 'interface', base, files(app=main.replace('helper(1)', 'helper(true)'), library='module data\n\nfn helper(value: Bool) -> I64:\n  if value:\n    1\n  else:\n    0\n'), True
    effect = helper.replace('-> I64:', '-> I64 effects[io]:').replace('  value + 1', '  io.print_i64(value)\n  value + 1')
    yield 'effect', base, files(app=main.replace('-> I64:', '-> I64 effects[io]:'), library=effect), True
    yield 'missing-effect', base, files(library=effect), False
    inserted = files(library=helper + '\nfn idle() -> I64:\n  0\n')
    yield 'insertion', base, inserted, True
    yield 'deletion', inserted, base, True
    yield 'reordering', inserted, files(library=helper.replace('fn helper', 'fn idle() -> I64:\n  0\n\nfn helper')), True
    yield 'rename', base, files(app=main.replace('helper', 'renamed'), library=helper.replace('helper', 'renamed'), project=manifest.replace('exports helper', 'exports renamed')), True
    yield 'module-rename', base, files(app=main.replace('data.', 'dep.'), library=helper.replace('module data', 'module dep'), project=manifest.replace('module data', 'module dep').replace('imports data', 'imports dep')), True
    inserted_module = files(project=manifest.replace('(module data', '(module aux "aux.slim" (imports) (exports)) (module data'))
    inserted_module['aux.slim'] = 'module aux\n\nfn idle() -> I64:\n  0\n'
    yield 'module-insertion', base, inserted_module, True
    yield 'module-deletion', inserted_module, base, True
    yield 'removed-import', base, files(project=manifest.replace('(imports data)', '(imports)')), False
    yield 'removed-export', base, files(project=manifest.replace('(exports helper)', '(exports)')), False
    yield 'invalid-argument', base, files(app=main.replace('helper(1)', 'helper(false)')), False
    yield 'invalid-syntax', base, files(library='module data\nfn helper(\n'), False
    yield 'literal-overflow', base, files(library=helper.replace('value + 1', '9223372036854775808')), False
    yield 'source-span-shift', base, files(app='# shifted caller\n' + main, library='# shifted callee\n' + helper), True
    yield 'crlf', base, {name: source.replace('\n', '\r\n') for name, source in base.items()}, True
    yield 'manifest-path', base, {'slim.project': manifest.replace('"data.slim"', '"renamed.slim"'), 'program.slim': main, 'renamed.slim': helper}, True
    partial_main = main.replace('-> I64:', '-> I64 effects[partial]:')
    partial_helper = helper.replace('-> I64:', '-> I64 effects[partial]:')
    yield 'recurrence', files(app=partial_main, library=partial_helper), files(app=partial_main, library=partial_helper.replace('value + 1', 'recur(value)')), True
    layout_project = manifest.replace('exports helper', 'exports Leaf Node helper')
    layout = 'module data\n\nstruct Leaf:\n  value: I64\n\nstruct Node:\n  leaf: Leaf\n\nfn helper(value: Node) -> I64:\n  0\n'
    idle = main.replace('data.helper(1)', '0')
    layout_after = layout.replace('value: I64', 'value: Vec[I64]')
    yield 'transitive-layout', files(app=idle, library=layout, project=layout_project), files(app=idle, library=layout_after, project=layout_project), True
    yield 'exclusive-parameter', files(app=idle, library=layout_after, project=layout_project), files(app=idle, library=layout_after.replace('helper(value: Node)', 'helper(value: @Node)'), project=layout_project), True

    def single(source):
        return {'slim.project': '(project 1 (entry modes) (module modes "program.slim" (imports) (exports)))\n', 'program.slim': source}

    owned = 'module modes\n\nstruct Box:\n  values: Vec[I64]\n\nfn take(value: ^Box) -> I64:\n  0\n\nfn relay(value: ^Box) -> I64:\n  take(^value)\n\nfn idle() -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()\n  relay(^Box(values: values))\n'
    exclusive = owned.replace('take(value: ^Box)', 'take(value: @Box)').replace('take(^value)', 'take(@value)')
    yield 'owned-to-exclusive-call', single(owned), single(exclusive), True
    yield 'owned-to-shared-call', single(owned), single(owned.replace('take(value: ^Box)', 'take(value: Box)').replace('take(^value)', 'take(value)')), True
    yield 'invalid-call-mode', single(owned), single(owned.replace('take(value: ^Box)', 'take(value: @Box)')), False
    copy = 'module modes\n\nstruct Leaf:\n  value: I64\n\nstruct Box:\n  leaf: Leaf\n\nfn inspect(value: Box) -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let value: Box = Box(leaf: Leaf(value: 0))\n  inspect(value)\n'
    yield 'transitive-copyability', single(copy), single(copy.replace('value: I64', 'value: Vec[I64]').replace('value: 0', 'value: vec.new()')), True
    calls = Path('tests/fixtures/retained_inputs_calls.slim').read_text()
    # Preserve the actual module declaration rather than guessing its name.
    module = re.search(r'(?m)^module (\w+)$', calls)[1]
    calls_manifest = f'(project 1 (entry {module}) (module {module} "program.slim" (imports) (exports)))\n'
    original = {'slim.project': calls_manifest, 'program.slim': calls}
    for label, old, new in [
        ('input-callee-body', '  first + second', '  first - second'),
        ('input-recurrence-mask', 'recur(n - 1, invariant)', 'recur(n - 1, invariant + 1)'),
        ('input-caller-value', 'combine(1, identity(3),', 'combine(2, identity(3),'),
        ('input-unknown-to-known', 'bytes.len("unknown")', '6'),
        ('input-parameter-order', 'first: I64, second: I64', 'second: I64, first: I64'),
    ]:
        assert old in calls, label
        yield label, original, {'slim.project': calls_manifest, 'program.slim': calls.replace(old, new)}, True
