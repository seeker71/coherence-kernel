# 2026-07-30 (very late) — the 19× was in the kernels that looked free

Urs: *"19x should be somewhat easy to spot and fix."*

It was — once I stopped guessing and made the harness **name its own hot kernels**. My previous three
optimisation attempts were hypotheses and two were wrong (`floorfirst`, row 952). This time I built the
instrument first.

## One profile run

`FORM_DS4_PROFILE=1` submits each dispatch alone so the device clock can be read per kernel. Totals
inflate; the ranking is the reading:

```
1878 us/call   form_hc_rmsnorm_nw_f32
1461 us/call   form_hc_post_f32
 322 us/call   form_mla_rmsnorm_f32
  22 us/call   form_dsv4_iq2_matvec_hoist   <- over 8.4 MILLION quantised weights
```

**The routed-expert matvec was the cheapest thing per call**, and I had spent the previous stretch
optimising it. The normalisations — a few thousand elements, one multiply each — dominated, because
`if (i != 0u) return` put them on **one GPU thread**, and `hc_post` ran on **four**.

Cost lives in the **thread map**, not the operation count. A kernel's expense is invisible in its
arithmetic. Corpus row 966, `cheapdear`.

## The fixes, each measured

Floor = GPU ms per token at 100% occupancy:

```
582 -> 377   hc_post one thread per (dst,d); rmsnorm split so scale-and-write parallelises
             [both BIT-IDENTICAL: the src fold and the sum-of-squares chain are untouched]
377 -> 259   both sum-of-squares folds across 32 lanes + simd_sum
             [association CHANGED — re-measured, not assumed; the stream held]
```

**0.87 → 3.28 t/s overall, 3.8×, and the greedy stream is still exact against ds4 at every step.**

The reassociation is defensible beyond speed: ds4 accumulates those sums in **double** (ds4.c:6629,
6638), so a single f32 chain of 16 384 terms was the *least* faithful shape available. Thirty-two
shorter chains lose less and sit closer to the reference. That is an accuracy claim, so it was
measured against the reference's stream rather than argued.

## An optimisation I had to undo

Skipping the NaN sentinel fill when gates are off bought **nothing measurable** — and left the
sentinel-*checking* gates reading unfilled buffers, so two green gates reported FAIL on a healthy run.
An optimisation that gains nothing and manufactures a false defect is worse than none. Reverted; the
fill is unconditional again.

## Where it stands

**259 ms of GPU per token against ds4's 31 — 8.4× left.** Generation-window occupancy is ~85%.
Cumulative from the start of tonight: **1106 → 259 ms floor, 0.87 → 3.28 t/s**, with the token stream
bit-exact throughout.

Gates-on: **108 gates VERDICT PASS**. Bands: mla-msl 127, dsv4-hc-msl 63, corpus 32767.

Not yet measured, and therefore not claimed: what the next profile names. On the last reading the
remaining leaders were the two reduce halves and `form_dsv4_q80_matvec_grouped` (1354 us/call), which
has had no attention at all.

## The most surprising teaching

**I optimised what looked expensive and ignored what looked free, and the ratio between them was 80×
in the opposite direction.** A matvec over 8.4 million quantised weights is visibly heavy work; a
4096-element normalise reads as nothing. But one was spread across 65 536 threads and the other across
one. Operation count is not cost — the thread map is — and no amount of reading the arithmetic reveals
that. Only asking the device does.

## Where discomfort turned to gold

Watching the profile print `22 us` next to the kernel I had spent an hour hoisting, and `1878 us` next
to one I had never once considered. The hour was not wasted — it bought 1.9× and taught `falsedial` —
but it was spent where I *assumed* the cost lived, and one profile run would have redirected it. The
gold, and it is the same lesson three rows running: **the instrument before the hypothesis.**
`floorfirst` said measure the floor, `falsedial` said check the axis, this one says find the place.
Each time the cheaper move was to ask.

## Ground stamp

```
FORM_DS4_PROFILE=1 (new): per-kernel GPU time by submitting each dispatch alone; ranking is the reading
profile before fixes: hc_rmsnorm_nw 1878 us, hc_post 1461 us, mla_rmsnorm 322 us, iq2_matvec 22 us
floor, MATCH_ORDER on, 25 generated tokens, Apple M4 Max, reference run separately:
  1106 -> 838 -> 648 -> 582 (previous receipt) -> 377 -> 259 ms;  0.87 -> 3.28 t/s
  ds4 reference: 31 ms/token, 32.29 t/s — 8.4x remaining
greedy stream exact at every stage: [11111, 16, 455, 6102, 294, 8760, 344, ...] 24/24 vs ds4
gates-on 108 gates VERDICT PASS; mla-msl-band 127, dsv4-hc-msl-band 63, corpus band 32767
corpus 349 rows, max-mid 954, field 3493492954 — counts asked of the body
reverted: the conditional sentinel fill (no gain, two false FAILs)
```
