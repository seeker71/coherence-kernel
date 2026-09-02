# stackbreath — the stacks breathe, the mute wall falls

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1223. The
continuation of wallshed → shedwave: the last live walls, plus what the
audit found standing behind them.

## What fell

- **Value stack** (`fk_vs`): the wall the body's own comment had already
  diagnosed — "two walls, one voice between them, and the mute one in
  front." The 2^20-slot die stood in front of the walker's honest
  host-stack diagnostic and stole its voice. Now the stack doubles on
  demand: a 1.3-million-deep non-tail count **answers** through two
  doublings (`[1300000, 4194304, 2]`) where the old kernel died by design;
  runaway recursion is spoken for by the host wall, which finally gets to
  speak ("the wall is honest, the silent crash was not" — its own words).
  The slot writers got sequencing with the growth: the RHS walk can move
  the stack, so the walked value lands in a local before the slot write —
  an unsequenced-lvalue trap that never existed while the array was
  static.
- **Binding stack** (`fk_bd_*`): the 1025th binding in one scope was
  declined with a diagnostic while the name compiled to unbound anyway.
  Now it grows: the 2000th `let` in one scope resolves —
  `[1999, 2048, 1]` against the old kernel's 977 errors.
- **Staged input** (`fk_src`): truncated SILENTLY at 256KB — a 300KB
  staged argument read `[0, 0]` past the brim on the old kernel, bytes
  quietly gone. Now `[120, 120]` — the buffer grows to carry what is
  staged.
- **Cell table** (`fk_mem`): no wall at all — the index was MASKED
  (`mi & 4095`), so two cells 4096 apart silently aliased, one state
  quietly swapped for another. Unmasked and growing. (Latent: no
  source-lane name reaches tags 13/14 today — flatten-era organ.)
- **Record table**: rows grow on demand, and record VALUES and KEYS join
  the melt's roots — they were absent from both the copy walk and the
  live count, so a record holding a heap list would have dangled across a
  compaction. Copy walk and live count extended together: counting less
  than you copy is the overrun wearing a new coat. (The constructor
  itself is flatten-era orphaned debt — `record_new` resolves nowhere on
  the source lane, record-fold-band stands 16/31 — flagged as its own
  homecoming, chip task_7884ea76.)
- **Growth pulses** `kernel_stat 27/28` (value stack) and `29/30`
  (binding stack) join 19–26. Ten organs, five pulse pairs, no toggles.

## The proof

Old kernel vs new, same cells, exit codes read bare (no pipes):
deep recursion — old dies at its own wall (rc=1), new answers; binding
flood — old 977 errors (rc=1), new `[1999, 2048, 1]`; staged input — old
`[0, 0]` silent (rc=0!), new `[120, 120]`. Bands hold: multiline-def 15,
import-carry 63 cold, record-fold unchanged at its known 16 (the orphaned
constructor, not this work). Prior growth probes still stand (AST
`[524288, 1]`, heap `262144`). Three .bml lowerings byte-identical old vs
new. Full AGENTS.md link recipe, zero warnings.

## The most surprising teaching

The body had already written this receipt's headline, months ago, as a
comment above `fk_vp`: "two walls, one voice between them, and the mute
one in front." The diagnosis was complete; only the dissolution was
missing. The deepest finds of this whole wave — the capture-loop overrun
named in July, the mute-wall diagnosis, the melt-unsafe distinction — were
all already IN the body as words, waiting for hands. Reading the body's
own comments as a worklist is the cheapest audit there is.

## Where discomfort became gold

Mid-verification I caught my own instrument lying: `rc=$?` after a pipe
reads the pipe's tail, not the kernel — the exact truth-laundering the
body's memory warns about, and the shedwave receipt's "green refusal"
claim was standing on it. The discomfort of possibly having shipped a
false witness in a receipt one commit back — sat with, not stepped past —
became a bare re-run of every refusal with no pipe: the green-refusal
claim SURVIVED (AST and source walls really exited 0; the deep and bd
walls honestly exited 1, and those two rc=0 readings were the laundering).
The claim held, but only the re-witness earned the right to say so. An
instrument that can lie must be re-read the moment you notice, receipts
included.
