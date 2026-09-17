"""
Suporte reforçado para motor TT — modelo parametrico (CadQuery)
================================================================
Fonte-de-verdade dimensional: params.json (leia esse arquivo, nao edite
valores soltos aqui). Eixos conforme especificado pelo usuario:
    Z = vertical, comprimento do motor (~70 mm)
    Y = largura da carcaca amarela (22 mm CONFIRMADO)
    X = profundidade / direcao do eixo de saida (para a roda)

Geometria (vista de cima, plano XY):
    - Parede traseira (-X fechado) com janela para a ponta interna do eixo
    - Duas paredes laterais (bandas em Y) que abracam a largura confirmada
      de 22 mm com folga de 0.2 mm por lado
    - Base fechando o fundo (Z baixo)
    - Topo e frente (lado +X) ABERTOS -> cavidade onde o motor entra
    - Duas orelhas (bosses) solidas, unidas as paredes por nervuras/gussets
      ate a base, cada uma com furo passante + rebaixo hexagonal para porca
      cativa na face traseira da orelha
    - Tampa frontal (peca separada) que fecha a abertura em +X, encosta
      nas orelhas e aperta o motor contra a parede traseira
"""
import json
import os
import cadquery as cq

HERE = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(HERE, "params.json")) as f:
    P = json.load(f)

M = P["motor"]
FIT = P["fit"]
S = P["structure"]
FC = P["front_cap"]
SCR = P["screws"]
RW = P["rear_window"]

# ---- Dimensoes derivadas -------------------------------------------------
half_inner_y = FIT["cradle_internal_width_y"] / 2.0          # 11.2
wall_t = S["side_wall_thickness_x"]
back_t = S["back_wall_thickness_x"]
half_outer_y = half_inner_y + wall_t                          # 16.7

interior_x_start = back_t                                     # 5.5
interior_x_end = back_t + M["thickness_x_assumed"]            # motor front face
wall_total_x_depth = interior_x_end + S["base_margin_front_x"]  # 28.0-ish

base_t = S["base_thickness_z"]
z_top = base_t + S["wall_height_z"]                            # topo total

boss_front_x = wall_total_x_depth + SCR["boss_protrusion_x"]   # face frontal da orelha
nut_pocket_x0 = wall_total_x_depth                              # rebaixo comeca na face traseira da orelha
nut_pocket_x1 = wall_total_x_depth + SCR["nut_pocket_depth"]

shaft_z = base_t + M["shaft_height_from_gearbox_bottom_assumed"]

screw_positions = [
    # (Y do centro da parede, Z do furo)  -> parede esquerda embaixo, direita em cima (diagonal)
    (-(half_inner_y + wall_t / 2.0), base_t + SCR["boss_z_low"]),
    (+(half_inner_y + wall_t / 2.0), base_t + SCR["boss_z_high"]),
]

FILLET_INT = S["fillet_internal"]
FILLET_EXT = S["fillet_external"]


def hex_nut_cutter(across_flats, height):
    r = across_flats / (3 ** 0.5)  # circumradius from across-flats
    return cq.Workplane("XY").polygon(6, 2 * r).extrude(height)


def build_main_body():
    # Base -------------------------------------------------------------
    base = (
        cq.Workplane("XY")
        .box(wall_total_x_depth, 2 * half_outer_y, base_t, centered=(False, True, False))
    )

    # Parede traseira ----------------------------------------------------
    back = (
        cq.Workplane("XY")
        .box(back_t, 2 * half_outer_y, z_top, centered=(False, True, False))
    )

    # Paredes laterais -----------------------------------------------------
    side_l = (
        cq.Workplane("XY")
        .workplane(offset=0)
        .center(0, -(half_inner_y + wall_t / 2.0))
        .rect(wall_total_x_depth, wall_t)
        .extrude(z_top)
        .translate((0, 0, 0))
    )
    # o box acima ficou centrado em X pelo rect -> ajusta para comecar em X=0
    side_l = side_l.translate((wall_total_x_depth / 2.0, 0, 0))
    side_r = side_l.mirror(mirrorPlane="XZ", basePointVector=(0, 0, 0))

    body = base.union(back).union(side_l).union(side_r)

    # Orelhas (bosses) + gussets + furo + rebaixo hexagonal --------------
    for (y_c, z_c) in screw_positions:
        ear = (
            cq.Workplane("XY")
            .center(0, y_c)
            .rect(SCR["boss_protrusion_x"], SCR["boss_width_y"])
            .extrude(SCR["boss_height_z"])
            .translate((wall_total_x_depth + SCR["boss_protrusion_x"] / 2.0, 0, z_c - SCR["boss_height_z"] / 2.0))
        )
        # gusset triangular ligando a orelha a base (placa inclinada solida)
        # -> pontos levemente sobrepostos na parede/base e na orelha para
        #    evitar faces exatamente coincidentes na uniao booleana
        _ov = 1.0
        gusset_pts = [
            (wall_total_x_depth - _ov, base_t),
            (boss_front_x + _ov, base_t),
            (boss_front_x + _ov, z_c - SCR["boss_height_z"] / 2.0 + _ov),
            (wall_total_x_depth - _ov, base_t),
        ]
        gusset = (
            cq.Workplane("XZ")
            .center(0, 0)
            .moveTo(*gusset_pts[0])
            .lineTo(*gusset_pts[1])
            .lineTo(*gusset_pts[2])
            .close()
            .extrude(SCR["boss_width_y"], both=False)
            .translate((0, y_c + SCR["boss_width_y"] / 2.0, 0))
        )
        body = body.union(ear).union(gusset)

        # furo passante horizontal (eixo X) na orelha
        hole = (
            cq.Workplane("YZ")
            .center(y_c, z_c)
            .circle(SCR["clearance_hole_diam"] / 2.0)
            .extrude(boss_front_x + 1, both=False)
            .translate((-1, 0, 0))
        )
        body = body.cut(hole)

        # rebaixo hexagonal para porca cativa (face traseira da orelha)
        nut = hex_nut_cutter(SCR["nut_across_flats_assumed"] + 0.3, SCR["nut_pocket_depth"] + 0.2)
        nut = nut.rotate((0, 0, 0), (0, 1, 0), 90).translate((nut_pocket_x0 - 0.1, y_c, z_c))
        body = body.cut(nut)

    # Janela traseira para a ponta interna do eixo ------------------------
    rear_hole = (
        cq.Workplane("YZ")
        .center(0, shaft_z)
        .rect(RW["width_y"], RW["height_z"])
        .extrude(back_t + 2, both=False)
        .translate((-1, 0, 0))
    )
    body = body.cut(rear_hole)

    # Recorte inferior-frontal para a lingueta amarela (a confirmar) -----
    tab_notch = (
        cq.Workplane("XY")
        .center(interior_x_end - 6, 0)
        .rect(10, 10)
        .extrude(base_t + 0.01)
    )
    body = body.cut(tab_notch)

    # Arredondamentos ------------------------------------------------------
    try:
        body = body.edges("|Z and (>>X[0] or <<X[0])").fillet(FILLET_EXT)
    except Exception:
        pass

    return body


def build_front_cap():
    cap_t = FC["thickness_x"]
    cap = (
        cq.Workplane("YZ")
        .center(0, z_top / 2.0)
        .rect(2 * half_outer_y, z_top)
        .extrude(cap_t)
        .translate((boss_front_x, 0, 0))
    )

    # Furo de passagem do eixo (NAO e furo de roda: a roda fica do lado
    # de fora, encaixada diretamente no eixo)
    shaft_cut = (
        cq.Workplane("YZ")
        .center(0, shaft_z)
        .circle(FC["shaft_clear_hole_diam"] / 2.0)
        .extrude(cap_t + 2)
        .translate((boss_front_x - 1, 0, 0))
    )
    cap = cap.cut(shaft_cut)

    # Rebaixo (alivio) na face externa para o cubo da roda poder se
    # aproximar mais do motor, ja que o comprimento livre do eixo e
    # curto (ver relatorio / secao critica de folga axial)
    hub_relief = (
        cq.Workplane("YZ")
        .center(0, shaft_z)
        .circle(FC["hub_relief_diam"] / 2.0)
        .extrude(FC["hub_relief_depth"])
        .translate((boss_front_x + cap_t - FC["hub_relief_depth"], 0, 0))
    )
    cap = cap.cut(hub_relief)

    for (y_c, z_c) in screw_positions:
        hole = (
            cq.Workplane("YZ")
            .center(y_c, z_c)
            .circle(SCR["clearance_hole_diam"] / 2.0)
            .extrude(cap_t + 2)
            .translate((boss_front_x - 1, 0, 0))
        )
        cap = cap.cut(hole)
        # rebaixo (spigot socket) para autocentragem com a orelha
        spigot = (
            cq.Workplane("YZ")
            .center(y_c, z_c)
            .circle((SCR["boss_width_y"] - 2) / 2.0)
            .extrude(1.4)
            .translate((boss_front_x - 1.4, 0, 0))
        )
        cap = cap.cut(spigot)

    try:
        cap = cap.edges("|X").fillet(FILLET_EXT)
    except Exception:
        pass

    return cap


if __name__ == "__main__":
    out = os.path.join(HERE, "..", "out")
    os.makedirs(out, exist_ok=True)

    body = build_main_body()
    cap = build_front_cap()

    cq.exporters.export(body, os.path.join(out, "T03A_suporte_motor_reforcado_v3.stl"), tolerance=0.05, angularTolerance=0.1)
    cq.exporters.export(cap, os.path.join(out, "T03B_frente_apoiada_v3.stl"), tolerance=0.05, angularTolerance=0.1)

    assy = cq.Assembly()
    assy.add(body, name="corpo_principal")
    assy.add(cap, name="frente")
    cq.exporters.export(body, os.path.join(out, "T03A_suporte_motor_reforcado_v3.step"))
    cq.exporters.export(cap, os.path.join(out, "T03B_frente_apoiada_v3.step"))

    print("OK")
    print("body bbox:", body.val().BoundingBox())
    print("cap bbox:", cap.val().BoundingBox())
