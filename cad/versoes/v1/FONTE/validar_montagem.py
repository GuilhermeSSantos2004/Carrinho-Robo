from pathlib import Path
import importlib.util,json,itertools,math
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('cubi',ROOT/'FONTE/gerar_cubi.py');g=importlib.util.module_from_spec(spec);spec.loader.exec_module(g)

def bounds_intersect(a,b):
    x=a.BoundingBox();y=b.BoundingBox()
    return all(min(getattr(x,v+'max'),getattr(y,v+'max'))-max(getattr(x,v+'min'),getattr(y,v+'min'))>1e-5 for v in 'xyz')

parts={'base':g.base(),'bandeja':g.tray(),'carcaca':g.shell(),'tampa':g.lid(),'pescoco':g.neck(),'chave':g.neck_key(),'cabeca':g.head(),'tampa_cabeca':g.head_cover()}
clamp=g.motor_clamp()
for sx in (-1,1):
    for y in (-47.5,47.5):
        parts[f'presilha_{sx}_{y}']=(clamp if sx>0 else clamp.rotate((0,0,0),(0,0,1),180)).translate((0,y,0))
parts['espacador_articulacao']=g.pivot_spacer().rotate((0,0,0),(0,1,0),90).translate((8.8,0,166))

collisions=[];checks=0
for (na,a),(nb,b) in itertools.combinations(parts.items(),2):
    if bounds_intersect(a,b):
        v=a.intersect(b).Volume();checks+=1
        if v>0.02:collisions.append({'a':na,'b':nb,'volume_mm3':round(v,5)})
print('INTERSECOES PECAS',collisions,flush=True)

envelopes={
 'suporte_com_celulas_75x60x26':g.box(75,60,26,y=-10,z=4),
 'L298N_43x43x27':g.box(43,43,27,x=-26,y=-32,z=41.4),
 'L298N_opcional_43x43x27':g.box(43,43,27,x=26,y=-32,z=41.4),
 'ESP32_31x58x15':g.box(31,58,15,x=26,y=35,z=45.4),
 'buck_motor_44x20x15':g.box(44,20,15,x=-26,y=25,z=41.4),
 'buck_logica_44x20x15':g.box(44,20,15,x=-26,y=55,z=41.4),
 'BMS_55x24x10':g.box(55,24,10,y=39,z=6),
 'HC_SR04_PCB':g.box(45,1.6,20,y=-13.2,z=186),
}
for i,(x,y) in enumerate(g.SPACERS):envelopes['hex_'+str(i)]=g.hexagon(9,29,x=x,y=y,z=4)
for sx in (-1,1):
    for y in (-47.5,47.5):
        envelopes[f'motor_{sx}_{y}']=g.box(18,22,70,x=sx*71.6,y=y,z=5)
        envelopes[f'pneu_{sx}_{y}']=g.cyl(35,26,x=sx*95-13,y=y,z=17,axis='X')
for xx in (-13,13):envelopes['transdutor_'+str(xx)]=g.cyl(8,12,x=xx,y=-26,z=196,axis='Y')

component_collisions=[]
for ne,e in envelopes.items():
    for np_,p in parts.items():
        if bounds_intersect(e,p):
            v=e.intersect(p).Volume();checks+=1
            if v>0.02:component_collisions.append({'component':ne,'part':np_,'volume_mm3':round(v,5)})
print('INTERSECOES ENVELOPES',component_collisions,flush=True)

tilt=[]
for deg in (-10,0,10):
    c=parts['cabeca'].rotate((0,0,166),(1,0,166),deg)
    v=c.intersect(parts['pescoco']).Volume()
    tilt.append({'tilt_degrees':deg,'intersection_mm3':round(v,5)})
print('PIVO',tilt,flush=True)

report={
 'status':'APROVADO NO MODELO NOMINAL' if not collisions and not component_collisions else 'CORRIGIR',
 'scope':'Verificacao geometrica CAD; nao e ensaio fisico, estrutural, termico, de carga ou validacao em fatiador.',
 'boolean_checks':checks,'printed_part_intersections':collisions,'component_envelope_intersections':component_collisions,
 'pivot_check':tilt,
 'assumed_clearances_mm':{
 'carcaca_lingueta_base_por_lado':0.4,'carcaca_aba_tampa_por_lado':0.3,
 'pescoco_garfo_por_lado':0.3,'chave_pescoco_largura_total':0.4,
 'suporte_bateria_folga_total_por_eixo':1.4,'suporte_bateria_altura_livre_sobre_envelope_26mm':4.4,
 'rodas_carcaca_lateral':2.0,'pneu_70mm_solo_base':18.0,
 'rodas_dianteira_traseira_distancia_bordas':25.0,
 'ultrassom_furo_menos_diametro':2.4,'ultrassom_frente_adiantada_ao_aro':1.0,
 'hex_9mm_folga_total_socket':0.4,'hex_folga_axial_ate_labio':0.3,
 'presilha_motores_altura_acima_do_pneu':6.0},
 'hardware_budget':{'M4x27':7,'porcas_M4':7,'M3x9':6,'hex_29mm':6},
 'not_validated':['Medidas reais do motor e linguetas laterais','Modelo e pinos da placa ESP32','Rosca e medidas reais de todas as ferragens','Comprimento util do eixo e insercao no cubo da roda','Acesso dos fios e conectores reais','Contracao e tolerancia da impressora','Resistencia das travas e torque dos parafusos','Carga, tracao, estabilidade, corrente e aquecimento','Suportes, primeira camada, material e tempo reais no fatiador'],
}
(ROOT/'VALIDACAO/validacao_montagem.json').write_text(json.dumps(report,ensure_ascii=False,indent=2))
assert not collisions and not component_collisions, 'Geometria precisa ser corrigida'
assert all(r['intersection_mm3']<.02 for r in tilt)
