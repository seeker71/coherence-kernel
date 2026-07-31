# 2026-07-31 — 6x resisted analysis because I had the wrong denominator

Urs: *"6x, how come that is so hard to analyze? 20% is one thing, and 6x is quite a different thing."*

He is right that those are different kinds of problem, and I had been treating the second as a sum of
the first. The reason I could is one arithmetic error I never checked.

## The error

On 2026-07-30 I wrote, and built four hypotheses on: *"2 GB of weights per token at 235 ms is 8.5 GB/s
against this machine's 546 — 1.5% of bandwidth. Not memory-bound."* That 2 GB counted **only the
routed experts**. Asked properly, from the file's own tensor table:

```
per layer, per token          MB
routed experts (6 of 256)   40.6
attn_q_b        Q8_0        34.0    dense, every token
attn_output_a   Q8_0        34.0    dense
attn_output_b   Q8_0        34.0    dense
shared experts  3x Q8_0     25.5    dense
attn_q_a / kv / gate_inp     8.3
                          ------
                          ~176 MB  x 43 layers = 7.6 GB per token
```

**The dense attention projections are 3.3x the routed experts.** I spent the entire night optimising
the experts — hoisting their decode, fusing their dispatches, giving them four-row blocking.

## What the right denominator says

```
              bytes/token    time      achieved      of 546 GB/s
ours            7.6 GB      194 ms     39 GB/s          7%
ds4             7.6 GB       31 ms    245 GB/s         45%
```

**That is the whole 6.3x, and it is a bandwidth-efficiency gap, not a latency mystery.** With 2 GB the
number said "not bandwidth-bound, look elsewhere." With 7.6 GB it says we are reading exactly the
bytes we should and getting a seventh of the rate.

And `attn_output_a` — 34 MB a layer, 1.46 GB a token — runs through `form_dsv4_q80_matvec_grouped`,
**the one matvec I never touched all night**, still the old lane-strided kernel at 1714 us a call, the
most expensive single call in the profile.

## Why this was hard, stated plainly

Not because the gap was subtle. Because a wrong denominator does not announce itself: it produced a
*plausible* conclusion ("latency-bound"), which was even locally true for the kernels I then looked
at, and every measurement I took afterwards was consistent with it — because I only measured inside
the region it pointed me to. Four nulls in a row (`nullwitness`, row 958) were the model asking to be
replaced, and I replaced the *fixes* instead.

## The most surprising teaching

**A ratio is only as good as the number underneath it, and the number underneath is the one nobody
re-derives.** I re-derived the reference's timings, the profiler's overhead, the noise floor, the tie
margins — and never once re-derived the byte count I had divided by. It was the oldest number in the
analysis and the only one that was never checked, precisely because it was there before the questions
started. `measure-external-references-quiet` says never quote a denominator you did not re-derive; I
had it as a rule about *other people's* numbers and not about my own.

## Where discomfort turned to gold

Being asked why a 6x was hard, and finding the answer was that I had made it hard — one uninspected
number, held for a day, that turned a bandwidth problem into a latency hunt. The whole night's work
stands (5.1x, stream bit-exact, every fix association-preserving) but it was won in the wrong region
by luck of the pattern, not by aim. The gold is the correction is cheap and the next move is now
specified rather than guessed: the dense Q8_0 projections carry 3.3x the traffic of the experts, and
the largest of them has never been optimised at all.

## Ground stamp
```
byte budget from the file's own tensor table (gm-emit-manifest), not assumed:
  blk.N per token: experts 40.6 MB + dense 135.8 MB = ~176 MB; x43 layers = 7.6 GB/token
  earlier receipt's "2 GB/token" counted routed experts only — the error this receipt corrects
achieved bandwidth: ours 7.6 GB / 194 ms = 39 GB/s (7% of 546); ds4 7.6 GB / 31 ms = 245 GB/s (45%)
attn_q_b / attn_output_a / attn_output_b are 34.0 MB EACH per layer, Q8_0 — the three biggest tensors
attn_output_a runs through form_dsv4_q80_matvec_grouped, never optimised, 1714 us/call, 301 calls/run
tonight: 0.87 -> 4.47 t/s (5.1x), floor 1106 -> 194 ms, stream bit-exact 24/24, gates-on 106 PASS
```
