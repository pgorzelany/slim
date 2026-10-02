#!/usr/bin/env python3
"""Independent RFC-0159 production-command conformance; no SLIM semantics.

Expectations are from docs/CONTEXT.md and fixed source fixtures. This script
never uses reference C/Rust checking or parses context source as an executable IR.
"""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parent.parent
LIMITS = {'input_bytes':16777216,'modules':128,'canonical_nodes':1000000,
          'selected_source_bytes':65536,'providers':64,'facts':512,'references':512,
          'type_bytes':4096,'type_nodes':128,'signature_bytes':8192,
          'report_bytes':1048576,'operand_bytes':4096}
OPTIONS = None


class ContextTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='slim-context-verification-')
        self.directory = Path(self.temporary.name)

    def tearDown(self):
        self.temporary.cleanup()

    def invoke(self, *args):
        return subprocess.run([OPTIONS.compiler,*map(str,args)],cwd=ROOT,capture_output=True,timeout=60)

    def source(self, source, name='program.slim'):
        path = self.directory/name
        path.write_bytes(source.encode() if isinstance(source,str) else source)
        expected = self.directory/('expected-'+name)
        shutil.copyfile(path,expected)
        return path,expected

    def rejected(self, args, code, status=1):
        result = self.invoke('context',*args)
        self.assertEqual(result.returncode,status,(result.stdout,result.stderr))
        self.assertEqual(result.stdout,(code+'@0:0\n').encode(),result.stderr)
        self.assertEqual(result.stderr,b'')
        return result

    def report(self, path, expected, selector, normal_check=True):
        if normal_check:
            checked = self.invoke('check',path)
            self.assertEqual(checked.returncode,0,(checked.stdout,checked.stderr))
        first = self.invoke('context',path,expected,selector)
        self.assertEqual(first.returncode,0,(first.stdout,first.stderr))
        self.assertEqual(first.stderr,b'')
        self.assertEqual(first.stdout,self.invoke('context',path,expected,selector).stdout)
        data = json.loads(first.stdout)
        self.assertEqual(set(data),{'schema','encoding','identity_evidence','selector','input','limits','selected','providers','facts','references','work'})
        self.assertEqual(data['schema'],1)
        self.assertEqual(data['encoding'],'json-byte-escapes-v1')
        self.assertEqual(data['identity_evidence'],'exact-expected-input-bytes')
        self.assertEqual(data['selector'],selector)
        self.assertEqual(data['limits'],LIMITS)
        self.assertEqual(data['work']['report_bytes'],len(first.stdout))
        self.assertLessEqual(len(first.stdout),LIMITS['report_bytes'])
        for name,count_name in [('providers','provider_rows'),('facts','fact_rows'),('references','reference_rows')]:
            section = data[name]
            self.assertEqual(set(section),{'evidence','limit','reason','rows'})
            self.assertEqual(section['limit'],LIMITS[name])
            self.assertLessEqual(len(section['rows']),LIMITS[name])
            self.assertEqual(data['work'][count_name],len(section['rows']))
            self.assertEqual(section['reason'],'row-limit' if section['evidence']=='bounded' else '')
        self.validate_spans(data)
        if normal_check:
            self.assertEqual(self.invoke('check',path).returncode,0)
        return data

    def validate_spans(self, data):
        sources = {}
        for file in data['input']['files']:
            path = file['path'].encode('latin1').decode()
            source = Path(path).read_bytes()
            self.assertEqual(file['bytes'],len(source))
            sources[file['slot']] = source
        def traverse(value):
            if isinstance(value,list):
                for row in value: traverse(row)
            elif isinstance(value,dict):
                if set(value)=={'file','start','end'}:
                    self.assertIn(value['file'],sources)
                    self.assertTrue(0 <= value['start'] <= value['end'] <= len(sources[value['file']]))
                for child in value.values(): traverse(child)
                for text_key,span_key in [('source','span'),('signature','signature_span'),('name','name_span')]:
                    if text_key in value and span_key in value and value[text_key] is not None:
                        span = value[span_key]
                        self.assertEqual(value[text_key].encode('latin1'),sources[span['file']][span['start']:span['end']])
                if value.get('kind')=='source-form' and value.get('evidence')=='exact':
                    span = value['span']
                    self.assertEqual(value['text'].encode('latin1'),sources[span['file']][span['start']:span['end']])
        traverse(data)

    def test_modes_bindings_types_and_direct_provenance(self):
        source = '''module sample

struct Bag:
  values: Vec[I64]

enum Choice:
  None
  Some(Vec[I64])

fn identity(value: I64) -> I64:
  value

fn modes(var number: I64, shared: Vec[I64], exclusive: @Vec[I64], owned: ^Vec[I64], bag: Bag) -> I64 effects[alloc, partial]:
  let length: I64 = vec.len(shared)
  vec.push(@exclusive, number)
  let moved: Vec[I64] = owned
  let local: Bag = Bag(values: vec.new())
  let choice: Choice = Choice::None()
  let first: I64 = identity(number)
  let second: I64 = identity(first)
  let identity: I64 = second
  identity + length

fn main(args: Vec[Bytes]) -> I64:
  0
'''
        path,expected = self.source(source)
        data = self.report(path,expected,'sample.modes')
        selected = data['selected']
        self.assertEqual(selected['kind'],'function')
        self.assertEqual(selected['effect_ceiling'],['alloc','partial'])
        self.assertEqual(selected['effect_evidence'],'declared-ceiling')
        parameters = selected['parameters']
        self.assertEqual([p['declared_mode'] for p in parameters],['ordinary','ordinary','exclusive','owned','ordinary'])
        self.assertEqual([p['borrow_mode'] for p in parameters],['value','shared','exclusive','value','shared'])
        self.assertEqual([p['mutable'] for p in parameters],[True,False,False,False,False])
        self.assertEqual([p['type']['text'] for p in parameters],['I64','Vec[I64]','Vec[I64]','Vec[I64]','Bag'])
        providers = [(p['module'],p['name']) for p in data['providers']['rows']]
        self.assertEqual(set(providers),{('sample','Bag'),('sample','Choice'),('sample','identity')})
        self.assertEqual(len(providers),3)
        calls = [r for r in data['references']['rows'] if r['role']=='call' and r['target_name']=='identity']
        self.assertEqual(len(calls),2)
        self.assertTrue(all(r['target_kind']=='source' and r['provenance']=='checked-call-link' for r in calls))
        self.assertTrue(any(r['target_kind']=='builtin' and r['target_name']=='vec.len' for r in data['references']['rows']))
        bindings = {f['name']:f for f in data['facts']['rows'] if f['name'] is not None}
        self.assertEqual(bindings['moved']['borrow_mode'],'value')
        self.assertEqual(bindings['shared']['borrow_mode'],'shared')
        self.assertEqual(bindings['exclusive']['borrow_mode'],'exclusive')
        self.assertEqual(bindings['identity']['type']['text'],'I64')
        self.assertTrue(all(r['evidence']=='exact' for r in calls))
        bag = self.report(path,expected,'sample.Bag')['selected']
        choice = self.report(path,expected,'sample.Choice')['selected']
        self.assertEqual([(f['name'],f['type']['text']) for f in bag['fields']],[('values','Vec[I64]')])
        self.assertEqual([(c['name'],[t['text'] for t in c['payload']]) for c in choice['cases']],[('None',[]),('Some',['Vec[I64]'])])
        self.assertEqual(bag['return_type'],None)
        self.assertEqual(choice['parameters'],[])
        self.assertIsNotNone(parameters[0]['type']['span'])
        self.assertEqual(self.report(path,expected,'sample.main')['selected']['return_type']['text'],'I64')
        main_facts = self.report(path,expected,'sample.main')['facts']['rows']
        literals = [f for f in main_facts if f['name'] is None and f['span'] and path.read_bytes()[f['span']['start']:f['span']['end']]==b'0']
        self.assertTrue(literals)
        self.assertTrue(all(f['type']['text']=='I64' and f['type']['span'] is None for f in literals))

    def test_header_and_original_bytes(self):
        header = b'fn complicated(value: I64, data: Vec[I64]) -> I64 effects[partial]:'
        source = b'module multiline\n\n'+header+b'\n  # caf\xc3\xa9 body comment\n  vec.len(data) + value\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
        path,expected = self.source(source)
        selected = self.report(path,expected,'multiline.complicated')['selected']
        self.assertEqual(selected['signature'].encode('latin1'),header)
        self.assertEqual(selected['signature_span']['start'],source.index(b'fn complicated'))
        self.assertEqual(selected['signature_span']['end'],source.index(header)+len(header))
        self.assertTrue(selected['source'].encode('latin1').startswith(header+b'\n  # caf\xc3\xa9 body comment\n  vec.len(data) + value'))
        # Current ordinary checker rejects multiline declaration parameters.
        # The context command must preserve that rejection, not add syntax.
        path.write_bytes(source.replace(b'value: I64, data:',b'value: I64,\n  data:'))
        expected.write_bytes(path.read_bytes())
        checked,queried = self.invoke('check',path),self.invoke('context',path,expected,'multiline.complicated')
        self.assertEqual(checked.returncode,1)
        self.assertEqual((queried.returncode,queried.stdout),(checked.returncode,checked.stdout))

    def project(self, modules=3):
        current,snapshot = self.directory/'current',self.directory/'snapshot'
        current.mkdir(); snapshot.mkdir()
        files = {'app.slim':'''module app

fn main(args: Vec[Bytes]) -> I64 effects[alloc, partial]:
  let packet: types.Packet = types.Packet(value: 7, values: vec.new())
  helpers.count(packet) + helpers.count(packet)
''', 'helpers.slim':'''module helpers

fn count(packet: types.Packet) -> I64:
  vec.len(packet.values)
''', 'types.slim':'''module types

struct Packet:
  value: I64
  values: Vec[I64]

enum Choice:
  No
  Yes(I64)
''', 'slim.project':'''(project 1
  (entry app)
  (module app "app.slim" (imports helpers types) (exports))
  (module helpers "helpers.slim" (imports types) (exports count))
  (module types "types.slim" (imports) (exports Choice Packet)))
'''}
        for name,source in files.items():
            (current/name).write_text(source); (snapshot/name).write_text(source)
        return current/'slim.project',snapshot/'slim.project'

    def test_project_original_providers_and_complete_snapshot_staleness(self):
        path,expected = self.project()
        data = self.report(path,expected,'app.main')
        self.assertEqual(data['input']['kind'],'project')
        self.assertEqual([f['module'] for f in data['input']['files']],['app','helpers','types'])
        providers = data['providers']['rows']
        self.assertEqual([(p['module'],p['name']) for p in providers],[('types','Packet'),('helpers','count')])
        self.assertTrue(all(p['source'] is None for p in providers))
        calls = [r for r in data['references']['rows'] if r['target_name']=='count']
        self.assertEqual(len(calls),2)
        self.assertTrue(all(r['target_module']=='helpers' for r in calls))
        self.assertNotIn('helpers_count',json.dumps(data))
        self.assertEqual(self.report(path,expected,'helpers.count')['selected']['parameters'][0]['type']['text'],'types.Packet')
        module = path.parent/'types.slim'
        module.write_text(module.read_text().replace('  Yes(I64)','  Yes(U8)'))
        self.rejected([path,expected,'app.main'],'E0450')
        module.write_bytes((expected.parent/'types.slim').read_bytes())
        module.write_bytes(module.read_bytes()+b'\n# unselected change\n')
        self.rejected([path,expected,'helpers.count'],'E0450')

    def test_same_length_stale_checksum_collision_and_byte_escaping(self):
        source = b'module encoding\n\nfn main(args: Vec[Bytes]) -> I64:\n  # abc caf\xc3\xa9 \x80\x01\t quote " slash \\\n  0\n'
        path,expected = self.source(source)
        before = self.report(path,expected,'encoding.main')
        self.assertIn(b'\xc3\xa9 \x80\x01\t',before['selected']['source'].encode('latin1'))
        path.write_bytes(source.replace(b'abc',b'b`d'))
        self.rejected([path,expected,'encoding.main'],'E0450')
        changed_expected = self.directory/'changed.slim'; shutil.copyfile(path,changed_expected)
        after = self.report(path,changed_expected,'encoding.main')
        self.assertEqual(before['input']['files'][0]['weighted_checksum'],after['input']['files'][0]['weighted_checksum'])
        self.assertEqual(before['input']['files'][0]['bytes'],after['input']['files'][0]['bytes'])

    def test_self_reference_missing_selector_arguments_and_invalid_source(self):
        source = '''module self_call

fn bounce(value: I64) -> I64 effects[partial]:
  if value <= 0:
    0
  else:
    bounce(value - 1)

fn main(args: Vec[Bytes]) -> I64:
  0
'''
        path,expected = self.source(source)
        data = self.report(path,expected,'self_call.bounce')
        self.assertEqual(data['providers']['rows'],[])
        self.assertTrue(any(r['target_node']==data['selected']['node'] and r['target_name']=='bounce' for r in data['references']['rows']))
        self.rejected([path,expected,'self_call.absent'],'E0452')
        self.rejected([path,expected],'E0453',64)
        self.rejected([path,expected,'x'*4097],'E0453',64)
        self.rejected(['x'*4097,expected,'self_call.main'],'E0453',64)
        project = self.directory/'kind.project'
        project.write_text('(project 1 (entry app) (module app "app.slim" (imports) (exports)))\n')
        self.rejected([path,project,'self_call.main'],'E0453',64)
        missing = self.invoke('context',path,self.directory/'missing.slim','self_call.main')
        self.assertNotEqual(missing.returncode,0)
        self.assertFalse(missing.stdout.startswith(b'{'))
        path.write_bytes(b'module bad\n\nfn main(args: Vec[Bytes]) -> I64:\n  missing(1)\n')
        expected.write_bytes(path.read_bytes())
        checked,queried = self.invoke('check',path),self.invoke('context',path,expected,'bad.main')
        self.assertNotEqual(checked.returncode,0)
        self.assertEqual((queried.returncode,queried.stdout,queried.stderr),(checked.returncode,checked.stdout,checked.stderr))

    def test_maintained_library_and_compiler_selections(self):
        for directory,selector in [('library','std_decimal.parse_i64'),('selfhost','check.check_source')]:
            snapshot = self.directory/('snapshot-'+directory)
            shutil.copytree(ROOT/directory,snapshot)
            data = self.report(ROOT/directory/'slim.project',snapshot/'slim.project',selector)
            self.assertEqual(data['input']['kind'],'project')
            self.assertEqual(data['selector'],selector)
            self.assertGreater(data['work']['inspected_nodes'],0)
            self.assertTrue(data['facts']['rows'])

    def test_current_project_capture_failure_precedes_expected_input(self):
        path,expected = self.project()
        original = path.read_bytes()
        malformed = b'(project 1 (entry app) (module app "app.slim" (imports)))\n'
        path.write_bytes(malformed); expected.write_bytes(malformed)
        checked = self.invoke('check',path)
        queried = self.invoke('context',path,expected,'app.main')
        self.assertEqual(checked.returncode,1)
        self.assertEqual((queried.returncode,queried.stdout,queried.stderr),(checked.returncode,checked.stdout,checked.stderr))
        path.write_bytes(original)
        (path.parent/'types.slim').unlink()
        expected.write_bytes(b'(project bad expected manifest)\n')
        checked = self.invoke('check',path)
        queried = self.invoke('context',path,expected,'app.main')
        self.assertEqual(checked.returncode,1)
        self.assertTrue(checked.stdout.startswith(b'E0409@'))
        self.assertEqual((queried.returncode,queried.stdout,queried.stderr),(checked.returncode,checked.stdout,checked.stderr))


class LimitTests(ContextTests):
    # Base conformance tests are deliberately not inherited a second time.
    def test_row_provider_caps(self):
        for count in [64,65]:
            helpers = ''.join(f'fn p{i}() -> I64:\n  {i}\n\n' for i in range(count))
            body = ''.join(f'  let v{i}: I64 = p{i}()\n' for i in range(count))
            path,expected = self.source('module providers\n\n'+helpers+'fn selected() -> I64:\n'+body+'  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
            data = self.report(path,expected,'providers.selected')
            self.assertEqual(len(data['providers']['rows']),64)
            self.assertEqual(data['providers']['evidence'],'exact' if count==64 else 'bounded')
        for count in [512,513]:
            body = ''.join(f'  let v{i}: I64 = vec.len(args)\n' for i in range(count))
            path,expected = self.source('module rows\n\nfn main(args: Vec[Bytes]) -> I64:\n'+body+'  0\n')
            data = self.report(path,expected,'rows.main')
            self.assertEqual(data['references']['evidence'],'exact' if count==512 else 'bounded')
            self.assertEqual(data['facts']['evidence'],'bounded')
            self.assertEqual(len(data['facts']['rows']),512)
            self.assertEqual(len(data['references']['rows']),512)
        # Independent small reports establish an intercept of2 (args and
        # final literal), four facts per typed let/call and three per Void
        # prefix (continuation, synthetic binding and expression). Thus
        # 2+126*4+2*3=512 and2+127*4+1*3=513 eligible facts.
        for extra_fact in [False,True]:
            count=127 if extra_fact else 126
            body = ''.join(f'  let v{i}: I64 = vec.len(args)\n' for i in range(count))
            body += '  void\n'*(1 if extra_fact else 2)
            path,expected = self.source('module fact_rows\n\nfn main(args: Vec[Bytes]) -> I64:\n'+body+'  0\n')
            data = self.report(path,expected,'fact_rows.main')
            self.assertEqual(len(data['facts']['rows']),512)
            self.assertEqual(data['facts']['evidence'],'bounded' if extra_fact else 'exact')

    def test_type_text_and_type_node_caps(self):
        for length in [4096,4097]:
            name = 'T'+'x'*(length-1)
            source = f'module type_bytes\n\nstruct {name}:\n  value: I64\n\nfn selected(value: {name}) -> I64:\n  value.value\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
            path,expected = self.source(source)
            kind = self.report(path,expected,'type_bytes.selected')['selected']['parameters'][0]['type']
            self.assertEqual(kind['evidence'],'exact' if length==4096 else 'unknown')
            self.assertEqual(kind['reason'],'' if length==4096 else 'type-budget')
            self.assertIsNotNone(kind['span'])
        for nesting in [127,128]:
            form = 'Vec['*nesting+'I64'+']'*nesting
            path,expected = self.source(f'module type_nodes\n\nfn selected(value: {form}) -> I64:\n  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
            kind = self.report(path,expected,'type_nodes.selected')['selected']['parameters'][0]['type']
            self.assertEqual(kind['evidence'],'exact' if nesting==127 else 'unknown')
            self.assertEqual(kind['reason'],'' if nesting==127 else 'type-budget')

    def test_input_modules_selected_source_and_report_caps(self):
        for size in [16777216,16777217]:
            prefix,bottom = b'module input_limit\n#',b'\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
            path,expected = self.source(prefix+b'x'*(size-len(prefix)-len(bottom))+bottom)
            if size==16777216:
                self.report(path,expected,'input_limit.main',normal_check=False)
            else: self.rejected([path,expected,'input_limit.main'],'E0451')
        for size in [65536,65537]:
            head,tail = b'fn selected() -> I64:\n  #',b'\n  0'
            declaration = head+b'x'*(size-len(head)-len(tail))+tail
            source = b'module selected_limit\n\n'+declaration+b'\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
            path,expected = self.source(source)
            if size==65536:
                data = self.report(path,expected,'selected_limit.selected')
                self.assertEqual(len(data['selected']['source'].encode('latin1')),65536)
            else: self.rejected([path,expected,'selected_limit.selected'],'E0451')
        for count in [128,129]:
            current = self.directory/f'modules-{count}'; current.mkdir()
            snapshot = self.directory/f'snapshot-{count}'; snapshot.mkdir()
            rows = ['(project 1\n  (entry app)\n  (module app "app.slim" (imports) (exports))']
            (current/'app.slim').write_text('module app\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
            for index in range(count-1):
                name=f'm{index:03d}'
                rows.append(f'  (module {name} "{name}.slim" (imports) (exports))')
                (current/(name+'.slim')).write_text('module '+name+'\n')
            (current/'slim.project').write_text('\n'.join(rows)+')\n')
            for p in current.iterdir(): shutil.copyfile(p,snapshot/p.name)
            if count==128: self.report(current/'slim.project',snapshot/'slim.project','app.main')
            else: self.rejected([current/'slim.project',snapshot/'slim.project','app.main'],'E0451')
        header = lambda i:f'fn p{i}('.encode()+b'n'*7900+b': I64) -> I64:\n  0\n\n'
        providers = b''.join(header(i) for i in range(64))
        body = ''.join(f'  let v{i}: I64 = p{i}(0)\n' for i in range(64)).encode()
        path,expected = self.source(b'module report_limit\n\n'+providers+b'fn selected() -> I64:\n'+body+b'  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n')
        self.assertEqual(self.invoke('check',path).returncode,0)
        self.rejected([path,expected,'report_limit.selected'],'E0451')

    def test_summed_project_input_admission_boundary(self):
        path,expected = self.project()
        base = sum(p.stat().st_size for p in path.parent.iterdir())
        app = path.parent/'app.slim'
        original = app.read_bytes()
        for size in [16777216,16777217]:
            app.write_bytes(original+b'#'+b'x'*(size-base-2)+b'\n')
            self.assertEqual(sum(p.stat().st_size for p in path.parent.iterdir()),size)
            (expected.parent/'app.slim').write_bytes(app.read_bytes())
            if size==16777216:
                data = self.report(path,expected,'helpers.count',normal_check=False)
                self.assertEqual(sum(f['bytes'] for f in data['input']['files'])+data['input']['manifest']['bytes'],size)
            else: self.rejected([path,expected,'helpers.count'],'E0451')

    def test_exact_response_capacity_boundary(self):
        # Finite byte-length calibration uses the production response, never a
        # second semantic interpreter. There are at most11 binary-search probes
        # and256 one-byte padding probes, with independently checked source.
        def fixture(length,padding=0):
            providers = []
            for index in range(64):
                spaces = ' '*padding if index==63 else ''
                header = f'fn p{index}('+('n'*length)+': I64'+spaces+') -> I64:'
                self.assertLessEqual(len(header),8192)
                providers.append(header+'\n  0\n\n')
            body = ''.join(f'  let v{i}: I64 = p{i}(0)\n' for i in range(64))
            source = 'module exact_report\n\n'+''.join(providers)+'fn main(args: Vec[Bytes]) -> I64:\n'+body+'  0\n'
            return self.source(source)
        low,high = 7000,7900
        best = None
        while low<=high:
            middle=(low+high)//2
            path,expected = fixture(middle)
            result = self.invoke('context',path,expected,'exact_report.main')
            if result.returncode==0:
                best=(middle,len(result.stdout)); low=middle+1
            else:
                self.assertEqual((result.returncode,result.stdout),(1,b'E0451@0:0\n'))
                high=middle-1
        self.assertIsNotNone(best)
        length,observed=best
        remaining=LIMITS['report_bytes']-observed
        self.assertTrue(0<=remaining<256)
        exact_padding = None
        # Starting close to the observed difference normally needs one probe;
        # two spare positions account for decimal metadata-width transitions.
        for padding in range(max(0,remaining-2),min(256,remaining+3)):
            path,expected = fixture(length,padding)
            result=self.invoke('context',path,expected,'exact_report.main')
            if result.returncode==0 and len(result.stdout)==LIMITS['report_bytes']:
                exact_padding=padding; break
        self.assertIsNotNone(exact_padding,'exact response-size fixture could not be constructed within its fixed calibration budget')
        self.assertEqual(self.invoke('check',path).returncode,0)
        data=self.report(path,expected,'exact_report.main',normal_check=False)
        self.assertEqual(data['work']['report_bytes'],1048576)
        path,expected=fixture(length,exact_padding+1)
        self.assertEqual(self.invoke('check',path).returncode,0)
        self.rejected([path,expected,'exact_report.main'],'E0451')

    def test_provider_signature_cap(self):
        for size in [8192,8193]:
            prefix,bottom = b'fn provider(',b') -> I64:'
            header = prefix+b' '*(size-len(prefix)-len(bottom))+bottom
            source = b'module signature_limit\n\n'+header+b'\n  0\n\nfn main(args: Vec[Bytes]) -> I64:\n  provider()\n'
            path,expected = self.source(source)
            declaration = self.report(path,expected,'signature_limit.main')['providers']['rows'][0]
            self.assertEqual(declaration['signature_evidence'],'exact' if size==8192 else 'unknown')
            self.assertEqual(declaration['signature_reason'],'' if size==8192 else 'signature-budget')

    def test_exact_million_canonical_node_admission(self):
        # The fixed production parser shape has module4 + helpers11 each +
        # main18 nodes; each declared effect atom adds one. This formula was
        # separately observed through the production canonical-node observer.
        # It counts canonical nodes, not source/model lexical-token proxies.
        helpers = ''.join(f'fn value{i}() -> I64:\n  0\n\n' for i in range(90907))
        for effects,nodes in [('partial',1000000),('io, partial',1000001)]:
            source = 'module node_limit\n\n'+helpers+f'fn main(args: Vec[Bytes]) -> I64 effects[{effects}]:\n  0\n'
            path,expected = self.source(source)
            self.assertLess(path.stat().st_size,LIMITS['input_bytes'])
            if nodes==1000000:
                self.assertEqual(self.invoke('check',path).returncode,0)
                data = self.report(path,expected,'node_limit.main',normal_check=False)
                self.assertEqual(data['selected']['effect_ceiling'],['partial'])
                self.assertEqual(data['selected']['effect_evidence'],'declared-ceiling')
            else:
                # The ordinary parser shares this node admission ceiling and
                # rejects generically; context must report its explicit limit.
                checked = self.invoke('check',path)
                self.assertEqual(checked.returncode,1)
                self.assertTrue(checked.stdout.startswith(b'E0102@'))
                self.rejected([path,expected,'node_limit.main'],'E0451')

    def test_large_module_spelling_missing_selector_remains_supported(self):
        name = 'm'+'x'*8191
        helpers = ''.join(f'fn value{i}() -> I64:\n  0\n\n' for i in range(512))
        path,expected = self.source('module '+name+'\n\n'+helpers+'fn main(args: Vec[Bytes]) -> I64:\n  0\n')
        self.assertEqual(self.invoke('check',path).returncode,0)
        self.rejected([path,expected,'absent.main'],'E0452')


def main():
    global OPTIONS
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler',default=str(ROOT/'build/toolchain/slimc'))
    parser.add_argument('--section',choices=['basic','limits','all'],default='all')
    OPTIONS = parser.parse_args()
    if not Path(OPTIONS.compiler).is_file(): parser.error('production compiler must already exist; bootstrap explicitly first')
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    if OPTIONS.section in ['basic','all']: suite.addTests(loader.loadTestsFromTestCase(ContextTests))
    if OPTIONS.section in ['limits','all']:
        suite.addTests(LimitTests(name) for name in loader.getTestCaseNames(LimitTests) if name not in loader.getTestCaseNames(ContextTests))
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    return 0 if result.wasSuccessful() else 1


if __name__=='__main__':
    raise SystemExit(main())
