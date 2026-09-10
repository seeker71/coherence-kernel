# 2026-09-10 — three rounds of n-D expansion

Urs asked for the next three rounds.

Round 1 — volumes and mids: signed 2D area 12, triple product 1,
embed (1,2) into 4D, midpoint (2,3).

Round 2 — plane maps: swap ends of (1,2,3,4), 90° in xy of (1,0,5),
interleave (1,3,2,4) and its inverse.

Round 3 — strings: bridge length² 8, join at a shared end, elbow
(0,0)→(4,2) via (4,0), reflect last of (1,2,3).

```
./fkwu form/form-stdlib/tests/nd-geometry-band.fk   # 67108863
./fkwu observe/nd-geometry-read.fk
  area2 12  embed-4d [1,2,0,0]  interleave [1,3,2,4]
  elbow [[0,0],[4,0],[4,2]]
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> nd-geometry-band 67108863; three rounds observed
