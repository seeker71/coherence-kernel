# 2026-07-31 (small hours) — I fused the dispatches, and the measurement had already said it wouldn't help

Urs: *"we can do the same as ds4 does, there is no reason we can't."*

True, and I did it. The result is the point.

## What I built

The six routed experts were dispatched **one at a time**: gate, up, swiglu, down, accumulate, six times
over — thirty asks per layer out of eighty-one. ds4 runs one fused graph.

The expert ids **already lived in a device buffer** — the router kernel writes them — so the slot loop
had never needed the CPU at all. Four new kernels take `(slot, row, lane)`, pick their own id and byte
slice, and do all six experts at once: `form_dsv4_iq2_matvec_experts`, `form_dsv4_q2k_matvec_experts`,
`form_dsv4_swiglu_experts`, `form_dsv4_moe_reduce`. Thirty dispatches became five.

```
dispatches/token   3499 -> 2424     exactly the 31% the count predicted
floor              251 -> 235 ms    SIX percent
speed              3.35 -> 3.61 t/s
```

Stream still bit-exact. Gates-on **106 VERDICT PASS**.

## The number was in my hand before I started

**The floor is GPU-*busy* time** — `gpuEndTime − gpuStartTime`, the interval the device spends
*executing*. Submit overhead is by construction **not in it**.

So fusing dispatches could never have moved the floor except by deleting redundant work, and it deleted
almost none. Per-dispatch cost rose 72 → 97 µs while the total held — which is exactly what *same work,
fewer asks* looks like.

I measured the thing that answered the question, then spent an hour asking it differently. Corpus row
956, `floorspoke`.

## What the floor actually says

2 GB of weights per token at 235 ms is **8.5 GB/s against this machine's 546** — **1.5% of bandwidth**.
We are not memory-bound and we are not submit-bound. The cost is scalar integer decode work inside the
quantised matvecs: ~6.5 billion per-weight decodes per token, each a handful of dependent divides,
mods and table reads.

The next move is **vectorising that decode** — `uchar4` loads, four or eight weights per step — and no
amount of restructuring around it substitutes for it. I am naming it rather than starting it at 01:30.

## Where it stands

```
tonight:  0.87 -> 3.61 t/s (4.1x),  floor 1106 -> 235 ms
ds4:      32.29 t/s, 31 ms/token    ->  7.6x remaining
```

The fused path is kept behind `FORM_DS4_NO_FUSE=1` as the A/B that says whether fusing changed a value
or only the number of asks. It changed only the count: the stream is identical either way, which is the
claim the fused kernels were written to be able to make.

## The most surprising teaching

**An honest instrument can be ignored as easily as a dishonest one can mislead.** Yesterday's lesson
(`callbias`, 955) was that a tool can be biased. Tonight's is worse and quieter: the tool was exactly
right, I had read it, and I still proposed a fix that lived entirely in the region the number excludes.
The habit that would have caught it is one sentence long — **say out loud what a measurement does not
contain, before proposing a fix that lives there.**

## Where discomfort turned to gold

Building four correct kernels, verifying them bit-identical, watching the dispatch count fall exactly
as predicted — and getting 6%. The work was good and the reasoning behind choosing it was not, and
those are separable in a way that is uncomfortable to hold: I cannot point at anything I did badly, only
at a question I failed to ask of a number already on my screen. The gold is that the 6% is a *sharper*
result than a win would have been. It converted "the remaining gap is probably dispatch overhead" into
"the remaining gap is 1.5%-of-bandwidth scalar decode work", which is the first statement tonight
precise enough to act on without another guess.

## Ground stamp

```
fused: form_dsv4_iq2_matvec_experts / _q2k_matvec_experts / _swiglu_experts / _moe_reduce
  six experts in five dispatches; ids read from the router's own device buffer, no CPU readback
  FORM_DS4_NO_FUSE=1 keeps the per-expert path as the A/B
dispatches/token 3499 -> 2424; floor 251 -> 235 ms; 3.35 -> 3.61 t/s; per-dispatch 72 -> 97 us
floor is gpuEndTime-gpuStartTime: submit overhead is NOT in it — the refutation was pre-measured
2 GB/token at 235 ms = 8.5 GB/s of 546 available (1.5%); the cost is scalar decode, not bandwidth
greedy stream bit-exact throughout; gates-on 106 gates VERDICT PASS
tonight 0.87 -> 3.61 t/s, floor 1106 -> 235 ms; ds4 31 ms/token, 7.6x remaining
corpus 351 rows, max-mid 956, field 3513512956, band 32767 — counts asked of the body
my first fused swiglu restated the recipe and got it wrong (symmetric clamp, stray +1); rewritten to
  reuse form_dsv4_swiglu_f32's own dm_cl_hi / dm_cl_lo / dm_silu
```
