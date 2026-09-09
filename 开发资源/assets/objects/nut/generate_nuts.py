#!/usr/bin/env python3
"""Generate simple hollow hexagonal nut ASCII STL meshes (units: metres)."""
import math
from pathlib import Path

OUT = Path(__file__).parent

def tri(a, b, c):
    return f"  facet normal 0 0 0\n    outer loop\n      vertex {' '.join(f'{x:.8f}' for x in a)}\n      vertex {' '.join(f'{x:.8f}' for x in b)}\n      vertex {' '.join(f'{x:.8f}' for x in c)}\n    endloop\n  endfacet\n"

def mesh(name, across_flats, height, bore):
    # Outer hexagon has the requested across-flats dimension.  The bore is circular.
    ro = across_flats / math.sqrt(3.0)
    ri = bore / 2.0
    n = 12
    outer = [(ro * math.cos(math.radians(30 + 60*i)), ro * math.sin(math.radians(30 + 60*i))) for i in range(6)]
    inner = [(ri * math.cos(2*math.pi*i/n), ri * math.sin(2*math.pi*i/n)) for i in range(n)]
    s = f"solid {name}\n"
    z0, z1 = -height/2, height/2
    # top/bottom annuli: split each six-sided sector into triangles.
    for k in range(6):
        o0, o1 = outer[k], outer[(k+1)%6]
        i0, i1 = inner[(2*k)%n], inner[(2*k+1)%n]
        for a,b,c in [((o0[0],o0[1],z1),(o1[0],o1[1],z1),(i1[0],i1[1],z1)), ((o0[0],o0[1],z1),(i1[0],i1[1],z1),(i0[0],i0[1],z1)), ((o1[0],o1[1],z0),(o0[0],o0[1],z0),(i0[0],i0[1],z0)), ((o1[0],o1[1],z0),(i0[0],i0[1],z0),(i1[0],i1[1],z0))]: s += tri(a,b,c)
        # outer hex wall
        s += tri((o0[0],o0[1],z0),(o1[0],o1[1],z0),(o1[0],o1[1],z1))
        s += tri((o0[0],o0[1],z0),(o1[0],o1[1],z1),(o0[0],o0[1],z1))
    # bore wall
    for i in range(n):
        a,b = inner[i], inner[(i+1)%n]
        s += tri((a[0],a[1],z0),(b[0],b[1],z1),(b[0],b[1],z0))
        s += tri((a[0],a[1],z0),(a[0],a[1],z1),(b[0],b[1],z1))
    (OUT / f"{name}.stl").write_text(s + "endsolid\n")

# ISO-style hex dimensions (nominal thread, across flats, height, visual bore).
# The bore is represented as a clearance opening rather than modeled threads.
for name, af, h, bore in [("nut_m45", .075, .036, .039), ("nut_m33", .055, .027, .029), ("nut_m27", .046, .023, .024)]:
    mesh(name, af, h, bore)
