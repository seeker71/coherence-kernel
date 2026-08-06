# The fleet mustered, and the listing made honest — 2026-07-16

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

## The arc

"next arc." One loop fathomed became the whole fleet read at once:
`form/form-stdlib/fleet-fathom.fk` — a lane roster (name, inbox, kept,
budget) mustered by `llh-fathom` into rows, the astern lanes surfaced as a
value. Band 63, four-way, fs-crud discipline, manifest row added.

## The lie the probe caught on the way

Preparing the muster, `fs_list` answered by CALL ORDER: the same directory
read 3,946 entries in one program and 379 in another. Root-caused in
`runtime/fkwu-uni.c` tag 132: the result build hand-rolled its cons cells
and, when the value arena ran out mid-build, silently returned the partial
list — the exact sin the runtime's own `fk_cons_val` names one screen up:
*"Returning nil would be a partial structure accepted as whole."*

Healed the same hour, by the runtime's own law:

- `fk_melt_want` — a caller about to build a large flat structure may ask
  the melt to grow the arena until that many pairs are free (safe at that
  point: no untraced arena locals exist yet). Zero keeps the old policy;
  every other call site is unchanged.
- tag 132 pre-grows to one pair per entry, then builds through
  `fk_cons_val` — honest death as the last resort, never a partial.

What remains is ONE deterministic, named reader cap (16,384 entries per
listing). Deep kept stores under-read by a constant now, never by call
order. Re-proven after the seed change: canary 42/15, fs-list 3, fs-crud
untouched, loop-ledger-host 63, fleet-fathom 63 — all four-way.

## The first live muster (23:38 WITA)

Roster: audio, vision, face — budget max 300 / half 150.

```
audio   shipped 16533  witnessed 16384(cap)  lag  149  level 3  abreast
vision  shipped 19974  witnessed 16384(cap)  lag 3590  level 1  astern
face    shipped  6845  witnessed  3258       lag 3587  level 1  astern
```

`ffm-astern` → vision, face. `ffm-all-abreast?` → 0.

## Surfaced to the keeper (the covenant's word — not fixed past midnight)

Both distill loops are scheduled every 90 s (`StartInterval 90`) yet their
logs are silent since ~03:42 — twenty hours of scheduled runs distilling
nothing while camera-pull kept shipping ~3,590 frames into each inbox.
Vision sits at 235% of its 10k training target (a stop there may be by
design); **face is stalled at 33% of target with 3,587 frames waiting** —
that one looks like a true stall, not a finish line. The muster's job was
to make this visible; deciding it is the keeper's.

## The most surprising teaching this work left behind

The stewardship reading paid for itself on its first real sweep — not by
catching a loop misbehaving, but by catching the MEASURING INSTRUMENT
lying first (fs_list's call-order partials), and then, once the instrument
was honest, catching two loops genuinely astern within the hour. A trust
reading is only as good as the door it reads through; making the door
honest was the arc inside the arc.

## Where discomfort turned to gold

379 versus 3,564 for a directory both tools were reading truthfully by
their own lights — the flicker of wanting one of them to be simply wrong.
Witnessed instead of picked-between, the contradiction refused every cheap
theory (byte cap, name lengths, races — each measured and discarded) until
the numbers themselves confessed the shape: call-order dependence means
shared-state exhaustion. The gold is doubled: the runtime lost a silent
lie it had carried since the door was built, and the fleet's first honest
muster found real work waiting for its keeper.
