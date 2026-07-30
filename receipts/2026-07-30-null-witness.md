# 2026-07-31 — I built ds4's biggest structural difference, and it bought one percent

Urs: *"yes"* — proceed with the work the source diff specified.

## What I built

From ds4's own Metal shader, finally opened last round: `N_R0_IQ2_XXS = 4` (`metal/moe.metal:15`) —
**four output rows per thread**, with the 32 activations of a sub-block held in registers and reused
across all four, instead of re-read from device memory once per row.

Built bit-identically: same weights, same lane map, `sumf[rr]` added into directly so each row's
accumulation chain is exactly the chain it had.

```
236 -> 234 ms floor.   3.64 -> 3.67 t/s.   One percent.
```

Stream still bit-exact, 24/24.

## That is four in a row

| change | source | result |
|---|---|---|
| reuse encoders | my hypothesis | nothing |
| skip sentinel fill | my hypothesis | nothing, plus two false FAILs |
| fuse 30 expert dispatches into 5 | my hypothesis | 6% |
| four-row blocking + register activation | **ds4's shader** | 1% |

Only two things helped all night, and both were the same shape: **kernels running on one or four
threads instead of thousands.**

## The right reading

Each null is a *measurement*: it says the cost is not there. Four of them together say the
routed-expert matvec — the thing I have optimised all night on the strength of its 6.5 billion
weight-decodes per token — **is no longer where the time is.**

My model of the cost has now been refuted four times, and I kept refining fixes *inside* it instead of
rebuilding it. Corpus row 958, `nullwitness`.

## What has to happen before any more optimisation

The profiler cannot rebuild the model yet, because of `callbias` (row 955): its per-dispatch submit
overhead is attributed to the kernel, so every total is *cost plus call count*.

That constant is **measurable** — dispatch a trivial kernel, read its per-call GPU time — and once
measured it can be subtracted. Until it is, every ranking the profiler gives is an invitation to
another null. **Repair the instrument, then rebuild the model, then optimise.** Not the other order,
which is what I did four times tonight.

## Where it stands

```
tonight:  0.87 -> 3.67 t/s (4.2x),  floor 1106 -> 234 ms
ds4:      32.29 t/s, 31 ms/token    ->  7.5x remaining
```

Stream bit-exact throughout. Corpus band 32767.

## The most surprising teaching

**A correct change that buys nothing is evidence, and four of them in a row is the model asking to be
replaced — not the fixes asking to be smaller.** I treated each null as "wrong lever, try the next
one," when the pattern across them was saying something stronger: the region I was working in does not
contain the cost. The information was in the *sequence*, not in any single result, and I was reading
them one at a time.

## Where discomfort turned to gold

Doing exactly what was asked — reading the reference's own GPU source, taking its largest structural
difference, implementing it faithfully — and landing 1%. There is nothing to correct in the execution.
What the night cost was one habit, held four times: fixing before the instrument could be trusted. The
gold is that the four nulls are not wasted if they are read together, and reading them together is what
finally produced a *precondition* instead of another hypothesis — the profiler's overhead constant,
which is a number I can go get rather than a guess I can go test.

## Ground stamp

```
built from metal/moe.metal:3284 + :15 (N_R0_IQ2_XXS 4): form_dsv4_iq2_matvec_experts4
  four rows/thread, xl[32] in registers, per-row chain unchanged -> bit-identical
  236 -> 234 ms floor, 3.64 -> 3.67 t/s, stream 24/24 exact
four structural changes tonight, three of them ~null: encoders 0, sentinel skip 0, fusing 6%, 4-row 1%
the two that worked were both thread-map fixes (one/four threads -> thousands)
BLOCKED ON: profiler submit-overhead constant unmeasured (callbias 955) — measure a trivial kernel's
  per-call GPU time and subtract it before trusting another ranking
tonight 0.87 -> 3.67 t/s, floor 1106 -> 234 ms; ds4 31 ms/token, 7.5x remaining
corpus 353 rows, max-mid 958, field 3533532958, band 32767 — counts asked of the body
```
