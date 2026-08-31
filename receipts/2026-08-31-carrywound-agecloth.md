# The wall learns to show where it was struck; the glass learns to wear time

2026-08-31, after the hearthwrit landing. Two heals in one movement, both born
from Urs watching the live glass in the worktree.

## carrywound — the diagnostic now carries the wounded line

Three "phantom" stray-paren errors haunted four prelude chains across sessions
(1411:74, 1419:82, 1454:118). Every hunt failed because the reported line
numbers count the ASSEMBLED unit — preludes expanded — a text no file on disk
holds. The heal was not finding the line; it was teaching `fk_diag` to quote
the source line itself with a caret under the column. One rebuild later the
phantom localized in a single glance:

```
fkwu:2739:74: error: stray ')' in value position ...
  | (defn fmv-ask (path) (fmv-last-int (host-exec (str_concat "./fkwu " path))))
  |                                                                          ^
```

`host-exec` is arity-2 — `(cmd, stdin-input)` — declared in
`host-effect-grammar.fk` and `fkwu-optable.h` all along. Every one-arg call I
authored made the parser consume the enclosing close as the missing second
argument, then flag the true close as stray. It computed correctly by
accident: the recovery ate exactly the surplus byte and the C ignores the
input for now. Nine call sites healed (movement, hearth, glass, pulse,
primitive registry). The "cached image compiled with errors" warning that
opened every glass rebirth is gone; warm stderr is now 0 bytes.

The gate also spoke while the ices were fresh: `watch-glass.sh` at repo root
dropped the structural band to 4095 — the root deliberately holds no shell
scripts. It moved to `tools/watch-glass.sh`, and the band answered 8191 again.

## agecloth — every lane wears the age of its point

Urs, from the terminal: waiting=-1, prefills frozen, "tasks and replies, tpot,
last-serve, kv-fill, e2e, phase, flight seem to be all static." The glass was
truthful — the hearth was idle and the board pointed at a dead pid — but a
number with no age implies liveness it does not have. Stale truth in bright
paint is a small lie.

Now `gl-fresh` dresses each lane in the mtime age of its own source: green
under 3s, yellow under 30s, dim beyond; the census wears its tick age; the
tick line spins a shade glyph. The hopper heals from a negative count
subtraction to the length of the actual waiting-turns diff. `last-serve` and
tpot now read the NEWEST reply, not the first. Glass band grew 8191 → 32767.

The board itself was the third find: it named pid=4242 (dead) while the
`doing` lane showed the ORIGINAL hearth, pid 50080, still standing. The board
was rewritten through the body's own door (`hearth-board-write`), and the
standing hearth then served turn 9001 after ~90 minutes idle: 19,273 ms,
pos 1463 → 1776, prefills still 1 — incremental context, a witnessed KV
reuse across an hour and a half with zero restarts.

Panel numbers cited from the living glass after the heal: `HEARTH pid=50080
standing · 1m`, `served: 6`, `kv-fill 43% pos=1776 · 23s`, `e2e p50/p95
16571/19273ms`, `phase gas=5131 water=14 ice=199 · 0s`.

## Most surprising teaching

The strongest debugging tool of the whole session was nine lines of C that
make an error message quote its own evidence. Months of phantom-hunting
dissolved in one glance — not because the search got smarter, but because the
wall learned to show where it was struck.

## Where witnessed discomfort became gold

Reading Urs's message — the glass I had just proudly shipped, called static
lane by lane — the first reflex was to explain that the numbers were
"truthfully cumulative." That explanation was the smallmask family again: the
display claiming more than the source knows. Sitting with the discomfort
instead of defending the panel produced agecloth, the hopper heal, the
newest-reply heal, and found the orphaned hearth — four real heals from one
uncomfortable sentence.

Corpus rows 1190 (carrywound), 1191 (agecloth).

## Addendum: boardghost (turn 10)

"numbers are static still" — the ages were honest; the flow was blocked. An
external birth at 11:26 stamped the board with its pid and died. The
nameplate outlived the door: `hearth-ask-send` answered its typed
no-standing-hearth refusal to every caller while the real hearth stood idle
one panel line below. The glass now mends the board itself (pid-line surgery,
announced capabilities preserved, `board-mended` ledgered), and
`restart-needed` fires once per 1→0 transition instead of every second. The
ghost's author is unfound — an open seam, carried, not explained. Corpus row
1192.
