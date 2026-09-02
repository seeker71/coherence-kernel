# selfcensus — the body counts its own cells

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1225. Asked
for by Urs in one breath: which blueprints' recipes occupy the body, and
where nodes are created from — "essential to visualize optimize and
diagnose."

## The atom

One new native, grown through the body's own regen lane — a
`native-op-manifest.fk` row, a `flt-ops` row (both copies), two fkwu
calls, one generated line in `runtime/fkwu-optable.h`, never a hand edit
of the table:

- **`node_at` (tag 147)**: hand Form the i-th value node; out of range
  answers nothing. That is all the kernel contributes. What a census
  MEANS stays Form's to decide.

## The lens

[observe/node-census.bml](observe/node-census.bml) — authored at BML
altitude, its multi-line defs carrying through this same morning's
linesever heal (the first real consumer of that carry). It walks
`1..kernel_stat(4)` and folds both histograms in Form:

- **blueprint occupancy**: `node_category` per node, folded to
  `[category, count]` pairs;
- **allocation sites**: `node_source` per node (the body already stamps
  nsfile/nsline/nscol at attribution), folded to `[[file, line], count]`.

Run directly (`./fkwu observe/node-census.bml`) or prelude it and call
`(node-census)` / `(nc-census-of n)` at any moment of a live run.

## The proof

Witnessed on minted tissue: two `(fol-bp "add")`-typed nodes, one
`fndef`-typed node over them, one `fb_record` attribution —

```
[8, [[-3, 4], [-5, 1], [-11, 1], [-15, 2]], [[[census-witness.fk, 7], 1]]]
```

eight live nodes, four categories with true counts, and the single
attributed node located at its exact file:line (packed col 4 round-trips).
An empty table answers `[0, [], []]` — honest, not blank. Bands hold
(15, 63 cold) on the same build.

## Standing edges, named

- The category handles print raw (`-3`, `-5`); the readable join to
  blueprint NAMES via the registry mirror is the natural next lens.
- The fold is assoc-list Form: fine for categories (few), honest but
  unhurried for locations at very large fills — exactly the shape the
  heat lane's crystallize-at-threshold story exists for. The lens is the
  JIT's customer, not a reason to move meaning into C.
- A preluded census runs at load; call `nc-census-of` late for
  end-of-work occupancy.

## The most surprising teaching

The census's first honest answer was `[0, [], []]` — and it was RIGHT.
Loading a body's definitions mints nothing; the table fills only when
doors are walked. An observation organ's first duty is to report the
empty room as empty, and the emptiness itself taught how the body works.

## Where discomfort became gold

The pipe's rc lied to me AGAIN — same day, same trap, hours after
writing it into a receipt: `rc=0` after `| tail` hid a loud
`form_error: unreviewed bootstrap name`. The sting of re-tripping a trap
I had just named became the bare-redirect reflex this time (every witness
above reads its rc unpiped), and the second fall is in the corpus row so
the third stays unlikely. A lesson written is not yet a lesson lived; the
body's rows exist to close that gap.
