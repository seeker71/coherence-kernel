# 2026-09-06 — the dead slot is the next string

Urs, evening: "continue with optimizing." The frames-stay-open sibling had
named the next wound with its rate: the live glass mints about 230 permanent
strings a tick and self-molts near frame 1200.

## What the seed had

Every `str_concat`, `int_to_str` and `byte_to_str` result is interned into
one table for the life of the process. The pair heap melts; the string table
never did. Sampled on the live glass loop's own page: 22,762 strings at two
seconds, 77,079 at twelve — five thousand a second, and the loop's own
`selfmolt` rule restarting it when the growth passed 262,144. Every
long-running kernel — sensors, machine, organs, hearth — carried the same
clock.

## What stands

**The string melt.** After the pair melt has copied the live heap, `fk_smelt`
marks every local string reachable from the melt's own roots — the value
stack, the memory cells, record values and blueprints, value nodes — plus the
two holders that keep a raw index rather than a word: record keys and the
AST's string-literal nodes. Every unmarked slot is unlinked from its hash
chain and pushed on a free list. Live strings never move: their indices,
offsets and bytes stay where another process may be reading them through the
shared store. `fk_sintern` takes a freed slot before growing the table, and
reuses the slot's old bytes when the new string fits (the scratch just
written is discarded), else points the slot at the fresh bytes.
`kernel_stat 51` counts slots reclaimed.

**Eleven arms push what they hold.** `str_concat`, `str_eq`, `str_byte_at`,
`substring`, the record setters and the method definer each held a string
index in a C local while walking a later child. A melt during that walk
could now free and reuse the slot. Each pushes the held value on the value
stack around the later walk and pops it after — the discipline `nth` already
kept for lists, applied where strings needed it. A first probe with a
top-only rollback reclaimed nothing at all: one live recent string blocks a
rollback, and mid-render there is always one.

**Witnessed.**

| probe | before | after |
| --- | --- | --- |
| 120K unique temporaries across 42 melts, table size after | 188,605 | 7,433 |
| slots reclaimed | 0 | 254,318 |
| 500 kept strings, one held string, one mixed concat | intact | intact |
| live glass loop strings at 4 / 12 / 24 s | 22K / 77K / growing | 14K / 23K / 33.5K flat from 16 s |
| live glass loop CPU | 41–49% | 45% |
| python-bmf-grammar cold compile | 2.32 s | 2.40 s |
| 140 string / record / grammar / glass bands | | verdict for verdict the same |

Quartet 42 / 31 / 1 / 2047. record-band 176 and sha256-band 2 on this seed
and the one before it.

## The most surprising teaching

Reclaiming from the top of the table was the obvious shape — the tick's
temporaries are born last — and it reclaimed exactly zero, twice. The
newest string is almost always alive when a melt fires, because a melt fires
mid-computation and the computation is holding what it just made. A
free-list of dead slots anywhere in the table is not the clever version of
the same idea; it is the only version that meets the melt where it happens.

## Where discomfort turned to gold

The marker could not see C locals, and the pull was to argue the hazard away
by rarity: a melt during a child walk, while an arm holds a string it has
not used yet. The BML floor compiler melts every few thousand conses; rarity
was not an argument there. Listing the arms that walk two children with no
push between them gave nineteen; reading each gave eleven that hold a
string index across the second walk. Eleven small pushes, and the marker
sees everything the melt sees.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2047, smelt probe 188605->7433 kept 500/500, glass strings flat at 33512, 140-band sweep identical
