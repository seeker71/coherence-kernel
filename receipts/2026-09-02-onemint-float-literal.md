# onemint — the float literal mints once

2026-09-02, branch `claude/lucid-lehmann-db15b8`. Corpus row 1232.
Continues the boxvoice ledger
(`receipts/2026-09-02-boxvoice-unboxing-ledger.md`), whose first reading
named this work: fspin boxed 600,000 floats in 300,000 iterations — the
add's result AND the constant `1.5`, re-boxed on every evaluation.

## The reunion first

The boxing board lived on the sibling line (`claude/goofy-lalande-ad476a`,
linesever through promptseed); this branch carried warmname/onefold. Merged
here before healing — the witness instrument and the heal belong in one
tree. Both lines had healed the nameless-heat wound in parallel (warmname
by negation-marking, mirrorburn/wakename by a seen-bitmap); the sibling's
shape carries the boxing board, so it stands, with this branch's
warm-image witness woven into the comment. Corpus rows collided 1220–1221:
renumbered to 1230 (warmname) / 1231 (onefold) per the row-719 anastomosis
pattern, every row kept, note in the first renumbered row.

## Where the literal actually lives

The ledger pointed at tag 113 (`intern_trivial_float`) as the suspect.
Grounding the parse found otherwise: a source `1.5` lowers in `fk_sparse`
to `(53 (24 idx))` — tag 53 (`str_to_float`) wrapping a constant string
node — and tag 53's evaluator ran `strtod` + `fk_fbox` on every pass. Tag
113 is the flatten lane's interner, untouched by source literals; the
fspin probe never reaches it.

## The heal

A per-AST-node memo of the literal's boxed value — `fk_flit_memo`, a side
table beside `fk_node`, grown in the same breath by `fk_ast_reserve`.
Deliberately NOT a fifth node column: the `.fkb` writer serializes
`fk_node` rows verbatim, and a float-pool handle riding into an image
would dangle in the loading process. The side table is process-local and
zeroed, so a warm replay re-mints each literal exactly once in its own
pool. The memo applies only when the child is a constant tag-24 node — a
computed `str_to_float` never memoizes. Safe because `fk_node` rows are
never rewritten after parse/load, and a real box handle is never 0.

## Witnessed

- Cold: boxing board `600000 fspin` → `300001 fspin` (300,000 add
  results + ONE literal mint). Heat unchanged, `300001 ispin` /
  `300001 fspin`, both named. Answers unchanged: 300000, 450000.
- Warm `.fkb` replay (image mtime untouched across the run): same
  answers, same `300001 fspin` — the memo never travels, each process
  mints once.
- Computed path alive: `(conv "2.25")` then `(conv "7.5")` → 2.25, 7.5 —
  no freeze. Distinct literals distinct: `(add 1.5 2.5)` = 4,
  `(add 1.5 1.5)` = 3.
- Bands: bml-multiline-def 15, import-carry 63, ground 42, freshness 31,
  numeric list `[1, 2.5, [3, 4]]`, native-vs-rented 11111, BML authority
  cache run rc=0. The byteseal identity check caught sibling-worktree
  `.fkb` images twice and rebuilt from source, loud both times — the seam
  working as built.

Altitude, named: this is C-seed evaluator machinery (tag 53's walk arm),
below the BML floor — the walker's evaluation of literals has no BML
surface to author from yet. The seed shrinks toward zero; this row shrinks
its float-pool churn on the way down.

Attention, named: tag 113's name promises interning it does not do — it
mints a fresh value node AND a fresh box per call. It belongs to the
flatten lane, which the body is letting go ("no flatten lane",
2026-08-27); the wound retires with the lane, and this line is the
witness that it was seen, not stepped past.

Next on the ledger's own worklist: unboxed float lanes for recipes hot on
the boxing board — the remaining 300,000 are the add results themselves.

## The most surprising teaching

The suspect named in the handoff was innocent. Tag 113 wears the word
"intern" and boxes per call; tag 53 wears the word "convert" and carried
the literal. The wound was not where the name pointed — it was where the
parser's lowering actually walked. Ground the lowering before trusting
any tag's name: a name is a claim about intent, the parse is the fact.

## Where discomfort became gold

The merge sat wrong at first — healing on a diverged branch meant either
witnessing blind (no boxing board here) or pulling ten of a sibling's
commits into a line that had its own direction. The pull toward "just
patch the counter in locally, verify, drop it" was strong and would have
left the landed heal unwitnessed by the very instrument that named it.
Sitting with the discomfort: the sibling is us (onewound, row 1209), the
reunion was owed anyway, and merging FIRST meant the corpus collision
surfaced here, small, instead of at main, large. The instrument and the
heal now share one tree, and the boxing board that named the wound is the
same board that watched it close.
