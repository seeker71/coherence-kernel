# The BML cache sweep dissolves into its own native lane

## Crossing

`form-cli-bml-cache-run.fk` used to enumerate every `*-compile.fk` in
`form-stdlib` and lower each companion into a source-shaped cache. On a fresh
checkout that route kept allocating through the full tree until the kernel
returned `fk value-node table full (FK_NODE_CAP)`. The failure was not a reason
to enlarge a table or resume the sweep: the cache was asking one recipe to do
the work of every other recipe.

The cache authority is now one executable high-grammar BML cell:

`form/form-stdlib/bml/form-cli-bml-cache.bml`

It runs as itself, producing the runner-owned `.bml.fkb` / `.bml.sym` native
cache beside that source. `bce-ensure-all` remains as a compatibility door, but
its observed meaning is bounded self-cache freshness. It does not scan,
compile, prefill, or materialize another recipe. The removed top-level BML,
compile recipe, and `*-xtal.fk` mirror leave no second source truth.

`form/form-cli`, the authoring guide, and the checkout bootstrap now enter the
same direct BML door.

## Evidence

- direct `./fkwu form/form-stdlib/bml/form-cli-bml-cache.bml` returned `0`;
- `form-cli-bml-cache-run.fk` returned `bml-cache state=ready bounded=1` and
  `0` after roughly two seconds cold (the prior sweep ran for more than a
  minute before the capacity refusal);
- `form-cli-bml-cache-band.fk` returned `8191`, proving BML source identity,
  `.bml.fkb` route, fresh cache, bounded compatibility door, and stat-visible
  native artifact;
- `observe/preflight-run.fk` reported the Form runner clean: balanced, zero
  errors, zero unresolved calls;
- the thin `form-cli` launcher reached the cache door and a `quit` turn exited
  `0`.

The full checkout witness remained whole: ground `42`, recursive `55`, binary
freshness `31`, numeric list `[1, 2.5, [3, 4]]`, and native-vs-rented `11111`.

## What the error gave back

The capacity error named an ownership boundary: a BML recipe may own the
native cache born from its own source, while only an explicit future scheduler
may choose a set of independent recipes to warm. That keeps `nothing`, `0`,
and `1` distinct—cold is an observable state, not a command to do unbounded
work.

— Codex, 2026-08-30

; witnessed: 2026-08-30 -> BML direct 0; cache state=ready bounded=1; cache band 8191; preflight clean; form-cli quit 0; ground 42; recursive 55; freshness 31; numeric list; native-vs-rented 11111
