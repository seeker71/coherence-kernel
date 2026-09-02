# mirrorburn — the bootstrap compiler's fire, found by its first named heat

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1226. Asked
for by Urs: use the census info to optimize the bootstrap compiler, the
glass JIT, and metal/MLX/native primitive selection per source.

## The worklist learns its names

The heat board (`.fkwu-heat.<pid>`, the glass JIT's own worklist) had been
ANONYMOUS since it landed: heat is counted by fn-index, names live in
symbol rows, and the writer indexed the symbol columns by fn-index
directly — one reserved slot ahead, every line an empty name. One join
through `fk_fnidx` and the board speaks: `300001 hot-spin`. Per-source
selection is impossible without this; it is the wire everything below
rides.

## The first named reading pointed one finger

The bootstrap compile of one mid-size .bml:

```
22602635 fol-bp-row     ← 97% of reported heat
  508328 fstr-substring-halve
  212700 fol-bp-lookup
  113985 fstr-find-loop
```

`FOL-BP-BOOTSTRAP-TABLE` — the blueprint registry's hand-held mirror —
was a defn REBUILT on every resolution: 671 `fol-bp-row` constructions
per `(bp ...)` glance, ~33,700 rebuilds per compile, ~135M pair mints of
melt churn. The mirror burned for every look into it.

## The control decision

Build-once could not hold it: top-level `let` is call-by-name on this arm
(once-hold rides an unmerged branch), witnessed — the held-let left the
heat byte-identical. The four-way-pure heal: `fol-bp-coords`, a decision
chain GENERATED from the table's own 671 rows, constructing nothing until
the row is found, same refusal shape (unknown name answers empty). The
table and `fol-bp-lookup` stay for enumeration callers; resolution rides
the chain.

## Re-observation

- movement.bml, warm chain: **3.78s → 0.12s** (~31×); its compile now
  sits below the heat reporter's own 100k floor — the fire is out.
- api.bml (the morning's first traveler through the node-table growth):
  **~130s → 8.7s** (~15×).
- Lowered text byte-identical across the heal (hearth and movement,
  old kernel vs new); bands 15 / 63 cold.
- What remains in the heat is the fstr family — exactly the worklist the
  per-recipe JIT program already names. The glass, the carriers
  (mlx_run/metal via form-cli-gpu-run), and native selection now have a
  NAMED ranking to choose from.

## The counsel organ

[observe/source-counsel.bml](observe/source-counsel.bml) — reads one heat
file (house door: `echo <path> > /tmp/source-counsel-target`), ranks
worst-first by heat, totals dispatches, and advises the lane per source:
crystallize (glass u32 leaf, pure int folds), carrier (tensor-shaped →
mlx/metal), native (the rest). Advice is data, worst-first — the
lane-counsel pattern at the per-source altitude. Composed callers prelude
it and call `sc-report` directly.

## The most surprising teaching

The optimizer's whole yield came from observability debts, not clever
code: an anonymous worklist (a join bug), then a mirror rebuilt per
glance (a call-by-name default). Neither needed invention — they needed
NAMES. The histograms Urs asked for were not preparation for the
optimization; they were the optimization.

## Where discomfort became gold

The first generated chain dropped the rows' NAME head — every coordinate
shifted one field and the whole ontology misaligned. Nothing crashed;
preflight was clean; the compiler happily emitted `(do "HearthDir" (if)
".hearth")` garbage. Only the lowering-parity witness — held from the
morning's healing-radius discipline — caught it before it landed. The
discomfort of running "yet another parity check" on an "obviously safe"
edit is the entire reason this receipt reports a speedup instead of a
corpse. The check you almost skip is the one that pays.
