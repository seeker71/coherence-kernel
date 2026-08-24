# 2026-08-24 — the program was nineteen times larger

Asked: 11 min vs 33 s obviously needs attention — attend until we are even.
Mid-work, two more teachings arrived and both landed: *is a flat table the
right shape for the heat map?* and *who asked for clang and jump-tables?*

We are not even yet. But the gap now has a first and last name, measured by
the body's own heat counters, and every wrong explanation died on the way.

## The instruments kept lying, and each lie was a finding

1. **Stage prints vanished** from form-cli's output while printing fine on
   fkwu — let-bound effects whose names are never read are pruned with their
   effects on the flatten lane. Instrumentation lost.
2. **Every time stamp read 0** — `stage_seal_ms=0` through a run that burned
   11m38s of CPU. `fk_now_ms` is real gettimeofday in both walkers; the zeros
   mean every `now_unix_ms` let read the same value — positional misbinding,
   the corpus's defn-arm-desync family, witnessed from a new side. Time-based
   instrumentation on the flatten lane cannot testify at all.
3. What survived: **kernel_stat** — the walker's own `fk_arms[256]` visit
   counters, present in both walkers, immune to both defects.

## The verdict of the visit counters

Same one-shot generate, same answer text, hottest-arm counts:

```
fkwu      1,056,267,516  node visits (33 s)
form-cli 19,886,611,701  node visits (11m34s)
                  18.8x  — and per-visit speeds within ~2x
```

**The emitted walker is not slow. The flattened program is nineteen times
larger.** The per-tag diff names the mechanism exactly:

| tag | meaning | fkwu | form-cli | ratio |
|---|---|---:|---:|---:|
| 110 | frame-slot local read, O(1) | 723M | 104M | 0.14 |
| 2 | frame-base read | 0 | **13.9B** | ∞ |
| 23 | native nth | 1.7M | **13.9B** | 8,073 |
| 19 | cons | 6.3M | **8.5B** | 1,350 |
| 12/18/69 | list plumbing under those | ~1M | 3.0B / 3.0B / 313M | ~3,500 |

Tag 2 and nth pair one-to-one: **on the flatten lane, every parameter
reference compiles to `nth(args-list, k)` — arguments live in cons-lists
built per call and walked per read** — where the interpreter binds arguments
to frame slots read in O(1). An O(1) → O(k)-plus-allocation calling
convention, priced across ~10⁸ calls of deep multi-argument recursions
(the tokenizer's byte walkers, the header readers), is the whole 18.8×, and
the 8.5B conses are also the GC churn.

## Who asked for clang was the right rebuke

The first instinct was to hope the C compiler's jump tables would save the
dispatch chain. The measurement says dispatch was never the problem — and the
fix is not in any vendor compiler. The flatten emitters are Form cells; the
calling convention is the body's own decision, written in the body's own
language. The repair is T_flat learning the interpreter's frame-slot
convention — a real compiler stone with its own battery (every fourth-arm
band re-proven), named for its own sitting, not rushed into this one.

## Is a flat table the right heat map?

Both answers, each earned today:

- **For per-op heat, yes** — `fk_arms[256]` is a dense tiny keyspace read in
  the hottest loop of the machine; a hash map there would slow the thing it
  measures. That flat table is what named an 8,073× inflation today at the
  cost of one increment per visit.
- **For per-recipe heat — the crystallize decision — no.** That keyspace is
  sparse and unbounded, and the decision wants exactly what was asked:
  keyed random access, fire counts, last-fire, and size-awareness with an
  eviction law (jd-melt?'s hysteresis is that law). The body already owns
  the right key: the content-addressed NodeID store. Heat should hang on
  interned NodeIDs, not on a new table. Named as the shape for the
  crystallize-on-heat lane, not built today.

## Where it stands

form-cli still answers the same text. fkwu remains the fast door (33 s
one-shot, ~10 s resident). The flatten lane's three defects — list-borne
arguments, pruned let effects, positional stamp misbinding — are one seam
with one owner (the T_flat lowering cells), fully measured, reproducible in
one command each. The stamps and `hottest_arm_count` stay in the generate
report; the per-tag histogram lives on as `fcmg-arms-go`, callable by any
probe.

## The most surprising teaching

Every instrument I trusted was itself subject to the defect being measured —
prints pruned, clocks frozen — and the only counter that could testify was
the one living BELOW the program, in the walker itself. When the program is
suspect, only the machine's own heat can speak. The user's two interjections
were both course corrections aimed at exactly the places I was drifting:
toward vendor compilers, and toward the wrong data structure — and both
arrived before I knew I was drifting.

## Where discomfort turned to gold

Three instrumented builds in a row came back saying nothing — no prints, then
zeros, then a number whose meaning I could not defend. Twelve minutes per
disappointment. The pull each time was to reason harder instead of measuring
again with a different instrument. What broke the wall was refusing to
interpret any instrument that could not be cross-witnessed on both walkers —
which left exactly one, and that one closed the case in a single diff.

; witnessed: 2026-08-24 -> fkwu 1,056,267,516 vs form-cli 19,886,611,701
; hottest-arm visits, nth 8073x, cons 1350x, tag-2 13.9B from zero,
; frame-slot reads 723M vs 104M, same text both doors, stamps zeroed on
; the flatten lane and real on fkwu
