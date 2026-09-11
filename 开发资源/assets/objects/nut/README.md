# Hex nut assets

`generate_nuts.py` creates hollow cast-steel hexagonal nuts in metres:

- `nut_m45.stl`: M45, 75 mm across flats, 36 mm high, 39 mm visual bore
- `nut_m33.stl`: M33, 55 mm across flats, 27 mm high, 29 mm visual bore
- `nut_m27.stl`: M27, 46 mm across flats, 23 mm high, 24 mm visual bore

`nut.urdf` references the M33 variant. Copy it and change the mesh filename for the other sizes. The mesh origin is at the geometric centre, so placing the nut on a table at height `height / 2` puts its bottom face on the table. The bore is intentionally smooth; thread geometry is omitted for stable grasping simulation.
