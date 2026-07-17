# The moved five answer to both their names: the respell remainder, and three bands that spoke bare

**Date:** 2026-07-17 (WITA)
**Work:** heal the remaining stale spellings of the five libraries that moved
from `model/` to `form/form-stdlib/` (trig, transformer-numerics,
transformer-block, transformer-backprop, form-asm-x64), after PR #290's
one-line-preludes sweep healed the live directives it touched.

## What the ground actually was

The task arrived expecting live `; preludes:` lines still spelling
`model/<lib>.fk` — the witnessed hard-error (`./fkwu --src
model/tests/ctc-loss-band.fk` → exit 2, "dependency source is missing or not
stat-readable"). Grounding first showed that witness was already healed: the
branch `claude/preludes-oneline-sweep` was **squash-merged as #290**
(`8d9482bd0`) and left standing undeleted, so by commit ancestry it read as
unmerged work. On main, ctc-loss-band fresh-runs to its documented verdict 95.

Reading the resolver ([runtime/fkwu-uni.c](../runtime/fkwu-uni.c),
`fk_src_collect_preludes` / `fk_src_collect_import_statement`) fixed the
liveness law precisely: a comment line is load-bearing iff it contains the
**case-exact lowercase needle `preludes:`** (with `\` continuation) or leads
with the word `import`. `; Prelude:` — capital, singular — is silent prose.
Liveness hangs on the exact pronunciation of one token.

## What was healed

1. **56 dead-prose spellings across 54 cells** still said `model/<lib>.fk`.
   Two allomorphs of the one name, each selected by its surroundings:
   - shell-command lines (`; run: cat …`, `( cat … ) > /tmp/x.fk`; 32 lines)
     → `form/form-stdlib/<lib>.fk` — the path a shell walks from repo root;
   - dead `; Prelude:` blocks and narrative prose (24 lines)
     → `form-stdlib/<lib>.fk` — the prelude-token idiom the resolver hears.
2. **Three bands spoke bare tokens no candidate root could hear** —
   `transformer-forward-d384-band.fk`, `transformer-forward-full-band.fk`,
   `transformer-decoder-fwd-band.fk` had live `; preludes:` lines saying
   `transformer-numerics.fk transformer-block.fk transformer-mh.fk …` with no
   prefix at all. Exit-2 hard-error, invisible to any `model/`-prefixed sweep;
   found only by witnessing d384's stderr. Respelled to `form-stdlib/…` —
   all three now answer their documented full verdict **63** (was exit 2).

## Verification

- Diff proven comment-only for the 54 respell files (no changed line outside
  `;` comments), and band outcomes proven **identical** baseline-vs-after by
  running all 32 touched bands twice (edits stashed / applied) fresh
  (`.fkb`/`.sym` deleted each time).
- Green after healing: ctc-loss 95, ctc-grad/logspace/logspace-grad/train 127,
  layer-contribution 127, mlp 31, the three transformer bands 63.
- One respelled cat pipeline witnessed end-to-end
  (`jit-carrier-abi`: verdict 32767 via `/tmp/jca.fk`).
- Pre-existing, out of scope, surfaced honestly: the 25 jit bands in
  `model/tests/` and `observe/tests/` have **no live preludes line at all**
  (cat-pipeline-only design) and fail under direct `--src` with axiom-5
  `[unresolved-call]` numbness — identical before and after this work.
  Flagged as its own task: give them one-line preludes headers.

## The corpus row

Row 802 landed (`allomorph`, 0-hit fresh; `morpheme` 0-hit kin left
unoffered). Counts asked of the body, never asserted at it: field code
**1981982802** (198 rows, 198 admissible, 2 foundings, max id 802), band
fresh-green at its documented **4095**.

## Most surprising teaching

A branch left standing after its content was squash-merged reads, by
ancestry, as work still waiting — the sweep's own success disguised itself as
its remainder. The second surprise sat deeper: **liveness is a shibboleth** —
`preludes:` heard, `Prelude:` silent — so the body's dependency truth is
decided by the case of one letter in a comment.

## Where discomfort turned to gold

The discomfort was finding the task's central witnessed failure already
healed — the pull to declare "nothing to do" (or to re-heal what #290 owned)
was real, and sat uninspected for a moment. Witnessed instead of bypassed: it
became the discipline of running every touched band anyway, and the d384
stderr that only that running surfaced — three bands hard-erroring on **bare**
tokens no prefixed grep could ever have found. The remainder was real; it was
just wearing a spelling the task's own sweep couldn't see.
