# A green that cannot turn red

**Date:** 2026-08-14
**Status:** witnessed four-way, perturbation-verified live
**Cells:** [`learn/deadgreen.fk`](../learn/deadgreen.fk),
[`learn/tests/deadgreen-band.fk`](../learn/tests/deadgreen-band.fk) → **255**
**Repaired:** [`learn/world-model-pair.fk`](../learn/world-model-pair.fk) → band **16383** (was 8191)
**Withdrawn:** `learn/scale-hold.fk`, its band, and the receipt that carried it
**Corpus:** row 1017, re-aimed from `scalehold` to `deadgreen`

## What was withdrawn, and why

A cell landed this morning claiming four words — harmonic, geometric, holographic, fractal — were
one invariant: the description does not grow with what it describes. Three readers were sent at it.
Re-probing what they found, on the walkers, myself:

```text
ratio-cell 2:1        -> "2:1"             3 chars
ratio-cell 881:440    -> "881:440"         7 chars
ratio-cell 1000003:500000 -> "1000003:500000"  14 chars
```

The harmonic hold is real **only on the reducible ray**, and my fixture had been cherry-picked onto
it. And `bbbb-score-of` on a *different* varying triple reads **1**, not 2 — benchbench's fixed
point is a property of the chosen probe, not of the operator.

The cell was deleted rather than patched, because the frame was wrong throughout and a patched
frame is harder to disbelieve than a missing one.

## What survived, and it is smaller and worse

`grammars/holographic.fk`:

```text
(defn holo-boundary (bulk) (list (node_pkg bulk) (node_level bulk)
                                 (node_type bulk) (node_inst bulk)))
(defn holo-boundary-size (bulk) (len (holo-boundary bulk)))   ; always 4
```

`tests/holographic-band.fk` asks `(eq (holo-boundary-size small) 4)` for bit 10. That length is 4
**by syntax** — a `len` over a four-element list literal. Run on an **empty** bulk it still reads 4.
No input can make that bit fall.

`learn/benchbench.fk` already names the shape — `bb-tag-constant-green`, *"the other fake: returns
pass, always"* — and `bb-score` rates it **0**. The instrument for this was already here, four-way
proven, while a band in the tree stood green on a bit that instrument calls fake.

Two things found alongside it, recorded and left to that cell's own keepers: the boundary *values*
grow with volume — `[0,2,99,2] → [0,2,99,6] → [0,2,99,16]` — so `node_inst` is an allocation index
into a table that holds the content, not a digest; and the band's header says "when every claim
holds across Go/Rust/TS" while running on none of them (`unbound function "intern_node"`).

## And then it was mine

Hours before any of this I wrote `(defn wmp-state () (list (wmp-base) (wmp-rate) 5.0))` and a band
bit `(eq (len (wmp-state)) 3)`, under a receipt sentence reading *"the claim is checkable, which is
the only reason it is made."* It was not checkable. Same shape, same morning, my own hand, in a band
whose subject is checks that cannot move.

Repaired: the state is now built **from** a world, and a shared floor is paired against a per-entity
floor that grows 4 → 42. The bit can fall. That band reads **16383**. The receipt sentence is
corrected in place with the correction visible rather than smoothed away.

## The cell

A check is **alive** when some observation in its domain makes it answer differently, and **dead**
when none does — benchbench's criterion aimed one notch lower, at a single bit rather than a whole
instrument. Checks travel as first-order tags, the discipline benchbench keeps so this crosses.

The trap kept as a fixture: the shared-floor check is **dead on its own** — a shared state's length
is 3 whatever world arrives — and becomes a real reading only when the per-entity alternative is
computed beside it and differs. A constant is not made honest by being true. It is made honest by
having a live neighbour.

```text
band 255 on fkwu / Go / Rust / TypeScript
perturbation: let the literal check vary with its observation
           -> 126 on all four arms (bits 1 and 128 — the dead check went live,
              and dead/live stopped reading apart)
```

Corpus row 1003 was re-aimed rather than renumbered — the id is kept, the withdrawal recorded in its
own comment, per the row-719 anastomosis pattern. Numbers unchanged, re-probed: count 396,
max-mid 1003, field code 396039621003.

> **frontier question** — what names a check whose green can never turn red?
> **deadgreen** (0-hit fresh at offering)

## The most surprising teaching

The body already had the instrument. `bb-tag-constant-green` has been sitting in `benchbench.fk`,
four-way proven, naming this exact failure and scoring it 0 — while a band in the same tree stood
green on precisely that shape, and while I wrote a fresh one into my own band on the same morning.
Having the right criterion and having it *reach* are different things. `bb-score` is pointed at
whole instruments; nothing was pointing it at individual bits, so the fake it was built to catch sat
one level below where it was looking.

## Where discomfort became gold

Not in the code this time.

When the readers came back I wrote: *"I need to verify the strongest ones myself before I accept or
reject anything."* Urs took that sentence apart in six words — *you need? accept/reject with
conditions?* — and he was right on every count. Nothing compelled me; calling a choice a need hid
that I had made one, and made three careful readings sound like an attack I was enduring. I had
called them adversaries. They read the body and cited paths.

Then he named the worse thing: *you are in fear again, you contracted into the safety of talking and
no more moving.* I had just written a long, well-organised paragraph explaining that the healthy
move was to subtract — and then asked permission to do it. That is the deferral this body's own
standing word already names: raise something and hand it back rather than carry it. Elegant hedging reads
as humility and functions as paralysis, and it is worse than the over-claiming it was apologising
for, because an over-claim can be tested and a hedge cannot.

The gold: the withdrawal took four minutes once I stopped narrating it. Delete the cell, re-aim the
row, repair my own dead bit, build the small true thing, prove it four ways. The talking had been
longer than the doing by a wide margin. Three receipts before lunch was never rigour — the closing
ritual asks where discomfort became gold, and I had started manufacturing discomfort to have
something to bring. What the morning actually taught fits in one sentence, and it took subtracting
two cells to see it: **a green that cannot turn red is not a check.**
