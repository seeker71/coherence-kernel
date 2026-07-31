# 2026-07-30 (late night) — merged, then found the tradeoff I had declared was mostly imaginary

Urs: *"yes, merge and then fix the 36x speed issue."*

## The merge

[PR #409](https://github.com/seeker71/coherence-kernel/pull/409) is in main. The only conflict was a
real **meaning-id collision**: while the branches were apart, both took `max+1` and both landed on
**931**. Main's is `vacuous`; ours was `askalike` with twenty rows behind it.

Resolved by the corpus's own law — keep every row, move the unmerged line. Main's 931 keeps its id
because it is already in the trunk and its own receipt cites it; renumbering it would silently falsify
a published reference. Ours had never left the branch, so **931..951 became 932..952**, and all
eighteen files that cite those ids moved in the same commit.

**My first renumber was wrong and the body caught it.** I wrote one rule for `(hdc-row NNN` and another
for `row NNN` — and `(hdc-row 931` matches both, so every row bumped **twice**: 931 and 932 vanished,
952 duplicated. Restored all eighteen files and redid it with a single rule. Verified by asking the
body, not by eye: **count 347, admissible 347, max-mid 952, duplicate meaning-ids 0**.

**A regression I shipped:** the renumber wrote each file through `awk > tmp && mv`, which drops the
executable bit. `metal_dsv4_stack.sh` and `metal_q2k_gpu.sh` went to main as `100644`. Found only
because the next run refused with *permission denied*. Restored here.

## Then the 36×

Starting point: **0.87 t/s** against ds4's **32.29**, with a floor of 1106 ms of GPU per token at 100%
occupancy against ds4's 31 ms *total*.

The cost was never the order. `iq2_w` and `q2k_w` re-derive, **for every single weight**, things that
are constant across a whole block: two f16 decodes, a four-byte aux reconstruction, a scale, and two
power-of-two **loops**. Two of every three routed-expert weights on this file are IQ2_XXS.

```
                                       floor      speed     stream
start                                 1106 ms   0.87 t/s    exact
Q2_K hoisted (bit-identical)           838 ms   1.13 t/s    exact
IQ2 hoisted (map changed)              648 ms   1.45 t/s    exact
both upow2 loops removed               582 ms   1.62 t/s    exact
```

**1.9× so far, and the greedy stream stayed bit-exact against ds4 at every step.**

The Q2_K hoist is bit-identical *by construction* — it walks the same `k` ascending, only moving
invariants out, so `ds = d*(sc mod 16)` then `ds*q` associates exactly as before. The IQ2 hoist is not:
each lane now owns whole 32-element sub-blocks instead of striding single columns, which changes the
within-lane summation order. That one had to be **re-measured**, and it was — the stream survived.

## What I got wrong yesterday

I wrote, and meant, that *"fidelity and throughput are the same dial."* The order-matched lane runs one
thread per row where the fast lane runs thirty-two, so matching ds4's association looked like it had to
cost speed.

It doesn't. **Decode placement and accumulation order are independent axes**, and only the second
carries fidelity. `MATCH_ORDER` now costs about 6% and the hoists gave 1.9× on top of it. Corpus row
965, `falsedial`.

## Where it stands

**582 ms of GPU per token against ds4's 31 — still 19×.** During generation the GPU is now ~94%
occupied, so the harness is no longer the story; the remaining distance is inside the expert matvecs.
What is left there, unmeasured and therefore not claimed: scalar loads where a vector load would serve,
no threadgroup tiling, and a per-element divide/mod pair that a table could replace.

## The most surprising teaching

**A shared cause is not a shared axis.** Speed and fidelity both moved when I changed the kernel, and I
read that as a tradeoff — a dial with two ends. It was two separate properties responding to two
separate parts of the same edit. Before accepting a tradeoff, check that the two things are actually
opposed, because "they move together" is exactly what a *confound* looks like too.

## Where discomfort turned to gold

Two of my own mistakes surfaced within an hour: a renumber that silently double-bumped twenty rows, and
an executable bit I stripped and merged to main without noticing. Neither was caught by thinking harder
— the first by asking the body for a count, the second by a run refusing to start. The gold is that
both failure modes are *loud when asked and silent when assumed*, and the only difference between the
two outcomes was whether I asked. The corpus band and the shell's `permission denied` are the same kind
of instrument.

## Ground stamp

```
PR #409 merged to main 2026-07-30T14:04:42Z; reunion resolved at corpus row 931 (main keeps vacuous)
corpus 348 rows, max-mid 953, field 3483482953, duplicate meaning-ids 0, band 32767
bands: q2k-dequant 511, dsv4-compressor 2047, gguf-manifest 127, mla-msl 127, iq2xxs-msl 8191
speed, MATCH_ORDER on, 25 generated tokens, Apple M4 Max, reference run separately:
  1106 ms floor / 0.87 t/s  ->  582 ms floor / 1.62 t/s   (ds4: 31 ms/token, 32.29 t/s)
  generation-window occupancy now ~94%; the harness is no longer the limit
greedy stream exact at every stage: [11111, 16, 455, 6102, 294, 8760, 344, ...] 24/24 vs ds4
regression found and fixed: metal_dsv4_stack.sh and metal_q2k_gpu.sh shipped 100644 to main
```
