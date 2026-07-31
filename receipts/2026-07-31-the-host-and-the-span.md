# 2026-07-31 — the host stopped taking turns, and a simdgroup learned to reach

Goal: form-native DeepSeek-V4-Flash decode under 33 ms/token on this M4 Max, stream never broken,
ds4's source as oracle. **Not reached. 43.6 ms wall / 43 ms GPU, from 60 / 48.** 1.38x, stream
bit-exact at every step, 106 gates green at every landed commit.

## Where it went

| | GPU floor | wall | t/s |
|---|---|---|---|
| start | 48 ms | 60 | 16.57 |
| buffer pool + commit-is-not-wait | 48 | 48.6 | 20.57 |
| Q8_0 pointer walk, one scale load | 47 | 47.6 | 20.95 |
| Q8_0 four blocks a chain step | 45 | 45.8 | 21.85 |
| Q8_0 threadgroup 256 -> 64 | **43** | **43.6** | **22.47** |

## The host was never busy — it was taking turns

`makeBuffer` was 5.0 of the 12 host milliseconds a token, over ~1650 allocations asking the SAME
sizes in the SAME order every pass, because a pass is the same 43 layers every time. A pass now takes
its transients from a slot list reset at the pass head. What makes it safe is said rather than
assumed: the slot counter only increases inside a pass, so no slot is live twice; across passes a
slot is reused, so the three producers that outlive a pass — the compressor's rounded row, its packed
arena, and everything born before generation — take `sentinelledKeep` or are born with the pool off.
The sentinel is untouched; every buffer is still memset all-NaN before it is handed out.

The larger half was one word. `commit()` and `waitUntilCompleted()` sat together, so the host encoded
2287 dispatches with the device idle and then the device ran 48 ms with the host idle. They are
independent engines and nothing but that word made them alternate. `submit` hands work to the queue,
`drain` is the only place the host blocks, and every reader drains through `fp()`. One queue keeps
submission order, so no arithmetic moved.

**Together: 60 -> 48.6 ms. The device is now busy 98% of the wall, from 79%.**

## The span a simdgroup reaches, which is not the loads it issues

The theory going in was load count. It was wrong, and the run said so in the same breath it was
formed: the **grouped** Q8_0 kernel reads its bytes ONE AT A TIME — 34 scalar uchar loads a block —
and reaches 383 GB/s, while `ordered8` issued six vector loads per eight weight bytes and reached
241. Neither is paying for its load count.

What differs is the span. Grouped gives a simdgroup 32 consecutive blocks of one row: 1088 contiguous
bytes a pass. `ordered8` gave it 68 bytes of each of four rows — four thin streams, no fetched line
spent where it was fetched. Writing the chain's fma out four times per iteration puts the row's eight
threads on blocks b..b+7 together, 272 contiguous bytes, which is ds4's own footprint. Four steps of
a chain is four steps of a chain whatever the loop header says.

Then the threadgroup size, which no dispatch in this file had ever measured — 256 everywhere because
the first one said 256:

```
Q8_0 ordered8:   32  43 ms   64  43   128  44   256  45   512  46   1024  51
grouped + experts: 32  44     64  44   128  44   256  43
```

**The answer is opposite for the two shapes, so it is not a rule about Apple threadgroups.** Eight
threads share a row in `ordered8`, so a 1024-wide group puts 128 rows on one core and each streams
its own bytes through the same L1. A kernel that gives a row its own simdgroup does not care. Both
numbers are knobs now, with the measurement written next to them.

**Q8_0 is now 16 ms for 5.07 GB = 317 GB/s** — above ds4's 295 GB/s token average.

## The reference's other arm lost its speed argument

`FORM_DS4_Q80_METAL_ORDER=1` runs ds4's own Metal association. It was the faster arm when it was
found: 55 ms against 59. Re-measured at this HEAD: **45 ms / 21.56 t/s against our 43 / 22.47.** The
pinned CPU arm is now the faster one.

That does not settle anything about accuracy — the Metal arm still never quantises the activation and
is still the more accurate of the reference's two arms by ~1.1e-3 on rows of magnitude 8.8e-1. It
means the open question for Urs is now purely an accuracy question, with no speed number leaning on
it. That is a cleaner question than the one it was.

## Six nulls on one kernel, and what they rule out

`FORM_DS4_SKIP` prices the routed experts at 12 ms of the 43 for 1.83 GB — 152 GB/s. Six changes,
each a real hypothesis, each measured, all landing at 43-44 ms:

| hypothesis | change | result |
|---|---|---|
| load count | scalar byte reads -> packed_uchar4/packed_float4, 37 loads per 16 weights -> 10 | 43 |
| per-weight arithmetic | sign folded into the scale (one multiply gone), sign bit by index not by walking | 43 |
| row blocking / register spill | `FORM_DS4_IQ2_ROWS4=0`, one row a thread instead of four | 43 |
| **the association's no-FMA rule** | `-ffp-contract=fast` on every metallib | **43** |
| constant-table load rate | the 8 grid bytes as two packed loads instead of eight | 43 |
| accumulator chain latency | a second accumulator, which the association forbids | 44 |

And residency, asked outside the kernel: 15 MB of pageins across the whole generation window against
227 GB of weight reads. The weights are resident.

**The fourth row is the one worth carrying.** `-ffp-contract=off` is deliberate, stated at the top of
`ds4-order-match.fk`, and the whole stream match rests on it — a fused multiply-add rounds once where
ours rounds twice. The standing worry was that it costs throughput, since ds4 fuses everywhere. It
costs nothing. The association is not what is holding these kernels back.

What is left untested is the one lever that has actually paid: **the span.** The IQ2 kernel gives a
simdgroup 264 contiguous bytes of each of four rows whose bases are 1056 bytes apart — the exact
shape `ordered8` had before this session, and the only one of its properties none of the six changes
touched. Unrolling its sub-block loop by four would give it the whole row window. That is the sized
next action.

## Where it stands

```
43 ms of GPU, 43.6 ms of wall, 22.47 t/s, 210 GB/s of a measured 471
  Q8_0 ordered8   16 ms   5.07 GB   317 GB/s   <- above ds4's token average
  routed experts  12 ms   1.83 GB   152 GB/s   <- six nulls, span untested
  grouped Q8_0     4 ms   1.53 GB   383 GB/s
  f16 matvec       4 ms   0.678 GB  170 GB/s
  hyper-connections 4 ms, attention 3 ms — mechanism, almost no weight bytes
  dispatch floor   2.65 ms (2287 asks at a re-measured 1.16 us)
host share: makeBuffer 5.0 -> 0.5 ms a token, NaN fill 3.4 -> 1.1
NOT reached: < 33 ms. Still no measured reason it is impossible.
```

## The most surprising teaching

**A kernel that reads its bytes one at a time can be twice as fast as one that reads them four at a
time, and the reason is neither of those facts.** The grouped Q8_0 kernel issues 34 scalar loads per
34-byte block and reaches 383 GB/s; `ordered8` issued six vector loads per eight bytes and reached
241. Every instinct I brought — count the loads, widen the loads, cut the arithmetic, check the FMA —
was a question about what a thread does. The number that moved was about what a *simdgroup reaches
together*: 68 bytes against 1088. A thread is not the unit that talks to memory, and I spent five
measurements learning that the hard way after the first one had already said it.

The corollary is sharper: six changes to the IQ2 kernel's inner loop, all sound, all bit-identical,
all worth nothing — because every one of them was about a thread again.

## Where discomfort turned to gold

Two places, and the second only because of the first.

Reaching for `git stash` to diff the emitted MSL, and popping a *sibling agent's* stash into my
worktree — three files in conflict, changes I had never seen staged under my name. The comfortable
move was to `git stash drop` the mess and get on with it. What was uncomfortable was that I could not
tell, for about a minute, whether I had just destroyed another agent's in-flight work. I checked
`git stash list` before touching anything, found the entry still on the stack because the pop had
conflicted, saved my one real edit to scratch, and hard-reset. Nothing of theirs was lost. The gold
is that the stash stack is repo-global and worktree isolation does not touch it — which is written in
my own memory and which I reached past anyway, because "just stash it for a second" does not feel
like an action with a blast radius.

Then the six nulls. Every one of them was a real hypothesis with a real mechanism, written carefully,
verified by a character-level diff of the emitted MSL, and worth exactly nothing. The pull, by the
fourth, was to stop measuring and start believing — to keep the packed loads because they *should* be
faster, to keep the arithmetic cut because it *obviously* removes work. I reverted all of them. What
turned it to gold was that the fourth null is the most valuable finding in this receipt: the no-FMA
rule that the entire stream match rests on, and that everyone including me assumed was expensive,
**is free**. That only became sayable because I was still willing to run the experiment that made my
own optimisation look pointless, and to keep the answer when it came back "nothing here either."

## Ground stamp
```
HEAD f924527a5 -> this receipt; branch claude/kat-coder-2-5-ingestion-ac9385
48 -> 43 ms of GPU per generated token; 16.57 -> 22.47 t/s; 60 -> 43.6 ms wall
stream [11111, 16, 455, 6102, 294, 8760, 344, ...] bit-exact at all 30 steps, every commit
VERDICT PASS 106 gates at every landed commit (FORM_DS4_KV_CAP=16 KV_STEPS=7 KV_SEQUENCE=1;
  the previous receipt's 96 is the same command WITHOUT KV_SEQUENCE=1 — the mystery is closed)
bus 471 GB/s; token floor 19.3 ms; we now reach 210 GB/s, ds4 reaches 295
ds4's own Metal arm re-measured here: 45 ms / 21.56 t/s — now the SLOWER arm
open for Urs, unchanged and now cleaner: the pinned stream is ds4's CPU arm; its Metal arm is
  more accurate, and no longer faster
next, sized: the IQ2 expert kernel's span (264 bytes a row a pass, rows 1056 apart)
```
