#!/usr/bin/env python3
"""CUBI-04 v1 - prototipo dimensional, unidades em mm.

Requer CadQuery 2.7.x. Execute: python gerar_cubi.py
As medidas de motor, ferragens e placas NAO foram medidas no hardware do usuario.
Nao escale STLs para corrigir um encaixe; altere parametros e regenere.
"""
from pathlib import Path
import math, json, argparse
import cadquery as cq

ROOT = Path(__file__).resolve().parents[1]
P = {
    "body_width":160.0, "body_depth":160.0, "body_top":132.0,
    "wall":2.8, "base_thickness":4.0,
    "wheel_diameter":70.0, "wheel_width":26.0,
    "wheel_x":95.0, "axle_y":47.5, "axle_z":17.0,
    "motor_thickness":18.0, "motor_width":22.0, "motor_length":70.0,
    "motor_inboard_x":62.6, "motor_bottom":5.0,
    "holder_width":75.0, "holder_depth":60.0, "holder_loaded_height":26.0,
    "spacer_af":9.0, "spacer_length":29.0, "spacer_socket_af":9.4,
    "socket_lip_af":8.6, "socket_top":34.4,
    "tray_bottom":34.4, "tray_thickness":3.0,
    "m4_clearance":4.5, "m4_nut_af":7.4, "m4_nut_depth":3.4,
    "m3_clearance":3.4, "sensor_pitch":26.0, "sensor_hole":18.4,
}
CONFIG = Path(__file__).with_name("parametros.json")
if CONFIG.exists(): P.update(json.loads(CONFIG.read_text()))
else: CONFIG.write_text(json.dumps(P,indent=2)+"\n")

def box(w,d,h,x=0,y=0,z=0):
    return cq.Workplane("XY").box(w,d,h,centered=(True,True,False)).val().translate((x,y,z))

def rounded(w,d,h,r,x=0,y=0,z=0):
    a=cq.Workplane("XY").box(w,d,h,centered=(True,True,False))
    if r: a=a.edges("|Z").fillet(r)
    return a.val().translate((x,y,z))

def cyl(r,h,x=0,y=0,z=0,axis="Z"):
    a=cq.Solid.makeCylinder(r,h)
    if axis=="X": a=a.rotate((0,0,0),(0,1,0),90)
    if axis=="Y": a=a.rotate((0,0,0),(1,0,0),-90)
    return a.translate((x,y,z))

def hexagon(af,h,x=0,y=0,z=0,axis="Z"):
    r=af/math.sqrt(3)
    points=[(r*math.cos(math.radians(30+60*i)),r*math.sin(math.radians(30+60*i))) for i in range(6)]
    a=cq.Workplane("XY").polyline(points).close().extrude(h).val()
    if axis=="X": a=a.rotate((0,0,0),(0,1,0),90)
    if axis=="Y": a=a.rotate((0,0,0),(1,0,0),-90)
    return a.translate((x,y,z))

def hex_loft(af1,af2,h,z):
    a=cq.Workplane("XY").polygon(6,2*af1/math.sqrt(3)).workplane(offset=h).polygon(6,2*af2/math.sqrt(3)).loft().val()
    return a.rotate((0,0,0),(0,0,1),30).translate((0,0,z))

def fuse(a,*bs):
    for b in bs: a=a.fuse(b)
    return a.clean()

def cut(a,*bs):
    for b in bs: a=a.cut(b)
    return a.clean()

def label(a,text,size,x,y,z,plane="XY",depth=.65):
    try:
        t=cq.Workplane(plane,origin=(x,y,z)).text(text,size,depth,font="DejaVu Sans",combine=True).val()
        return a.fuse(t).clean()
    except Exception: return a

SPACERS=[(x,y) for x in (-46,46) for y in (-58,0,58)]

def socket(af=None):
    """Quatro linguetas flexiveis e labios superiores capturam a ponta do hex."""
    af=af or P["spacer_socket_af"]
    a=cyl(max(5.5,af/math.sqrt(3)+2.2),30.4,z=4)
    a=cut(a,hexagon(af,29.32,z=4),hexagon(af-.8,1.3,z=33.3),
          hex_loft(af-.8,af+.8,.85,33.7))
    a=cut(a,box(1,18,18.6,z=16),box(18,1,18.6,z=16))
    return a

def motor_station():
    # x > 0: corpo do motor ao alto, ponta do eixo para +X.
    back=box(4.5,32,85,x=59.55,z=4)
    foot=box(25,33,5,x=69,z=0)
    side1=box(19,3.6,50,x=71.3,y=14,z=4)
    side2=box(19,3.6,50,x=71.3,y=-14,z=4)
    a=fuse(back,foot,side1,side2,box(.6,16,35,x=62.1,z=30),
           box(10,1,15,x=67,y=11.7,z=30),box(10,1,15,x=67,y=-11.7,z=30))
    a=cut(a,cyl(8,7,x=56.5,z=P["axle_z"],axis="X"),
          cyl(P["m4_clearance"]/2,8,x=56,z=82,axis="X"),
          hexagon(P["m4_nut_af"],P["m4_nut_depth"]+.1,x=57.2,z=82,axis="X"))
    # Rasgos para uma cinta opcional e fios; fixacao principal pelo parafuso.
    for z in (37,64): a=cut(a,box(8,9,3,x=59.5,z=z))
    return a

def motor_clamp():
    a=box(3,24,30,x=82.5,z=58)
    a=fuse(a,box(.5,16,8,x=80.85,z=62))
    a=cut(a,cyl(P["m4_clearance"]/2,8,x=79,z=82,axis="X"))
    return a

def base():
    a=rounded(156,156,4,3)
    lip=cut(rounded(153.6,153.6,4,2.8,z=4),rounded(148.8,148.8,5,1,z=3.9))
    for y in (-P["axle_y"],P["axle_y"]):
        for x in (-77,77): lip=cut(lip,box(14,38,6,x=x,y=y,z=3.9))
    a=fuse(a,lip)
    for y in (-P["axle_y"],P["axle_y"]):
        st=motor_station()
        a=fuse(a,st.translate((0,y,0)),st.rotate((0,0,0),(0,0,1),180).translate((0,y,0)))
    for x,y in SPACERS: a=fuse(a,socket().translate((x,y,0)))
    # Berco do suporte 3x18650, paredes baixas para retirar as celulas.
    cradle=cut(rounded(P["holder_width"]+5.4,P["holder_depth"]+5.4,7,2,y=-10,z=4),
               rounded(P["holder_width"]+1.4,P["holder_depth"]+1.4,8,.8,y=-10,z=4))
    cradle=cut(cradle,box(14,12,9,x=28,y=23,z=3.9))
    a=fuse(a,cradle)
    for x in (-24,24):
        for y in (-45,25): a=cut(a,rounded(15,3.6,7,1,x=x,y=y,z=-1))
    for y in (-1,1):
        boss=box(16,20.3,17,x=0,y=y*66.65,z=4)
        if y>0:
            boss=cut(boss,cyl(2.25,23,y=55,z=12,axis="Y"),hexagon(P["m4_nut_af"],3.5,y=56.4,z=12,axis="Y"))
        else:
            boss=cut(boss,cyl(2.25,23,y=-78,z=12,axis="Y"),hexagon(P["m4_nut_af"],3.5,y=-59.9,z=12,axis="Y"))
        a=fuse(a,boss)
    # Apoio e cintas da pequena placa de protecao, envelope 55 x 24 mm.
    for x in (-22,22):
        a=fuse(a,box(6,20,2,x=x,y=39,z=4))
        for y in (25,53): a=cut(a,box(10,3.2,6,x=x,y=y,z=-1))
    return a

def tray():
    a=rounded(108,140,3,4,z=P["tray_bottom"])
    for x,y in SPACERS: a=cut(a,cyl(P["m3_clearance"]/2,5,x=x,y=y,z=33.4))
    # Malha de rasgos nao pressupoe a furacao dos clones de ESP32/L298N.
    for x in (-39,-26,-13,0,13,26,39):
        for y in (-48,-32,-16,0,16,32,48):
            a=cut(a,rounded(3.4,10,5,1,x=x,y=y,z=33.4))
    for x in (-26,26):
        for dx in (-18,18):
            for dy in (-18,18): a=fuse(a,box(5,5,4,x=x+dx,y=-32+dy,z=37.4))
    # ESP32 em pezinhos que sustentam bordas; espuma/fita isolante entre placa e apoio.
    for x in (20,32):
        for y in (12,48): a=fuse(a,box(4,5,8,x=x,y=y,z=37.4))
    for y in (25,55):
        for x in (-44,-8): a=fuse(a,box(4,12,4,x=x,y=y,z=37.4))
    return a

def shell():
    w,d=P["body_width"],P["body_depth"]
    a=cut(rounded(w,d,128,5,z=4),rounded(w-2*P["wall"],d-2*P["wall"],130,2.2,z=3))
    for y in (-P["axle_y"],P["axle_y"]):
        for x in (-80,80): a=cut(a,box(14,38,87,x=x,y=y,z=3.9))
    for sy in (-1,1):
        a=cut(a,cyl(2.3,8,y=sy*80-4,z=12,axis="Y"))
        a=cut(a,box(13,10,2.3,y=sy*79,z=124.7))
        for z in (33,111,118):
            for x in (-30,-10,10,30): a=cut(a,box(13,10,3,x=x,y=sy*79,z=z))
    a=cut(a,box(30,10,23,x=26,y=79,z=47))
    # Painel frontal integrado e rebaixos geometricos.
    panel=box(90,1.5,40,y=-80.35,z=59)
    a=fuse(a,panel)
    for z in (62,95): a=cut(a,box(80,1.5,1.2,y=-81.15,z=z))
    a=label(a,"CUBI 04",10,0,-80.95,80,"XZ",.6)
    # Entradas laterais superiores para ventilacao.
    for x in (-79,79):
        for y in (-38,-19,0,19,38): a=cut(a,box(10,11,3,x=x,y=y,z=112))
    return a

def lid():
    a=rounded(160,160,3,5,z=132)
    rim=cut(rounded(153.8,153.8,6,2.7,z=126),rounded(150.2,150.2,7,1,z=125.9))
    for sy in (-1,1):
        rim=cut(rim,box(17,8,8,y=sy*77,z=125))
        tab=box(12,1.6,8,y=sy*76.1,z=124)
        hook=cq.Workplane("YZ").polyline([(75.6,124.3),(77.7,125.1),(77.7,126.3),(75.6,126.3)]).close().extrude(12).val().translate((-6,0,0))
        if sy<0: hook=hook.rotate((0,0,0),(0,0,1),180)
        rim=fuse(rim,tab,hook)
    a=fuse(a,rim)
    a=cut(a,box(20.5,18.5,5,z=131))
    for x in (-43,43):
        for y in (-20,-10,0,10,20): a=cut(a,rounded(28,3.5,5,1,x=x,y=y,z=131))
    return a

def neck():
    a=fuse(box(20,18,8,z=127),box(32,30,4,z=135),box(16,18,12,z=139),
           box(9,20,16,z=151),cyl(10,9,x=-4.5,z=166,axis="X"))
    a=cut(a,cyl(2.25,30,x=-15,z=166,axis="X"),box(7,7,31,z=126),
          box(7,14,6,y=-5,z=151),box(30,4.4,2.8,y=6,z=129.4))
    return a

def neck_key():
    a=fuse(box(33,4,2.4,y=6,z=129.6),box(6,8,2.4,x=-13.4,y=6,z=129.6))
    a=cut(a,box(24,.8,4,x=4.6,y=6,z=129))
    for sy in (-1,1):
        pts=[(10.5,6+sy*2),(10.5,6+sy*2.7),(12.5,6+sy*2),(12.5,6+sy*1.5)]
        hook=cq.Workplane("XY").polyline(pts).close().extrude(2.4).val().translate((0,0,129.6))
        a=fuse(a,hook)
    return a

def head():
    # Caixinha aberta atras; sensores apontam para -Y.
    a=cut(box(72,35,38,y=-3.5,z=177),box(66,35,32,y=-.4,z=180))
    for x in (-P["sensor_pitch"]/2,P["sensor_pitch"]/2):
        a=fuse(a,cyl(12.1,4.2,x=x,y=-25,z=196,axis="Y"))
        a=cut(a,cyl(P["sensor_hole"]/2,14,x=x,y=-28,z=196,axis="Y"))
    # Apoios dianteiros nos quatro cantos da placa 45 x 20 mm.
    for x in (-20.5,20.5):
        for z in (186.5,203.5):
            a=fuse(a,box(5,4.2,3,x=x,y=-16.15,z=z))
    for sx in (-1,1):
        ear=fuse(box(4,20,12,x=sx*6.8,z=165),cyl(10,4,x=sx*6.8-2,z=166,axis="X"))
        ear=cut(ear,cyl(2.25,25,x=-12,z=166,axis="X"))
        a=fuse(a,ear)
    a=cut(a,hexagon(P["m4_nut_af"],3.5,x=-8.9,z=166,axis="X"))
    for sx in (-1,1): a=cut(a,cyl(1.7,10,x=sx*34-5,y=10,z=196,axis="X"))
    a=cut(a,box(12,10,7,y=12,z=177))
    return a

def head_cover():
    a=box(72,2.4,38,y=15.2,z=177)
    lip=cut(box(65.4,5,31.4,y=11.5,z=180.3),box(61.4,6,27.4,y=11.5,z=182.3))
    a=fuse(a,lip)
    # Dedos apoiam somente cantos: conferir componentes do clone antes de fechar.
    for x in (-20.5,20.5):
        for z in (186.5,203.5): a=fuse(a,box(3,26.5,3,x=x,y=1.05,z=z))
    for sx in (-1,1): a=cut(a,cyl(1.7,10,x=sx*32-5,y=10,z=196,axis="X"))
    a=cut(a,box(12,8,7,y=14,z=177))
    return a

def printed_pin():
    a=fuse(cyl(3.3,1.8),cyl(1.72,8,z=1.8),cq.Solid.makeCone(1.72,1.35,2).translate((0,0,9.8)))
    return a

def pivot_spacer(): return cut(cyl(5,8),cyl(2.3,10,z=-1))

def coupon_hardware():
    a=rounded(100,56,4,3)
    for i,d in enumerate((3.2,3.4,3.6,4.3,4.5,4.7)):
        x=-40+i*16
        a=cut(a,cyl(d/2,6,x=x,y=12,z=-1))
        a=label(a,str(d),3.6,x,21,3.85)
    for i,af in enumerate((7.2,7.4,7.6)):
        x=-31+i*31
        b=cut(box(17,17,6,x=x,y=-13,z=4),hexagon(af,4,x=x,y=-13,z=6.1),cyl(2.25,12,x=x,y=-13,z=-1))
        a=fuse(a,b)
        a=label(a,"AF"+str(af),3.4,x,-25,3.85)
    return a

def coupon_hex():
    a=rounded(86,30,4,3)
    for i,af in enumerate((9.2,9.4,9.6)):
        x=(i-1)*28
        a=fuse(a,socket(af).translate((x,0,0)))
        a=label(a,str(af),4,x,-11,3.85)
    return a

def coupon_motor():
    a=rounded(30,39,4,2,x=69,y=0)
    return fuse(a,motor_station())

def coupon_eyes():
    a=rounded(64,28,3,3)
    for x in (-P["sensor_pitch"]/2,P["sensor_pitch"]/2): a=cut(a,cyl(P["sensor_hole"]/2,5,x=x,z=-1))
    return a

def shim(t): return box(t,15,18)

def normalised(shape,rotations=()):
    s=shape
    for axis,deg in rotations:
        vec={"X":(1,0,0),"Y":(0,1,0),"Z":(0,0,1)}[axis]
        s=s.rotate((0,0,0),vec,deg)
    bb=s.BoundingBox()
    return s.translate((-bb.xmin,-bb.ymin,-bb.zmin))

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--cache",default=str(ROOT.parent/"work_cad"))
    args=parser.parse_args()
    cache=Path(args.cache); (cache/"world").mkdir(parents=True,exist_ok=True)
    parts={}; manifest=[]; scene=[]
    def save(code,name,shape,qty=1,rots=(),color="#e7aa32",folder="STL",notes=""):
        shape=shape.clean()
        assert shape.isValid(),f"BRep invalido: {code}"
        assert len(shape.Solids())==1,f"Solidos desconectados: {code} {len(shape.Solids())}"
        s=normalised(shape,rots); bb=s.BoundingBox()
        path=ROOT/folder/(code+"_"+name+".stl"); path.parent.mkdir(exist_ok=True)
        cq.exporters.export(s,str(path),tolerance=.06,angularTolerance=.13)
        entry={"code":code,"name":name,"file":str(path.relative_to(ROOT)),"quantity":qty,
               "size_mm":[round(bb.xlen,2),round(bb.ylen,2),round(bb.zlen,2)],
               "cad_valid":shape.isValid(),"solids":len(shape.Solids()),"volume_mm3":round(shape.Volume(),2),
               "color":color,"notes":notes,"rotations":rots}
        manifest.append(entry); parts[code]=shape
        print(code,name,entry["size_mm"],flush=True)
        return shape
    def add(name,s,color,group="printed",source=None):
        path=cache/"world"/(name+".stl")
        cq.exporters.export(s,str(path),tolerance=.1,angularTolerance=.18)
        scene.append({"name":name,"file":str(path),"color":color,"group":group,"source":source})

    b=save("01","base_bercos_verticais",base(),color="#3c4853",notes="Fundo no prato; 4 paredes; 30% preenchimento. Suportes localizados so se o fatiador exigir nos furos.")
    t=save("02","bandeja_placas",tray(),color="#536674",notes="Face plana embaixo. Rasgos para cintas de nylon; seis parafusos M3.")
    s=save("03","carcaca_cubo",shell(),color="#e9ad38",notes="Borda inferior no prato. SUPORTES apenas sob os tetos das 4 janelas laterais, aberturas de 38 mm.")
    l=save("04","tampa_com_travas",lid(),rots=(("X",180),),color="#c68b27",notes="Ja invertida: face externa plana embaixo; linguetas para cima. PETG preferivel para travas.")
    c=save("05","presilha_motor",motor_clamp(),qty=4,rots=(("Y",90),),color="#485b67",notes="4 copias. Testar junto do berco de teste antes da base.")
    n=save("06","pescoco",neck(),rots=(("X",90),),color="#596572",notes="Deitado. SUPORTES localizados sob ressalto do pe e regiao da articulacao.")
    k=save("07","chave_pescoco",neck_key(),color="#cc8830",notes="Inserir pela lateral sob a tampa; a passagem de fios fica separada da chave.")
    h=save("08","cabeca_ultrassom",head(),rots=(("X",90),),color="#d7a044",notes="Olhos no prato. SUPORTES a partir do prato em torno dos aros e sob o garfo; excluir furos dos olhos.")
    hc=save("09","tampa_cabeca",head_cover(),rots=(("X",-90),),color="#54616a",notes="Face externa no prato; dedos internos para cima.")
    pin=save("10","pino_tampa_cabeca",printed_pin(),qty=2,color="#d5a348",notes="2 copias; teste o furo 3,4 mm. Lixe levemente se ficar excessivamente justo.")
    ps=save("11","espacador_articulacao",pivot_spacer(),color="#6b7882",notes="Consumir comprimento do M4 de 27 mm no lado da cabeca do parafuso.")
    for thickness in (.5,1): save("12" if thickness==.5 else "13",f"calco_motor_{str(thickness).replace('.','p')}mm",shim(thickness),qty=4,rots=(("Y",90),),color="#555e63",notes="OPCIONAL: usar somente para retirar folga; nao forcar carcaca do motor.")
    save("T01","teste_parafusos_porcas",coupon_hardware(),folder="TESTES",color="#aaa")
    save("T02","teste_hex_com_trava",coupon_hex(),folder="TESTES",color="#aaa")
    save("T03","teste_berco_motor_em_pe",coupon_motor(),folder="TESTES",color="#aaa")
    save("T04","teste_olhos_hcsr04",coupon_eyes(),folder="TESTES",color="#aaa")
    add("base",b,"#394851",source="01")
    add("bandeja",t,"#596e7b",source="02")
    add("carcaca",s,"#e9ad38",source="03")
    add("tampa",l,"#c68b27",source="04")
    for sx in (-1,1):
        for yy in (-P["axle_y"],P["axle_y"]):
            tag=f"{'E' if sx<0 else 'D'}_{'F' if yy<0 else 'T'}"
            cs=c if sx>0 else c.rotate((0,0,0),(0,0,1),180)
            add("presilha_"+tag,cs.translate((0,yy,0)),"#526671",source="05")
            mb=box(P["motor_thickness"],P["motor_width"],P["motor_length"],x=P["motor_inboard_x"]+P["motor_thickness"]/2,z=P["motor_bottom"])
            shaft=cyl(2.7,35,x=55.6,z=P["axle_z"],axis="X")
            motor=fuse(mb,shaft)
            if sx<0: motor=motor.rotate((0,0,0),(0,0,1),180)
            add("motor_"+tag,motor.translate((0,yy,0)),"#e7bc31","hardware")
            tire=cyl(P["wheel_diameter"]/2,P["wheel_width"],x=sx*P["wheel_x"]-P["wheel_width"]/2,y=yy,z=P["axle_z"],axis="X")
            hub=cyl(20,1.1,x=(sx*108 if sx>0 else -109.1),y=yy,z=P["axle_z"],axis="X")
            # Sulcos simples para a referencia visual; as rodas sao compradas.
            for dz in (-10,0,10): tire=cut(tire,box(27,1.8,.9,x=sx*95,y=yy-33.5,z=17+dz))
            add("pneu_"+tag,tire,"#272e33","hardware")
            add("calota_"+tag,hub,"#d9a434","hardware")
    add("pescoco",n,"#596570",source="06")
    add("chave_pescoco",k,"#ca8b2c",source="07")
    add("cabeca",h,"#dca744",source="08")
    add("tampa_cabeca",hc,"#52616b",source="09")
    for sx in (-1,1):
        pp=pin.rotate((0,0,0),(0,1,0),-90 if sx>0 else 90).translate((sx*37.8,10,196))
        add("pino_"+str(sx),pp,"#bb8833",source="10")
    add("espacador_articulacao",ps.rotate((0,0,0),(0,1,0),90).translate((8.8,0,166)),"#626f78",source="11")
    for i,(x,y) in enumerate(SPACERS): add("hex_"+str(i+1),hexagon(P["spacer_af"],P["spacer_length"],x=x,y=y,z=4),"#887355","hardware")
    holder=cut(box(75,60,18,y=-10,z=4),box(70,56,17,y=-10,z=6))
    add("suporte_baterias",holder,"#24303b","hardware")
    for yy in (-30,-10,10): add("celula_"+str(yy),cyl(9.3,65,x=-32.5,y=yy,z=15.3,axis="X"),"#57838b","hardware")
    for xx in (-26,26):
        add("L298N_"+str(xx),box(43,43,2,x=xx,y=-32,z=41.4),"#9b4141","hardware" if xx<0 else "optional")
        add("dissipador_"+str(xx),box(28,11,25,x=xx,y=-27,z=43.4),"#202c31","hardware" if xx<0 else "optional")
        for yy in (-46,-18): add("terminal_"+str(xx)+"_"+str(yy),box(14,8,10,x=xx,y=yy,z=43.4),"#47827d","hardware" if xx<0 else "optional")
    add("ESP32",box(31,58,2,x=26,y=35,z=45.4),"#234550","hardware")
    add("ESP32_modulo",box(17,24,3,x=26,y=33,z=47.4),"#bac4c6","hardware")
    for yy in (25,55): add("buck_"+str(yy),box(44,20,15,x=-26,y=yy,z=41.4),"#447b93","optional")
    add("BMS_reserva",box(55,24,10,y=39,z=6),"#367a6b","optional")
    add("HC_SR04_PCB",box(45,1.6,20,y=-13.2,z=186),"#2c7c8b","hardware")
    for xx in (-13,13):
        metal=cut(cyl(8,12,x=xx,y=-26,z=196,axis="Y"),cyl(6.5,1.2,x=xx,y=-26.1,z=196,axis="Y"))
        add("sensor_"+str(xx),metal,"#bfcbd0","hardware")
        add("tela_sensor_"+str(xx),cyl(6.5,.4,x=xx,y=-25.5,z=196,axis="Y"),"#43545f","hardware")
    # Representacoes de ferragens para conferir comprimento e acesso.
    for sx in (-1,1):
        for yy in (-P["axle_y"],P["axle_y"]):
            bolt=fuse(cyl(2,27,x=57,y=yy,z=82,axis="X"),cyl(3.6,2.7,x=84,y=yy,z=82,axis="X"))
            nut=hexagon(7,3.2,x=57.3,y=yy,z=82,axis="X")
            if sx<0:
                bolt=bolt.rotate((0,0,0),(0,0,1),180);nut=nut.rotate((0,0,0),(0,0,1),180)
            add("parafuso_motor_"+str(sx)+"_"+str(yy),bolt,"#b0b5b6","fastener")
            add("porca_motor_"+str(sx)+"_"+str(yy),nut,"#8b9297","fastener")
    # BRep editavel: somente as pecas impressas, em posicao de montagem.
    assembly=cq.Assembly(name="CUBI_04_v1_prototipo")
    for row in scene:
        if row["group"]=="printed":
            # STEP contem solidos CAD, preservados do dicionario quando unicos.
            pass
    for code in ("01","02","03","04","06","07","08","09"):
        assembly.add(parts[code],name=code,color=cq.Color("orange"))
    for i,(sx,yy) in enumerate(((-1,-47.5),(-1,47.5),(1,-47.5),(1,47.5))):
        cs=c if sx>0 else c.rotate((0,0,0),(0,0,1),180)
        assembly.add(cs.translate((0,yy,0)),name="presilha"+str(i))
    assembly.add(ps.rotate((0,0,0),(0,1,0),90).translate((8.8,0,166)),name="espacador_pivo")
    cq.exporters.export(assembly.toCompound(),str(ROOT/"FONTE"/"CUBI_04_montagem.step"))
    (ROOT/"VALIDACAO"/"manifesto_pecas.json").write_text(json.dumps(manifest,ensure_ascii=False,indent=2))
    (cache/"scene.json").write_text(json.dumps(scene,indent=2))
    (cache/"parameters.json").write_text(json.dumps(P,indent=2))
    print("EXPORTACAO CONCLUIDA",flush=True)

if __name__=="__main__": main()
