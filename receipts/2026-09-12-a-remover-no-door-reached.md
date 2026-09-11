# A remover no door reached

2026-09-12, a little after three in the morning, M4 Max, Hati Suci. Receipt 16 left fkwu's rmdir
measured as not recursive. Following it also showed that receipt 16's own mkdir heal had turned a
band red on three kernels while validate printed it green.

## Carried

- **fs_rmdir removes the tree on every kernel** (5b0e9d16). fkwu's tag 55 unlinked `seg-*.log`
  names — cell-log-store's — and called rmdir, so any other tree stood; Go, Rust and TS removed the
  whole tree. fkwu already carried fk_rmtree, a recursive remover that no door reached. Tag 55 now
  calls it: 0 when the tree is gone, -1 when the path is not a directory or part of it stays. The
  walk reads lstat, so a symlink is removed as itself and its target stays, as RemoveAll,
  remove_dir_all and rmSync do. A probe reads the same on all four kernels: rmdir of a file -1; a
  tree holding a nested file, a symlink and two fifos 0 and gone; the link's target kept.
  fk_unlink_segments had no caller left and goes.
- **fs-crud-band reads its declared 11111111 on all four kernels, and is rowed.** It made its
  nested tree with one fs_mkdir and leaned on the siblings' old mkdir -p. Receipt 16's mkdir heal
  (3d0d93ed) turned it to 11000000 on Go, Rust and TS: the committed band reads 11111111 on a Go
  built from the commit before and 11000000 on the current one. validate printed it green, because
  a band without a row is judged by agreement alone. It now makes each level with its own atomic
  mkdir and asserts made 1, again 0.
- **fs-mkdir-parents goes.** form-fs's word called the one-level door under a name that promised
  parents, and had no caller. The form-cli bootstrap is regenerated (stamp 0cb6d8d7352ae83f; the
  voice canary answers pong).
- **validate.sh expands every explicit file's declared chain** (b7a011c9). Given one band it
  expanded the band's preludes; given several files it handed them over as they were, and the
  source compiler drops every `//` line from a lowered .bml, its `// preludes:` directive with
  it — the Go kernel's own loader says so and reads a .bml's directives from the raw file. Now
  every file's chain comes first, each dependency once. A documented call from
  form-samples/cross-modal/20 (sha256.fk with its sample) failed on Go, Rust and TS under the old
  rule and agrees on all three now; one band alone reads as before.
- **doorless is row 1460** (6b1af197).

Witnessed at 5b0e9d16 through validate.sh: fs-crud-band 11111111 on fkwu, Go, Rust and TS, drift
31 of 31, porcelain 0 before and after. Go's fkwu tests pass after the regen. At b7a011c9, formbin-artifacts-band agrees on all four kernels
and the sample-20 call on three; freshness 31, the corpus band 32767, the drift run 8191 of 8191.
Every one of the 44 bands that make directories reads the same on a Go built before 3d0d93ed and
on the current one, except fs-crud-band, whose new assertion (again 0) the older binary cannot
give: no other band leaned on mkdir -p.

## Still open, measured

- **Two bands in one program meet the name seam.** formbin-artifacts-band with
  primitive-registry-band now carries both chains; TS reads 63, Go and Rust stop at "bp:
  unreviewed bootstrap name: PRIM-REGISTRY-PROBE". form-ontology-bp.fk, in formbin's chain,
  defines a Form `bp`; on Go and Rust a user binding shadows a same-named native (main.go says so
  beside the lookup), on fkwu and TS the native answers. Each band alone agrees.
- The open items of receipts 12 to 16 stand where they are not named here.

## Surprise, and where the discomfort went

The remover was already written. fk_rmtree walked a tree, joined paths into a checked buffer and
removed everything under it; nothing called it. The door beside it knew one store's file names
instead.

The discomfort was finding my own regression on main. Receipt 16 leaned on a scan for nested
literal paths, and fs-crud-band builds its path with str_concat, where no literal scan can see it.
What turned it: the band's own header, "mkdir -p a nested tree", and then the difference itself as
the instrument — the committed band on a Go built from the commit before and on the current one,
and every band that makes directories swept the same way.

Frontier word, row 1460: **doorless**, a working part of a kernel that no door reaches, so its
callers make do with something weaker.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
