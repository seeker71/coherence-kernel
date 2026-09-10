# 2026-09-10 — more n-D maps

Urs said more. Same host, more known maps: 2D perpendicular, 3D cross,
composed 90° turns, 2D⊗2D lifting to 4D, projective homog, affine shift.

```
./fkwu form/form-stdlib/tests/nd-geometry-band.fk   # 16383
./fkwu observe/nd-geometry-read.fk
  perp2 [-4, 3]
  cross3 [0, 0, 1]
  tensor-4d [3, 4, 6, 8]
  homog [2, 3]
  string-theory 0
```

Two 90° maps compose to 180°. Affine identity+(0,2) sends (1,0) to (1,2).

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> nd-geometry-band 16383; perp; cross; tensor; homog
