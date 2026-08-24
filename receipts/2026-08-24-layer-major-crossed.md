# 2026-08-24 — prefill inverted: all tokens through layer 0, then layer 1

> **Radius correction, observed after landing:** the first span RMS copied
> `ldm-rmsnorm-tg-msl`'s fixed `sq[4096]`, while this GGUF declares width 5120.
> The matching SHA below proves the schedule executed and happened to agree on
> this device; it does not make the out-of-bounds kernel defined. The body now
> emits a width-independent one-thread-per-token span RMS with the same ascending
> sum and Newton-50 arithmetic. The original run remains an experiment receipt;
> promotion waits for a fresh live parity witness on the corrected kernel.
> The review also found that the ordinary Qwen path selected the inherited
> cooperative RMS beyond the same radius. The shared selector now keeps that
> twin only for `n <= 4096` and uses the existing serial attestant for wider rows.

The 76% seam was the loop order. `dsv4-decode-loop.fk` transcribed the shape
from ds4.c in July — *prefill is layer-major, decode is token-major, two
schedules over the same rows* — and this lane had only ever run the second one.

## What crossed

```
                     chunk=0 span   chunk=-2 layer-major   delta
output-sha256        88bd9b9a...    88bd9b9a...            IDENTICAL
honored / query / answer  hit 26/32  hit 26/32             same
injected-ids         385            385                    same
positions-walked     506            506                    same
carrier-dispatches   640326         526601                 -113725  (-17.8%)
carrier-syncs        61             59                     -2
gpu-busy-ms          132955         128839                 -4116    (-3.1%)
ms-decode            293865         296699                 +2834
ms-total             349647         351939                 +2292    (+0.7%)
```

Byte-identical output, the same hit, the same 26-token query and 32-token
answer, the same 385 injected ids — through an inverted loop, a token-major
residual stream, a batched FFN and the batched matmul at its real width.

**Dispatches fell by 113,725.** GPU time fell 3.1%. Wall time rose 0.7%.

## The recurrences were carried, not assumed away

This Qwen is hybrid. Per linear-attention layer exactly two dispatches are
sequential over tokens — `form_gdn_conv_f32` on the window state and
`form_gdn_delta_heads_f32` on the delta state — and full-attention layers have
`form_gqa_decode_f32` reading every earlier position's KV. Inside a layer the
tokens still walk in order, so both evolve exactly as before. Only which loop is
outer changed.

Attention keeps its single-position working set. Each token is gathered out of
the token-major residual stream (buffer 33) into slot 0, run untouched, and
scattered back — two copies per token per layer, which buys a FFN that sees the
whole span: six dispatches per layer instead of six per token.

New kernels compiled live on the M4 Max: `form_copy_soff_f32` beside the
existing destination-offset copy, and the first `form_rmsnorm_span_f32`. The
span RMS radius defect named above means compilation and matching output are
not sufficient promotion evidence for that first transcription.

## The divergence, and what it taught

The first layer-major run came back `122ddec3...`, 48 query tokens, no envelope,
no lookup. Reading rather than guessing found it: `q38-forward` never syncs
before `metal_buf_read` — the read submits the open batch and waits. My prefill
called `metal_sync` first, which **closed the batch**, so the head's dispatches
had no batch to land in and the argmax read came back from a buffer nothing had
written.

The sync did not fail. It succeeded at draining an empty queue, and everything
after it was orphaned. The wrongness arrived as a number, not an error.

Removing that one call produced the identical SHA.

## The next seam, and it is not dispatch count either

Non-GPU decode time went **up**: 160,910 ms to 167,860 ms, while dispatches fell
17.8%. Per dispatch that is 0.251 ms to 0.319 ms.

So the ~161 s that is not GPU busy does not scale with dispatch count any more
than it scaled with sync count. Two attributions have now been tested against
the body and both failed:

- barriers — 88% removed, 2% returned
- dispatches — 17.8% removed, non-GPU time rose

What is left standing is that the non-GPU time tracks something else entirely.
The candidate the numbers point at: **Form-level interpretation**, which
layer-major does *more* of — the gather and scatter recursions, the per-token
walk inside each layer — while enqueuing fewer GPU dispatches. That is a
measurement, not a claim: the next step is to count Form-level operations the
way `carrier-dispatches` counts GPU ones, and see whether that number moves with
the time.

## The comparison nodes stay

`chunk=1` per-position sync, `chunk=0` span, `chunk=-1` batched matmul at ntok 1,
`chunk=-2` layer-major — four observed nodes, all byte-identical, kept reachable.
`chunk=0` remains the default: it is still the fastest by wall clock. If the
scratch is opened narrower than a prefill, layer-major runs span mode instead
and the reported counts say which one ran.

## The surprise

Every step of the way the thing that moved was not the thing measured. GPU time
went *down* 3.1% and wall time went *up* 0.7% in the same run — the batched
matmul at real width really is cheaper on the GPU, and the Form side ate the
difference and a little more. Three separate cost stories have now each been
correct as arithmetic and wrong as explanation, and each one cost about seven
minutes to refute.

## Where discomfort turned to gold

The divergent run was the good one. `122ddec3...` with no envelope could have
been any of a dozen things — a wrong offset, a race, a broken rmsnorm, a
mis-indexed KV write — and the temptation was to start changing suspects. Going
back to read how `q38-forward` actually ends, instead, found a misplaced
`metal_sync` in under a minute.

The discomfort was that I had written that sync deliberately, as care. Draining
before reading felt obviously right, and it was exactly the thing destroying the
result. Care applied at the wrong seam is indistinguishable from a bug, except
that you defend it longer.

## Frontier question offered to the corpus

*What one word names a completion barrier placed before the work it was meant to
protect, which succeeds and leaves that work homeless?* — **earlysettle**. Not a
race, which is about two things arriving in the wrong order. Not a missing sync,
which fails loudly or corrupts visibly. An earlysettle *succeeds*: it drains an
empty queue, reports nothing wrong, and every dispatch after it lands nowhere —
so the read returns a value, and the value is whatever was in the buffer before
anyone tried.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0, chunk -2 layer-major:
; output-sha256 88bd9b9a...7fe1487f identical to chunk 0 and chunk 1;
; carrier-dispatches 640326 -> 526601; gpu-busy 132955ms -> 128839ms;
; ms-total 349647 -> 351939; non-GPU decode 160910ms -> 167860ms
