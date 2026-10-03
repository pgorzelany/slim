#!/usr/bin/env python3
"""Prospective RFC168 finite DATA oracle; no parser, checker, or native children.

Explicit --freeze only, into a fresh ignored directory. It materializes four
fixed snapshot byte inputs and complete expected report/diagnostic bytes. The
closure model is a least-set relation over declared literal edges, independent
of the proposed native marks/queue/linked-head representation. It never consumes
an arbitrary graph/catalog/capture or interprets SLIM/project syntax. The two
real source bundles use a fixed listed read set and exact manifest-byte pins.
Module bodies are bounded opaque bytes, hashed per fresh immutable campaign.
Successful compiler/producer acceptance remains unknown until real invocations.
"""
import argparse
from dataclasses import dataclass, replace
import hashlib
import json
from pathlib import Path


MAX_FILE = 1048576
MAX_SOURCE = 4194304
MAX_REPORT = 8388608
MAX_N = 4095
MAX_E = 65536
COUNTER_CAP = 1000000000
MAX_CASES = 128
MAX_FROZEN_BYTES = 64 * MAX_FILE
MAX_MODEL = 4 * MAX_FILE
STAGES = ('before-catalog', 'before-graph', 'after-catalog', 'after-graph')
NAME = frozenset(b'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-')
HEX = frozenset(b'0123456789abcdef')
H0 = b'0' * 64
H1 = b'1' * 64


def require(condition, reason):
    if not condition:
        raise ValueError(reason)


def repository_root():
    # Lazy discovery works from scripts/, ignored drafts, and held copies.
    # Import defines data/helpers only; neither root nor source reads occur.
    for parent in Path(__file__).resolve().parents:
        if (parent/'design/FEATURE_POLICY.md').is_file():
            return parent
    raise ValueError('repository feature-policy ancestor unavailable')


def sha(data):
    return hashlib.sha256(data).hexdigest()


def frame(data):
    require(type(data) is bytes and len(data) <= MAX_FILE, 'fixed payload admission')
    return str(len(data)).encode('ascii') + b':' + data + b','


@dataclass(frozen=True)
class Record:
    name: bytes
    weight: int = 0
    path: bytes = b'x.slim'
    digest: bytes = H0

    def fields(self):
        return self.name, str(self.weight).encode('ascii'), self.path + b'\0' + self.digest

    def encoded(self):
        return b''.join(frame(value) for value in self.fields())


@dataclass(frozen=True)
class Task:
    name: bytes
    cost: int = 0
    imports: tuple = ()

    def fields(self):
        return self.name, str(self.cost).encode('ascii'), b','.join(self.imports)

    def encoded(self):
        return b''.join(frame(value) for value in self.fields())


@dataclass(frozen=True)
class Snapshot:
    records: tuple
    tasks: tuple

    def inputs(self):
        return (b''.join(row.encoded() for row in self.records),
                b''.join(row.encoded() for row in self.tasks))


def project(weight=0, digest=H0):
    return Record(b'@project', weight, b'slim.project', digest)


def snapshot(names=(), imports=None, weights=None):
    imports, weights = imports or {}, weights or {}
    rows = tuple(Record(name, weights.get(name, 0)) for name in names)
    tasks = tuple(Task(name, weights.get(name, 0), tuple(imports.get(name, ()))) for name in names)
    return Snapshot((project(),) + rows, tasks)


def fixture_admitted(value):
    """Assertions on our fixed typed data, never a parser for supplied inputs."""
    cats = {row.name: row for row in value.records}
    tasks = {row.name: row for row in value.tasks}
    require(len(cats) == len(value.records) and len(tasks) == len(value.tasks), 'fixed duplicate data')
    require(len(tasks) <= MAX_N and len(cats) <= MAX_N + 1, 'fixed module admission')
    total = 0
    for row in sorted(value.records, key=lambda item: item.name):
        require(type(row.weight) is int and 0 <= row.weight <= MAX_FILE, 'fixed weight admission')
        require(1 <= len(row.path) <= 256 and b'\0' not in row.path, 'fixed path label admission')
        require(len(row.digest) == 64 and set(row.digest) <= HEX, 'fixed digest shape')
        if row.name == b'@project':
            require(row.path == b'slim.project', 'fixed reserved path')
        else:
            require(1 <= len(row.name) <= 64 and set(row.name) <= NAME, 'fixed name admission')
        total += row.weight
        require(total <= MAX_SOURCE, 'fixed aggregate admission')
    require(b'@project' in cats and b'@project' not in tasks, 'fixed reserved data')
    require(set(cats) - {b'@project'} == set(tasks), 'fixed key association')
    edges = 0
    for row in value.tasks:
        require(row.cost == cats[row.name].weight, 'fixed weight association')
        require(len(set(row.imports)) == len(row.imports), 'fixed repeated edge')
        require(row.name not in row.imports and set(row.imports) <= set(tasks), 'fixed target admission')
        edges += len(row.imports)
    require(edges <= MAX_E, 'fixed edge admission')
    require(all(len(data) <= MAX_FILE for data in value.inputs()), 'fixed input extent')


def least_closure(tasks, roots):
    # Asynchronous least fixed point over the mathematical owner/prerequisite
    # relation; no queue, native ordinals, links, indexes or source parser.
    names = {row.name for row in tasks}
    require(set(roots) <= names, 'fixed roots absent')
    reached = set(roots)
    visits = 0
    for _ in range(len(names) + 1):
        previous = frozenset(reached)
        for row in sorted(tasks, key=lambda item: item.name):
            visits += 1 + len(row.imports)
            require(visits <= COUNTER_CAP, 'finite independent closure work cap')
            if set(row.imports) & reached:
                reached.add(row.name)
        if previous == reached:
            return reached
    raise ValueError('fixed closure did not converge inside N+1 rounds')


def relation(before, after):
    fixture_admitted(before)
    fixture_admitted(after)
    old = {row.name: row for row in before.records}
    new = {row.name: row for row in after.records}
    og = {row.name: row for row in before.tasks}
    ng = {row.name: row for row in after.tasks}
    changed = sorted(name for name in set(old) | set(new)
                     if name != b'@project' and old.get(name) != new.get(name))
    kinds = {name: b'added' if name not in old else b'removed' if name not in new else b'modified'
             for name in changed}
    graph_changed = sorted(name for name in set(og) & set(ng)
                           if og[name].imports != ng[name].imports)
    old_roots = (set(changed) & set(og)) | set(graph_changed)
    new_roots = (set(changed) & set(ng)) | set(graph_changed)
    old_closed = least_closure(before.tasks, old_roots)
    new_closed = least_closure(after.tasks, new_roots)
    manifest = old[b'@project'] != new[b'@project']
    surviving_old = old_closed & set(ng)
    require(surviving_old <= new_closed, 'fixed old-closure survival preservation')
    union_selection = new_closed | surviving_old
    require(union_selection == new_closed, 'fixed union/new-closure equivalence')
    affected = set(ng) if manifest else union_selection
    result = {'manifest_changed': manifest, 'changed': changed, 'kinds': kinds,
              'graph_changed': graph_changed, 'old_roots': sorted(old_roots),
              'new_roots': sorted(new_roots), 'old_closure': sorted(old_closed),
              'new_closure': sorted(new_closed), 'affected': sorted(affected)}
    work = {}
    for side, tasks, closed in (('old', before.tasks, old_closed), ('new', after.tasks, new_closed)):
        count = sum(prerequisite in closed for row in tasks for prerequisite in row.imports)
        work[side + '_queue_pushes'] = len(closed)
        work[side + '_queue_pops'] = len(closed)
        work[side + '_edge_visits'] = count
        work[side + '_link_headers'] = count + len(closed)
    require(all(type(value) is int and 0 <= value <= COUNTER_CAP for value in work.values()), 'fixed work operands')
    return result, work


def report(before, after, value):
    old, new = ({row.name: row for row in source.records} for source in (before, after))
    og, ng = ({row.name: row for row in source.tasks} for source in (before, after))
    fields = [b'slim-project-impact-1', b'1' if value['manifest_changed'] else b'0',
              old[b'@project'].encoded(), new[b'@project'].encoded(),
              str(len(value['changed'])).encode('ascii')]
    for name in value['changed']:
        fields += [value['kinds'][name], name, old[name].encoded() if name in old else b'',
                   new[name].encoded() if name in new else b'']
    fields.append(str(len(value['graph_changed'])).encode('ascii'))
    for name in value['graph_changed']:
        fields += [name, b','.join(og[name].imports), b','.join(ng[name].imports)]
    for key in ('old_roots', 'new_roots', 'old_closure', 'new_closure'):
        fields += [str(len(value[key])).encode('ascii')] + value[key]
    fields.append(str(len(value['affected'])).encode('ascii'))
    fields += [new[name].encoded() for name in value['affected']]
    encoded = b''.join(frame(field) for field in fields)
    require(len(encoded) <= MAX_REPORT, 'fixed report admission')
    return encoded


@dataclass(frozen=True)
class Case:
    name: str
    inputs: tuple
    stdout: bytes
    status: int
    facts: dict
    work: dict
    family: str = 'fixed'


def positive(name, before, after=None, claims=None, family='fixed'):
    after = before if after is None else after
    value, work = relation(before, after)
    for key, expected in (claims or {}).items():
        require(value[key] == expected, 'literal claim mismatch: ' + name + '/' + key)
    facts = {key: item for key, item in value.items() if key != 'kinds'}
    facts['changed_catalog_records'] = [(value['kinds'][key], key) for key in value['changed']]
    facts['dimensions'] = [{'modules':len(item.tasks),'catalog_rows':len(item.records),
                            'edges':sum(len(row.imports) for row in item.tasks),
                            'supplied_source_weight':sum(row.weight for row in item.records),
                            'maximum_name_bytes':max((len(row.name) for row in item.tasks),default=0),
                            'maximum_path_bytes':max(len(row.path) for row in item.records),
                            'catalog_bytes':len(item.inputs()[0]),'graph_bytes':len(item.inputs()[1])}
                           for item in (before,after)]
    return Case(name, before.inputs() + after.inputs(), report(before, after, value), 0, facts, work, family)


def payload_position(rows, ordinal, field=0, offset=0):
    """Offsets from known serialized literals; no read/parse of arbitrary bytes."""
    require(0 <= ordinal < len(rows) and 0 <= field < 3, 'fixed offset selector')
    prefix = sum(len(row.encoded()) for row in rows[:ordinal])
    values = rows[ordinal].fields()
    prefix += sum(len(frame(value)) for value in values[:field])
    return prefix + len(str(len(values[field]))) + 1 + offset


def negative(name, base, slot, data, code, position, status=65, source_slot=None):
    inputs = list(base.inputs() + base.inputs())
    inputs[slot] = data
    source_slot = slot if source_slot is None else source_slot
    message = f'error {code} in {STAGES[source_slot]} at {position}\n'.encode('ascii')
    facts = {'reason': name, 'successful_prefix': False, 'input_slot': slot,
             'diagnostic': {'code': code, 'source_slot': source_slot,
                            'stage': STAGES[source_slot], 'position': position}}
    return Case(name, tuple(inputs), message, status, facts, {})


def change_record(value, name, **fields):
    return replace(value, records=tuple(replace(row, **fields) if row.name == name else row
                                        for row in value.records))


def small_cases():
    a, b, c, d, z = b'a', b'b', b'c', b'd', b'z'
    chain = snapshot((a, b, c, z), {b: (a,), c: (b,)})
    cases = [positive('unchanged-chain', chain, claims={'changed': [], 'affected': []}),
             positive('empty', snapshot()),
             positive('source-digest', chain, change_record(chain, a, digest=H1),
                      {'changed': [a], 'graph_changed': [], 'old_closure': [a,b,c], 'new_closure': [a,b,c], 'affected': [a,b,c]}),
             positive('source-path', chain, change_record(chain, b, path=b'new/b.slim'),
                      {'changed': [b], 'affected': [b,c]}),
             positive('manifest-only', chain, change_record(chain, b'@project', digest=H1),
                      {'manifest_changed': True, 'changed': [], 'old_closure': [], 'affected': [a,b,c,z]}),
             positive('manifest-only-empty', snapshot(), change_record(snapshot(), b'@project', digest=H1),
                      {'manifest_changed': True, 'affected': []})]
    weighted = snapshot((a,b,c,z), {b:(a,), c:(b,)}, {a:1})
    cases.append(positive('source-weight', chain, weighted, {'changed':[a], 'affected':[a,b,c]}))
    old = snapshot((a,b,c,z), {b:(a,), c:(b,)})
    new = snapshot((b,c,z), {c:(b,)})
    cases.append(positive('removed-root-old-mapping', old, new,
                         {'changed':[a], 'graph_changed':[b], 'old_roots':[a,b], 'new_roots':[b],
                          'old_closure':[a,b,c], 'new_closure':[b,c], 'affected':[b,c]}))
    cases.append(positive('added-root', new, old,
                         {'changed':[a], 'graph_changed':[b], 'old_closure':[b,c], 'new_closure':[a,b,c], 'affected':[a,b,c]}))
    renamed = snapshot((b,c,d,z), {b:(d,), c:(b,)})
    cases.append(positive('rename', old, renamed, {'changed':[a,d], 'graph_changed':[b], 'affected':[b,c,d]}))
    swapped = snapshot((a,b,c,z), {b:(a,), c:(a,)})
    cases.append(positive('graph-only-edge-owner', chain, swapped,
                         {'changed':[], 'graph_changed':[c], 'old_roots':[c], 'new_roots':[c], 'affected':[c]}))
    edge_before = snapshot((a,b,c,d,z), {c:(a,b), d:(c,)})
    edge_after = snapshot((a,b,c,d,z), {c:(b,a), d:(c,)})
    cases.append(positive('graph-only-list-order', edge_before, edge_after,
                         {'changed':[], 'graph_changed':[c], 'affected':[c,d]}))
    cycle = snapshot((a,b,c,z), {a:(c,), b:(a,), c:(b,)})
    cases.append(positive('nonreciprocal-cycle', cycle, change_record(cycle, a, digest=H1),
                         {'old_closure':[a,b,c], 'new_closure':[a,b,c], 'affected':[a,b,c]}))
    cases.append(positive('unchanged-cycle', cycle, claims={'affected':[]}))
    reciprocal = snapshot((a,b,z), {a:(b,), b:(a,)})
    cases.append(positive('reciprocal-two-node-cycle', reciprocal,
                         change_record(reciprocal,a,digest=H1),
                         {'old_closure':[a,b], 'new_closure':[a,b], 'affected':[a,b]}))
    diamond = snapshot((a,b,c,d,z), {b:(a,), c:(a,), d:(b,c)})
    cases.append(positive('shared-dependent-once', diamond, change_record(diamond,a,digest=H1),
                         {'affected':[a,b,c,d]}))
    shuffled = replace(chain, records=tuple(reversed(chain.records)), tasks=tuple(reversed(chain.tasks)))
    cases.append(positive('row-order-is-irrelevant', chain, shuffled, {'changed':[], 'graph_changed':[], 'affected':[]}))
    moved = snapshot((a,b,c,z), {a:(b,), c:(a,)})
    cases.append(positive('reoriented-edges-multiple-roots', chain, change_record(moved,b,digest=H1),
                         {'changed':[b], 'graph_changed':[a,b,c], 'old_closure':[a,b,c], 'new_closure':[a,b,c], 'affected':[a,b,c]}))
    # Valid binary path labels are data, including non-UTF8; no path parser.
    binary = change_record(chain, a, path=b'\xff/a.slim')
    cases.append(positive('binary-path-label', chain, binary, {'changed':[a], 'affected':[a,b,c]}))
    around_reserved = snapshot((b'0',b'-',b'A',b'a'),{b'0':(b'-',),b'A':(b'0',),b'a':(b'A',)})
    cases.append(positive('names-sort-around-project',around_reserved,change_record(around_reserved,b'-',digest=H1),
                         {'changed':[b'-'],'affected':[b'-',b'0',b'A',b'a']}))
    return cases


def geometry():
    cases = []
    for family in ('chain', 'fanout', 'cycle', 'shared', 'prefix64'):
        for n in (16,32,64,128,256):
            names = tuple((b'p' * 59 + f'{i:05d}'.encode()) if family == 'prefix64'
                          else f'm{i:04d}'.encode() for i in range(n))
            imports = {}
            for i, name in enumerate(names):
                if family == 'fanout':
                    deps = names[:1] if i else ()
                elif family == 'cycle':
                    deps = (names[i-1],)
                elif family == 'shared':
                    deps = names[:min(i,16)]
                else:
                    deps = (names[i-1],) if i else ()
                imports[name] = deps
            before = snapshot(names, imports)
            after = change_record(before, names[0], digest=H1)
            cases.append(positive(f'{family}-{n}', before, after,
                                  {'changed':[names[0]], 'graph_changed':[], 'affected':list(names),
                                   'old_closure':list(names), 'new_closure':list(names)}, family))
    return cases


def cap_cases():
    cases = []
    for width in (63,64):
        cases.append(positive(f'name-{width}', snapshot((b'n'*width,))))
    for width in (255,256):
        cases.append(positive(f'path-{width}', change_record(snapshot((b'a',)),b'a',path=b'p'*width)))
    for weight in (MAX_FILE-1, MAX_FILE):
        cases.append(positive(f'weight-{weight}', snapshot((b'a',), weights={b'a':weight})))
    names = (b'a',b'b',b'c',b'd')
    for total in (MAX_SOURCE-1, MAX_SOURCE):
        weights = {name:MAX_FILE for name in names}
        weights[b'd'] -= MAX_SOURCE-total
        cases.append(positive(f'aggregate-{total}', snapshot(names, weights=weights)))
    # First min(i,16) lower names yield 65384 edges. Add 152 to the last row.
    names = tuple(f'm{i:04d}'.encode() for i in range(MAX_N))
    imports = {name:names[:min(i,16)] for i,name in enumerate(names)}
    imports[names[-1]] += names[16:168]
    require(sum(map(len,imports.values())) == MAX_E, 'literal maximum edge formula')
    maximum = snapshot(names,imports)
    cases.append(positive('modules4095-edges65536',maximum,change_record(maximum,names[0],digest=H1),
                          {'affected':list(names),'old_closure':list(names),'new_closure':list(names)}))
    excessive = replace(maximum,tasks=maximum.tasks[:-1]+(replace(maximum.tasks[-1],imports=maximum.tasks[-1].imports+(names[168],)),))
    graph = excessive.inputs()[1]
    offset = len(b','.join(maximum.tasks[-1].imports)) + 1
    cases.append(negative('edges65537',maximum,1,graph,20,payload_position(excessive.tasks,MAX_N-1,2,offset)))
    extra = replace(maximum,records=maximum.records+(Record(b'm4095'),),tasks=maximum.tasks+(Task(b'm4095'),))
    excess = negative('modules4096',maximum,0,extra.inputs()[0],10,len(maximum.inputs()[0]))
    # Keep the excessive pair associated; the first catalog count gate wins.
    # A separate row observes the graph loader's primary task-count gate; its
    # unmatched last graph key is not reached and is not called admitted.
    cases.append(replace(excess,inputs=extra.inputs()+maximum.inputs()))
    cases.append(negative('graph-tasks4096-primary',maximum,1,extra.inputs()[1],10,len(maximum.inputs()[1])))
    # Exact catalog file cap: each path starts at width35, all value lengths
    # therefore remain three-digit; increasing a path adds exactly one byte.
    empty_edges = snapshot(names)
    records = [project()] + [Record(name,path=b'p'*35) for name in names]
    base = sum(len(row.encoded()) for row in records)
    whole,remainder = divmod(MAX_FILE-base,221)
    require(0 <= whole < MAX_N and 1 <= remainder <= 220, 'fixed catalog cap construction')
    for i in range(whole):
        records[i+1] = replace(records[i+1],path=b'p'*256)
    records[whole+1] = replace(records[whole+1],path=b'p'*(35+remainder))
    at_cap = replace(empty_edges,records=tuple(records))
    require(len(at_cap.inputs()[0]) == MAX_FILE,'literal catalog exact byte cap')
    below = change_record(at_cap,names[whole],path=b'p'*(34+remainder))
    above = change_record(at_cap,names[whole],path=b'p'*(36+remainder))
    require(len(below.inputs()[0]) == MAX_FILE-1 and len(above.inputs()[0]) == MAX_FILE+1, 'literal catalog adjacent caps')
    cases += [positive('catalog-bytes1048575',below),positive('catalog-bytes1048576',at_cap),
              negative('catalog-bytes1048577',at_cap,0,above.inputs()[0],10,0)]
    cases += graph_byte_caps()
    return cases


def graph_byte_caps():
    # Materialize a valid graph at exactly1MiB independently of native decoding.
    # 512 fixed 64-byte names provide >1MiB of available first32 lower edges.
    # Keep 35..100 spare bytes, then a single adjustable target name and the
    # first owner's canonical cost digit width fill that exact finite gap.
    names = tuple(b'p'*59+f'{i:05d}'.encode() for i in range(512))
    tasks = [Task(name) for name in names] + [Task(b'z')]
    total = sum(len(row.encoded()) for row in tasks)
    stopped = False
    for i in range(1,512):
        for prerequisite in names[:min(i,32)]:
            changed = replace(tasks[i],imports=tasks[i].imports+(prerequisite,))
            delta = len(changed.encoded())-len(tasks[i].encoded())
            if total+delta > MAX_FILE-35:
                stopped = True
                break
            total += delta
            tasks[i] = changed
        if stopped:
            break
    require(stopped and 35 <= MAX_FILE-total <= 101,'fixed graph exact-size reserve')
    adjusted = None
    for width in range(1,65):
        for digits_added in range(1,6):
            target = b'z'*width
            first = replace(tasks[0],cost=10**digits_added,imports=(target,))
            last = replace(tasks[-1],name=target)
            amount = total + len(first.encoded())-len(tasks[0].encoded()) + len(last.encoded())-len(tasks[-1].encoded())
            if amount == MAX_FILE:
                adjusted = (first,last,digits_added)
                break
        if adjusted is not None:
            break
    require(adjusted is not None,'fixed graph adjacent-byte construction')
    first,last,digits_added = adjusted
    tasks[0],tasks[-1] = first,last
    at_cap = Snapshot((project(),)+tuple(Record(row.name,row.cost) for row in tasks),tuple(tasks))
    def variant(power):
        cost = 10**power
        return replace(at_cap,records=(at_cap.records[0],replace(at_cap.records[1],weight=cost))+at_cap.records[2:],
                       tasks=(replace(at_cap.tasks[0],cost=cost),)+at_cap.tasks[1:])
    below,above = variant(digits_added-1),variant(digits_added+1)
    require(tuple(len(value.inputs()[1]) for value in (below,at_cap,above)) == (MAX_FILE-1,MAX_FILE,MAX_FILE+1), 'fixed graph adjacent byte extents')
    bad = negative('graph-bytes1048577',at_cap,1,above.inputs()[1],10,0)
    bad = replace(bad,inputs=above.inputs()+at_cap.inputs())
    return [positive('graph-bytes1048575',below),positive('graph-bytes1048576',at_cap),bad]


def bad_cases():
    base = snapshot((b'a',b'b'), {b'b':(b'a',)})
    cases = []
    for slot in range(4):
        cases.append(negative('missing-'+STAGES[slot],base,slot,None,1,0,66))
        cases.append(negative('malformed-'+STAGES[slot],base,slot,b':',102,0))
    # Each mutation is literal known bad data; expected diagnostics are declared,
    # not inferred through a second catalog/workplan parser.
    invalid_records = (
        ('name65',replace(base.records[1],name=b'n'*65)),
        ('name-invalid-ASCII',replace(base.records[1],name=b'a!')),
        ('path257',replace(base.records[1],path=b'p'*257)),
        ('path-empty',replace(base.records[1],path=b'')),
        ('path-NUL',replace(base.records[1],path=b'p\0q')),
        ('hash63',replace(base.records[1],digest=b'0'*63)),
        ('hash65',replace(base.records[1],digest=b'0'*65)),
        ('hash-uppercase',replace(base.records[1],digest=b'A'*64)),
        ('hash-nonhex',replace(base.records[1],digest=b'g'*64)),
        ('negative-weight',replace(base.records[1],weight=-1)),
        ('weight1048577',replace(base.records[1],weight=MAX_FILE+1)),
    )
    for name,row in invalid_records:
        bad = replace(base,records=(base.records[0],row,base.records[2]))
        cases.append(negative(name,base,0,bad.inputs()[0],40,payload_position(bad.records,1)))
    missing_project = replace(base,records=base.records[1:])
    cases.append(negative('missing-project',base,0,missing_project.inputs()[0],40,0))
    wrong_project = change_record(base,b'@project',path=b'other.project')
    cases.append(negative('project-path',base,0,wrong_project.inputs()[0],40,payload_position(wrong_project.records,0)))
    repeated = replace(base,records=base.records+(project(),))
    cases.append(negative('duplicate-project',base,0,repeated.inputs()[0],14,payload_position(repeated.records,3)))
    heavy = snapshot((b'a',b'b',b'c',b'd'),weights={name:MAX_FILE for name in (b'a',b'b',b'c',b'd')})
    heavy = change_record(heavy,b'@project',weight=1)
    cases.append(negative('aggregate4194305',base,0,heavy.inputs()[0],40,payload_position(heavy.records,4)))
    # Existing lexical rows are checked before reserved-row absence. Each
    # combined-invalid expectation is stated directly from known literal data.
    missing_bad_path = replace(missing_project,records=(replace(missing_project.records[0],path=b'p\0q'),missing_project.records[1]))
    cases.append(negative('shape-before-missing-project',base,0,missing_bad_path.inputs()[0],40,
                          payload_position(missing_bad_path.records,0)))
    missing_heavy = snapshot((b'a',b'b',b'c',b'd',b'e'),weights={name:MAX_FILE for name in (b'a',b'b',b'c',b'd')})
    missing_heavy = change_record(missing_heavy,b'e',weight=1)
    missing_heavy = replace(missing_heavy,records=missing_heavy.records[1:])
    cases.append(negative('sum-before-missing-project',base,0,missing_heavy.inputs()[0],40,
                          payload_position(missing_heavy.records,4)))
    missing = replace(base,tasks=base.tasks[:1])
    cases.append(negative('missing-graph-key',base,1,missing.inputs()[1],41,payload_position(base.records,2),source_slot=0))
    # Association positions belong to the unmatched key's actual source. The
    # missing graph key above therefore labels the before catalog, despite
    # detection only after successful before graph loading.
    extra = replace(base,tasks=base.tasks+(Task(b'c'),))
    cases.append(negative('extra-graph-key',base,1,extra.inputs()[1],41,payload_position(extra.tasks,2)))
    weight = replace(base,tasks=(replace(base.tasks[0],cost=1),base.tasks[1]))
    cases.append(negative('graph-weight',base,1,weight.inputs()[1],41,payload_position(weight.tasks,0)))
    weight_then_missing = replace(base,tasks=(replace(base.tasks[0],cost=1),))
    cases.append(negative('association-first-lexical-mismatch',base,1,weight_then_missing.inputs()[1],41,
                          payload_position(weight_then_missing.tasks,0)))
    # Mirror every primary new shape/association diagnostic, including the two
    # mixed reserved-absence priorities. The before pair is complete and valid;
    # both detection input and actual diagnostic source advance by two slots.
    mirrored_names = {name for name,_ in invalid_records} | {
        'missing-project','project-path','aggregate4194305',
        'shape-before-missing-project','sum-before-missing-project',
        'missing-graph-key','extra-graph-key','graph-weight',
        'association-first-lexical-mismatch'}
    originals = [case for case in cases if case.name in mirrored_names]
    require(len(originals) == 20, 'fixed after diagnostic mirror count')
    for original in originals:
        diagnostic = original.facts['diagnostic']
        slot = original.facts['input_slot'] + 2
        mirrored = negative('after-'+original.name,base,slot,original.inputs[slot-2],
                            diagnostic['code'],diagnostic['position'],original.status,
                            source_slot=diagnostic['source_slot']+2)
        cases.append(mirrored)
    reserved = replace(base,tasks=(Task(b'@project'),)+base.tasks)
    cases.append(negative('reserved-graph-name',base,1,reserved.inputs()[1],11,payload_position(reserved.tasks,0)))
    for label,deps,code,offset in (('unknown-edge',(b'z',),17,0),('self-edge',(b'b',),18,0),
                                   ('repeated-edge',(b'a',b'a'),19,2),('empty-edge',(b'a',b''),16,2)):
        bad = replace(base,tasks=(base.tasks[0],replace(base.tasks[1],imports=deps)))
        cases.append(negative(label,base,1,bad.inputs()[1],code,payload_position(bad.tasks,1,2,offset)))
    duplicate = replace(base,tasks=base.tasks+(Task(b'a'),))
    cases.append(negative('duplicate-graph-key',base,1,duplicate.inputs()[1],14,payload_position(duplicate.tasks,2)))
    priority = negative('four-file-priority',base,0,b':',102,0)
    cases.append(replace(priority,inputs=(b':',b':',b':',b':')))
    shape = replace(base,records=(base.records[0],replace(base.records[1],name=b'n'*65),base.records[2]))
    priority = negative('catalog-shape-before-graph-read',base,0,shape.inputs()[0],40,payload_position(shape.records,1))
    cases.append(replace(priority,inputs=(shape.inputs()[0],b':',*base.inputs())))
    priority = negative('after-catalog-shape-before-graph-read',base,2,shape.inputs()[0],40,payload_position(shape.records,1))
    cases.append(replace(priority,inputs=(*base.inputs(),shape.inputs()[0],b':')))
    priority = negative('association-before-after-catalog',base,1,missing.inputs()[1],41,payload_position(base.records,2),source_slot=0)
    cases.append(replace(priority,inputs=(*base.inputs()[:1],missing.inputs()[1],b':',base.inputs()[1])))
    return cases


# Fixed literal declarations, copied from inspected manifests; no manifest parser.
COMPILER = (
 ('analysis','analysis.slim','memory parallel quality ranges reduce syntax text typing'),
 ('cache','cache.slim','project syntax text'),
 ('check','check.slim','effects identity ir memory ranges retained syntax text typing validate'),
 ('codegen','codegen.slim','memory parallel ranges syntax text typing'),
 ('compiler','slimc.slim','analysis cache check codegen context edit equivalence format memory project proof reduce scheduler session syntax text typing validate'),
 ('context','context.slim','cache check identity project retained syntax text typing'),
 ('control','control.slim','syntax'), ('driver','driver.slim','compiler'),
 ('edit','edit.slim','format syntax text'), ('effects','effects.slim','syntax'),
 ('equivalence','equivalence.slim','syntax text'), ('flow','flow.slim','control identity retained syntax typing'),
 ('format','format.slim','syntax text'),
 ('fragments','fragments.slim','cache codegen identity memory parallel parallelcache project ranges retained syntax text typing'),
 ('identity','identity.slim',''), ('ir','ir.slim',''), ('memory','memory.slim','effects ir syntax'),
 ('nativebuild','nativebuild.slim','identity session'), ('nativecache','nativecache.slim','cache retained'),
 ('ownership','ownership.slim',''), ('parallel','parallel.slim','effects ranges syntax text typing'),
 ('parallelcache','parallelcache.slim','identity parallel project ranges retained syntax'),
 ('project','project.slim','check codegen format identity memory ranges retained scheduler syntax text typing validate'),
 ('proof','proof.slim','reduce syntax text'), ('quality','quality.slim','ranges reduce syntax text'),
 ('query','query.slim','identity project syntax text'), ('ranges','ranges.slim','effects syntax text typing'),
 ('reduce','reduce.slim','format syntax text'), ('retained','retained.slim','identity ir memory ranges syntax text typing'),
 ('scheduler','scheduler.slim','syntax'), ('session','session.slim','cache fragments identity parallelcache project query retained syntax'),
 ('syntax','syntax.slim','identity ir'), ('text','text.slim','syntax'),
 ('typing','typing.slim','control ir memory ownership syntax'), ('validate','validate.slim','syntax'),
)
CATALOG = (
 ('catalog_app','applications/catalog/main.slim','catalog_data catalog_diff_emit catalog_emit catalog_model catalog_reconcile std_byte_index std_bytes'),
 ('catalog_data','applications/catalog/catalog.slim','catalog_model framed_records std_byte_index std_decimal'),
 ('catalog_diff_emit','applications/catalog/diff_emit.slim','catalog_emit catalog_model catalog_reconcile std_netstring std_text'),
 ('catalog_emit','applications/catalog/emit.slim','catalog_model std_byte_index std_netstring std_text'),
 ('catalog_model','applications/catalog/model.slim','std_byte_index'),
 ('catalog_reconcile','applications/catalog/reconcile.slim','catalog_model std_byte_index std_bytes'),
 ('framed_records','components/records.slim','std_decimal std_netstring'),
 ('std_ascii','experimental/ascii.slim',''), ('std_byte_index','experimental/byte_index.slim',''),
 ('std_bytes','experimental/bytes.slim',''), ('std_decimal','experimental/decimal.slim','std_ascii'),
 ('std_netstring','experimental/netstring.slim','std_ascii std_bytes std_text'), ('std_text','experimental/text.slim','std_bytes'),
)
MANIFEST_PINS = {
    'selfhost/slim.project':'8f75c79f783c1942e1b1faa30867d34097ed86ee7b999cdf5153b58502dce9fe',
    'library/catalog.project':'28951ae70aaa93248fb5770ef3c8d0ec8649bd7f45456527136209c86195b16c',
}
SOURCE_PATHS = '''
selfhost/slim.project
selfhost/analysis.slim
selfhost/cache.slim
selfhost/check.slim
selfhost/codegen.slim
selfhost/context.slim
selfhost/control.slim
selfhost/driver.slim
selfhost/edit.slim
selfhost/effects.slim
selfhost/equivalence.slim
selfhost/flow.slim
selfhost/format.slim
selfhost/fragments.slim
selfhost/identity.slim
selfhost/ir.slim
selfhost/memory.slim
selfhost/nativebuild.slim
selfhost/nativecache.slim
selfhost/ownership.slim
selfhost/parallel.slim
selfhost/parallelcache.slim
selfhost/project.slim
selfhost/proof.slim
selfhost/quality.slim
selfhost/query.slim
selfhost/ranges.slim
selfhost/reduce.slim
selfhost/retained.slim
selfhost/scheduler.slim
selfhost/session.slim
selfhost/slimc.slim
selfhost/syntax.slim
selfhost/text.slim
selfhost/typing.slim
selfhost/validate.slim
library/catalog.project
library/applications/catalog/catalog.slim
library/applications/catalog/diff_emit.slim
library/applications/catalog/emit.slim
library/applications/catalog/main.slim
library/applications/catalog/model.slim
library/applications/catalog/reconcile.slim
library/components/records.slim
library/experimental/ascii.slim
library/experimental/byte_index.slim
library/experimental/bytes.slim
library/experimental/decimal.slim
library/experimental/netstring.slim
library/experimental/text.slim
'''


def fixed_sources(root):
    rows = tuple(line for line in SOURCE_PATHS.splitlines() if line)
    declared = set(MANIFEST_PINS) | {'selfhost/'+path for _,path,_ in COMPILER} | {'library/'+path for _,path,_ in CATALOG}
    require(len(rows) == 50 and len(set(rows)) == 50 and set(rows) == declared,'fixed real read set')
    sources = {}
    for name in rows:
        path = root / name
        require(not path.is_symlink() and path.is_file(),'fixed real source path')
        with path.open('rb') as handle:
            data = handle.read(MAX_FILE+1)
        require(len(data) <= MAX_FILE,'fixed real source byte bound: '+name)
        if name in MANIFEST_PINS:
            require(sha(data) == MANIFEST_PINS[name],'fixed manifest byte drift: '+name)
        sources[name] = data
    return sources


def real_cases(sources):
    cases, bundles = [], []
    for label, directory, manifest, declarations, edited in (
        ('compiler','selfhost','slim.project',COMPILER,'project'),
        ('catalog','library','catalog.project',CATALOG,'catalog_reconcile'),
    ):
        require(tuple(name for name,_,_ in declarations) == tuple(sorted(name for name,_,_ in declarations)), 'fixed manifest declaration order')
        manifest_bytes = sources[directory+'/'+manifest]
        old_records, new_records = [project(len(manifest_bytes),sha(manifest_bytes).encode())], [project(len(manifest_bytes),sha(manifest_bytes).encode())]
        old_tasks, new_tasks, old_files, new_files = [], [], {'slim.project':manifest_bytes}, {'slim.project':manifest_bytes}
        for name,path,imports in declarations:
            data = sources[directory+'/'+path]
            edited_data = data+b'\n' if name == edited else data
            deps = tuple(value.encode() for value in imports.split())
            require(deps == tuple(sorted(deps)),'fixed manifest import order')
            key,relative = name.encode(),path.encode()
            old_records.append(Record(key,len(data),relative,sha(data).encode()))
            new_records.append(Record(key,len(edited_data),relative,sha(edited_data).encode()))
            old_tasks.append(Task(key,len(data),deps))
            new_tasks.append(Task(key,len(edited_data),deps))
            old_files[path],new_files[path] = data,edited_data
        before,after = Snapshot(tuple(old_records),tuple(old_tasks)),Snapshot(tuple(new_records),tuple(new_tasks))
        cases.append(positive('real-'+label+'-one-newline',before,after,
                              {'manifest_changed':False,'changed':[edited.encode()],'graph_changed':[]},'real'))
        bundles.append((label,old_files,new_files))
    return cases,bundles


def json_data(value):
    if isinstance(value,bytes):
        return {'hex':value.hex()}
    if isinstance(value,dict):
        return {key:json_data(item) for key,item in value.items()}
    if isinstance(value,(tuple,list)):
        return [json_data(item) for item in value]
    return value


def freeze(destination):
    root = repository_root()
    destination = destination.resolve()
    allowed = (root/'build/overnight-project-impact').resolve()
    require(destination.is_relative_to(allowed) and destination != allowed and not destination.exists(), 'fresh ignored output')
    source = Path(__file__).read_bytes()
    sources = fixed_sources(root)
    real,bundles = real_cases(sources)
    cases = small_cases()+geometry()+cap_cases()+bad_cases()+real
    require(len(cases) <= MAX_CASES and len({case.name for case in cases}) == len(cases),'fixed campaign case cap')
    argument_stdout = b'error 2 in arguments at 0\n'
    argument_controls = [{'name':'input-path-count-'+str(count),'input_path_count':count,
                          'native_status':64,'stdout_bytes':len(argument_stdout),
                          'stdout_sha256':sha(argument_stdout),'stdout_artifact':'controls/arguments/stdout',
                          'stderr_bytes':0,'stderr_sha256':sha(b'')}
                         for count in (0,3,5)]
    require(len(cases)+len(argument_controls) <= MAX_CASES,'fixed CLI case/argument cap')
    artifacts = {}
    def retain(name,data):
        require(type(data) is bytes and name not in artifacts,'fixed artifact identity')
        artifacts[name] = data
    retain('controls/arguments/stdout',argument_stdout)
    retain('controls/report-limit/stdout-invalid',b'error 42 in impact at 0\n')
    for case in cases:
        for label,data in zip(STAGES,case.inputs):
            if data is not None:
                require(len(data) <= MAX_FILE+1,'fixed excess exactly one byte/file cap')
                retain('cases/'+case.name+'/'+label,data)
        retain('cases/'+case.name+'/stdout',case.stdout)
        retain('cases/'+case.name+'/stderr',b'')
    for label,old,new in bundles:
        for side,files in (('old',old),('new',new)):
            for path,data in files.items():
                retain('real/'+label+'/'+side+'/'+path,data)
    # Helper output-limit crossing belongs to preparation, not four-file CLI.
    selected = next(case for case in cases if case.name == 'source-digest')
    amount = len(selected.stdout)
    helper = [{'name':'report-size-'+str(delta),'output_limit':amount+delta,
               'status':'invalid' if delta < 0 else 'ready','code':42 if delta < 0 else 0}
              for delta in (-1,0,1)]
    helper += [{'name':'limit-'+str(limit),'output_limit':limit,
                'status':'invalid' if limit < amount or limit > MAX_REPORT else 'ready',
                'code':42 if limit < amount or limit > MAX_REPORT else 0}
               for limit in (-1,0,MAX_REPORT-1,MAX_REPORT,MAX_REPORT+1)]
    for control in helper:
        expected = b'error 42 in impact at 0\n' if control['status'] == 'invalid' else selected.stdout
        control.update(input_case=selected.name,native_status=65 if control['status'] == 'invalid' else 0,
                       stdout_bytes=len(expected),stdout_sha256=sha(expected),stderr_bytes=0,stderr_sha256=sha(b''),
                       stdout_artifact='controls/report-limit/stdout-invalid' if control['status'] == 'invalid'
                       else 'cases/'+selected.name+'/stdout')
    fifo_controls = [
        {'name':'catalog-loader-before-graph','input_case':'malformed-before-catalog','fifo_slot':1},
        {'name':'catalog-shape-before-graph','input_case':'catalog-shape-before-graph-read','fifo_slot':1},
        {'name':'graph-loader-before-next-catalog','input_case':'malformed-before-graph','fifo_slot':2},
        {'name':'graph-association-before-next-catalog','input_case':'missing-graph-key','fifo_slot':2},
        {'name':'new-catalog-before-new-graph','input_case':'malformed-after-catalog','fifo_slot':3},
        {'name':'new-catalog-shape-before-new-graph','input_case':'after-catalog-shape-before-graph-read','fifo_slot':3},
    ]
    for control in fifo_controls:
        expected = next(case for case in cases if case.name == control['input_case'])
        control.update(native_status=expected.status,stdout_bytes=len(expected.stdout),stdout_sha256=sha(expected.stdout),
                       stderr_bytes=0,stderr_sha256=sha(b''),no_fifo_writer=True,
                       timeout_is_failure=True,scope='fixed harness FIFO replaces the next input, preserving all prior bytes')
    arithmetic = [
        {'operation':'operand','value':-1,'admitted':False},
        {'operation':'operand','value':COUNTER_CAP-1,'admitted':True},
        {'operation':'operand','value':COUNTER_CAP,'admitted':True},
        {'operation':'operand','value':COUNTER_CAP+1,'admitted':False},
        {'operation':'operand','value':18446744073709551615,'admitted':False},
        {'operation':'operand','value':True,'admitted':False},
        {'operation':'increment','value':COUNTER_CAP-1,'admitted':True},
        {'operation':'increment','value':COUNTER_CAP,'admitted':False},
        {'operation':'sum','operands':[COUNTER_CAP-1,1],'admitted':True},
        {'operation':'sum','operands':[COUNTER_CAP,1],'admitted':False},
        {'operation':'product','operands':[COUNTER_CAP,1],'admitted':True},
        {'operation':'product','operands':[COUNTER_CAP,2],'admitted':False},
    ]
    model = {'schema':1,'rfc':168,'status':'prospective-data-only',
             'source_sha256':sha(source),'case_count':len(cases),
             'cli_case_count':len(cases)+len(argument_controls),'argument_controls':argument_controls,
             'bounds':{'module':MAX_N,'edges':MAX_E,'file_bytes':MAX_FILE,'source_bytes':MAX_SOURCE,
                       'report_bytes':MAX_REPORT,'counter':COUNTER_CAP,'case_count':MAX_CASES,
                       'materialized_bytes':MAX_FROZEN_BYTES,'model_bytes':MAX_MODEL},
             'cases':[{'name':case.name,'family':case.family,'status':case.status,'facts':json_data(case.facts),
                       'work':case.work,'missing_files':[label for label,data in zip(STAGES,case.inputs) if data is None],
                       'inputs':[{ 'bytes':len(data),'sha256':sha(data)} if data is not None else None for data in case.inputs],
                       'stdout':{'bytes':len(case.stdout),'sha256':sha(case.stdout)},'stderr':{'bytes':0,'sha256':sha(b'')}} for case in cases],
             'helper_output_controls':helper,'fifo_precedence_controls':fifo_controls,'counter_arithmetic_controls':arithmetic,
             'manifest_pins':MANIFEST_PINS,
             'module_body_binding':'bounded opaque current bytes per fresh freeze; held campaign immutable',
             'source_pins':{name:sha(data) for name,data in sources.items()},
             'unknown':{'compiler_source_acceptance':'requires matching actual trusted producer/checker invocations',
                        'digest_source_equality':'supplied digest equality does not prove byte equality',
                        'parsed_node_count':'not measured; no 1000000-node crossing inferred',
                        'default_report_cap_crossing':'reduced helper output controls do not cross the whole 8MiB cap',
                        'semantic_recheck':'declared-edge candidates confer no compiler incremental authority',
                        'native_work_and_resources':'prospective only; none executed by this oracle'}}
    encoded = (json.dumps(model,sort_keys=True,indent=2)+'\n').encode()
    require(len(encoded) <= MAX_MODEL,'fixed model byte cap')
    retain('model.json',encoded)
    retain('oracle.py',source)
    require(sum(map(len,artifacts.values())) <= MAX_FROZEN_BYTES,'fixed finite materialized byte cap')
    # Check the complete prospective artifact set BEFORE publishing anything.
    require(Path(__file__).read_bytes() == source and fixed_sources(root) == sources,'fixed source drift before publication')
    destination.mkdir(parents=True,exist_ok=False)
    for name,data in sorted(artifacts.items()):
        path = destination/name
        path.parent.mkdir(parents=True,exist_ok=True)
        path.write_bytes(data)
        require(path.read_bytes() == data,'fixed artifact readback')
    require(Path(__file__).read_bytes() == source and fixed_sources(root) == sources,'fixed source drift after publication')
    receipt = {'schema':1,'status':'frozen-data-only','oracle_sha256':sha(source),
               'model_sha256':sha(encoded),'files':{name:{'bytes':len(data),'sha256':sha(data)} for name,data in sorted(artifacts.items())}}
    (destination/'freeze.json').write_text(json.dumps(receipt,sort_keys=True,indent=2)+'\n')
    return receipt


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--freeze',type=Path,required=True)
    args = parser.parse_args()
    receipt = freeze(args.freeze)
    print('FROZEN DATA ONLY '+receipt['model_sha256'])


if __name__ == '__main__':
    main()
