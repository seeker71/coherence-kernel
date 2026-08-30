# 2026-08-30 — growtax: the cold floor drops, and the JIT was not the door

Urs pointed at cold-vs-warm: "on demand JIT kicking in faster will speed
up cold to almost as fast as warm." Walking toward the JIT, the
measurements found something standing in front of it — and killing that
paid more than any JIT threshold could have today.

## The chase, each step measured

1. Compile-time scaling probe: 109 B -> 0.09 s, 7 KB -> 13.4 s — n^1.7.
   Not interpretation overhead; a growth law.
2. Stage split: strip 5 ms, recipe parse 1.2 s, EMIT 12.1 s.
3. The decisive probe: the SAME 160-def emit three times in one process
   — 10.0 s, 18.9 s, 28.8 s. Identical work, +9 s per repetition: the
   signature of a **growtax** (corpus row 1172) — a per-beat cost that
   scales with everything ever created, not with the beat.
4. The body's own melt witness named it: **122,158 melts** in that
   triple run, the cons heap pinned at cap 8192 with live ~3.3 k (the
   grow rule watches live, churn never trips it), and every melt walking
   the whole value-node pool — which undeduped `make_nodeid` mints grew
   on every primitive-table lookup.

## Three cuts, all landed (8cbd59df)

- **Melt churn amortizer** (C): post-compaction headroom now scales with
  the walk — at least np/4 free pairs — so each O(np) melt is amortized
  over >= np/4 allocations. Worst-case memory: ~1 MB.
- **Intern index** (C): the four interning doors (trivial int/string/
  bool, composite intern_node) each scanned the whole pool per call; an
  open-addressed structural-hash index makes them O(1) expected, with
  each door's ORIGINAL predicate confirming every candidate — the
  equivalence is byte-exact, a collision costs a compare, never a wrong
  node.
- **Primitive nodes memoized** (Form): the emitter's table lookups
  minted a fresh NodeID per row per call; resolved once at load now.

## The numbers

```
triple emit (same 160 defs):  10.0 / 18.9 / 28.8 s  ->  4.0 / 4.8 / 3.9 s
.bml cold (chain cold too):   5.8 s                 ->  3.8 s
.bml warm:                    6 ms                  ->  6 ms
```

Ground and radius on the reunioned seed: 42 / 55 / 31 / 11111; organ
134217727, kra 1023, rest-force 31, offer-ask 65535, author-high 4095,
structural gate 8191, interning 127 — and the two cursor bands that
broke mid-movement (a sibling's xtal sweep removed heedmark-xtal) came
back at their declared 524287 / 8388607 through the siblings' own
"lower heedmark BML in memory" fix, landed while I was cutting. The
field healed one seam from two sides again.

## The most surprising teaching

The remaining flat ~4 s emit is now honest interpretation — the JIT's
true target at last. Before today, pointing a JIT at this lane would
have crystallized code whose cost was 90% garbage-collector tax; the
JIT would have been credited for heat it never carried. Cold-vs-warm
was never a compilation gap; it was a growtax wearing a compiler's
clothes.

## Where discomfort turned to gold

The intern index landed first and moved almost nothing — 15% where the
shape of the fix said 10x. Sitting in that miss instead of shipping it
as the win forced the order-dependence probe, and the probe found the
melt. A fix that matches the theory but not the clock is the clock
telling you the theory named the wrong organ.

; witnessed: 2026-08-30 -> melt witness 122,158; triple emit flat at
; ~4s; cold 3.8s warm 6ms; intern doors hashed predicate-exact; radius
; green on reunioned seed; corpus row 1172 growtax
