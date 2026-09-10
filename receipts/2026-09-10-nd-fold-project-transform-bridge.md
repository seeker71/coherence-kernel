# 2026-09-10 — n-D fold, project, transform, bridge-strings

Urs asked for the known geometries, especially higher-dimensional
folding, projecting, transforming, and bridge-building strings.

The operations now run as Form native integers. 4D drops to 3D. Last
two coordinates fold. A vector projects onto an axis when the inner
product divides. A matrix turns (1,0) to (0,1). A bridge is a string
of points that divides (b−a) evenly, or nothing.

```
./fkwu form/form-stdlib/tests/nd-geometry-band.fk   # 255
./fkwu observe/nd-geometry-read.fk
  project-axis-4d-3d [1, 2, 3]
  fold-last [1, 2, 7]
  project-onto-x [3, 0]
  turn-90 [0, 1]
  bridge-3d [[0,0,0],[0,0,2],[0,0,4],[0,0,6]]
  string-theory 0
```

Not string theory. Not every manifold. The four verbs that host more
maps.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> nd-geometry-band 255; 4D project; fold; 90°; bridge
