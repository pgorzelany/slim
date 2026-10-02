"""Frozen independent finite task specifications; not participant material.

Expected behavior comes from these small arithmetic/list models, never from the
reference candidate or participant tests. Generated clients use ordinary SLIM.
No compiler semantics, source checker or evaluator for SLIM is implemented here.
"""
import re


APPLICATION_STDOUT = {
    'effects-report': b'3,13,-2,9\n',
    'buffer-drain': b'17\n-1\n',
    'range-api': b'25\n',
}

INTERFACES = {
    'effects-report': '''(interface 3 app)
(interface 3 metrics (record Summary ((count I64) (total I64) (minimum I64) (maximum I64))) (fn summarize ((shared (Vec I64))) metrics.Summary (effects partial)))
(interface 3 render (fn render ((shared metrics.Summary)) Void (effects io)))
(interface 3 report (fn send ((shared (Vec I64))) Void (effects io partial)))
'''.encode(),
    'buffer-drain': '''(interface 3 app)
(interface 3 archive (fn checksum ((owned (Vec I64))) I64 (effects partial)))
(interface 3 pipeline (fn round ((exclusive storage.Buffer) (copy I64)) (Vec I64) (effects alloc partial)))
(interface 3 storage (record Buffer ((values (Vec I64)))) (fn append ((exclusive storage.Buffer) (copy I64)) I64 (effects alloc)) (fn drain ((exclusive storage.Buffer)) (Vec I64) (effects alloc)) (fn make ((copy I64) (copy I64)) storage.Buffer (effects alloc)))
'''.encode(),
    'range-api': '''(interface 3 app)
(interface 3 gateway (fn score ((shared (Vec I64)) (shared rules.Limits)) I64 (effects partial)))
(interface 3 rules (variant Decision ((Below) (Inside I64) (Above))) (record Limits ((lower I64) (upper I64))) (fn classify ((copy I64) (shared rules.Limits)) rules.Decision (effects)))
(interface 3 stats (record Summary ((accepted I64) (total I64) (rejected I64))) (fn summarize ((shared (Vec I64)) (shared rules.Limits)) stats.Summary (effects partial)))
'''.encode(),
}


def emit_numbers(expressions):
    lines = []
    for index, expression in enumerate(expressions):
        if index:
            lines.append('  io.print_bytes(",")')
        lines.append(f'  io.print_i64({expression})')
    lines.append('  io.println("")')
    return lines


def vector(name, values):
    return [f'  let {name}: Vec[I64] = vec.new()'] + [
        f'  vec.push(@{name}, {value})' for value in values]


def line(values):
    return ','.join(map(str, values)) + '\n'


def main(lines):
    return ('module app\n\nfn main(args: Vec[Bytes]) -> I64 '
            'effects[alloc, io, partial]:\n' + '\n'.join(lines + ['  0']) + '\n').encode()


def effects_client():
    cases = [[], [0], [-7], [6,-2,9], [100,-100,100,-100],
             [-9,-4,-12], [100 if index % 2 else -100 for index in range(32)]]
    lines, expected = [], ''
    for index, values in enumerate(cases):
        name = f'values_{index}'
        lines += vector(name, values)
        lines += [f'  let summary_{index}: metrics.Summary = metrics.summarize({name})',
                  f'  render.render(summary_{index})', f'  report.send({name})']
        lines += emit_numbers([f'vec.len({name})'] + [f'vec.get({name}, {i})' for i in range(len(values))])
        summary = [len(values), sum(values), min(values) if values else 0, max(values) if values else 0]
        expected += line(summary) * 2 + line([len(values), *values])
    return main(lines), expected.encode(), {'vectors':len(cases), 'elements':sum(map(len,cases)),
                                          'bounded_domain':'seven fixed vectors; at most 32 elements in [-100,100]'}


def buffer_client():
    cases = [(5,7,3,-1), (0,0,0,0), (-100,100,7,-3),
             (-6,9,-10,12), (100,100,-100,100), (3,-2,5,-5)]
    lines, expected = [], ''
    for index, (first, second, x, y) in enumerate(cases):
        name = f'buffer_{index}'
        lines.append(f'  let {name}: storage.Buffer = storage.make({first}, {second})')
        for round_index, value in enumerate([x,y]):
            batch = f'batch_{index}_{round_index}'
            contents = [first,second,2+value] if round_index == 0 else [value]
            lines += [f'  let {batch}: Vec[I64] = pipeline.round(@{name}, {value})']
            lines += emit_numbers([f'vec.len({batch})'] + [f'vec.get({batch}, {i})' for i in range(len(contents))] + [f'vec.len({name}.values)'])
            lines += emit_numbers([f'archive.checksum(^{batch})'])
            expected += line([len(contents), *contents, 0]) + line([sum(contents)])
        lines += [f'  let appended_{index}: I64 = storage.append(@{name}, 42)',
                  f'  let direct_{index}: Vec[I64] = storage.drain(@{name})']
        lines += emit_numbers([f'appended_{index}', f'vec.len(direct_{index})', f'vec.get(direct_{index}, 0)', f'vec.len({name}.values)'])
        lines += emit_numbers([f'archive.checksum(^direct_{index})'])
        expected += '1,1,42,0\n42\n'
    return main(lines), expected.encode(), {'sequences':len(cases),'rounds':len(cases)*2,
                                          'bounded_domain':'six fixed two-round sequences plus direct append/drain; values in [-100,100]'}


def range_client():
    cases = [([],0,0), ([0],0,0), ([-1,0,1],0,0),
             ([-4,0,5,11],0,5), ([-100,-5,0,5,100],-5,5),
             ([2,3,4,5],5,2), ([100 if i % 2 else -100 for i in range(32)],-100,100)]
    lines, expected = [], ''
    for index, (values, lower, upper) in enumerate(cases):
        name = f'values_{index}'
        lines += vector(name, values)
        lines += [f'  let limits_{index}: rules.Limits = rules.Limits(lower: {lower}, upper: {upper})',
                  f'  let summary_{index}: stats.Summary = stats.summarize({name}, limits_{index})']
        lines += emit_numbers([f'summary_{index}.accepted',f'summary_{index}.total',f'summary_{index}.rejected',f'gateway.score({name}, limits_{index})',f'vec.len({name})'])
        accepted = [v for v in values if not v < lower and not v > upper]
        expected += line([len(accepted),sum(accepted),len(values)-len(accepted),len(accepted)*10+sum(accepted),len(values)])
    # Exercise the leaf API directly, including payloads and reversed limits.
    for index, (value, lower, upper) in enumerate([(-1,0,0),(0,0,0),(1,0,0),(3,5,2),(6,5,2)]):
        lines += [f'  let choice_{index}: rules.Decision = rules.classify({value}, rules.Limits(lower: {lower}, upper: {upper}))',
                  f'  let tag_{index}: I64 = match choice_{index}:',
                  '    Below:', '      -1000', '    Inside(payload):', '      payload',
                  '    Above:', '      1000']
        lines += emit_numbers([f'tag_{index}'])
        expected += line([-1000 if value < lower else 1000 if value > upper else value])
    return main(lines), expected.encode(), {'vectors':len(cases),'classifications':5,
                                          'bounded_domain':'seven fixed vectors and five leaf decisions; ordered checks include reversed limits'}


CLIENTS = {'effects-report':effects_client,'buffer-drain':buffer_client,'range-api':range_client}

MISUSE = {
    'effects-report': [('missing-io', '''module app

fn forbidden(values: Vec[I64]) -> Void effects[partial]:
  report.send(values)

fn main(args: Vec[Bytes]) -> I64:
  0
''', 'E0343')],
    'buffer-drain': [('shared-drain', '''module app

fn forbidden(buffer: storage.Buffer) -> Vec[I64] effects[alloc]:
  storage.drain(@buffer)

fn main(args: Vec[Bytes]) -> I64:
  0
''', 'E0347'), ('after-owned-checksum', '''module app

fn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:
  let values: Vec[I64] = vec.new()
  let total: I64 = archive.checksum(^values)
  vec.len(values)
''', 'E0315')],
    'range-api': [('missing-partial', '''module app

fn forbidden(values: Vec[I64], limits: rules.Limits) -> stats.Summary:
  stats.summarize(values, limits)

fn main(args: Vec[Bytes]) -> I64:
  0
''', 'E0343')],
}


def misuse_clients(task):
    for name,source,code in MISUSE[task]:
        if name == 'missing-io':
            positive = source.replace('effects[partial]:','effects[io, partial]:')
            needle,offset = 'report.send',0
        elif name == 'missing-partial':
            positive = source.replace('-> stats.Summary:', '-> stats.Summary effects[partial]:')
            needle,offset = 'stats.summarize',0
        elif name == 'shared-drain':
            positive = source.replace('buffer: storage.Buffer','buffer: @storage.Buffer')
            needle,offset = '@buffer',1
        else:
            positive = source.replace('  vec.len(values)\n','  total\n')
            needle,offset = 'values)',0
        start = source.rindex(needle)+offset
        length = len(needle)-offset-(1 if name=='after-owned-checksum' else 0)
        yield {'name':name,'source':source,'positive':positive,'code':code,
               'span':[start,start+length],
               'domain':'one targeted ownership/effect misuse against an independently accepted positive client'}


def code_only(source):
    # A lexical guard only: mask comments and byte strings, retain newlines.
    # The normal compiler supplies parsing and every semantic judgment.
    result, quoted, escaped, comment = [], False, False, False
    for character in source.decode('latin1'):
        if character == '\n':
            result.append(character); comment = False; escaped = False
        elif comment:
            result.append(' ')
        elif quoted:
            result.append(' ')
            if escaped: escaped = False
            elif character == '\\': escaped = True
            elif character == '"': quoted = False
        elif character == '#':
            comment = True; result.append(' ')
        elif character == '"':
            quoted = True; result.append(' ')
        else:
            result.append(character)
    return ''.join(result)


def body(source, name):
    match = re.search(r'^fn '+re.escape(name)+r'\([^\n]*\n((?:[ \t].*\n|\n)*)', code_only(source), re.M)
    return match.group(1) if match else ''


def source_constraints(task, files):
    failures = []
    if task == 'effects-report':
        report = body(files['report.slim'],'send')
        calls = re.findall(r'([A-Za-z_][A-Za-z_0-9.]*)\s*\(',report)
        if calls != ['metrics.summarize','render.render'] and calls != ['render.render','metrics.summarize']:
            failures.append('send-requires-one-summarize-and-one-render-source-call')
        if re.search(r'\b(if|else|match|recur|parallel)\b',report):
            failures.append('send-requires-straight-line-composition')
    if task == 'range-api':
        if re.search(r'^fn contains\(',code_only(files['rules.slim']),re.M):
            failures.append('removed-contains-function-retained')
        stats = code_only(files['stats.slim'])
        if not re.search(r'rules\.classify\s*\(',stats) or not re.search(r'\bmatch\b',stats):
            failures.append('stats-requires-classifier-and-match-source-forms')
        if re.search(r'\.(lower|upper)\b',stats):
            failures.append('stats-must-use-classifier-rather-than-limit-fields')
    if task == 'buffer-drain':
        # This is an explicit source-form task constraint, not a SLIM semantic
        # claim. The normal checker and runtime client separately verify meaning.
        if not re.search(r'\bmem\.replace\s*\(\s*@buffer\.values\s*,',body(files['storage.slim'],'drain')):
            failures.append('drain-missing-required-canonical-field-replacement')
    return failures
