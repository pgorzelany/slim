#!/usr/bin/env python3
"""Independent fixed diagnostic identities, source bytes and repair facts.

This verifier contains no SLIM parser, name resolver, type checker or host
fallback. Existing conformance identities and literal source constructions are
the oracle. Baseline freezing confirms that oracle before candidate execution;
it never derives expected facts from a candidate report.
"""

import argparse
import contextlib
from dataclasses import dataclass, field
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
LIMITS = dict(details=64, excerpt_bytes=256, type_bytes=256, type_depth=128,
              line_bytes=1048576, identity_bytes=256, row_bytes=8192)
RAW = re.compile(rb'(E[0-9]{4}|Q[0-9]{4})@(?:(.*?)@)?([0-9]+):([0-9]+)\n')
BASE_KEYS = {'schema', 'code', 'severity', 'message', 'file', 'span',
             'labels', 'notes', 'fixes'}
# Fixed public producer/catalog contract, authored before candidate execution.
CAUSES = {
    'E0102': ('syntax', 'The source violates a required declaration, expression, executable shape or supported canonical-node bound.'),
    'E0103': ('syntax', 'A tab occurs outside a byte string in source.'),
    'E0108': ('syntax', 'A list requires a comma separator or its closing delimiter here.'),
    'E0314': ('name', 'The name does not resolve to an available binding or declaration.'),
    'E0315': ('ownership', 'This binding may already have been moved on the current control path.'),
    'E0343': ('effect', 'The operation requires an effect outside the function declared capability ceiling.'),
    'E0344': ('type', 'The value type does not satisfy the type required at this operation.'),
    'E0347': ('ownership', 'The operation would move, mutate or let a borrowed value escape outside its allowed capability.'),
    'E0351': ('shape', 'The supplied argument, field, payload or alternative count does not match the required shape.'),
    'E0352': ('name', 'A declaration name is duplicated, or a type, member, field or alternative name violates its required declaration kind, membership or canonical order.'),
    'E0359': ('ownership', 'Assignment targets an immutable binding.'),
    'E0402': ('project', 'The project manifest version is unsupported.'),
    'E0409': ('input', 'The required source file could not be read.'),
    'E0411': ('project', 'An import names a module absent from the manifest.'),
    'E0414': ('project', 'An exported name has no matching declaration in its module.'),
    'E0415': ('project', 'The qualified reference does not identify an admitted exported declaration of the required kind.'),
    'E0416': ('project', 'The referenced provider is absent from the project or the current module declared imports.'),
    'E0417': ('project', 'A qualified reference uses the current module as its provider.'),
    'E0418': ('project', 'An exported signature references a local name that is not an exported type declaration.'),
}


def require(condition, *detail):
    if not condition:
        raise AssertionError(detail)


def digest(data):
    return hashlib.sha256(data).hexdigest()


@dataclass
class Case:
    name: str
    path: Path
    raw: bytes
    origins: list
    facts: dict = field(default_factory=dict)
    repaired: bytes | None = None
    repaired_project: dict | None = None
    status: int = 1


def fixture(name, path, raw, origin=None, **kwargs):
    path = ROOT / path
    origins = [path if origin is None else ROOT / origin] * len(raw.splitlines())
    return Case(name, path, raw, origins, **kwargs)


def private_type_source_cases():
    # These existing conformance intervals address manifest export names, not
    # the module whose private type triggered rejection. Literal byte checks
    # below are sealed before native execution; candidate output is no oracle.
    field = ROOT/'conformance/projects/namespace/field-var-private/slim.project'
    leak = ROOT/'conformance/projects/rules/private-type-leak.project'
    return [
        fixture('private-type-field-manifest', str(field), b'E0418@app@68:74\n',
                facts={'primary_literal':'Holder','origin_bytes':79,'origin_location':[2,46],
                       'unrelated_source':{'path':str(field.parent/'app.slim'),'bytes':105,
                                           'interval_literal':'n main'}},
                repaired_project={'slim.project':field.read_bytes().replace(b'(exports Holder)',b'(exports Holder Item)')}),
        fixture('private-type-signature-manifest', str(leak),
                b'E0418@library@114:120\nE0418@library@114:120\n',
                facts={'primary_literal':'reveal','origin_bytes':124,'origin_location':[1,115],
                       'unrelated_source':{'path':str(leak.parent/'leak.slim'),'bytes':89,
                                           'interval_literal':None}},
                repaired_project={'private-type-leak.project':leak.read_bytes().replace(b'(exports reveal)',b'(exports Hidden reveal)')}),
    ]


def declaration_name_cases():
    duplicate = ROOT/'conformance/fail/declaration_name_conflict.slim'
    unknown = ROOT/'conformance/fail/unknown_field.slim'
    order = ROOT/'conformance/fail/field_order.slim'
    return [
        fixture('duplicate-declaration', str(duplicate), b'E0352@64:68\n',
                facts={'primary_literal':'Item','origin_bytes':122,'origin_location':[6,4],
                       'related':(duplicate,41,45),'related_literal':'Item',
                       'producer_rule':'duplicate-declaration',
                       'cause_contract':('name','A declaration name is duplicated in the module namespace.'),
                       'unrecorded_types':True,'requirement_unavailable':True},
                repaired=duplicate.read_bytes().replace(b'fn Item()',b'fn make_item()')),
        fixture('unknown-member-name', str(unknown), b'E0352@145:152\n',
                facts={'primary_literal':'missing','origin_bytes':153,'origin_location':[9,8],
                       'producer_rule':'','related_unavailable':True,'requirement_unavailable':True},
                repaired=unknown.read_bytes().replace(b'pair.missing',b'pair.left')),
        fixture('record-field-order', str(order), b'E0352@117:122\n',
                facts={'primary_literal':'right','origin_bytes':148,'origin_location':[8,25],
                       'producer_rule':'','related_unavailable':True,'requirement_unavailable':True},
                repaired=order.read_bytes().replace(b'Pair(right: 2, left: 1)',b'Pair(left: 1, right: 2)')),
    ]


def public_reference_cases(directory):
    # The public-type guard requires a resolved, exported type declaration.
    # These independently constructed inputs cross its absent-name and
    # wrong-declaration-kind arms while retaining the existing manifest span.
    base = ROOT/'conformance/projects/namespace/field-var-private'
    manifest = (base/'slim.project').read_bytes()
    module = (base/'app.slim').read_bytes()
    result = []
    for name, typename, size, wrong in (
        ('public-unresolved-type', b'Missing', 108, '\n\nfn m'),
        ('public-wrong-kind-type', b'main', 105, 'n main'),
    ):
        root = directory/name
        root.mkdir()
        path = root/'slim.project'
        path.write_bytes(manifest)
        source = root/'app.slim'
        source.write_bytes(module.replace(b'var: Item', b'var: '+typename))
        result.append(Case(name, path, b'E0418@app@68:74\n', [path],
            facts={'primary_literal':'Holder','origin_bytes':79,'origin_location':[2,46],
                   'unrelated_source':{'path':str(source),'bytes':size,'interval_literal':wrong},
                   'producer_rule':'','unrecorded_types':True,
                   'related_unavailable':True,'requirement_unavailable':True},
            repaired_project={'app.slim':module.replace(b'var: Item',b'var: I64')}))
    # Self qualification is rejected before declaration lookup. The suffix
    # deliberately has no declaration, so a cause claiming that it exists
    # would be false even though the original raw identity remains unchanged.
    root = directory/'self-qualified-missing-name'
    root.mkdir()
    path = root/'slim.project'
    path.write_bytes(b'(project 1 (entry app) (module app "app.slim" (imports) (exports)))\n')
    source = root/'app.slim'
    data = b'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  app.missing()\n'
    source.write_bytes(data)
    result.append(Case('self-qualified-missing-name',path,b'E0417@app@48:59\n',[source],
        facts={'primary_literal':'app.missing','origin_bytes':62,'origin_location':[4,3],
               'producer_rule':'','unrecorded_types':True,
               'related_unavailable':True,'requirement_unavailable':True},
        repaired_project={'app.slim':data.replace(b'app.missing()',b'0')}))
    return result


def closing_delimiter_cases(directory):
    # The final I64 is a complete parameter type. At byte 34 there is neither
    # the next comma nor the closing parenthesis; EOF retains a zero-width span.
    path = directory/'truncated-parameter-list'/'program.slim'
    path.parent.mkdir()
    path.write_bytes(b'module truncated_list\n\nfn f(a: I64')
    return [Case('truncated-parameter-list',path,b'E0108@34:34\n',[path],
        facts={'primary_literal':'','origin_bytes':34,'origin_location':[3,12],
               'producer_rule':'','unrecorded_types':True,
               'related_unavailable':True,'requirement_unavailable':True},
        repaired=b'module truncated_list\n\nfn f(a: I64) -> I64:\n  a\n\nfn main(args: Vec[Bytes]) -> I64:\n  f(0)\n')]


def cases(directory):
    fixed = [
        fixture('tab', 'conformance/fail/tab_indentation.slim', b'E0103@58:59\n'),
        fixture('malformed-block', 'conformance/fail/malformed_block.slim', b'E0102@69:69\n'),
        fixture('missing-comma', 'conformance/fail/missing_comma.slim', b'E0108@119:121\n'),
        fixture('unresolved-name', 'conformance/fail/unknown_name.slim', b'E0314@57:64\n'),
        fixture('annotation-type', 'conformance/fail/nested_type_mismatch.slim', b'E0344@82:86\n',
                facts={'expected': 'I64', 'actual': 'Bool', 'requirement': (ROOT/'conformance/fail/nested_type_mismatch.slim', 76, 79)}),
        fixture('argument-type', 'conformance/fail/call_argument_type.slim', b'E0344@113:118\n',
                facts={'expected': 'I64', 'actual': 'Bool', 'requirement': (ROOT/'conformance/fail/call_argument_type.slim', 46, 49)}),
        fixture('variant-type', 'conformance/fail/variant_payload_type.slim', b'E0344@128:133\n',
                facts={'expected': 'I64', 'actual': 'Bool', 'requirement': (ROOT/'conformance/fail/variant_payload_type.slim', 55, 58)}),
        fixture('unsupported-match', 'conformance/fail/boolean_match.slim', b'E0344@64:68\n', facts={'unrecorded_types':True}),
        fixture('argument-count', 'conformance/fail/call_arity.slim', b'E0351@109:112\n', facts={'expected_count':2,'actual_count':1}),
        fixture('io-capability', 'conformance/fail/missing_effect.slim', b'E0343@59:69\n', facts={'required_effect':'io'}),
        fixture('partial-capability', 'conformance/fail/missing_partial.slim', b'E0343@120:126\n', facts={'required_effect':'partial'}),
        fixture('recurrence-unproven', 'conformance/fail/termination_unconditional.slim', b'E0343@50:55\n',
                facts={'termination_cause':True,'producer_rule':'recurrence-totality'}),
        fixture('call-cycle', 'conformance/fail/termination_mutual.slim', b'E0343@95:99\n',
                facts={'termination_cause':True,'producer_rule':'checked-call-cycle'}),
        fixture('shared-mutation', 'conformance/fail/shared_mutation.slim', b'E0347@88:94\n'),
        fixture('moved-use', 'conformance/fail/use_after_move.slim', b'E0315@174:180\n',
                facts={'requirement':(ROOT/'conformance/fail/use_after_move.slim',115,121),
                       'related':(ROOT/'conformance/fail/use_after_move.slim',155,161),'producer_rule':'prior-possible-move'}),
        fixture('branch-move', 'conformance/fail/branch_move_join.slim', b'E0315@229:235\n',
                facts={'requirement':(ROOT/'conformance/fail/branch_move_join.slim',128,134),
                       'related':(ROOT/'conformance/fail/branch_move_join.slim',194,200),'producer_rule':'prior-possible-move'}),
        fixture('reinit-one-branch', 'conformance/fail/reinit_one_arm.slim', b'E0315@243:249\n',
                facts={'requirement':(ROOT/'conformance/fail/reinit_one_arm.slim',125,131),
                       'related':(ROOT/'conformance/fail/reinit_one_arm.slim',165,171),'producer_rule':'prior-possible-move'}),
        fixture('reinit-all-branches', 'conformance/pass/reinit_branches.slim', b'',status=0),
        fixture('immutable-assignment', 'conformance/fail/immutable_assignment.slim', b'E0359@86:91\n'),
        fixture('manifest-version', 'conformance/projects/malformed/slim.project', b'E0402@-@9:10\n'),
        fixture('unknown-import', 'conformance/projects/rules/unknown-import.project', b'E0411@-@55:62\n'),
        fixture('missing-module', 'conformance/projects/rules/missing-source.project', b'E0409@app@35:49\n'),
        fixture('absent-export', 'conformance/projects/rules/absent-export.project', b'E0414@app@65:72\n'),
        fixture('private-reference', 'conformance/projects/private/slim.project', b'E0415@app@48:59\n',
                origin='conformance/projects/private/app.slim'),
        fixture('unimported-reference', 'conformance/projects/rules/unimported-reference.project', b'E0416@app@48:61\n',
                origin='conformance/projects/rules/unimported.slim'),
        fixture('module-parser', 'conformance/projects/malformed-module/slim.project', b'E0102@app@67:67\n',
                origin='conformance/projects/malformed-module/app.slim'),
    ]
    # Literal repairs are acceptance controls, independent of report contents.
    repairs = {
        'tab': (b'\t', b'  '),
        'malformed-block': (b'  if true:\n', b'  0\n'),
        'missing-comma': (b'add(20 22)', b'add(20, 22)'),
        'unresolved-name': (b'missing', b'0'),
        'annotation-type': (b'= true', b'= 0'),
        'argument-type': (b'identity(false)', b'identity(0)'),
        'variant-type': (b'Some(false)', b'Some(0)'),
        'argument-count': (b'add(1)', b'add(1, 2)'),
        'io-capability': (b'-> I64:', b'-> I64 effects[io]:'),
        'partial-capability': (b'fn main(args: Vec[Bytes]) -> I64:', b'fn main(args: Vec[Bytes]) -> I64 effects[partial]:'),
        'shared-mutation': (b'values: Vec[I64]', b'values: @Vec[I64]'),
        'immutable-assignment': (b'let value:', b'var value:'),
        'recurrence-unproven': (b'fn spin(value: I64) -> I64:', b'fn spin(value: I64) -> I64 effects[partial]:'),
    }
    for case in fixed:
        if case.name in repairs:
            before, after = repairs[case.name]
            data = case.path.read_bytes()
            require(before in data, case.name, 'repair literal disappeared')
            case.repaired = data.replace(before, after, 1)
        if case.name == 'moved-use':
            data = case.path.read_bytes()
            position = data.rfind(b'  consume(^values)\n')
            require(position >= 0, case.name)
            case.repaired = data[:position] + b'  void\n' + data[position + len(b'  consume(^values)\n'):]
        if case.name == 'unsupported-match':
            case.repaired = b'module boolean_match\n\nfn main(args: Vec[Bytes]) -> I64:\n  if true:\n    0\n  else:\n    1\n'
        if case.name == 'call-cycle':
            case.repaired = case.path.read_bytes().replace(b'-> I64:\n',b'-> I64 effects[partial]:\n',2)

    # These controls repair one project rejection while retaining the module
    # declarations that made the original source provenance observable.
    project_repairs = {
        'manifest-version': {'slim.project': b'(project 1 (entry app) (module app "app.slim" (imports) (exports)))\n',
                             'app.slim': b'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'},
        'unknown-import': {'unknown-import.project': (ROOT/'conformance/projects/rules/unknown-import.project').read_bytes().replace(b'(imports missing)',b'(imports)')},
        'missing-module': {'missing-source.project': (ROOT/'conformance/projects/rules/missing-source.project').read_bytes().replace(b'"missing.slim"',b'"app.slim"')},
        'absent-export': {'absent-export.project': (ROOT/'conformance/projects/rules/absent-export.project').read_bytes().replace(b'(exports missing)',b'(exports)')},
        'private-reference': {'slim.project': (ROOT/'conformance/projects/private/slim.project').read_bytes().replace(b'(exports)))',b'(exports answer)))')},
        'unimported-reference': {'unimported-reference.project': (ROOT/'conformance/projects/rules/unimported-reference.project').read_bytes().replace(b'(imports) (exports)) (module library',b'(imports library) (exports)) (module library')},
        'module-parser': {'app.slim': (ROOT/'conformance/projects/malformed-module/app.slim').read_bytes()+b'  value\n'},
    }
    for case in fixed:
        case.repaired_project = project_repairs.get(case.name)
    fixed.extend(private_type_source_cases())
    fixed.extend(declaration_name_cases())
    fixed.extend(public_reference_cases(directory))
    fixed.extend(closing_delimiter_cases(directory))

    def source_case(name, data, raw, facts=None, repaired=None, path=None):
        path = path or directory/name/'program.slim'
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        fixed.append(Case(name, path, raw, [path]*len(raw.splitlines()), facts or {}, repaired))

    source = b'module source_bytes\n\nfn main(args: Vec[Bytes]) -> I64:\n  missing\n'
    at = source.index(b'missing')
    source_case('same-length-before', source, f'E0314@{at}:{at+7}\n'.encode(),
                repaired=source.replace(b'missing', b'0'))
    mutated = source.replace(b'missing', b'absentx')
    source_case('same-length-after', mutated, f'E0314@{at}:{at+7}\n'.encode())
    prefix = b'# caf\xc3\xa9 \xe2\x98\x83\n'
    source_case('unicode-byte-prefix', prefix+source,
                f'E0314@{at+len(prefix)}:{at+len(prefix)+7}\n'.encode())
    inline_tab=b'module inline_tab\n\nfn main(args:\tVec[Bytes]) -> I64:\n  0\n'
    tab_at=inline_tab.index(b'\t')
    source_case('inline-tab',inline_tab,f'E0103@{tab_at}:{tab_at+1}\n'.encode(),
                repaired=inline_tab.replace(b'\t',b' '))

    api = b'module api\n\nfn expect(value: I64) -> I64:\n  value\n'
    app = b'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  api.expect(false)\n'
    cross = directory/'cross-module'
    cross.mkdir()
    (cross/'api.slim').write_bytes(api)
    (cross/'app.slim').write_bytes(app)
    manifest = cross/'slim.project'
    manifest.write_bytes(b'(project 1 (entry app) (module api "api.slim" (imports) (exports expect)) (module app "app.slim" (imports api) (exports)))\n')
    fixed.append(Case('cross-module-requirement', manifest, b'E0344@app@59:64\n', [cross/'app.slim'],
                      {'expected':'I64', 'actual':'Bool', 'requirement':(cross/'api.slim',29,32)},
                      repaired_project={'app.slim':app.replace(b'false',b'0')}))

    missing = directory/'not-present.slim'
    fixed.append(Case('missing-source', missing, b'E0409@0:0\n', [None]))
    head = b'module detail_limit\n\nfn consume(value: ^Vec[I64]) -> Void:\n  void\n\nfn main(args: Vec[Bytes]) -> I64 effects[alloc]:\n  let values: Vec[I64] = vec.new()\n  consume(^values)\n'
    repeated = b'  consume(^values)\n'
    for count in (64, 65, 66):
        data = head+repeated*count+b'  0\n'
        raw = b''.join(f'E0315@{len(head)+i*len(repeated)+11}:{len(head)+i*len(repeated)+17}\n'.encode() for i in range(count))
        source_case(f'detail-rows-{count}', data, raw, {'detail_rows':count})

    # Primary literal spans independently cross excerpt capacity.
    for width in (255, 256, 257):
        data = b'module excerpt\n\nfn main(args: Vec[Bytes]) -> I64:\n  "'+b'a'*(width-2)+b'"\n'
        start = data.index(b'"')
        source_case(f'excerpt-bytes-{width}', data, f'E0344@{start}:{start+width}\n'.encode(),
                    {'expected':'I64', 'actual':'Bytes', 'excerpt_width':width})

    for length in (255, 256, 257):
        name = b'T'+b'x'*(length-1)
        data = b'module type_text\n\nstruct '+name+b':\n\nfn main(args: Vec[Bytes]) -> I64:\n  let value: '+name+b' = true\n  0\n'
        start = data.index(b'true')
        source_case(f'type-text-{length}', data, f'E0344@{start}:{start+4}\n'.encode(),
                    {'expected':name.decode() if length<=256 else None, 'actual':'Bool', 'type_width':length})

    for depth in (127, 128, 129):
        nested = b'Vec['*depth+b'I64'+b']'*depth
        data = b'module type_depth\n\nfn main(args: Vec[Bytes]) -> I64:\n  let value: '+nested+b' = true\n  0\n'
        start = data.index(b'true')
        source_case(f'type-depth-{depth}', data, f'E0344@{start}:{start+4}\n'.encode(), {'type_depth':depth})

    for position in (1048575, 1048576, 1048577):
        length = position-at
        comment = b'#'+b'a'*(length-2)+b'\n'
        data = comment+source
        source_case(f'line-scan-{position}', data, f'E0314@{position}:{position+7}\n'.encode(),
                    {'line_budget':position>1048576})

    # Filesystem components stay within native filename limits; complete paths
    # cross identity capacity. The requested root is a temporary test directory.
    for width in (255,256,257):
        long_root=directory/f'identity-{width}'
        long_root.mkdir()
        remaining=width-len(str(long_root/'program.slim').encode())-1
        require(0<remaining<255,'temporary root cannot exercise identity boundary',directory)
        path=long_root/('x'*remaining)/'program.slim'
        require(len(str(path).encode())==width,'identity fixture length',path)
        source_case(f'identity-bytes-{width}', source, f'E0314@{at}:{at+7}\n'.encode(),
                    {'identity_limit':width>256}, path=path)
        project_root=directory/f'project-identity-{width}'
        project_root.mkdir()
        remaining=width-len(str(project_root/'app.slim').encode())-1
        require(0<remaining<255,'temporary root cannot exercise project identity boundary',directory)
        module_path=project_root/('x'*remaining)/'app.slim'
        module_path.parent.mkdir()
        module_source=b'module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  missing\n'
        module_path.write_bytes(module_source)
        module_at=module_source.index(b'missing')
        project_path=project_root/'slim.project'
        relative=module_path.relative_to(project_root).as_posix()
        project_path.write_text(f'(project 1 (entry app) (module app "{relative}" (imports) (exports)))\n')
        fixed.append(Case(f'project-identity-bytes-{width}',project_path,
                          f'E0314@app@{module_at}:{module_at+7}\n'.encode(),[module_path],{'identity_limit':width>256}))
    return fixed


def execute(compiler, command, path):
    return subprocess.run([str(compiler), command, str(path)], cwd=ROOT,
                          capture_output=True, timeout=20)


def node_admission_cases(directory):
    # The fixed production canonical-node observer independently established
    # module4 + each constant helper11 + main18 + one node per effect atom.
    # See verify-context.py:test_exact_million_canonical_node_admission. This
    # restricted literal constructor does not parse or check SLIM in Python.
    root=directory/'canonical-node-admission'
    root.mkdir()
    def helpers(count,prefix=b''):
        return b''.join(b'fn '+prefix+b'value'+str(i).encode()+b'() -> I64:\n  0\n\n' for i in range(count))
    main=b'fn main(args: Vec[Bytes]) -> I64 effects[io, partial]:\n  0\n'
    count=4+90907*11+18+2
    require(count==1000001,'fixed standalone canonical-node count')
    source=b'module node_limit\n\n'+helpers(90907)+main
    path=root/'direct.slim'
    path.write_bytes(source)
    cause=('resource','The canonical input exceeds the supported 1000000-node admission bound.')
    facts={'producer_rule':'canonical-node-limit','expected_count':1000000,'actual_count':count,
           'cause_contract':cause,'node_formula':'4 + 90907*11 + 18 + 2'}
    direct=Case('canonical-node-limit-direct',path,f'E0102@0:{len(source)}\n'.encode(),[path],facts)

    project=root/'project'
    project.mkdir()
    api=b'module api\n\n'+helpers(2)
    app=b'module app\n\n'+helpers(90905)+main
    require(4+2*11==26 and 4+90905*11+18+2==999979,'fixed per-module admission counts')
    api_path=project/'api.slim'; api_path.write_bytes(api)
    app_path=project/'app.slim'; app_path.write_bytes(app)
    manifest=project/'slim.project'
    manifest.write_bytes(b'(project 1 (entry app) (module api "api.slim" (imports) (exports)) (module app "app.slim" (imports api) (exports)))\n')
    # Pure constant helper names get the fixed pMODULE__ namespace prefix;
    # main retains its entry name. The literal canonical formatter places one
    # blank line between declarations and emits the global project header.
    # Only the extent is an oracle; no separately parsed representation exists.
    flat_extent=len(b'module project\n\n')+len(helpers(2,b'papi__'))+len(helpers(90905,b'papp__'))+len(main)
    project_facts=dict(facts,source_kind='unavailable',source_label='-',source_path=str(manifest),
                       node_formula='4 + (2 + 90905)*11 + 18 + 2',flat_bytes=flat_extent,
                       local_node_hypotheses=[26,999979],
                       source_inputs=[{'path':str(api_path),'bytes':len(api),'sha256':digest(api)},
                                      {'path':str(app_path),'bytes':len(app),'sha256':digest(app)}])
    whole=Case('canonical-node-limit-project',manifest,f'E0102@0:{flat_extent}\n'.encode(),[None],project_facts)
    admitted=Case('canonical-node-admitted-app',app_path,b'',[],
                  {'node_formula':'4 + 90905*11 + 18 + 2','node_hypothesis':999979},status=0)
    return [direct,whole,admitted]


def raw_rows(case):
    result = []
    for line in case.raw.splitlines(keepends=True):
        match = RAW.fullmatch(line)
        require(match is not None, case.name, line)
        result.append((match[1].decode(), None if match[2] is None else match[2].decode(),
                       int(match[3]), int(match[4])))
    require(len(result)==len(case.origins), case.name)
    return result


def check_raw(compiler, case):
    result = execute(compiler, 'check', case.path)
    require((result.returncode,result.stdout,result.stderr)==(case.status,case.raw,b''),
            case.name, 'raw identity/status changed', result)


def validate_declared_requirement_spans(corpus):
    # Expected types and origin intervals are independently authored literals.
    # Validate their byte agreement before any native command, so an oracle
    # offset error cannot be mistaken for a candidate compiler regression.
    for case in corpus:
        if 'requirement' not in case.facts or case.facts.get('expected') is None:
            continue
        path,start,end=case.facts['requirement']
        data=path.read_bytes()
        expected=case.facts['expected'].encode('ascii')
        require(0<=start<=end<=len(data),case.name,'declared requirement interval',start,end,len(data))
        require(data[start:end]==expected,case.name,'declared requirement oracle bytes',start,end,data[start:end],expected)


def validate_primary_span_oracles(corpus):
    for case in corpus:
        if 'primary_literal' not in case.facts:
            continue
        unrelated=case.facts.get('unrelated_source')
        module=None if unrelated is None else Path(unrelated['path']).read_bytes()
        if unrelated is not None:
            require(len(module)==unrelated['bytes'],case.name,'unrelated module fixture length')
        for identity,origin in zip(raw_rows(case),case.origins):
            data=origin.read_bytes()
            start,end=identity[2:]
            require(len(data)==case.facts['origin_bytes'],case.name,'manifest fixture length')
            require(data[start:end]==case.facts['primary_literal'].encode('ascii'),case.name,'literal primary origin bytes')
            actual=[data[:start].count(b'\n')+1,start-data.rfind(b'\n',0,start)]
            require(actual==case.facts['origin_location'],case.name,'literal primary origin location')
            if unrelated is not None:
                wrong=unrelated['interval_literal']
                if wrong is None:
                    require(end>len(module),case.name,'manifest span lies outside unrelated module')
                else:
                    require(module[start:end]==wrong.encode('ascii') and module[start:end]!=data[start:end],
                            case.name,'manifest and module byte trap')
        if 'related_literal' in case.facts:
            path,start,end=case.facts['related']
            require(path.read_bytes()[start:end]==case.facts['related_literal'].encode('ascii'),
                    case.name,'literal prior declaration name')


def source_bytes(value, expected_path, case, detailed=True):
    require(isinstance(value,dict),case.name,'source object',value)
    for key in ('kind','label','path','identity_bytes','path_bytes','bytes','span_valid','location_reason','excerpt'):
        require(key in value,case.name,'missing source field',key)
    if expected_path is None:
        require(value['bytes'] is None and value['excerpt'] is None and not value['span_valid'],case.name,value)
        require(value['location_reason'],case.name,'unavailable source reason')
        return None
    data = expected_path.read_bytes()
    require(value['bytes']==len(data),case.name,'captured length',value)
    if not case.facts.get('identity_limit'):
        require(value['path'].encode('latin1')==os.fsencode(expected_path),case.name,'actual source path bytes',value,expected_path)
    excerpt = value['excerpt']
    if detailed:
        require(excerpt is not None,case.name,'captured source excerpt missing')
    else:
        require(excerpt is None,case.name,'bounded row retains detail',value)
    if excerpt is not None:
        require(isinstance(excerpt,dict) and {'start','end','text'}<=set(excerpt),case.name,excerpt)
        start,end = excerpt['start'],excerpt['end']
        require(type(start) is int and type(end) is int and 0<=start<=end<=len(data),case.name,excerpt)
        require(end-start<=256 and excerpt['text'].encode('latin1')==data[start:end],case.name,'excerpt byte authority',excerpt)
    return data


def verify_json(compiler,case):
    first = execute(compiler,'check-json',case.path)
    repeat = execute(compiler,'check-json',case.path)
    require((first.returncode,first.stdout,first.stderr)==(repeat.returncode,repeat.stdout,repeat.stderr),case.name,'deterministic JSON')
    require(first.returncode==case.status and first.stderr==b'',case.name,'native JSON status/transport',first)
    lines = first.stdout.splitlines(keepends=True)
    require(len(lines)==len(case.origins),case.name,'one complete row per raw diagnostic')
    decoded=[]
    for index,(line,identity,origin) in enumerate(zip(lines,raw_rows(case),case.origins)):
        require(line.endswith(b'\n') and len(line)<=8192,case.name,'complete bounded row',len(line))
        row=json.loads(line)
        require(BASE_KEYS<=set(row) and row['schema']==1 and row['severity']=='error',case.name,row)
        require(row.get('encoding')=='json-byte-escapes-v1',case.name,'byte encoding')
        require(row['code']==identity[0],case.name,'code',row)
        file_identity=identity[1] if identity[1] is not None else str(case.path)
        require(row['file'].encode('latin1')==(b'' if case.facts.get('identity_limit') else os.fsencode(file_identity)),case.name,'raw file identity bytes',row)
        require(row['span']['start']==identity[2] and row['span']['end']==identity[3],case.name,'primary span changed',row)
        require(all(isinstance(row[key],list) for key in ('labels','notes','fixes')),case.name,row)
        repair=row['repair']
        require({'status','reason','category','cause','source','expected','actual','requirement','related',
                 'expected_count','actual_count','required_effect','snapshot_requirement','identity_bytes','producer_rule'}<=set(repair),case.name,repair)
        if 'cause_contract' in case.facts:
            require((repair['category'],repair['cause'])==tuple(case.facts['cause_contract']),case.name,'exact producer cause',repair)
        elif identity[0] in CAUSES and not case.facts.get('termination_cause'):
            require((repair['category'],repair['cause'])==CAUSES[identity[0]],case.name,'concrete common cause',repair)
        else:
            require(repair['category'] and repair['cause'] and repair['cause']!='rejected by the SLIM compiler',case.name,'concrete common cause')
        require(repair['snapshot_requirement']=='retain-complete-input-and-recheck',case.name,'source revision binding',repair)
        require(row['message']==repair['cause'],case.name,'primary cause is the human explanation',row)
        detailed=index<64 and not case.facts.get('identity_limit')
        data=source_bytes(repair['source'],origin,case,detailed)
        if origin is None and case.facts.get('source_kind'):
            source=repair['source']
            require((source['kind'],source['label'],source['path'])==(case.facts['source_kind'],case.facts['source_label'],case.facts['source_path']),case.name,'canonical extent has no original source',source)
            require(source['location_reason']=='source-unavailable' and repair['status']=='unknown' and repair['reason']=='source-unavailable',case.name,'unavailable source is explicit',repair)
            require(row['span']['line']==0 and row['span']['column']==0,case.name,'canonical span cannot imply original lines',row)
        if data is not None:
            require(repair['source']['span_valid'],case.name,'valid source interval',row)
            start=identity[2]
            if detailed and start<=1048576:
                require(row['span']['line']==data[:start].count(b'\n')+1,case.name,'byte line',row)
                require(row['span']['column']==start-data.rfind(b'\n',0,start),case.name,'byte column',row)
                require(repair['source']['location_reason']=='',case.name,'exact primary location',row)
            expected_kind='manifest' if origin.suffix=='.project' else 'module'
            require(repair['source']['kind']==expected_kind,case.name,'source kind',row)
            if detailed:
                expected_label='-' if expected_kind=='manifest' else file_identity
                require(repair['source']['label'].encode('latin1')==os.fsencode(expected_label),case.name,'actual source identity bytes',row)
                require(repair['source']['identity_bytes']==len(expected_label.encode()),case.name,'source identity byte count',row)
            require(repair['source']['path_bytes']==len(str(origin).encode()),case.name,'source path byte count',row)
            if 'primary_literal' in case.facts:
                require(data[identity[2]:identity[3]]==case.facts['primary_literal'].encode('ascii'),case.name,'primary literal')
                require([row['span']['line'],row['span']['column']]==case.facts['origin_location'],case.name,'literal manifest location',row)
                require(repair['status']=='exact' and repair['reason']=='',case.name,'known manifest source',repair)
        for key in ('expected','actual'):
            value=repair[key]
            require(isinstance(value,dict) and {'evidence','kind','text','reason'}<=set(value),case.name,key,value)
            require(value['evidence'] in ('exact','bounded','unknown'),case.name,key,value)
            if value['evidence']!='exact':
                require(value['reason'],case.name,'missing optional detail reason',key,value)
            if value['text'] is not None:
                require(len(value['text'].encode('latin1'))<=256,case.name,'type text cap',key,value)
            if key in case.facts and case.facts[key] is not None:
                require(value['evidence']=='exact' and value['text']==case.facts[key],case.name,'checked type',key,value)
        for key in ('requirement','related'):
            if key in case.facts:
                path,start,end=case.facts[key]
                value=repair[key]
                require(isinstance(value,dict),case.name,'checked origin',key,value)
                source_bytes(value['source'],path,case)
                require((value['span']['start'],value['span']['end'])==(start,end),case.name,'causal source origin',key,value)
            if case.facts.get(key+'_unavailable'):
                require(repair[key] is None,case.name,'unretained optional location',key,repair[key])
        for key in ('required_effect','expected_count','actual_count','producer_rule'):
            if key in case.facts:
                require(repair[key]==case.facts[key],case.name,'producer fact',key,repair)
        if case.facts.get('unrecorded_types'):
            for key in ('expected','actual'):
                require(repair[key]['evidence']=='unknown' and repair[key]['text'] is None and repair[key]['reason']=='not-recorded-at-rejection',case.name,'unsupported specialized detail',key,repair)
        if case.facts.get('termination_cause'):
            require(repair['required_effect'] is None and repair['cause']!=CAUSES['E0343'][1],case.name,'termination cause distinction',repair)
            cause=('Recurrence is not proven total while partial is absent from the function capability ceiling.'
                   if case.facts['producer_rule']=='recurrence-totality'
                   else 'The checked-total call graph contains a cycle outside supported total recurrence.')
            require((repair['category'],repair['cause'])==('effect',cause),case.name,'honest termination cause',repair)
        if index>=64:
            require(repair['status']=='bounded' and repair['reason']=='diagnostic-limit',case.name,'detail row bound',repair)
        if case.facts.get('type_width',0)>256:
            require(repair['expected']['text'] is None and repair['expected']['reason']=='type-text-limit',case.name,'type text exceeded',repair)
        if case.facts.get('type_depth'):
            expected_reason='type-text-limit' if case.facts['type_depth']==127 else 'type-depth-limit'
            require(repair['expected']['text'] is None and repair['expected']['evidence']=='bounded' and repair['expected']['reason']==expected_reason,case.name,'independent depth decision precedence',repair)
        if case.facts.get('line_budget'):
            require(row['span']['line']==0 and row['span']['column']==0 and repair['source']['location_reason'],case.name,'line scan bound',row)
        if case.facts.get('identity_limit'):
            require(row['file']=='' and repair['source']['path']=='' and repair['source']['label']=='',case.name,'identity fallback',row)
            require(repair['source']['path_bytes']>256 and repair['reason']=='identity-limit',case.name,'identity budget evidence',row)
        decoded.append(row)
    return first.stdout,decoded


def positive_controls(compiler,case,directory,json_mode=True):
    if case.repaired is None and case.repaired_project is None:
        return
    if case.repaired_project is not None:
        root=directory/(case.name+'-repaired-project')
        shutil.copytree(case.path.parent,root)
        for name,data in case.repaired_project.items():
            (root/name).write_bytes(data)
        path=root/case.path.name
    else:
        path=directory/(case.name+'-repaired.slim')
        path.write_bytes(case.repaired)
    for command in (('check','check-json') if json_mode else ('check',)):
        result=execute(compiler,command,path)
        require((result.returncode,result.stdout,result.stderr)==(0,b'',b''),case.name,'positive repair',command,result)


def launcher(compiler,directory):
    install=directory/'launcher'
    (install/'build/toolchain').mkdir(parents=True)
    shutil.copy2(ROOT/'slimc',install/'slimc')
    shutil.copy2(ROOT/'VERSION',install/'VERSION')
    (install/'build/toolchain/slimc').symlink_to(compiler)
    return install/'slimc'


def verify_launcher(compiler,cases,directory,outputs):
    public=launcher(compiler,directory)
    for case in cases:
        result=subprocess.run([str(public),'--message-format=json','check',str(case.path)],cwd=ROOT,capture_output=True,timeout=20)
        require((result.returncode,result.stdout,result.stderr)==(case.status,b'',outputs[case.name]),case.name,'JSON launcher passthrough',result)
        human=subprocess.run([str(public),'check',str(case.path)],cwd=ROOT,capture_output=True,timeout=20)
        expected=b''.join(b'error['+line+b']: rejected by the SLIM compiler\n' for line in case.raw.splitlines())
        require((human.returncode,human.stdout,human.stderr)==(case.status,b'',expected),case.name,'unchanged human default',human)
        if case.name in ('annotation-type','cross-module-requirement'):
            jobs=subprocess.run([str(public),'--message-format=json','check',str(case.path),'--jobs','2'],cwd=ROOT,capture_output=True,timeout=20)
            require((jobs.returncode,jobs.stdout,jobs.stderr)==(case.status,b'',outputs[case.name]),case.name,'JSON jobs argument compatibility',jobs)
    # An unsupported native command must remain an exit64 failure. This is a
    # literal transport fixture, not an alternate checker or compiler path.
    unsupported=directory/'unsupported-native'
    unsupported.write_bytes(b'#!/bin/sh\ntest "$1" = check-json || exit 42\nprintf "native command unavailable\\n"\nexit 64\n')
    unsupported.chmod(0o755)
    native=public.parent/'build/toolchain/slimc'
    native.unlink()
    native.symlink_to(unsupported)
    result=subprocess.run([str(public),'--message-format=json','check',str(cases[0].path)],capture_output=True,timeout=20)
    require((result.returncode,result.stdout,result.stderr)==(64,b'',b'native command unavailable\n'),'no semantic fallback',result)
    protocol_error=b'slimc: native compiler does not support the structured diagnostic protocol\n'
    for name,body in (
        ('generated-c-success',b'printf "/* legacy generated C */\\n"\nexit 0\n'),
        ('raw-rejection',b'printf "E0409@0:0\\n"\nexit 1\n'),
        ('empty-rejection',b'exit 1\n'),
        ('incomplete-row',b'printf \'{"schema":1,"code":"E0344"}\'\nexit 1\n'),
    ):
        unsupported.write_bytes(b'#!/bin/sh\n'+body)
        result=subprocess.run([str(public),'--message-format=json','check',str(cases[0].path)],capture_output=True,timeout=20)
        require((result.returncode,result.stdout,result.stderr)==(64,b'',protocol_error),'unsupported diagnostic transport',name,result)


def legacy_transport_control(compiler,directory):
    # A genuine pre-protocol compiler treats unknown check-json as a filename.
    # A valid decoy in cwd must never make the requested invalid source pass.
    root=directory/'legacy-command-decoy'
    root.mkdir()
    (root/'check-json').write_bytes(b'module decoy\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
    path=root/'requested.slim'
    path.write_bytes(b'module source_bytes\n\nfn main(args: Vec[Bytes]) -> I64:\n  missing\n')
    raw=subprocess.run([str(compiler),'check',str(path)],cwd=root,capture_output=True,timeout=20)
    require((raw.returncode,raw.stdout,raw.stderr)==(1,b'E0314@57:64\n',b''),'legacy requested source baseline',raw)
    wrong=subprocess.run([str(compiler),'check-json',str(path)],cwd=root,capture_output=True,timeout=20)
    require(wrong.returncode==0 and wrong.stderr==b'' and b'#include "slim_rt.h"' in wrong.stdout,'legacy counterexample identity',wrong)
    public=launcher(compiler,root)
    result=subprocess.run([str(public),'--message-format=json','check',str(path)],cwd=root,capture_output=True,timeout=20)
    protocol_error=b'slimc: native compiler does not support the structured diagnostic protocol\n'
    require((result.returncode,result.stdout,result.stderr)==(64,b'',protocol_error),'legacy decoy must fail explicitly',result)
    (root/'check-json').unlink()
    result=subprocess.run([str(public),'--message-format=json','check',str(path)],cwd=root,capture_output=True,timeout=20)
    require((result.returncode,result.stdout,result.stderr)==(64,b'',protocol_error),'legacy raw rejection must fail explicitly',result)
    return dict(compiler_sha256=digest(compiler.read_bytes()),incorrect_success_stdout_sha256=digest(wrong.stdout),
                result='unsupported-protocol-rejected')


def mutation_control(compiler,directory,json_mode):
    # One source identity and equal byte lengths cross a real revision boundary.
    path=directory/'same-path-mutation.slim'
    before=b'module source_bytes\n\nfn main(args: Vec[Bytes]) -> I64:\n  missing\n'
    after=before.replace(b'missing',b'absentx')
    start=before.index(b'missing')
    raw=f'E0314@{start}:{start+7}\n'.encode()
    outputs=[]
    public=launcher(compiler,directory/'mutation-transport') if json_mode else None
    for revision,data in enumerate((before,after)):
        path.write_bytes(data)
        case=Case(f'same-path-revision-{revision}',path,raw,[path])
        check_raw(compiler,case)
        if json_mode:
            output,rows=verify_json(compiler,case)
            excerpt=rows[0]['repair']['source']['excerpt']['text'].encode('latin1')
            require(data[start:start+7] in excerpt,case.name,'current captured revision')
            forwarded=subprocess.run([str(public),'--message-format=json','check',str(path)],capture_output=True,timeout=20)
            require((forwarded.returncode,forwarded.stdout,forwarded.stderr)==(1,b'',output),case.name,'mutated source JSON transport',forwarded)
            outputs.append(output)
    if json_mode:
        require(outputs[0]!=outputs[1],'equal-length mutation reused stale source context')


def row_limit_control(compiler,directory,json_mode):
    # A short independent temporary root lets a real admitted path retain 256
    # bytes dominated by escaped bytes. Two captured 256-byte source excerpts
    # and their repeated source identity then cross the complete-row budget.
    # The source remains ordinary SLIM: the high bytes occur only in comments.
    with tempfile.TemporaryDirectory(prefix='sr.',dir='/tmp') as short:
        root=Path(short)
        available=256-len(str(root).encode())-1
        name='\u0080'*(available//2)+('x' if available%2 else '')
        path=root/name
        require(len(str(path).encode())==256,'row-size path fixture')
        comment=b'#'+b'\x80'*255+b'\n'
        data=b'module row_limit\n'+comment+b'\nfn main(args: Vec[Bytes]) -> I64:\n  false\n'+comment
        path.write_bytes(data)
        start=data.index(b'false')
        case=Case('complete-row-limit',path,f'E0344@{start}:{start+5}\n'.encode(),[path])
        check_raw(compiler,case)
        if json_mode:
            first=execute(compiler,'check-json',path)
            repeat=execute(compiler,'check-json',path)
            require((first.returncode,first.stdout,first.stderr)==(1,repeat.stdout,b''),'row-limit native transport',first)
            require(first.stdout.endswith(b'\n') and first.stdout.count(b'\n')==1 and len(first.stdout)<=8192,'row-limit complete NDJSON')
            row=json.loads(first.stdout)
            require(BASE_KEYS<=set(row) and row['schema']==1 and row['code']=='E0344','row-limit schema',row)
            require((row['span']['start'],row['span']['end'])==(start,start+5),'row-limit raw interval',row)
            repair=row['repair']
            require(repair['status']=='bounded' and repair['reason']=='row-limit','complete row fallback',row)
            require(row['file']=='' and repair['source']['path']=='' and repair['source']['label']=='','row fallback identities',row)
            require(repair['source']['excerpt'] is None and repair['requirement'] is None and repair['related'] is None,'row fallback optional details',row)
            public=launcher(compiler,directory/'row-limit-transport')
            forwarded=subprocess.run([str(public),'--message-format=json','check',str(path)],capture_output=True,timeout=20)
            require((forwarded.returncode,forwarded.stdout,forwarded.stderr)==(1,b'',first.stdout),'row-limit stderr-only transport',forwarded)
            (directory/'row-limit-report.jsonl').write_bytes(first.stdout)
        (directory/'row-limit-source.slim').write_bytes(data)


def specification(cases):
    result=[]
    for case in cases:
        entries=[]
        unrelated=case.facts.get('unrelated_source')
        extra=[] if unrelated is None else [Path(unrelated['path'])]
        for path in dict.fromkeys([case.path,*case.origins,*extra]):
            if path is not None:
                data=path.read_bytes() if path.is_file() else None
                entries.append(dict(path=str(path),bytes=None if data is None else len(data),sha256=None if data is None else digest(data)))
        result.append(dict(name=case.name,status=case.status,raw_hex=case.raw.hex(),sources=entries,
                           repaired_sha256=None if case.repaired is None else digest(case.repaired),
                           repaired_project_sha256=None if case.repaired_project is None else {name:digest(data) for name,data in case.repaired_project.items()},
                           facts={key:([str(value[0]),*value[1:]] if key in ('requirement','related') else value) for key,value in case.facts.items()}))
    return dict(schema=1,authority='literal construction and existing conformance contracts; not candidate output',
                verifier_sha256=digest(Path(__file__).read_bytes()),launcher_sha256=digest((ROOT/'slimc').read_bytes()),
                limits=LIMITS,cases=result,
                extra_controls=(['same-path-equal-length-mutation','complete-row-limit'] if any(case.name=='tab' for case in cases) else []),
                cause_contract=CAUSES)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler',type=Path,default=ROOT/'build/toolchain/slimc')
    parser.add_argument('--freeze-baseline',action='store_true')
    parser.add_argument('--baseline-compiler',type=Path,help='pre-protocol compiler for the real unknown-command decoy regression')
    parser.add_argument('--section',choices=('small','node-admission','private-type-source','declaration-name','public-reference','closing-delimiter','all'),default='all')
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    compiler=args.compiler.resolve()
    with contextlib.ExitStack() as stack:
        if args.output:
            args.output=args.output.resolve()
            directory=args.output/'corpus'
            directory.mkdir(parents=True,exist_ok=False)
        else:
            directory=Path(stack.enter_context(tempfile.TemporaryDirectory(prefix='slim-repair-diagnostics-')))
        corpus=[]
        if args.section in ('small','all'):
            corpus.extend(cases(directory))
        if args.section in ('node-admission','all'):
            corpus.extend(node_admission_cases(directory))
        if args.section=='private-type-source':
            corpus.extend(private_type_source_cases())
        if args.section=='declaration-name':
            corpus.extend(declaration_name_cases())
        if args.section=='public-reference':
            corpus.extend(public_reference_cases(directory))
        if args.section=='closing-delimiter':
            corpus.extend(closing_delimiter_cases(directory))
        validate_declared_requirement_spans(corpus)
        validate_primary_span_oracles(corpus)
        spec=specification(corpus)
        if args.output:
            args.output.mkdir(parents=True,exist_ok=True)
            (args.output/'specification.json').write_text(json.dumps(spec,indent=2,sort_keys=True)+'\n')
        outputs={}
        for case in corpus:
            check_raw(compiler,case)
            if not args.freeze_baseline:
                outputs[case.name],_=verify_json(compiler,case)
            positive_controls(compiler,case,directory,not args.freeze_baseline)
            print('repair-diagnostics',case.name,'baseline-confirmed' if args.freeze_baseline else 'passed',sep='\t',flush=True)
        if not args.freeze_baseline:
            verify_launcher(compiler,corpus,directory,outputs)
        if args.section in ('small','all'):
            mutation_control(compiler,directory,not args.freeze_baseline)
            row_limit_control(compiler,directory,not args.freeze_baseline)
        legacy=None
        if args.baseline_compiler:
            legacy=legacy_transport_control(args.baseline_compiler.resolve(),directory)
        if args.output:
            (args.output/'result.json').write_text(json.dumps(dict(schema=1,result='baseline-confirmed' if args.freeze_baseline else 'passed',
                cases=len(corpus),compiler_sha256=digest(compiler.read_bytes()),legacy_transport=legacy,
                specification_sha256=digest(json.dumps(spec,sort_keys=True).encode())),indent=2)+'\n')
        print(f'repair diagnostics: {len(corpus)} independent cases; '+('raw baseline frozen' if args.freeze_baseline else 'raw/native JSON/launcher and positive controls passed'),flush=True)


if __name__=='__main__':
    main()
