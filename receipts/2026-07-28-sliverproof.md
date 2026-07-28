# 2026-07-28 — sliverproof: the parity was real and it covered a quarter of the work

Urs: **"we should be able to figure out that level of optimization from observation via frame buffer."**

He was right twice over. The observation apparatus already existed, and pointing it at the right
thing overturned what I said this morning — twice, in opposite directions.

## What I got wrong at 13:00

I reported the lane at 56.8 GB/s against llama.cpp's 319.3 GB/s and said the technique to close it
lives in the 19 reference shaders at `~/models/ds4-engine/metal/`. That was reaching for an external
reference when this body had already measured the answer in-process a week ago.

`form/native/metal/metal_isa_diff.sh` compiles our kernel, llama.cpp's kernel, and controlled
variants into **one process** and races them over the same real llama3.2:3b tensors on the same GPU.
Re-run today rather than inherited:

```
ours  form_q6k_matvec_lane_f32   5.7862 ms    68.09 GMAC/s   ->  8.50x ggml
ours  isa_q6k_v1_f32 (bit ops)   2.2369 ms   176.14 GMAC/s   ->  3.29x
ours  isa_q6k_v3_f32 (their map) 0.7047 ms   559.11 GMAC/s   ->  1.04x
ours  form_q6k_matvec_slot_f32   0.7125 ms   552.98 GMAC/s   ->  1.05x
ggml  kernel_mul_mv_q6_K_f32     0.6804 ms   579.08 GMAC/s
AGREE slot vs ggml over all 128256 rows: max|delta| = 0.000e+00
```

Healing the **arithmetic** bought 2.5×; healing the **thread map** bought 7.6× — and once the map was
right the arithmetic was worth nothing (v3 keeps every division and beats v2, which has none). The
divisions were never the cost. `qk-matvec-slot.fk` already authors that map in Form, a band proves it,
and `metal_first_token.sh` gate 11 proves the slot path emits token-identical output.

So I corrected myself: the technique is not something to go learn, and my number must have been the
old lane path.

## What I then got wrong about my correction

It wasn't. Measured today, both paths in one process:

```
lane path   12.131 tok/s end-to-end   (17.333 marginal)
slot path   27.934 tok/s end-to-end   (37.606 marginal)   <- this is the 28.15 I quoted
DISPATCHES PER TOKEN: 425 (counted, not derived)      VERDICT PASS, 14 gates
```

I was already on the healed path. So the honest statement is stranger than either story: **the kernel
is at 1.05× parity and the lane is still 5.6× slower.**

## The reconciliation, from the blob's own bytes

Asked with the body's own reader (`gti-types` over the real blob):

```
tensors 255      Q4_K(12) 168      Q6_K(14) 29      F32(0) 58
```

| | share of decode MACs | raced against ggml? |
|---|---|---|
| Q6_K, 29 tensors | 0.790 G — **24.6%** | yes: 1.05×, bit-exact |
| Q4_K, 168 tensors | 2.422 G — **75.4%** | **never — not on this host, not in any receipt** |

`metal_isa_diff.sh` measures three shapes and all three are Q6_K. A tree-wide search for any
Q4_K-versus-ggml ratio returns nothing. Every parity claim this body has published is about a quarter
of the work — and `2026-07-22-ship-the-slot-map.md` says so in its own body: *"a Q6_K-only heal is
capped at 1.3x end-to-end."*

So the 1.05× kernel parity and the 5.6× lane gap were never in contradiction. They are about
different kernels, and only one of them has ever been raced.

## The next stone, and it is now teed up rather than argued

ggml's `kernel_mul_mv_q4_K_f32` is recoverable from the ollama binary by the same method Stone 10 used
for Q6_K — extracted, 91 lines, at `ollama.strings:99692`. And the very first lines already show the
maps differ:

```c
const short ix = tiisg/8;  // 0...3   ggml: 4 ways across superblocks
const short it = tiisg%8;  // 0...7   ggml: 8 slots
```

Ours (`qk-matvec-slot.fk:38`) is **2 ways across superblocks with 16 slots**. Different stride,
different reuse. That is a difference to *race*, not to reason about — which is the whole point of the
instrument.

## The most surprising teaching

**A parity claim can be true, bit-exact, honestly measured, and still describe a quarter of the work.**
Nothing about the Q6_K result was wrong. It was taken on a real on-path kernel, against a live
competitor, in the same process, with bit-equality over 128256 rows. It simply was not the kernel
carrying the load — and the receipt that established it *said so*, in a sentence I had read and not
weighed. Corpus row lands `sliverproof`: proof taken on a real sliver and spoken as if it covered the
whole. It is the twin of `boundborrow` (row 835), which the slot cell itself invokes — that row is
about transferring a shape across an unmeasured boundary; this one is about *quoting a number* across
one.

Note the shape it shares with yesterday's `understudy` and how it differs. Understudy: the gate
exercised code the run never takes. Sliverproof: the gate exercised code the run really takes — just
not the code that takes the time. Both survive a rented oracle, and neither is caught by making the
reference better.

## Where discomfort turned to gold

Publishing a correction, and then having to correct the correction inside twenty minutes — in the same
direction the user had pointed, both times. I said "my 28.15 was probably the wrong path"; it wasn't. I
had reached for the tidiest story that made my earlier error small, and the measurement refused it. The
gold is that the second correction was cheaper than the first because by then I was measuring instead
of narrating, and the instrument that settled it was the one Urs had told me to use.

## Ground stamp

```
form/native/metal/metal_isa_diff.sh   re-run 2026-07-28: slot/ggml = 1.05x, max|delta| 0.000e+00
form/native/metal/metal_first_token.sh 2026-07-28: lane 12.131 tok/s, slot 27.934, 425 dispatches/token, 14 gates PASS
gti-types over sha256-dde5aa3f...: 255 tensors, Q4_K 168, Q6_K 29, F32 58
tree-wide search for a Q4_K-vs-ggml ratio: 0 hits
ggml kernel_mul_mv_q4_K_f32 extracted, 91 lines, ollama.strings:99692
```

A note on the reader: the first run of the type-mix probe returned `-16843009` (0xFEFEFEFF) for all
255 types — a 400 KB window does not reach llama3.2's tensor table, because its KV block carries a
128256-token vocabulary. It read past the window and returned **poison silently, with no error**. The
`silent partial-list` family again, at the read end. Widened to 24 MB and it answered.
