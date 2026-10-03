#!/usr/bin/env python3
"""Portable proposed path-byte work oracle; no SLIM source parsing or acceptance.

Import defines finite helpers and resolves ROOT, without loading the old oracle,
writing files or launching children. model() checks the immutable case oracle
and full fixture-data hash before deriving separately pinned path expectations.
Only main() can explicitly publish a fresh bounded file.
"""
from pathlib import Path
import argparse,hashlib,json,math,sys
from types import ModuleType
ROOT=next(parent for parent in Path(__file__).resolve().parents
          if (parent/'design/FEATURE_POLICY.md').is_file() and (parent/'scripts/manifest-validation-oracle.py').is_file())
OLD_SHA='492508410de8ed99455f2ab44066449556664e2f6aa316333482a5722a768cf7'
FIXTURE_SHA='8589f7476ab274ceac3273db35d0cb6fa5cd1f27405ca9a612994e8c671fa82f'
MODEL_LIMIT=131072

def load_fixed_data(root):
 path=Path(root)/'scripts/manifest-validation-oracle.py'
 source=path.read_bytes()
 if hashlib.sha256(source).hexdigest()!=OLD_SHA:
  raise ValueError('fixed case oracle identity drift before import')
 data=ModuleType('fixed_manifest_case_data')
 data.__file__=str(path)
 sys.modules[data.__name__]=data
 # Execute exactly the pinned bytes, without import-loader bytecode writes.
 exec(compile(source,str(path),'exec'),data.__dict__)
 return data

CAP=1000000000
COUNTERS=('builder_module_headers','insert_calls','insertion_character_headers','edge_headers','nodes_appended','edges_appended','terminal_value_reads')
ELIGIBILITY=('preflight_headers','preflight_token_reads','preflight_span_checks')
def add(counts,name):
 value=counts[name]
 if type(value) is not int or not 0<=value<CAP:raise ValueError('counter admission')
 counts[name]=value+1
def total(values):
 result=0
 for value in values:
  if type(value) is not int or not 0<=value<=CAP or result>CAP-value:raise ValueError('sum admission')
  result+=value
 return result
def product(left,right):
 if type(left) is not int or type(right) is not int or not 0<=left<=CAP or not 0<=right<=CAP:raise ValueError('product operands')
 if left and right>CAP//left:raise ValueError('product admission')
 return left*right
class PathBytes:
 def __init__(self):
  self.counts=dict.fromkeys(COUNTERS,0);self.nodes=[];self.node()
 def node(self):
  add(self.counts,'nodes_appended');self.nodes.append([-1,[]]);return len(self.nodes)-1
 def insert(self,key,value):
  add(self.counts,'insert_calls');at=0
  for code in key:
   add(self.counts,'insertion_character_headers');child=-1
   for candidate,target in self.nodes[at][1]:
    add(self.counts,'edge_headers')
    if candidate==code:child=target;break
   if child<0:
    add(self.counts,'edge_headers');child=self.node();add(self.counts,'edges_appended')
    self.nodes[at][1].insert(0,(code,child))
   at=child
  add(self.counts,'insertion_character_headers');add(self.counts,'terminal_value_reads')
  previous=self.nodes[at][0]
  if previous<0:self.nodes[at][0]=value
  return previous
def model(root=ROOT):
 data=load_fixed_data(root)
 original={'schema':1,'authority':'finite-measurement-data-only','counter_cap':CAP,
           'counts':{'geometric':29,'control':12,'malformed':25},
           'cases':[data.record(case) for case in data.cases()]}
 fixed=(json.dumps(original,sort_keys=True,indent=2)+'\n').encode()
 if len(fixed)>1048576 or hashlib.sha256(fixed).hexdigest()!=FIXTURE_SHA:
  raise ValueError('fixed fixture oracle drift before deriving expectations')
 if total((1,total(total((1,len(row['files']))) for row in original['cases'])))!=3456:
  raise ValueError('fixed fixture file-count drift')
 rows=[]
 for case in data.cases():
  row=data.record(case); legacy=row['legacy']; model=None
  if case.gate not in ('entry','order','malformed'):
   ext=row['extents'];n,e,bp=ext['N'],ext['E'],ext['Bp'];trie=PathBytes();cursor=7
   for index,module in enumerate(case.modules):
    add(trie.counts,'builder_module_headers');previous=trie.insert(module.path,total((cursor,3)))
    if previous>=0:
     if index!=case.path_stop:raise ValueError('fixed duplicate plan mismatch')
     break
    if index==case.path_stop:raise ValueError('fixed duplicate plan absent')
    cursor=total((cursor,11,len(module.imports),len(module.exports)))
   else:add(trie.counts,'builder_module_headers')
   eligibility=dict(zip(ELIGIBILITY,(total((product(2,n),e,1)),total((product(12,n),e,5)),total((product(2,n),e)))))
   w=total(trie.counts.values());local=total((w,total(eligibility.values())))
   work_bound=total((product(260,bp),product(4,n),2));local_bound=total((product(260,bp),product(20,n),product(3,e),8))
   envelope=product(800,total((ext['B'],n,e,1)))
   if not 0<=local<=local_bound<=envelope<=CAP or w>work_bound:raise ValueError('checked finite work admission')
   model={'scope':'exact-fixed-path-byte-geometry','work':trie.counts,'eligibility':eligibility,'W':w,'local_work':local,'work_bound':work_bound,'local_bound':local_bound,'ceiling':envelope}
   # Only the old path scan is replaced. Membership and every cycle unit remain.
   legacy={'work':dict(legacy['work']),'trace':legacy['trace']}
   for key in ('path_module_headers','prior_path_headers','path_pairs'):legacy['work'][key]=0
  rows.append({'id':row['id'],'family':row['family'],'extents':row['extents'],'manifest_sha256':row['manifest_sha256'],
               'scope':'not-reached' if case.gate in ('entry','order') else ('malformed-work-unknown' if case.gate=='malformed' else 'eligible'),
               'path_only':model,'legacy':legacy})
 exponents={}
 for family,count in {'paths':5,'duplicate':5,'front':5,'back':5,'edges':5,'bytes':4}.items():
  points=[]
  for row in rows:
   if row['family']==family:
    ext=row['extents'];points.append({'id':row['id'],'X':total((ext['B'],ext['N'],ext['E'])),'local_work':row['path_only']['local_work']})
  if len(points)!=count or any(a['X']>=b['X'] for a,b in zip(points,points[1:])):raise ValueError('geometric domain')
  first,last=points[0],points[-1]
  if not 0<first['local_work']<=CAP or not 0<last['local_work']<=CAP or not 0<first['X']<last['X']<=CAP:
   raise ValueError('exponent operand admission')
  exponent=math.log(last['local_work']/first['local_work'])/math.log(last['X']/first['X'])
  if not math.isfinite(exponent) or exponent>1.15:raise ValueError('unchanged prospective exponent gate')
  exponents[family]={'points':points,'endpoint_exponent':exponent,'budget':1.15}
 payload={'status':'proposal-model-only','compiler_acceptance_authority':'unchanged production SLIM checker','source_oracle_sha256':OLD_SHA,
          'fixture_data_sha256':FIXTURE_SHA,
          'failed_original_receipt_sha256':'d3192ca6668fe89db0f3c7a012f1884767bdfafd8e4c78e5fa7cf8fad2059ba7',
          'counts':{'structured':41,'malformed':25,'geometric':29},'counter_cap':CAP,'rows':rows,'exponents':exponents,
          'unchanged_budgets':{'exponent':1.15,'same_host_ratio':1.10,'envelope_coefficient':800},
          'unknown':['new production/native counts','source/diagnostic/C parity','same-host ratios','retained controls','resource/RSS/allocation measurements']}
 return payload

def encoded_model(root=ROOT):
 result=(json.dumps(model(root),sort_keys=True,indent=2)+'\n').encode()
 if len(result)>MODEL_LIMIT:raise ValueError('derived model publication cap')
 return result

def main():
 parser=argparse.ArgumentParser(description=__doc__)
 parser.add_argument('--root',type=Path,default=ROOT)
 parser.add_argument('--output',type=Path,help='explicit fresh output file; default stdout')
 args=parser.parse_args();result=encoded_model(args.root)
 if args.output is None:sys.stdout.buffer.write(result)
 else:
  with args.output.open('xb') as output:output.write(result)
  print(json.dumps({'sha256':hashlib.sha256(result).hexdigest(),'bytes':len(result)},sort_keys=True))

if __name__=='__main__':main()
