#!/usr/bin/env python3
"""Prospective RFC172 finite literal data. Import performs no IO or generation.

No decoder, SLIM/manifest parser, subprocess, candidate import or reference
checker exists. Expected outcomes are assigned to fixed constructor cards.
Only an explicitly authorized later freezer may call campaign()/real_bundle().
"""
import hashlib

MIB = 1048576
GLOBAL_SECONDS = 900
CHILD_SECONDS = 60
CASE_CAP = 64
GEOMETRY_CASES = 4
REAL_CASES = 3
TOTAL_CASE_CAP = 72
FILE_CAP = 192
MODEL_CAP = 4*MIB
DATA_CAP = 64*MIB
INPUT_CAP = 8*MIB
OUTPUT_CAP = 8*MIB
INPUT_TAG = b'slim-project-input-1'
OUTPUT_TAG = b'slim-source-context-1'


def frame(payload):
    return str(len(payload)).encode('ascii') + b':' + payload + b','


def output(records):
    return frame(OUTPUT_TAG) + frame(str(len(records)).encode('ascii')) + b''.join(
        frame(field) for record in records for field in record)


def transport(records=(), *, manifest=b'', graph=b'', edges=b'0', modules=None,
              tag=INPUT_TAG):
    """Construct known bytes and offsets; do not decode or validate bytes."""
    count = str(len(records)).encode('ascii') if modules is None else modules
    fields = (tag, count, edges, manifest, graph) + tuple(
        field for record in records for field in record)
    cursor = 0
    chunks = []
    positions = []
    for payload in fields:
        encoded = frame(payload)
        positions.append({'frame':cursor,
                          'payload':cursor+len(str(len(payload)))+1,
                          'next':cursor+len(encoded)})
        chunks.append(encoded)
        cursor += len(encoded)
    return b''.join(chunks), tuple(positions)


def diagnostic(code, stage, position):
    return f'error {code} in {stage} at {position}\n'.encode('ascii')


def card(label, value, queries=(), *, selected=(), status=0, code=None,
         stage='capture', position=0, capture='input', reason):
    expected = output(selected) if status == 0 else diagnostic(code,stage,position)
    return {'label':label, 'input':value, 'queries':tuple(queries),
            'capture_argument':capture, 'status':status,
            'stdout':expected, 'stderr':b'', 'reason':reason,
            'authority':'literal transport/selection data; checked source unknown'}


def campaign():
    cases = []
    def add(label,value,queries=(),**kw):
        cases.append(card(label,value,queries,**kw))
    def bad(label,value,queries=(), *, code,position=0,stage='capture',
            status=65,capture='input',reason):
        add(label,value,queries,status=status,code=code,stage=stage,
            position=position,capture=capture,reason=reason)

    zero, _ = transport()
    a = (b'a',b'a.slim',b'module a\n')
    b = (b'b',b'b.slim',b'body b\n')
    z = (b'z',b'z.slim',b'body z\n')
    single, _ = transport((a,))
    add('C01',zero,reason='N=Q=0, empty opaque manifest/graph')
    add('C02',single,reason='Q=0 still validates a complete nonempty capture')
    unordered,_ = transport((z,a,b))
    add('C03',unordered,(b'a',b'b',b'z'),selected=(a,b,z),
        reason='arbitrary physical record order; lexical query order')
    opaque_a = (b'a',b'poison.fifo',b'\x00\xff\n,\x80')
    opaque_hidden = (b'\x00secret',b'\x00\xff',b'not SLIM\x00')
    opaque_z = (b'z',b'../never-open',b'\xff\x00')
    opaque,_ = transport((opaque_z,opaque_hidden,opaque_a),
                         manifest=b'\x00manifest',graph=b'not a graph\xff')
    add('C04',opaque,(b'a',b'z'),selected=(opaque_a,opaque_z),
        reason='opaque paths/bodies/NUL key; embedded poison FIFO never read')
    binary = (b'\xff',b'',b'\x80\x00')
    binary_input,_ = transport((binary,))
    add('C05',binary_input,(b'\xff',),selected=(binary,),
        reason='non-UTF8 key selected by exact POSIX argv bytes')
    only_b,_ = transport((b,))
    bad('C06',only_b,(b'a',),code=40,stage='queries',position=0,
        reason='first requested key absent')
    bad('C07',single,(b'a',b'z'),code=40,stage='queries',position=1,
        reason='later missing key prevents any successful prefix')
    for label,queries,reason in (
        ('C08',(b'a',b'a'),'duplicate queries'),
        ('C09',(b'z',b'a'),'descending queries'),
        ('C10',(b'',),'empty query'),
        ('C11',(b'x'*65,),'query width65')):
        bad(label,single,queries,code=2,status=64,stage='arguments',reason=reason)
    queries4096 = tuple(f'm{i:04d}'.encode('ascii') for i in range(4096))
    bad('C12',None,queries4096,code=2,status=64,stage='arguments',
        capture='missing',reason='Q4096 before opening missing capture')
    bad('C13',None,code=2,status=64,stage='arguments',capture='absent',
        reason='required capture argument missing')
    bad('C14',None,code=1,status=66,capture='missing',
        reason='capture file read failure with Q0')
    wrong,positions = transport(tag=b'wrong')
    bad('C15',wrong,code=30,position=positions[0]['payload'],
        reason='wrong identity after complete first frame')
    for label,value,is_edge,code,reason in (
        ('C16',b'-1',False,31,'negative module counter'),
        ('C17',b'00',False,31,'leading-zero module counter'),
        ('C18',b'4096',False,32,'module count4096 single excess'),
        ('C19',b'-1',True,31,'negative edge counter'),
        ('C20',b'00',True,31,'leading-zero edge counter'),
        ('C21',b'65537',True,32,'edge count65537 single excess')):
        raw,p = transport(edges=value) if is_edge else transport(modules=value)
        bad(label,raw,code=code,position=p[2 if is_edge else 1]['payload'],
            reason=reason)
    max_edges,_ = transport(edges=b'65536')
    add('C22',max_edges,reason='edge counter65536 with opaque empty graph, N0')
    missing,_ = transport(modules=b'1')
    bad('C23',missing,code=8,position=len(missing),
        reason='declared first name frame absent')
    bad('C24',zero+b'X',code=36,position=len(zero),reason='trailing byte')
    bad('C25',b'x',code=2,position=0,reason='invalid first frame digit')
    bad('C26',b'1',code=3,position=1,reason='frame header ends before colon')
    bad('C27',b'00:,',code=4,position=1,reason='frame length leading zero')
    bad('C28',b'3:ab',code=6,position=4,reason='truncated payload')
    bad('C29',b'0:',code=7,position=2,reason='missing comma at exact end')
    empty_name,p = transport(((b'',b'',b''),))
    bad('C30',empty_name,code=33,position=p[5]['payload'],reason='empty name')
    wide_name,p = transport(((b'x'*65,b'',b''),))
    bad('C31',wide_name,code=5,position=p[5]['frame']+1,
        reason='name frame65 exceeds64 on second header digit')
    wide_path,p = transport(((b'a',b'x'*257,b''),))
    bad('C32',wide_path,code=5,position=p[6]['frame']+2,
        reason='path frame257 exceeds256 on third header digit')
    duplicates,p = transport((z,a,z,a))
    bad('C33',duplicates,code=34,position=p[14]['payload'],
        reason='lexical first duplicated a, second physical occurrence')
    late_missing,_ = transport((a,a),modules=b'3')
    late_missing += frame(b'b')
    bad('C34',late_missing,code=8,position=len(late_missing),
        reason='later missing path precedes earlier duplicate')
    duplicate_pair,_ = transport((a,a))
    bad('C35',duplicate_pair+b'X',code=36,position=len(duplicate_pair),
        reason='trailing rejection precedes duplicate rejection')
    maximal_key = (b'x'*64,b'\x00'*256,b'\xff\x00')
    maximal_input,_ = transport((maximal_key,))
    add('C36',maximal_input,(b'x'*64,),selected=(maximal_key,),
        reason='name/query64 and path256 exact, opaque NUL path')
    rows4095 = tuple((f'm{i:04d}'.encode('ascii'),b'',b'x') for i in range(4095))
    all4095,_ = transport(rows4095)
    all4094,_ = transport(rows4095[:-1])
    add('C37',all4094,queries4096[:4094],selected=rows4095[:-1],
        reason='N/Q4094 single below')
    add('C38',all4095,queries4096[:4095],selected=rows4095,
        reason='N/Q4095 exact with short argv keys')
    add('C39',all4095,queries4096[:4094],selected=rows4095[:-1],
        reason='N4095 with Q4094 subset')
    for label,size in (('C40',MIB-1),('C41',MIB)):
        row = (b'a',b'',b'x'*size)
        raw,_ = transport((row,))
        add(label,raw,(b'a',),selected=(row,),reason='body payload '+str(size))
    prefix,p = transport(((b'a',b'',b''),))
    body_header = p[7]['frame']
    bad('C42',prefix[:body_header]+b'1048577:',code=5,
        position=body_header+6,reason='body1MiB+1 rejected on length, no payload')
    four_names = (b'a',b'b',b'c',b'd')
    for label,delta in (('C43',-1),('C44',0)):
        rows = tuple((name,b'',b'x'*(MIB+(delta if i==3 else 0)))
                     for i,name in enumerate(four_names))
        raw,_ = transport(rows)
        add(label,raw,four_names,selected=rows,reason='source4MiB '+str(delta))
    excessive = tuple((name,b'',b'x'*MIB) for name in four_names)+( (b'e',b'',b'x'), )
    raw,p = transport(excessive)
    bad('C45',raw,code=35,position=p[19]['payload'],
        reason='aggregate4MiB+1, each body within1MiB')
    for label,size in (('C46',MIB-1),('C47',MIB)):
        raw,_ = transport(graph=b'g'*size)
        add(label,raw,reason='opaque graph payload '+str(size))
    raw,p = transport()
    graph_start = p[4]['frame']
    bad('C48',raw[:graph_start]+b'1048577:',code=5,position=graph_start+6,
        reason='graph1MiB+1 header cap')
    for label,size in (('C49',MIB-1),('C50',MIB)):
        raw,_ = transport(manifest=b'm'*size)
        add(label,raw,reason='opaque manifest payload '+str(size))
    manifest_start = p[3]['frame']
    bad('C51',zero[:manifest_start]+b'1048577:',code=5,position=manifest_start+6,
        reason='manifest1MiB+1 header cap')
    for label,size in (('C52',INPUT_CAP-1),('C53',INPUT_CAP),('C54',INPUT_CAP+1)):
        raw = zero+b'x'*(size-len(zero))
        if size > INPUT_CAP:
            bad(label,raw,code=10,position=0,reason='input8MiB+1 read admission')
        else:
            bad(label,raw,code=36,position=len(zero),
                reason='input8MiB bound reached only with trailing malformed data')
    rows = tuple((name,b'',b'x'*(MIB if i<3 else 1))
                 for i,name in enumerate(four_names))
    raw,p = transport(rows,manifest=b'm'*MIB)
    bad('C55',raw,code=35,position=p[16]['payload'],
        reason='manifest counts in source aggregate; first excess final body')
    empty_record = (b'a',b'',b'')
    raw,_ = transport((empty_record,))
    add('C56',raw,(b'a',),selected=(empty_record,),
        reason='selected empty path and body are preserved')
    raw,p = transport(modules=b'')
    bad('C57',raw,code=31,position=p[1]['payload'],
        reason='empty module counter payload is noncanonical')
    for label,value,is_edge,code,reason in (
        ('C58',b'1000000000000000',False,32,'16-digit canonical module counter exceeds N cap'),
        ('C59',b'10000000000000000',False,5,'17-byte module counter exceeds frame cap before syntax/value'),
        ('C60',b'1x1',False,31,'middle nondigit module counter'),
        ('C61',b'1000000000000000',True,32,'16-digit canonical edge counter exceeds E cap'),
        ('C62',b'10000000000000000',True,5,'17-byte edge counter exceeds frame cap before syntax/value'),
        ('C63',b'1x1',True,31,'middle nondigit edge counter')):
        raw,p = transport(edges=value) if is_edge else transport(modules=value)
        field = p[2 if is_edge else 1]
        position = field['frame']+1 if code == 5 else field['payload']
        bad(label,raw,code=code,position=position,reason=reason)
    bad('C64',b'1048577:',code=5,position=6,
        reason='identity1MiB+1 header exceeds general payload cap before tag comparison')
    assert len(cases) == CASE_CAP
    assert tuple(row['label'] for row in cases) == tuple(f'C{i:02d}' for i in range(1,65))
    assert CASE_CAP+GEOMETRY_CASES+REAL_CASES == 71 <= TOTAL_CASE_CAP
    return tuple(cases)


def geometry():
    cards = []
    for count in (64,128,256,512):
        lexical = tuple((f'm{i:04d}'.encode('ascii'),b'',b'x') for i in range(count))
        raw,_ = transport(tuple(reversed(lexical)))
        cards.append(card('G'+str(count),raw,tuple(row[0] for row in lexical),
                          selected=lexical,reason='reversed physical/all lexical queries'))
    return tuple(cards)


# Literal declarations, not a parser. Manifest bytes remain pinned separately.
COMPILER_ROWS = (
 ('analysis','analysis.slim','memory parallel quality ranges reduce syntax text typing'),
 ('cache','cache.slim','project syntax text'),
 ('check','check.slim','diagnostics effects identity ir memory ranges retained syntax text typing validate'),
 ('codegen','codegen.slim','memory parallel ranges syntax text typing'),
 ('compiler','slimc.slim','analysis cache check codegen context edit equivalence format memory project proof reduce scheduler session syntax text typing validate'),
 ('context','context.slim','cache check identity project retained syntax text typing'),
 ('control','control.slim','syntax'),
 ('diagnostics','diagnostics.slim','syntax text typing'),('driver','driver.slim','compiler'),
 ('edit','edit.slim','format syntax text'),('effects','effects.slim','syntax'),
 ('equivalence','equivalence.slim','syntax text'),
 ('flow','flow.slim','control identity retained syntax typing'),
 ('format','format.slim','syntax text'),
 ('fragments','fragments.slim','cache codegen identity memory parallel parallelcache project ranges retained syntax text typing'),
 ('identity','identity.slim',''),('ir','ir.slim',''),
 ('memory','memory.slim','effects ir syntax'),('nativebuild','nativebuild.slim','identity session'),
 ('nativecache','nativecache.slim','cache retained'),('ownership','ownership.slim',''),
 ('parallel','parallel.slim','effects ranges syntax text typing'),
 ('parallelcache','parallelcache.slim','identity parallel project ranges retained syntax'),
 ('project','project.slim','check codegen diagnostics format identity memory ranges retained scheduler syntax text typing validate'),
 ('proof','proof.slim','reduce syntax text'),('quality','quality.slim','ranges reduce syntax text'),
 ('query','query.slim','identity project syntax text'),
 ('ranges','ranges.slim','effects syntax text typing'),
 ('reduce','reduce.slim','format syntax text'),
 ('retained','retained.slim','identity ir memory ranges syntax text typing'),
 ('scheduler','scheduler.slim','syntax'),
 ('session','session.slim','cache fragments identity parallelcache project query retained syntax'),
 ('syntax','syntax.slim','identity ir'),('text','text.slim','syntax'),
 ('typing','typing.slim','control ir memory ownership syntax'),('validate','validate.slim','syntax'))
LEDGER_ROWS = (
 ('ledger','applications/ledger/main.slim','ledger_emit ledger_model ledger_state'),
 ('ledger_emit','applications/ledger/emit.slim','ledger_model ledger_state std_text'),
 ('ledger_model','applications/ledger/model.slim',''),
 ('ledger_parser','applications/ledger/parser.slim','ledger_model std_bytes std_decimal'),
 ('ledger_state','applications/ledger/state.slim','ledger_model ledger_parser std_bytes'),
 ('std_ascii','experimental/ascii.slim',''),('std_bytes','experimental/bytes.slim',''),
 ('std_decimal','experimental/decimal.slim','std_ascii'),
 ('std_text','experimental/text.slim','std_bytes'))
CATALOG_ROWS = (
 ('catalog_app','applications/catalog/main.slim','catalog_data catalog_diff_emit catalog_emit catalog_model catalog_reconcile std_byte_index std_bytes'),
 ('catalog_data','applications/catalog/catalog.slim','catalog_model framed_records std_byte_index std_decimal'),
 ('catalog_diff_emit','applications/catalog/diff_emit.slim','catalog_emit catalog_model catalog_reconcile std_netstring std_text'),
 ('catalog_emit','applications/catalog/emit.slim','catalog_model std_byte_index std_netstring std_text'),
 ('catalog_model','applications/catalog/model.slim','std_byte_index'),
 ('catalog_reconcile','applications/catalog/reconcile.slim','catalog_model std_byte_index std_bytes'),
 ('framed_records','components/records.slim','std_decimal std_netstring'),
 ('std_ascii','experimental/ascii.slim',''),('std_byte_index','experimental/byte_index.slim',''),
 ('std_bytes','experimental/bytes.slim',''),('std_decimal','experimental/decimal.slim','std_ascii'),
 ('std_netstring','experimental/netstring.slim','std_ascii std_bytes std_text'),
 ('std_text','experimental/text.slim','std_bytes'))
REAL = {
 'compiler':('selfhost/slim.project','selfhost',
             '2076323f8f1888aa67c2b88946b99ceb047b34ccf216900224009eae41aa850f',
             COMPILER_ROWS,('compiler','context','project')),
 'ledger':('library/ledger.project','library',
           '376145c7517aa1a65180d7d9b44949ea5a8a479752b7643701b12a14e46891b1',
           LEDGER_ROWS,('ledger','ledger_emit','ledger_state')),
 'catalog':('library/catalog.project','library',
            '28951ae70aaa93248fb5770ef3c8d0ec8649bd7f45456527136209c86195b16c',
            CATALOG_ROWS,('catalog_app','catalog_data','catalog_reconcile'))}


def real_bundle(key, manifest, bodies):
    """Supplied fixed-registry opaque bytes; source acceptance is never inferred."""
    _,base,manifest_sha,declarations,queries = REAL[key]
    assert hashlib.sha256(manifest).hexdigest() == manifest_sha
    assert set(bodies) == {base+'/'+path for _,path,_ in declarations}
    assert all(type(body) is bytes and len(body) <= MIB for body in bodies.values())
    assert len(manifest) <= MIB and len(manifest)+sum(map(len,bodies.values())) <= 4*MIB
    records = tuple((name.encode('ascii'),path.encode('ascii'),bodies[base+'/'+path])
                    for name,path,_ in declarations)
    graph = b''.join(frame(name.encode('ascii'))+frame(str(len(bodies[base+'/'+path])).encode('ascii'))+
                     frame(imports.replace(' ',',').encode('ascii'))
                     for name,path,imports in declarations)
    edges = sum(len(imports.split()) for _,_,imports in declarations)
    capture,_ = transport(records,manifest=manifest,graph=graph,
                          edges=str(edges).encode('ascii'))
    selected = tuple(records[next(i for i,row in enumerate(declarations) if row[0] == name)]
                     for name in queries)
    return {'capture':capture,'graph':graph,'queries':tuple(q.encode('ascii') for q in queries),
            'stdout':output(selected),'status':0,'stderr':b'',
            'modules':len(declarations),'edges':edges,
            'selected_records':selected,'source_authority':'unknown until actual producer0',
            'module_body_bytes':sum(len(body) for body in bodies.values()),
            'selected_body_bytes':sum(len(row[2]) for row in selected)}


def labels():
    result = ['cleanup-descendant','check-application','emit-application',
              'build-application-ordinary','build-application-sanitized',
              'emit-production-producer','build-production-producer']
    result.extend(f'C{i:02d}-{variant}' for i in range(1,65)
                  for variant in ('ordinary','sanitized'))
    result.extend(('argument-fifo-C08','argument-fifo-C09'))
    result.extend(f'G{n}-{variant}' for n in (64,128,256,512)
                  for variant in ('ordinary','sanitized'))
    result.extend(f'oom-{ordinal:02d}-{variant}' for ordinal in range(1,33)
                  for variant in ('ordinary','sanitized'))
    result.extend(f'{key}-{role}' for key in ('compiler','ledger','catalog')
                  for role in ('capture','select-ordinary','select-sanitized'))
    assert len(result) == 218 and len(set(result)) == 218
    return tuple(result)
