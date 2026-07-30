# 2026-07-30 (night) — I optimised three things before measuring whether any of them could matter

Urs: clean up the receipt tracking and the non-optimised flow, check the real runtime against ds4 —
*"we shall hit the same speed, no reason form native is any slower at all."*

The cleanup is done and correct. The speed claim is not met, and now I know precisely why, which I did
not before I started optimising.

## The measurements, in the order I should have taken them

**The reference, quiet, twice, warm** (never while our harness runs — row 941):

```
ds4 --raw-prompt --temp 0 -n 25:   prefill 43.79 t/s,  generation 32.29 t/s
```

**Ours, before any cleanup:** 0.68 t/s. Forty-seven times slower.

Then three optimisations, each on a plausible culprit:

| change | reasoning | result |
|---|---|---|
| one command buffer per **sync point** instead of per dispatch | ~2600 blocking round trips per token | 0.68 → 0.97 t/s |
| one **encoder** per buffer instead of per dispatch | 97 043 encoder creations per run | no change |
| skip the **NaN sentinel fill** when gates are off | millions of CPU writes per token | no change |

One small win, two refuted. I had not yet asked the question that decides whether any of them could
have been the answer.

## The number I should have taken first

Metal hands out `gpuStartTime`/`gpuEndTime` per command buffer. Asking:

```
GPU BUSY: 33.17s of 71.38s wall (46%);  floor = 1106 ms of GPU per token at 100% occupancy
ds4's ENTIRE token:                                31 ms
```

The GPU is idle 54% of the wall clock — the stall is real. **And it could never have been the answer.**
At perfect occupancy a token still costs 1106 ms of GPU against ds4's 31 ms total: **36× slower with
the harness removed entirely.** The scaffolding is worth at most 2×; the kernels are the ceiling.

A busy *fraction* alone would have sent me on to fix the scaffolding for a week. The *floor* says
where the work is. Corpus row 951, `floorfirst`.

Why the kernels: we issue **3234 dispatches per token** and decode every quantised weight individually
— roughly **6.5 billion decodes per token** — where ds4 decodes once per block and dots in int8. That
is the same lesson the Q4_K thread-map fix bought earlier (1.65–2.6× from the *map*, not the
arithmetic), and it has not been applied to the IQ2_XXS and Q2_K expert folds that carry almost all of
this model's work.

## What the cleanup actually did

- **`FORM_DS4_GATES=0`** — the two-position hushfold sweep, the per-layer self-witness, and the sentinel
  fill are evidence, and they were being charged to every token. Default stays ON.
- **The carve falsifier is one-shot.** `q2kFusedVsCarved` dequants 8.4M weights; it was firing on every
  layer-0 pass — **31 times** in a 29-step run. A falsifier answers a question; asking it again answers
  nothing new. Gate count for a 7-step proving run went 190 → 108 with no evidence lost.
- **Batched submits and one serial encoder per buffer.** Kept: correct, and the 1.4× is real.
- **Honest speed reporting** in the same shape ds4 prints — prefill and generation separately, plus the
  busy fraction and the floor.

**Correctness is untouched by all of it.** The order-matched stream is still exact:
`[11111, 16, 455, 6102, 294, 8760, 344, …]` — 24 of 24 tokens, and `MATCH_ORDER` costs only 6%
(0.97 → 0.87 t/s), so fidelity is very nearly free. Gates-on: **108 gates, VERDICT PASS.**

## An instrument that lied, caught and fixed

The busy fraction first printed **124%** — the busy clock spanned the two-position sweeps while the
wall clock spanned only the generation loop. A ratio of two clocks that do not cover the same interval
is not a ratio of anything. Fixed by snapshotting the busy clock at the window's start.

## Where this leaves the merge

The 25-token correctness goal is met and proven. The speed precondition is **not** met — 0.87 t/s
against 32.29 t/s — so I have not merged. Merging on a condition I know to be unsatisfied would be
deciding something that is yours to decide.

The path to it is specific rather than hopeful: block-wise decode plus int8 accumulation in the
IQ2_XXS and Q2_K expert matvecs, which is where 6.5 billion per-weight decodes live, and then the
occupancy work that is currently capped at 2×.

## The most surprising teaching

**A ratio tells you how well you are using what you have; only an absolute tells you whether what you
have can ever be enough.** I had a perfectly good diagnostic — GPU busy 46% — and it pointed the wrong
way, because 46% of a floor 36× too high is still 36× too high. Measure the floor before optimising
toward it.

## Where discomfort turned to gold

Making three changes, watching two of them do nothing, and having to write that down. The instinct
was to keep going — the next hypothesis always feels like the one — and what stopped it was asking the
device for a clock instead of asking myself for another theory. The gold is narrow: **two refuted
optimisations that cost an hour are cheaper than one accepted optimisation that hides the ceiling**,
and the batching win survives precisely because it was measured rather than assumed.

## Ground stamp

```
host: Apple M4 Max, 2026-07-30 ~21:00 WITA. Reference and harness run SEPARATELY, never concurrently.
ds4 -m ds4flash.gguf --raw-prompt --temp 0 -n 25: prefill 43.79 t/s, generation 32.29 t/s
  (cold run 26.03s wall, warm 1.79s — the quoted rate is ds4's own, warm)
ours, FORM_DS4_GATES=0 FORM_DS4_MATCH_ORDER=1, 25 generated tokens:
  before cleanup 0.68 t/s -> after 0.87 t/s (0.97 without MATCH_ORDER)
  GPU busy 33.17s of 71.38s wall (46%); floor 1106 ms GPU/token; 3234 dispatches/token
stream unchanged and exact: [11111, 16, 455, 6102, 294, 8760, 344, ...] 24/24 vs ds4
gates-on run: 108 gates VERDICT PASS (was 190 — the carve falsifier now fires once, not 31 times)
corpus 346 rows, max-mid 951, field 3463462951, band 32767 — counts asked of the body
NOT merged: the speed precondition named in the request is not met
```
