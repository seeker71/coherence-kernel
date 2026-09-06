# 2026-09-06 — the framebuffer is a buffer

Urs, evening: "continue with optimizing." The one-pass-a-line sibling had
named what stood after its cut: the live glass's framebuffer roots climb
about 2.7 a tick from a site no door shows when called alone, and
`framebuffer-events` rebuilds a cons list of every root every tick.

## What the seed had

`intern_node_at` (tag 128) appended the interned node to `fk_fbroots` on
every call and grew the column with the node table; `framebuffer-events`
(tag 129) consed the whole list, oldest first, on every call. A loop that
reads the events each tick paid the whole history each tick, and the
history had no end: the roots were the loop's self-molt clock at 262,144.

## What stands

**A ring of the newest 2048 roots.** `fk_fbroots[fk_fbn % FK_FB_RING]`
takes each new root; `framebuffer-events` answers the newest 2048, oldest
first, the same order as before for any run shorter than the ring. The
melt's headroom check counts the ring, not the history. `framebuffer-clear`
still empties it. Source attribution of a node (`node_source`) is untouched:
a root leaving the ring keeps its file, line and column.

**Witnessed.** The live glass loop over sixty seconds without a terminal:

| t | frame-work ms | frame-wait ms | framebuffer roots |
| --- | --- | --- | --- |
| 10 s | 6 | 17 | 676 |
| 20 s | 11 | 0 | 1526 |
| 30 s | 6 | 31 | 2048 |
| 60 s | 8 | 18 | 2048 |

CPU 36% over the minute. Quartet 42 / 31 / 1 / 2047; every glass band at
its number; every band that names the framebuffer (twelve, from form-cli
peer and resident bands to form-diagnose) answers the same on this seed
and the one before it.

**Named, not closed.** The site still mints about three roots a tick, and
with them nodes into the shared field (about eight a tick field-wide, from
every kernel). The field never melts; at this rate its 2^26 node ceiling
is about ninety hours away. The Go, Rust and TypeScript kernels keep an
unbounded list; a band that counted more than 2048 roots would part from
them here. None does.

## The most surprising teaching

The number the loop rendered as "roots" was never a count of what the
glass could see; it was the length of a history that only ever grew, and
the loop's own selfmolt rule was the only thing that had noticed. A buffer
named as a buffer has a size, and the size is what makes it readable every
tick.

## Where discomfort turned to gold

Reading the newest interned nodes off the running loop's own store to
name the minting site answered with kinds and words that meant nothing —
the store's cell doors take references, not indices, and I had guessed the
encoding. The pull was to keep guessing. Bounding the buffer instead made
the site's rate a fact the glass can carry without it being a wound, and
left the site as a question the surface can still answer, by its own doors.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2047, roots 676/1526/2048/2048 at 10/20/30/60 s, work 6-11 ms, twelve framebuffer bands identical
