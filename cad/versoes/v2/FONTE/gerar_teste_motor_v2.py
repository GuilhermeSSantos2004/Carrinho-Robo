#!/usr/bin/env python3
"""Suporte de bancada CUBI T03 v2. Unidades: mm. CadQuery 2.7.x.

Somente teste do motor; nao altera os STLs do robo v1.
As cotas de eixo, lingueta e carcaca local nao foram medidas fisicamente.
O projeto usa aberturas amplas, presilha movel e porcas externas.
"""
from pathlib import Path
import hashlib
import json
import math
import struct
import cadquery as cq
import numpy as np

ROOT = Path(__file__).resolve().parents[1]
P = {
    "motor_length": 70.0,
    "motor_width_max": 37.0,
    "motor_thickness_max": 22.5,
    "motor_thickness_min_tested": 18.0,
    "motor_bottom": 8.0,
    "clear_width": 38.0,
    "shaft_diameter": 5.35,
    "shaft_z_reference": 20.0,
    "shaft_length_each_side_reference": 12.0,
    "hole_diameter": 3.6,
    "bolt_length": 27.0,
    "bolt_y": 25.0,
    "bolt_z": 46.0,
    "post_back": 6.0,
    "post_front": 13.0,
    "stand_height": 56.0,
    "clamp_thickness": 4.0,
}


def box(w, d, h, x=0, y=0, z=0):
    return cq.Workplane("XY").box(w, d, h, centered=(True, True, False)).val().translate((x, y, z))


def cyl(r, h, x=0, y=0, z=0, axis="Z"):
    directions = {"X": (1, 0, 0), "Y": (0, 1, 0), "Z": (0, 0, 1)}
    return cq.Solid.makeCylinder(r, h, cq.Vector(x, y, z), cq.Vector(*directions[axis]))


def union(*shapes):
    result = shapes[0]
    for shape in shapes[1:]:
        result = result.fuse(shape)
    return result.clean()


def subtract(shape, *tools):
    for tool in tools:
        shape = shape.cut(tool)
    return shape.clean()


def make_stand():
    # Base em U: a lingueta inferior pode passar pelo centro ate abaixo da base.
    foot = cq.Workplane("XY").box(33, 66, 4, centered=(True, True, False)).edges("|Z").fillet(2).val().translate((7.5, 0, 0))
    foot = subtract(foot, box(27, 14, 10, x=12.5, z=-1))
    # Apoio no corpo estacionario, dos dois lados da lingueta.
    pads = [box(24, 12, 4, x=12, y=sy*13, z=4) for sy in (-1, 1)]
    back = box(3.2, 42.6, 52, x=-1.6, z=4)
    # Janela aberta embaixo e teto a 45 graus: sem acertar um furo no eixo.
    aperture = cq.Workplane("YZ", origin=(-4, 0, 0)).polyline([
        (-11, 3.9), (11, 3.9), (11, 29), (0, 40), (-11, 29)
    ]).close().extrude(5).val()
    back = subtract(back, aperture)
    posts = []
    for sy in (-1, 1):
        post = box(7, 12, 52, x=9.5, y=sy*25, z=4)
        post = subtract(post, cyl(1.8, 9, x=5, y=sy*25, z=46, axis="X"))
        # Janela de alivio nao interfere com a porca ou com a base do poste.
        slot = cq.Workplane("YZ", origin=(5, sy*25, 20)).slot2D(23, 4.5, 90).extrude(9).val()
        post = subtract(post, slot)
        posts.append(post)
    # Guias curtas laterais; vao ate 38 mm de largura livre.
    guides = [box(16, 2.3, 16, x=5, y=sy*20.15, z=4) for sy in (-1, 1)]
    return union(foot, back, *pads, *posts, *guides)


def make_clamp(thickness=22.5):
    # Barra plana, sem tampa sobre o motor e sem parede diante dos eixos.
    s = box(P["clamp_thickness"], 62, 16, x=thickness+2, z=38)
    return subtract(s, *[
        cyl(1.8, 6, x=thickness-1, y=sy*25, z=46, axis="X")
        for sy in (-1, 1)
    ])


def normalise(s, rotation=None):
    if rotation:
        s = s.rotate((0, 0, 0), rotation[0], rotation[1])
    b = s.BoundingBox()
    return s.translate((-b.xmin, -b.ymin, -b.zmin))


def triangles(path):
    data = Path(path).read_bytes()
    count = struct.unpack("<I", data[80:84])[0]
    assert len(data) == 84+50*count
    dtype = np.dtype([("n", "<f4", 3), ("v", "<f4", (3, 3)), ("a", "<u2")])
    return np.frombuffer(data, dtype=dtype, count=count, offset=84)["v"].astype(float)


def mesh_report(path):
    t = triangles(path)
    vertices, idx = np.unique(np.round(t.reshape(-1, 3), 5), axis=0, return_inverse=True)
    idx = idx.reshape(-1, 3)
    edges = np.concatenate([idx[:, [0, 1]], idx[:, [1, 2]], idx[:, [2, 0]]])
    unique, counts = np.unique(np.sort(edges, axis=1), axis=0, return_counts=True)
    # Closed, manifold mesh and one connected component.
    parent = list(range(len(vertices)))
    def find(a):
        while parent[a] != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a
    for a, b in unique:
        a, b = find(int(a)), find(int(b))
        if a != b:
            parent[a] = b
    components = len({find(i) for i in range(len(vertices))})
    areas = np.linalg.norm(np.cross(t[:, 1]-t[:, 0], t[:, 2]-t[:, 0]), axis=1)/2
    volume = np.sum(np.einsum("ij,ij->i", t[:, 0], np.cross(t[:, 1], t[:, 2])))/6
    r = {"triangles": len(t), "closed": bool(np.all(counts == 2)),
         "components": components, "degenerate_triangles": int(np.sum(areas < 1e-8)),
         "signed_volume_mm3": round(float(volume), 3)}
    assert r["closed"] and components == 1 and r["degenerate_triangles"] == 0 and volume > 0, r
    return r


def write_stl(shape, filename, rotation=None):
    assert shape.isValid() and len(shape.Solids()) == 1
    s = normalise(shape, rotation)
    path = ROOT / "STL" / filename
    cq.exporters.export(s, str(path), tolerance=0.035, angularTolerance=0.12)
    b = s.BoundingBox()
    return {"file": "STL/"+filename, "quantity": 1,
            "dimensions_mm": [round(b.xlen, 3), round(b.ylen, 3), round(b.zlen, 3)],
            "cad_volume_mm3": round(shape.Volume(), 3), "cad_valid": True,
            "mesh": mesh_report(path), "sha256": hashlib.sha256(path.read_bytes()).hexdigest()}


def check_assembly(stand):
    checks = []
    def clear(name, a, b):
        overlap = a.intersect(b).Volume()
        assert overlap < 1e-5, (name, overlap)
        checks.append({"name": name, "overlap_mm3": round(overlap, 8)})
    for thickness in (18.0, 22.5):
        clamp = make_clamp(thickness)
        for width in (22.0, 22.5, 37.0):
            envelope = box(thickness, width, 70, x=thickness/2, z=8)
            clear(f"body_{thickness}_{width}_stand", envelope, stand)
            clear(f"body_{thickness}_{width}_clamp", envelope, clamp)
        clear(f"clamp_{thickness}_stand", clamp, stand)
        # Sweep the shaft's position without assuming its exact height.
        for z in (16.0, 20.0, 27.0):
            for y in (-2.0, 0.0, 2.0):
                shaft = cyl(5.35/2, thickness+24, x=-12, y=y, z=z, axis="X")
                clear(f"shaft_{thickness}_{y}_{z}_stand", shaft, stand)
                clear(f"shaft_{thickness}_{y}_{z}_clamp", shaft, clamp)
        # Large stationary bearing boss: illustrative, not a user measurement.
        boss = cyl(5.5, thickness+6, x=-3, z=20, axis="X")
        clear(f"boss_{thickness}_stand", boss, stand)
        clear(f"boss_{thickness}_clamp", boss, clamp)
        # Bottom molded tab, including variation across the thickness.
        tab = box(thickness, 10, 10, x=thickness/2, z=-2)
        clear(f"tab_{thickness}_stand", tab, stand)
        # Screw clearance; shafts modeled to 3.5 mm, clearance hole is 3.6 mm.
        for sign in (-1, 1):
            screw_start = thickness+P["clamp_thickness"]-P["bolt_length"]
            screw = cyl(1.75, P["bolt_length"], x=screw_start, y=sign*25, z=46, axis="X")
            clear(f"screw_{thickness}_{sign}_stand", screw, stand)
            clear(f"screw_{thickness}_{sign}_clamp", screw, clamp)
            assert screw_start <= P["post_back"]-4.0
        # Wheel clearance against printed parts only, reference 8-mm hub gap.
        wheel = cyl(35, 26, x=thickness+8, z=20, axis="X")
        clear(f"wheel_{thickness}_stand", wheel, stand)
        clear(f"wheel_{thickness}_clamp", wheel, clamp)
    # Explicit free passage for approved screw diameter, and opposing screw spacing.
    assert abs(P["bolt_y"]*2 - 50) < 1e-9
    return checks


def visual_motor(width=22.5, thickness=18.0):
    # Simplified reference only. The physical motor can differ locally.
    yellow = cq.Workplane("XY").box(thickness, width, 48, centered=(True, True, False)).edges("|Z").fillet(2).val().translate((thickness/2, 0, 8))
    can = box(thickness-1, min(width-2, 20), 21, x=thickness/2, z=55)
    cap = box(thickness-1.2, min(width-2.2, 19.8), 2, x=thickness/2, z=76)
    tab = box(6, 8, 5, x=thickness/2, z=3)
    shaft = cyl(2.675, thickness+20, x=-10, z=20, axis="X")
    top_tip = cyl(1.4, 3.5, x=thickness/2, z=78)
    return [("carcaca_referencia", yellow, "#e6b729"),
            ("motor_metal_referencia", can, "#b7bec1"),
            ("tampa_motor_referencia", cap, "#48525a"),
            ("lingueta_referencia", tab, "#d6a92b"),
            ("eixo_referencia", shaft, "#ece9dc"),
            ("ponta_superior_referencia", top_tip, "#59636b")]


def main():
    for folder in ("STL", "VALIDACAO", "PREVIA", "FONTE"):
        (ROOT/folder).mkdir(parents=True, exist_ok=True)
    cache = ROOT.parent / "work_motor_v2"
    cache.mkdir(exist_ok=True)
    stand, clamp = make_stand(), make_clamp()
    manifest = [
        write_stl(stand, "T03A_suporte_motor_em_pe_v2.stl"),
        write_stl(clamp, "T03B_presilha_regulavel_v2.stl", ((0, 1, 0), 90)),
    ]
    checks = check_assembly(stand)
    (ROOT/"FONTE/parametros.json").write_text(json.dumps(P, indent=2)+"\n")
    (ROOT/"VALIDACAO/resultado.json").write_text(json.dumps({
        "pieces": manifest, "assembly_checks": checks,
        "checks_count": len(checks),
        "limitations": "Only CAD envelopes checked. Physical fit and printer settings remain to verify. Shaft position and tab dimensions are assumed clearance envelopes, not measurements."
    }, indent=2)+"\n")
    assembly = cq.Compound.makeCompound([stand, clamp])
    cq.exporters.export(assembly, str(ROOT/"FONTE/T03_motor_v2_montagem.step"))
    scene = []
    objects = [("suporte", stand, "#617b87"),
               ("presilha", make_clamp(18), "#dc873d"), *visual_motor()]
    for sign in (-1, 1):
        shaft = cyl(1.5, 27, x=-5, y=sign*25, z=46, axis="X")
        head = cyl(3.0, 2.4, x=22, y=sign*25, z=46, axis="X")
        nut = cq.Workplane("YZ", origin=(3.6, sign*25, 46)).polygon(6, 5.5*2/math.sqrt(3)).extrude(2.4).val()
        objects.extend([(f"parafuso_{sign}", union(shaft, head), "#9babb3"),
                        (f"porca_{sign}", nut, "#a3b1b7")])
    for name, s, color in objects:
        path = cache/(name+".stl")
        cq.exporters.export(s, str(path), tolerance=0.06, angularTolerance=0.14)
        scene.append({"name": name, "file": str(path), "color": color})
    (cache/"scene.json").write_text(json.dumps(scene, indent=2)+"\n")
    print(json.dumps({"pieces": manifest, "assembly_checks_passed": len(checks)}, indent=2))


if __name__ == "__main__":
    main()
