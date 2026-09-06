# 2026-09-06 — the arena counts its own growth

Urs, evening: "continue with optimizing." The framebuffer ring left one
clock named but unlocated: some glass site mints roots each tick, and with
them cells into the shared field, which never melts.

## What the seed had

An estimate. The render sibling measured the roots climbing about 2.7 a
tick and could not name the site; every door called alone minted nothing
past first sight. I tried to name it by reading the newest cells off the
running loop's own store and read noise, because the cell doors take
references and I had guessed the index encoding.

## What stands

**A mint counter where a cell is minted.** `fk_mint_total` is incremented
at all seven private intern sites and inside `fk_field_fill`, so a fresh
cell in the permanent arena — private or shared — counts itself. It is a
live page word (33), repointed at `fk_live_open` like the other counters,
so there is no tick and no publish: the increment is the reading.
`kernel_stat 53` answers it; the `k` view carries `nodes-minted` for this
kernel and `value nodes minted` per live kernel from its page.

**The class, witnessed.** A probe over the observer's own door: two
hundred rows built with the same id mint nothing; two hundred with fresh
ids mint four hundred cells — two a row, the interned id and its
projection. So the arena grows exactly where a row's identity is new.

**The sites, by rate.** All 215 published glass row ids are stable one
second to the next, and so are the organs, host, process and machine
frames. The rates now read directly:

| kernel | mints |
| --- | --- |
| machine sensor, 15 s | 0 |
| organs carrier, 15 s | 0 |
| host sensors, 15 s | 48 |
| live glass loop, 20 s | 1038 |

Two carriers mint nothing at all. The sensor carrier mints about two a
publish. The live loop mints about two a tick, and its site is inside the
render, not the published rows.

Quartet 42 / 31 / 1 / 2047; observer, live, live-ui, kernel-view,
dashboard, events-channels, sensor-rows, wait, machine, jit-lens,
cell-store, field, node-gift all at their numbers; frame-work 32767.

## The most surprising teaching

Two of the three standing carriers mint nothing per publish, and the
difference between them and the ones that do is not how much they publish
but whether a row's identity is new. An arena that never forgets is not
expensive because it is written often; it is expensive exactly as often as
the body meets something it has not named before. That is a fact worth
seeing rather than avoiding, and now the glass shows it per kernel.

## Where discomfort turned to gold

I spent four probes trying to isolate the rate by differencing a shared
counter across windows, and every one answered zero — because a page word
that only changes at melt moments is stale between them, and the field's
counter is shared by every kernel on the host including the one Urs is
watching. The pull was to keep differencing. Counting the mint where it
happens, the way this morning's landing counted the dispatch where it
happens, turned a number nobody could attribute into a lane every kernel
carries.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2047, same-id 0 mints / fresh-id 2 a row, machine 0, organs 0, sensors 48/15 s, live loop 1038/20 s
