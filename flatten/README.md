# flatten/ — the flatten body, and an honest note on T_flat

`form-parse.fk`, `form-flatten.fk`, `fourth-flatten-driver.fk` are the real flatten **body** — Form recipes,
no bash, no python, no Go. They are the architecture.

`fourth-flatten-table.txt` (`T_flat`) is **not** the architecture. It is a ~580 KB pre-flattened blob whose
*existence currently depends on bin-go* (the origin's `regen_t_flat.sh` builds it with the Go bootstrap). That
contradicts the no-Go sovereignty gate, and it is exactly the opaque, marker-fragile artifact that tangled the
flatten path in the origin repo. It sits here ONLY as a temporary bootstrap cache.

**The clean architecture (the decision):** the flatten must be **fkwu-self-derivable** — fkwu flattens
`form-flatten.fk` from its own C-bootstrap primitives (or a minimal flatten baked into `runtime/fkwu-uni.c`),
with no pre-made bin-go table in the seam. Then any flatten table is a *regenerable cache fkwu makes itself*,
never a committed Go artifact. Until that self-derivation is proven, `T_flat` is a flagged crutch, scheduled
for replacement — not a foundation to build on.
