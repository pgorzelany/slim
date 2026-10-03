#!/usr/bin/env python3
"""RFC170 independent pure-data oracle; import performs no I/O or native work.

Creates declared report bytes and expected projections, never decodes a report
or SLIM/manifest source. Fixed RFC168 mathematics supplies independent expected
data. A current campaign seals these bytes before loading its collector. Current
opaque source bodies and direct tools are pinned; data is never source authority.
The collector source is observed as opaque custody bytes and is never loaded here.
"""
from datetime import datetime, timezone
from dataclasses import dataclass, replace
import hashlib
import json
import os
from pathlib import Path
import stat
import sys
import time
import types

MIB = 1048576
REPORT_CAP = 8*MIB
JSON_CAP = 24*MIB
COUNTER_CAP = 1000000000
MODEL_CAP = 4*MIB
FIXTURE_CAP = 16*MIB
FILE_CAP = 128
ADAPTER_SHA = '004b176cdd01aba975e53f3292cd13a759bba645831279c42fa0dabcb8003c00'
MANIFEST_PINS = {
    'project-input.project':'db5881e40b6ab96f08872318d8eac640a72c911e40b401dad662fa3a6e2263c0',
    'library/project-impact.project':'86a2c62e3e631b35b7b38b23d6f69c994fea38b70e0ab5cbc1dc7d0fec9f5b68',
}
H0 = b'0'*64
H1 = b'1'*64
NAME = frozenset(b'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-')
HEX = frozenset(b'0123456789abcdef')
ROLES = ('emit-producer','build-producer','emit-impact','build-impact',
         'capture-before','capture-after','impact')
REGISTRY = '''library/applications/catalog/catalog.slim
library/applications/catalog/emit.slim
library/applications/catalog/model.slim
library/applications/catalog/reconcile.slim
library/applications/project_impact/closure.slim
library/applications/project_impact/data.slim
library/applications/project_impact/input.slim
library/applications/project_impact/main.slim
library/applications/project_impact/model.slim
library/applications/project_impact/prepare.slim
library/applications/project_impact/report.slim
library/applications/project_input/main.slim
library/applications/workplan/load.slim
library/applications/workplan/model.slim
library/components/project_input_data.slim
library/components/project_input_emit.slim
library/components/project_input_limits.slim
library/components/project_input_model.slim
library/components/records.slim
library/experimental/ascii.slim
library/experimental/byte_index.slim
library/experimental/bytes.slim
library/experimental/decimal.slim
library/experimental/netstring.slim
library/experimental/text.slim
library/project-impact.project
project-input.project
runtime/slim_rt.c
runtime/slim_rt.h
scripts/project-impact-context.py
scripts/project-input-inventory.py
selfhost/check.slim
selfhost/codegen.slim
selfhost/control.slim
selfhost/diagnostics.slim
selfhost/effects.slim
selfhost/format.slim
selfhost/identity.slim
selfhost/ir.slim
selfhost/memory.slim
selfhost/ownership.slim
selfhost/parallel.slim
selfhost/project.slim
selfhost/ranges.slim
selfhost/retained.slim
selfhost/scheduler.slim
selfhost/syntax.slim
selfhost/text.slim
selfhost/typing.slim
selfhost/validate.slim'''.splitlines()
# Fixed body paths only. Current bytes are captured before helper/collector load.
SOURCE_PATHS = '''library/applications/catalog/catalog.slim
library/applications/catalog/diff_emit.slim
library/applications/catalog/emit.slim
library/applications/catalog/main.slim
library/applications/catalog/model.slim
library/applications/catalog/reconcile.slim
library/applications/project_impact/closure.slim
library/applications/project_impact/data.slim
library/applications/project_impact/input.slim
library/applications/project_impact/main.slim
library/applications/project_impact/model.slim
library/applications/project_impact/prepare.slim
library/applications/project_impact/report.slim
library/applications/project_input/main.slim
library/applications/workplan/load.slim
library/applications/workplan/model.slim
library/catalog.project
library/components/project_input_data.slim
library/components/project_input_emit.slim
library/components/project_input_limits.slim
library/components/project_input_model.slim
library/components/records.slim
library/experimental/ascii.slim
library/experimental/byte_index.slim
library/experimental/bytes.slim
library/experimental/decimal.slim
library/experimental/netstring.slim
library/experimental/text.slim
library/project-impact.project
project-input.project
runtime/slim_rt.c
runtime/slim_rt.h
scripts/project-input-inventory.py
selfhost/analysis.slim
selfhost/cache.slim
selfhost/check.slim
selfhost/codegen.slim
selfhost/context.slim
selfhost/control.slim
selfhost/diagnostics.slim
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
selfhost/slim.project
selfhost/slimc.slim
selfhost/syntax.slim
selfhost/text.slim
selfhost/typing.slim
selfhost/validate.slim'''.splitlines()
UNKNOWN = {
 'application_invariants':'not analyzed', 'agent_effectiveness':'not measured',
 'context_sufficiency':'declared-import selection is not a sufficiency proof',
 'saved_compiler_work':'not measured',
 'physical_source_deduplication':'counts and weights count supplied catalog rows',
 'atomic_live_capture':'no atomic or final-live identity attestation',
 'loaded_code_aba_host_boot':'before/after observed bytes only',
 'transitive_toolchain':'direct tools pinned; SDK/linker/transitive identity not established',
 'physical_memory':'logical caps are not RSS or libc allocation bounds',
}

def require(value, reason):
    if not value:
        raise ValueError(reason)

def sha(data):
    return hashlib.sha256(data).hexdigest()

def canonical(value):
    return json.dumps(value,sort_keys=True,separators=(',',':'),
                      ensure_ascii=True,allow_nan=False).encode('ascii')+b'\n'

def identity(data):
    return {'bytes':len(data),'sha256':sha(data)}

def frame(data):
    require(type(data) is bytes and len(data)<=MIB,'declared frame bound')
    return str(len(data)).encode()+b':'+data+b','

def admitted(value, cap):
    return type(value) is int and 0<=value<=cap

def addition(left, right, cap=COUNTER_CAP):
    if not admitted(left,cap) or not admitted(right,cap) or right>cap-left:
        return None
    return left+right

def product(left, right, cap=COUNTER_CAP):
    if not admitted(left,cap) or not admitted(right,cap):
        return None
    if left and right>cap//left:
        return None
    return left*right

@dataclass(frozen=True)
class Record:
    name: bytes
    weight: int = 0
    path: bytes = b'a.slim'
    digest: bytes = H0

    def fields(self):
        return self.name,str(self.weight).encode(),self.path+b'\0'+self.digest

    def projected(self):
        return {'name_hex':self.name.hex(),'bytes':self.weight,
                'path_hex':self.path.hex(),'source_sha256':self.digest.decode('ascii')}

@dataclass(frozen=True)
class Change:
    kind: bytes
    name: bytes
    before: object
    after: object

@dataclass(frozen=True)
class GraphChange:
    name: bytes
    before: bytes
    after: bytes

@dataclass(frozen=True)
class Declared:
    manifest: bool = False
    before: Record = Record(b'@project',path=b'slim.project')
    after: Record = Record(b'@project',path=b'slim.project')
    changes: tuple = ()
    graph: tuple = ()
    lists: tuple = ((),(),(),())
    candidates: tuple = ()

class Wire:
    def __init__(self):
        self.data = bytearray()
        self.positions = {}

    def append(self, label, value, nested=None):
        start=len(self.data)
        prefix=str(len(value)).encode()+b':'
        self.positions[label]={'header':start,'payload':start+len(prefix),
                               'end':start+len(prefix)+len(value)+1}
        if nested is not None:
            for key,position in nested.items():
                self.positions[label+'.'+key]={part:offset+start+len(prefix)
                                               for part,offset in position.items()}
        self.data.extend(frame(value))

def record_wire(record):
    wire=Wire()
    for key,value in zip(('key','weight','value'),record.fields()):
        wire.append(key,value)
    return bytes(wire.data),wire.positions

def report_wire(value, extras=None):
    extras=extras or {}
    wire=Wire()
    def nested(label,row):
        payload,positions=record_wire(row) if row is not None else (b'',{})
        wire.append(label,payload+extras.get(label,b''),positions)
        return len(payload)
    wire.append('tag',b'slim-project-impact-1')
    wire.append('flag',b'1' if value.manifest else b'0')
    nested('before-project',value.before)
    nested('after-project',value.after)
    wire.append('Dcount',str(len(value.changes)).encode())
    for i,row in enumerate(value.changes):
        label='D.'+str(i)
        wire.append(label+'.kind',row.kind)
        wire.append(label+'.key',row.name)
        nested(label+'.old',row.before)
        nested(label+'.new',row.after)
    wire.append('Gcount',str(len(value.graph)).encode())
    for i,row in enumerate(value.graph):
        for key,payload in zip(('key','old','new'),(row.name,row.before,row.after)):
            wire.append('G.'+str(i)+'.'+key,payload)
    for label,items in zip(('old-roots','new-roots','old-closure','new-closure'),value.lists):
        wire.append(label+'.count',str(len(items)).encode())
        for i,key in enumerate(items):
            wire.append(label+'.'+str(i),key)
    wire.append('Ccount',str(len(value.candidates)).encode())
    for i,row in enumerate(value.candidates):
        nested('C.'+str(i),row)
    return bytes(wire.data),wire.positions

def positive_domain(value):
    require(type(value.manifest) is bool,'literal Boolean')
    require(len(value.changes)<=8190 and len(value.graph)<=4095,'literal row caps')
    rows=[value.before,value.after,*value.candidates]
    for change in value.changes:
        require(change.kind in (b'added',b'removed',b'modified'),'literal kind')
        require((change.before is None)==(change.kind==b'added'),'literal old slot')
        require((change.after is None)==(change.kind==b'removed'),'literal new slot')
        for row in (change.before,change.after):
            if row is not None:
                require(row.name==change.name,'literal nested association')
                rows.append(row)
    for row in rows:
        require(admitted(row.weight,MIB),'literal weight')
        require(1<=len(row.path)<=256 and b'\0' not in row.path,'literal path')
        require(len(row.digest)==64 and set(row.digest)<=HEX,'literal digest')
        if row.name==b'@project':
            require(row.path==b'slim.project','literal reserved path')
        else:
            require(1<=len(row.name)<=64 and set(row.name)<=NAME,'literal name')
    for items in (tuple(row.name for row in value.changes),
                  tuple(row.name for row in value.graph),*value.lists,
                  tuple(row.name for row in value.candidates)):
        require(len(items)<=8190 and all(left<right for left,right in zip(items,items[1:])),
                'literal strict order')
        require(all(1<=len(item)<=64 and set(item)<=NAME for item in items),
                'literal ordered key domain')
    require(all(len(items)<=4095 for items in value.lists) and len(value.candidates)<=4095,
            'literal list caps')

def evidence():
    ident={'bytes':0,'sha256':'0'*64}
    return {'collector_sha256':'0'*64,'inventory_adapter_sha256':ADAPTER_SHA,
      'source_set_sha256':'0'*64,'compiler':dict(ident),'cc':dict(ident),
      'tool_arguments_hex':{'compiler':b'/compiler'.hex(),'cc':b'/cc'.hex()},
      'producer':{'c':dict(ident),'executable':dict(ident)},
      'impact':{'c':dict(ident),'executable':dict(ident)},
      'before_capture':dict(ident),'after_capture':dict(ident),
      'inputs':{key:dict(ident) for key in ('before_catalog','before_graph','after_catalog','after_graph')},
      'report':dict(ident),'before_project_argument_hex':b'/before.project'.hex(),
      'after_project_argument_hex':b'/after.project'.hex(),'successful_roles':list(ROLES)}

def declared_facts(value, before_modules=0, after_modules=0, before_edges=0,
                   after_edges=0, before_weight=0, after_weight=0):
    return {'before_module_count':before_modules,'after_module_count':after_modules,
      'before_direct_import_edges':before_edges,'after_direct_import_edges':after_edges,
      'before_manifest_bytes':value.before.weight,'after_manifest_bytes':value.after.weight,
      'before_module_catalog_weight_bytes':before_weight,'after_module_catalog_weight_bytes':after_weight,
      'module_change_count':len(value.changes),'import_change_count':len(value.graph),
      'before_root_count':len(value.lists[0]),'after_root_count':len(value.lists[1]),
      'before_closure_count':len(value.lists[2]),'after_closure_count':len(value.lists[3]),
      'current_candidate_count':len(value.candidates),
      'current_candidate_catalog_weight_bytes':guarded_sum(tuple(row.weight for row in value.candidates))}

def guarded_sum(values):
    total=0
    for value in values:
        next_value=addition(total,value)
        require(next_value is not None,'prospective guarded sum')
        total=next_value
    return total

def projection(value, facts, observed=None):
    # Expected data from literal objects; deliberately no report decoder.
    positive_domain(value)
    require(len(facts)==16 and all(admitted(item,COUNTER_CAP) for item in facts.values()),
            'expected fact admission')
    observed=evidence() if observed is None else observed
    obj={'format':'slim-project-impact-context-1','evidence':observed,'facts':facts,
      'impact':{'manifest_changed':value.manifest,'before_project':value.before.projected(),
        'after_project':value.after.projected(),
        'module_changes':[{'change':row.kind.decode(),'name_hex':row.name.hex(),
            'before':None if row.before is None else row.before.projected(),
            'after':None if row.after is None else row.after.projected()} for row in value.changes],
        'import_changes':[{'name_hex':row.name.hex(),'before_dependencies_hex':row.before.hex(),
                          'after_dependencies_hex':row.after.hex()} for row in value.graph],
        **{key:[item.hex() for item in items] for key,items in zip(
          ('before_roots_hex','after_roots_hex','before_closure_hex','after_closure_hex'),value.lists)}},
      'selection':{'snapshot':'after','catalog_sha256':observed['inputs']['after_catalog']['sha256'],
          'graph_sha256':observed['inputs']['after_graph']['sha256'],
          'candidates':[row.projected() for row in value.candidates]},'unknown':dict(UNKNOWN)}
    data=canonical(obj)
    require(len(data)<=JSON_CAP,'expected JSON cap')
    return data

def positive_cases():
    a=Record(b'a'); b=Record(b'b'); changed=replace(a,digest=H1)
    both=((b'a',),(b'a',),(b'a',),(b'a',))
    single=Declared(changes=(Change(b'modified',b'a',a,changed),),lists=both,candidates=(changed,))
    rows=[('P01-empty',Declared(),(0,0,0,0,0,0)),
          ('P02-unchanged',Declared(),(1,1,0,0,0,0))]
    weighted=replace(a,weight=1)
    rows += [('P03-weight',replace(single,changes=(Change(b'modified',b'a',a,weighted),),
                              candidates=(weighted,)),(1,1,0,0,0,1)),
             ('P04-digest',single,(1,1,0,0,0,0))]
    binary=replace(a,path=b'x\xff\n.slim')
    rows += [('P05-binary-path',replace(single,changes=(Change(b'modified',b'a',a,binary),),
                               candidates=(binary,)),(1,1,0,0,0,0)),
      ('P06-added',Declared(changes=(Change(b'added',b'a',None,a),),
          lists=((),(b'a',),(),(b'a',)),candidates=(a,)),(0,1,0,0,0,0)),
      ('P07-removed',Declared(changes=(Change(b'removed',b'a',a,None),),
          graph=(GraphChange(b'b',b'a',b''),),lists=((b'a',b'b'),(b'b',),(b'a',b'b'),(b'b',)),
          candidates=(b,)),(2,1,1,0,0,0)),
      ('P08-rename',Declared(changes=(Change(b'removed',b'a',a,None),Change(b'added',b'b',None,b)),
          lists=((b'a',),(b'b',),(b'a',),(b'b',)),candidates=(b,)),(1,1,0,0,0,0))]
    graph=Declared(graph=(GraphChange(b'b',b'a',b''),),
        lists=((b'b',),)*4,candidates=(b,))
    rows += [('P09-graph',graph,(2,2,1,0,0,0)),
      ('P10-manifest',Declared(manifest=True,after=replace(Declared().after,digest=H1),
          candidates=(a,b)),(2,2,0,0,0,0)),
      ('P11-cycle',replace(single,lists=((b'a',),(b'a',),(b'a',b'b'),(b'a',b'b')),
          candidates=(changed,b)),(2,2,2,2,0,0)),
      ('P12-empty-deps',graph,(2,2,1,0,0,0))]
    maximum=Record(b'a'*64,MIB,b'x'*256)
    oldmax=replace(maximum,digest=H1)
    rows += [('P13-maximum-record',Declared(changes=(Change(b'modified',maximum.name,oldmax,maximum),),
         lists=((maximum.name,),)*4,candidates=(maximum,)),(1,1,0,0,MIB,MIB))]
    newb=replace(b,digest=H1)
    rows += [('P14-overlap',Declared(changes=(Change(b'modified',b'a',a,changed),
        Change(b'modified',b'b',b,newb)),graph=(GraphChange(b'b',b'a',b''),),
        lists=((b'a',b'b'),)*4,candidates=(changed,newb)),(2,2,1,0,0,0))]
    return rows

def geometric_cases():
    rows=[]
    for i,n in enumerate((16,32,64,128,256),1):
        items=tuple(Record(('m'+str(j).zfill(4)).encode()) for j in range(n))
        rows.append(('G0'+str(i),Declared(manifest=True,
            after=replace(Declared().after,digest=H1),candidates=items),(n,n,0,0,0,0)))
    for i,p in enumerate((32,64,128,256),6):
        items=tuple(Record(('m'+str(j).zfill(4)).encode(),path=b'x'*p) for j in range(16))
        rows.append(('G0'+str(i),Declared(manifest=True,
            after=replace(Declared().after,digest=H1),candidates=items),(16,16,0,0,0,0)))
    return rows

def overflow_digit(digits, maximum):
    previous=0
    for i,byte in enumerate(digits):
        digit=byte-48
        if digit>maximum or previous>(maximum-digit)//10:
            return i
        previous=previous*10+digit
    raise ValueError('declared overflow absent')

def replace_field(data, positions, key, value):
    position=positions[key]
    return data[:position['header']]+frame(value)+data[position['end']:]

def negative_cases(positive):
    bases={name:value for name,value,_ in positive}
    result=[]
    def add(name,value,code,key=None,part='payload',extra=None,tail=b''):
        data,positions=report_wire(value,extra)
        position=len(data) if key is None else positions[key][part]
        result.append({'name':name,'kind':'report','data':data+tail,
          'expected':{'exit':65,'stage':'report','code':code,'position':position}})
    base=bases['P01-empty']; data,pos=report_wire(base)
    def raw(name,rawdata,code,position):
        result.append({'name':name,'kind':'report','data':rawdata,
          'expected':{'exit':65,'stage':'report','code':code,'position':position}})
    raw('N01-version',replace_field(data,pos,'tag',b'slim-project-impact-2'),'report-format',pos['tag']['payload'])
    raw('N02-leading-zero',b'0'+data,'leading-zero',1)
    shortened=data[:pos['tag']['payload']+20]
    raw('N03-truncated',shortened,'framing-truncated',len(shortened))
    colon=pos['tag']['payload']-1
    raw('N04-header',data[:colon]+b'x'+data[colon+1:],'framing-header',colon)
    comma=pos['tag']['end']-1
    raw('N05-comma',data[:comma]+b'x'+data[comma+1:],'framing-comma',comma)
    raw('N06-payload-limit',b'65:'+data[pos['tag']['payload']:],'payload-limit',1)
    raw('N07-flag',replace_field(data,pos,'flag',b'2'),'report-control',pos['flag']['payload'])
    single=bases['P03-weight']; change=single.changes[0]
    add('N08-nested-key',replace(single,changes=(replace(change,before=replace(change.before,name=b'b')),)),
        'report-record','D.0.old.key')
    add('N09-reserved-path',replace(base,before=replace(base.before,path=b'x.slim')),
        'report-record','before-project.value')
    for name,key,value,cap in (('N10-empty-count','Dcount',b'',None),
        ('N11-count-leading-zero','Dcount',b'00',None),('N12-D-limit','Dcount',b'8191',8190),
        ('N13-G-limit','Gcount',b'4096',4095),('N14-list-limit','old-roots.count',b'4096',4095),
        ('N15-candidate-limit','Ccount',b'4096',4095)):
        position=pos[key]['payload']+(0 if cap is None else overflow_digit(value,cap))
        raw(name,replace_field(data,pos,key,value),'number-canonical' if cap is None else 'number-limit',position)
    overlap=bases['P14-overlap']
    add('N16-D-duplicate',replace(overlap,changes=(overlap.changes[0],overlap.changes[0])),
        'report-order','D.1.key')
    add('N17-D-descending',replace(overlap,changes=tuple(reversed(overlap.changes))),
        'report-order','D.1.key')
    add('N18-G-duplicate',replace(base,graph=(GraphChange(b'a',b'',b''),)*2),'report-order','G.1.key')
    add('N19-list-duplicate',replace(base,lists=((b'a',b'a'),(),(),())),
        'report-order','old-roots.1')
    add('N20-candidate-duplicate',replace(base,candidates=(Record(b'a'),)*2),'report-order','C.1.key')
    add('N21-classification',replace(single,changes=(replace(change,kind=b'other'),)),
        'report-control','D.0.kind')
    added=bases['P06-added']; row=added.changes[0]
    add('N22-added-old-present',replace(added,changes=(replace(row,before=row.after),)),
        'report-control','D.0.old')
    removed=bases['P07-removed']; row=removed.changes[0]
    add('N23-removed-new-present',replace(removed,changes=(replace(row,after=row.before),)),
        'report-control','D.0.new')
    invalid,p=report_wire(single,{'D.0.old':b'x'})
    triple,_=record_wire(change.before)
    raw('N24-nested-trailing',invalid,'report-end',p['D.0.old']['payload']+len(triple))
    add('N25-digest',replace(single,changes=(replace(change,before=replace(change.before,digest=b'g'+H0[1:])),)),
        'report-record','D.0.old.value')
    add('N26-outer-trailing',base,'report-end',tail=b'x')
    require(len(result)==26,'negative case cardinality')
    return result

def scalar_cases():
    rows=[]
    for prefix,kind,cap,code in (('B0','report-admission',REPORT_CAP,'report-limit'),
                                ('B0','json-admission',JSON_CAP,'json-limit')):
        for delta in (-1,0,1):
            name='B'+str(len(rows)+1).zfill(2)
            rows.append({'name':name,'kind':kind,'value':cap+delta,'cap':cap,
                         'expected':{'admitted':delta<=0,'code':None if delta<=0 else code,'position':0}})
    values=(COUNTER_CAP-1,COUNTER_CAP,COUNTER_CAP+1,18446744073709551615,True,-1)
    for i,value in enumerate(values,1):
        ok=admitted(value,COUNTER_CAP)
        rows.append({'name':'A'+str(i).zfill(2),'kind':'integer-admission','value':value,
          'cap':COUNTER_CAP,'expected':{'admitted':ok,'code':None if ok else 'arithmetic-limit','position':0}})
    for i,(left,right,method) in enumerate(((COUNTER_CAP-2,1,addition),(COUNTER_CAP-1,1,addition),
      (COUNTER_CAP,1,addition),(COUNTER_CAP-1,1,product),(COUNTER_CAP,1,product),(COUNTER_CAP,2,product)),7):
        result=method(left,right)
        rows.append({'name':'A'+str(i).zfill(2),'kind':'addition' if i<10 else 'product',
          'left':left,'right':right,'cap':COUNTER_CAP,
          'expected':{'value':result,'admitted':result is not None,
                      'code':None if result is not None else 'arithmetic-limit','position':0}})
    return rows

def boundary_cases(maximum):
    rows=[]
    for i,length in enumerate((63,64,65),7):
        name=b'a'*length; old=replace(maximum.changes[0].before,name=name)
        new=replace(maximum.changes[0].after,name=name)
        value=replace(maximum,changes=(Change(b'modified',name,old,new),),
                      lists=((name,),)*4,candidates=(new,))
        data,pos=report_wire(value)
        expected={'exit':0} if length<=64 else {'exit':65,'stage':'report','code':'payload-limit',
            'position':pos['D.0.key']['header']+overflow_digit(str(length).encode(),64)}
        rows.append({'name':'B'+str(i).zfill(2),'kind':'report','data':data,'expected':expected,
                     'projection':projection(value,declared_facts(value,1,1,0,0,MIB,MIB)) if length<=64 else None})
    for i,length in enumerate((255,256,257),10):
        old=replace(maximum.changes[0].before,path=b'x'*length)
        new=replace(maximum.changes[0].after,path=b'x'*length)
        value=replace(maximum,changes=(Change(b'modified',old.name,old,new),),candidates=(new,))
        data,pos=report_wire(value)
        expected={'exit':0} if length<=256 else {'exit':65,'stage':'report','code':'report-record',
                                                'position':pos['D.0.old.value']['payload']}
        rows.append({'name':'B'+str(i).zfill(2),'kind':'report','data':data,'expected':expected,
                     'projection':projection(value,declared_facts(value,1,1,0,0,MIB,MIB)) if length<=256 else None})
    return rows

def artifact_names():
    return tuple(name for i in range(1,8) for name in
      ('phase-'+str(i).zfill(2)+'.stdout.bin','phase-'+str(i).zfill(2)+'.stderr.bin',
       'phase-'+str(i).zfill(2)+'.json'))+('producer','impact','before-catalog.ns','before-graph.ns',
       'after-catalog.ns','after-graph.ns','before-inventory.json','after-inventory.json',
       'sources-before.json','sources-after.json','tools-before.json','tools-after.json','context.json','receipt.json')

def native_plan():
    cases=[(name,7) for name in ('real-unchanged-tiny','real-changed-tiny','real-deletion-rename-tiny',
         'real-manifest-widen-tiny','real-compiler-body-newline','real-catalog-body-newline')]
    cases += [('cleanup-descendant',1),('invalid-before',5),('invalid-after',6),
      ('fail-emit-producer',1),('fail-build-producer',2),('fail-emit-impact',3),('fail-build-impact',4),
      ('timeout-impact',7),('output-limit-impact',7),('drift-source',7),('malformed-report',7),('fail-publication',7)]
    labels=[name+'/'+('control' if name=='cleanup-descendant' else str(i).zfill(2))
            for name,count in cases for i in range(1,count+1)]
    require(len(labels)==99 and len(set(labels))==99,'native99 labels')
    def failure(exitcode,code,stage,position=0):
        return {'exit':exitcode,'code':code,'stage':stage,'position':position,'stdout_hex':'',
                'stderr_hex':('project-impact-context: '+code+' in '+stage+' at '+str(position)+'\n').encode().hex()}
    outcomes={name:{'exit':0,'stdout_hex':b'project-impact-context: ok\n'.hex(),
        'stderr_hex':'','context':'exact held source-pair projection with measured evidence'} for name,_ in cases[:6]}
    outcomes['cleanup-descendant']={'leader_complete':True,'descendant_absent':True,'within_deadlines':True}
    for name,count in cases[7:13]:
        outcomes[name]=failure(74,'native-incomplete',ROLES[count-1])
        outcomes[name]['process_status']='native-error'
        outcomes[name]['returncode']=65 if name.startswith('invalid-') else 7
    outcomes['timeout-impact']=failure(74,'native-incomplete','impact')
    outcomes['timeout-impact']['process_status']='timeout'
    outcomes['output-limit-impact']=failure(74,'native-incomplete','impact')
    outcomes['output-limit-impact']['process_status']='output-limit'
    outcomes['drift-source']=failure(65,'source-identity','sources')
    ordinary,_=report_wire(Declared())
    outcomes['malformed-report']=failure(65,'report-end','report',len(ordinary))
    outcomes['fail-publication']=failure(73,'publication-error','output')
    outcomes['fail-publication']['complete_receipt_present']=False
    outcomes['fail-publication']['context_present']=True
    zero=('missing-before','extra-positional','unknown-option','existing-output',
          'symlink-output','outside-build','tool-output-alias','input-output-alias')
    return {'cases':[{'name':name,'children':count,
                       'unexecuted_roles':[] if name=='cleanup-descendant' else list(ROLES[count:]),
                       'expected':outcomes[name]} for name,count in cases],
      'labels':labels,'whole_seconds':900,'child_seconds':60,
      'fixed_fault_data':{'failed_tool_returncode':7,'failed_tool_stdout_hex':'',
         'failed_tool_stderr_hex':b'fixed-control\n'.hex(),'oversized_stdout_bytes':REPORT_CAP+1,
         'timeout_duration_seconds':61,'source_drift':'one LF appended to copied registered source',
         'malformed_report_case':'N26-outer-trailing','partial_publication':'isolated harness write fault before final receipt'},
      'zero_native':[{'name':name,'expected':failure(64,'invalid-arguments','arguments') if i<3
                    else failure(73,'unsafe-output','output')} for i,name in enumerate(zero)]}

def geometry(value, data):
    d,g,c=len(value.changes),len(value.graph),len(value.candidates)
    l=sum(len(items) for items in value.lists)
    outer=11+4*d+3*g+l+c
    nested=6+3*sum(row.before is not None for row in value.changes)+3*sum(row.after is not None for row in value.changes)+3*c
    return {'report_bytes':len(data),'D':d,'G':g,'C':c,'L':l,'outer_frames':outer,
            'nested_frames':nested,'envelope':2*len(data)+256*d+128*g+128*c+16*l+262144}

def overhead_proof():
    for n in (1,63,64):
        for p in (1,255,256):
            for weight in (0,1,MIB):
                row=Record(b'a'*n,weight,b'x'*p)
                width=len(str(weight))
                encoded=canonical(row.projected())[:-1]
                require(len(encoded)==121+width+2*n+2*p,'record exact fixed overhead')
                payload=n+width+p+65
                require(len(encoded)<=2*payload,'record covered by2payload')
    strings,integers=131,29
    # Independent peer's complete fixed-skeleton occurrence count is preserved;
    # the complete JSON object below independently checks the higher byte bound.
    header=evidence()
    for key in ('before_project_argument_hex','after_project_argument_hex'):
        header[key]='ff'*4096
    for key in ('compiler','cc'):
        header['tool_arguments_hex'][key]='ff'*4096
    header_json=projection(Declared(),{key:COUNTER_CAP for key in declared_facts(Declared())},header)
    ceiling=256*256+64*10+4*8192+8192
    maximum=2*REPORT_CAP+256*8190+128*4095+128*4095+16*(4*4095)+262144
    require(ceiling==107136 and maximum==20446400 and maximum<JSON_CAP,'literal envelope arithmetic')
    require(len(header_json)<262144,'complete max-argument header bytes')
    require(len(REGISTRY)==50 and len(set(REGISTRY))==50,'literal registry50')
    require(len(artifact_names())==35 and len(set(artifact_names()))==35,'artifact35')
    return {'record_fixed':121,'D_row_extra':47,'G_row_extra':73,'candidate_comma':1,
      'name_punctuation':3,'header_fixed_strings':strings,'header_integers':integers,
      'header_integer_and_string_ceiling':ceiling,'max_envelope':maximum,
      'max_argument_header_bytes':len(header_json),
      'max_outer_frames':65531,'max_nested_frames':61431,'artifact_names':artifact_names()}

def data_rows():
    positives=positive_cases()
    rows=[]
    for name,value,dimensions in positives+geometric_cases():
        positive_domain(value)
        data,positions=report_wire(value)
        require(len(data)<=REPORT_CAP,'literal successful report cap')
        context=projection(value,declared_facts(value,*dimensions))
        measured=geometry(value,data)
        require(len(context)<=measured['envelope'],'literal context envelope')
        rows.append({'name':name,'kind':'report','data':data,'expected':{'exit':0},
                     'projection':context,'positions':positions,'geometry':measured})
    p=rows[:14]; geometric=rows[14:]
    negatives=negative_cases(positives)
    scalar=scalar_cases(); bounds=boundary_cases(positives[12][1])
    b=scalar[:6]+bounds; a=scalar[6:]
    result=p+negatives+b+a+geometric
    require(len(result)==73 and len({row['name'] for row in result})==73,'literal73 rows')
    return result

def repository_root():
    for parent in Path(__file__).resolve().parents:
        if (parent/'design/FEATURE_POLICY.md').is_file():
            return parent
    raise ValueError('repository ancestor missing')

def read_small(path, cap=MIB):
    require(not path.is_symlink(),'source symlink')
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NOFOLLOW',0)|os.O_NONBLOCK)
    with os.fdopen(descriptor,'rb') as stream:
        require(stat.S_ISREG(os.fstat(stream.fileno()).st_mode),'ordinary source')
        data=stream.read(cap+1)
    require(len(data)<=cap,'source byte admission')
    return data

def inherited(root, expected):
    path=root/'scripts/project-impact-oracle.py'
    source=read_small(path)
    require(identity(source)==expected,'inherited captured bytes pin before execution')
    name='rfc170_inherited_fixed_oracle'
    module=types.ModuleType(name); module.__file__=str(path)
    require(name not in sys.modules,'fresh inherited module')
    sys.modules[name]=module
    try:
        exec(compile(source,str(path),'exec',dont_inherit=True),module.__dict__)
        require(read_small(path)==source,'inherited source endpoint pin')
        return module,source
    except BaseException:
        sys.modules.pop(name,None)
        raise

def tiny_pairs(old):
    a=b'module a\n\nfn p() -> Void:\n  void\n'
    b=b'module b\n\nfn main(args: Vec[Bytes]) -> I64:\n  0\n'
    c=b'module c\n\nfn p() -> Void:\n  void\n'
    base=b'(project 1\n  (entry b)\n  (module a "a.slim" (imports) (exports))\n  (module b "b.slim" (imports a) (exports)))\n'
    renamed=b'(project 1\n  (entry b)\n  (module b "b.slim" (imports c) (exports))\n  (module c "c.slim" (imports) (exports)))\n'
    definitions=[('real-unchanged-tiny',base,((b'a',b'a.slim',a,()),(b'b',b'b.slim',b,(b'a',))),
        base,((b'a',b'a.slim',a,()),(b'b',b'b.slim',b,(b'a',)))),
      ('real-changed-tiny',base,((b'a',b'a.slim',a,()),(b'b',b'b.slim',b,(b'a',))),
        base,((b'a',b'a.slim',a+b'\n',()),(b'b',b'b.slim',b,(b'a',)))),
      ('real-deletion-rename-tiny',base,((b'a',b'a.slim',a,()),(b'b',b'b.slim',b,(b'a',))),
        renamed,((b'b',b'b.slim',b,(b'c',)),(b'c',b'c.slim',c,()))),
      ('real-manifest-widen-tiny',base,((b'a',b'a.slim',a,()),(b'b',b'b.slim',b,(b'a',))),
        base+b'\n',((b'a',b'a.slim',a,()),(b'b',b'b.slim',b,(b'a',))))]
    results=[]
    for name,before_manifest,before,after_manifest,after in definitions:
        snapshots=[]; bundles=[]
        for manifest,rows in ((before_manifest,before),(after_manifest,after)):
            records=[old.project(len(manifest),sha(manifest).encode())]
            tasks=[]; files={'slim.project':manifest}
            for key,path,data,deps in rows:
                records.append(old.Record(key,len(data),path,sha(data).encode()))
                tasks.append(old.Task(key,len(data),deps)); files[path.decode()]=data
            snapshots.append(old.Snapshot(tuple(records),tuple(tasks))); bundles.append(files)
        case=old.positive(name,*snapshots,family='rfc170-literal-source')
        results.append((case,tuple(snapshots),tuple(bundles)))
    return results

def from_inherited(old,before,after):
    relation,_=old.relation(before,after)
    first={row.name:row for row in before.records}; second={row.name:row for row in after.records}
    bg={row.name:row for row in before.tasks}; ag={row.name:row for row in after.tasks}
    def row(value):
        return None if value is None else Record(value.name,value.weight,value.path,value.digest)
    return Declared(relation['manifest_changed'],row(first[b'@project']),row(second[b'@project']),
      tuple(Change(relation['kinds'][key],key,row(first.get(key)),row(second.get(key))) for key in relation['changed']),
      tuple(GraphChange(key,b','.join(bg[key].imports),b','.join(ag[key].imports)) for key in relation['graph_changed']),
      tuple(tuple(relation[key]) for key in ('old_roots','new_roots','old_closure','new_closure')),
      tuple(row(second[key]) for key in relation['affected']))

def fixture_bundle(files):
    items=tuple(sorted(files.items()))
    require(len(items)<=4096 and all(len(name.encode())<=256 for name,_ in items),'fixed bundle geometry')
    return frame(b'slim-context-fixture-bundle-1')+frame(str(len(items)).encode())+b''.join(
        frame(name.encode())+frame(data) for name,data in items)

def tool_identity(path):
    path=path.resolve(strict=True)
    require(len(os.fsencode(path))<=4096 and path.is_file(),'resolved tool path')
    descriptor=os.open(path,os.O_RDONLY|getattr(os,'O_NOFOLLOW',0)|os.O_NONBLOCK)
    digest=hashlib.sha256(); count=0; cap=128*MIB
    with os.fdopen(descriptor,'rb') as stream:
        info=os.fstat(stream.fileno()); require(stat.S_ISREG(info.st_mode) and info.st_size<=cap,'tool size')
        while True:
            data=stream.read(min(65536,cap-count+1))
            if not data:
                break
            require(len(data)<=cap-count,'tool observed byte cap')
            count+=len(data); digest.update(data)
    return {'path_hex':os.fsencode(path).hex(),'bytes':count,'sha256':digest.hexdigest()}

def freeze(destination, compiler, cc, admission):
    # An explicit data-only operation. Never Popen, compile SLIM or execute CC.
    start_ns=time.perf_counter_ns(); start_utc=datetime.now(timezone.utc).isoformat()
    root=repository_root(); before_self=read_small(Path(__file__))
    require(identity(before_self)==admission['controls']['oracle'],'oracle source before capture')
    collector_path='scripts/project-impact-context.py'
    collector_source=read_small(root/collector_path)
    require(identity(collector_source)==admission['controls']['collector'],'opaque collector custody before freeze')
    require(len(SOURCE_PATHS)==70 and len(set(SOURCE_PATHS))==70,'fixed70 opaque body paths')
    sources={name:read_small(root/name) for name in SOURCE_PATHS}
    require({name:identity(data) for name,data in sources.items()}==admission['sources'],
            'source admission before inherited helper execution')
    old,old_source=inherited(root,admission['controls']['inherited_oracle'])
    inherited_paths=tuple(line for line in old.SOURCE_PATHS.splitlines() if line)
    require(len(inherited_paths)==51 and len(set(inherited_paths))==51,'inherited fixed51 read set')
    names=tuple(sorted((set(REGISTRY)-{collector_path})|set(inherited_paths)))
    require(names==tuple(SOURCE_PATHS),'fixed70 registry union')
    for name,pin in {**old.MANIFEST_PINS,**MANIFEST_PINS}.items():
        require(sha(sources[name])==pin,'fixed manifest registration')
    require(sha(sources['scripts/project-input-inventory.py'])==ADAPTER_SHA,'adapter registration')
    tools={'compiler':tool_identity(Path(compiler)),
      'cc':tool_identity(Path(cc)), 'python':tool_identity(Path(sys.executable))}
    require(tools==admission['tools'],'direct tool admission before model generation')
    files={}; model_rows=[]
    for case in data_rows():
        entry={key:value for key,value in case.items() if key not in ('data','projection')}
        if 'data' in case:
            name='data/'+case['name']+'.ns'; files[name]=case['data']; entry['input']=name
            entry['input_identity']=identity(case['data'])
        if case.get('projection') is not None:
            entry['expected_json_hex']=case['projection'].hex()
            entry['expected_json_identity']=identity(case['projection'])
        model_rows.append(entry)
    pairs=tiny_pairs(old)
    real_cases,bundles=old.real_cases({name:sources[name] for name in inherited_paths})
    for case,(label,first,second) in zip(real_cases,bundles):
        # Recreate typed literal records from captured fixed source bodies, not a source parser.
        declarations=old.COMPILER if label=='compiler' else old.CATALOG
        snapshots=[]
        for actual_files in (first,second):
            manifest=actual_files['slim.project']; records=[old.project(len(manifest),sha(manifest).encode())]; tasks=[]
            for key,path,imports in declarations:
                data=actual_files[path]; name=key.encode(); deps=tuple(value.encode() for value in imports.split())
                records.append(old.Record(name,len(data),path.encode(),sha(data).encode()))
                tasks.append(old.Task(name,len(data),deps))
            snapshots.append(old.Snapshot(tuple(records),tuple(tasks)))
        renamed=replace(case,name='real-'+label+'-body-newline')
        pairs.append((renamed,tuple(snapshots),(first,second)))
    require(len(pairs)==6,'source pair cardinality')
    source_rows=[]
    invalid_files=dict(pairs[0][2][0]); invalid_files['b.slim']=b'module b\n@\n'
    files['controls/invalid-source.bundle.ns']=fixture_bundle(invalid_files)
    for case,(before,after),(first,second) in pairs:
        declared=from_inherited(old,before,after)
        encoded,_=report_wire(declared)
        require(encoded==case.stdout,'independent168 report byte parity')
        dimensions=(len(before.tasks),len(after.tasks),sum(len(row.imports) for row in before.tasks),
          sum(len(row.imports) for row in after.tasks),sum(row.weight for row in before.records if row.name!=b'@project'),
          sum(row.weight for row in after.records if row.name!=b'@project'))
        expected=projection(declared,declared_facts(declared,*dimensions))
        label=case.name
        for slot,data in zip(('before-catalog','before-graph','after-catalog','after-graph'),case.inputs):
            files['pairs/'+label+'/'+slot+'.ns']=data
        for slot,bundle in (('before',first),('after',second)):
            files['pairs/'+label+'/'+slot+'.bundle.ns']=fixture_bundle(bundle)
        source_rows.append({'name':label,'expected_report_hex':encoded.hex(),
          'expected_report_identity':identity(encoded),'expected_json_hex':expected.hex(),
          'native_source_acceptance':'unknown: no actual producer/compiler invocation'})
    model={'schema':1,'format':'rfc170-prospective-oracle-1','scope':'fixed data only; not source authority',
      'rows':model_rows,'source_pairs':source_rows,'source_registry':REGISTRY,'native':native_plan(),
      'proof':overhead_proof(),'collector_custody':{'path':collector_path,
          'identity':identity(collector_source),'scope':'opaque source only; not imported before data hold'},
      'inherited_oracle_sha256':sha(old_source),'caps':{'model':MODEL_CAP,'fixtures':FIXTURE_CAP,'files':FILE_CAP,
         'report':REPORT_CAP,'context':JSON_CAP,'counter':COUNTER_CAP},
      'invalid_source_control':{'bundle':'controls/invalid-source.bundle.ns',
          'predicted_producer_exit':65,'predicted_producer_stdout_hex':b'E0102@b@9:10\n'.hex(),
          'predicted_producer_stderr_hex':'',
          'authority':'prospective pinned-source diagnostic; actual observation pending'}}
    files['model.json']=canonical(model)
    require(len(files['model.json'])<=MODEL_CAP,'model4MiB publication cap')
    files['sources-before.json']=canonical({name:identity(data) for name,data in sources.items()})
    files['tools-before.json']=canonical(tools)
    files['oracle.py']=before_self
    require(len(files['sources-before.json'])<=65536 and len(files['tools-before.json'])<=65536,
            'source/tool pin document64KiB caps')
    require(len(files)<=FILE_CAP-3 and sum(map(len,files.values()))<=FIXTURE_CAP-MODEL_CAP,
            'pre-publication file/byte reserve')
    destination=Path(destination)
    require(not destination.is_symlink() and not destination.exists(),'fresh data destination')
    destination=destination.parent.resolve(strict=True)/destination.name
    require(destination.is_relative_to((root/'build').resolve()) and len(os.fsencode(destination))<=4096,
            'ignored data destination')
    destination.mkdir()
    for name,data in files.items():
        path=destination/name; path.parent.mkdir(parents=True,exist_ok=True)
        with path.open('xb') as stream:
            stream.write(data)
    after_sources={name:identity(read_small(root/name)) for name in names}
    require(after_sources=={name:identity(data) for name,data in sources.items()},'source endpoint invariance')
    require(read_small(Path(__file__))==before_self and read_small(root/'scripts/project-impact-oracle.py')==old_source,
            'oracle endpoint byte equality')
    after_tools={key:tool_identity(Path(os.fsdecode(bytes.fromhex(value['path_hex'])))) for key,value in tools.items()}
    require(after_tools==tools,'tool endpoint invariance')
    files['sources-after.json']=canonical(after_sources); files['tools-after.json']=canonical(after_tools)
    for name in ('sources-after.json','tools-after.json'):
        with (destination/name).open('xb') as stream:
            stream.write(files[name])
    require(all(read_small(destination/name,FIXTURE_CAP)==data for name,data in files.items()),
            'published artifact byte equality')
    require(all(identity(read_small(root/name))==after_sources[name] for name in names),
            'source endpoint after artifact publication')
    require(read_small(Path(__file__))==before_self and read_small(root/'scripts/project-impact-oracle.py')==old_source,
            'oracle byte equality after artifact publication')
    require(read_small(root/collector_path)==collector_source,'opaque collector endpoint before data hold')
    require(identity(read_small(root/'scripts/verify-project-impact-context.py'))==admission['controls']['verifier'],
            'verifier endpoint before data hold')
    final_tools={key:tool_identity(Path(os.fsdecode(bytes.fromhex(value['path_hex']))))
                 for key,value in tools.items()}
    require(final_tools==tools,'tool endpoint after artifact publication checks')
    receipt={'schema':1,'status':'complete-data-only','rows':73,'source_pairs':6,'native_children':0,
      'model':identity(files['model.json']),'files':{name:identity(data) for name,data in sorted(files.items())},
      'control_sources':admission['controls'],'source_pins':admission['sources'],'tools':admission['tools'],
      'source_acceptance':'unknown: data materialization performs no compiler invocation',
      'started_utc':start_utc,'finished_observation_utc':datetime.now(timezone.utc).isoformat(),
      'elapsed_ns':time.perf_counter_ns()-start_ns,
      'elapsed_scope':'data materialization through artifact/source observation; before final receipt write',
      'non_aba_scope':'observed source/tool endpoints only'}
    receipt_bytes=canonical(receipt)
    require(len(files)+1<=FILE_CAP and sum(map(len,files.values()))+len(receipt_bytes)<=FIXTURE_CAP,
            'final materialization file/byte cap')
    with (destination/'freeze.json').open('xb') as stream:
        stream.write(receipt_bytes)
    return receipt
