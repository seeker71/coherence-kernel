# 2026-07-30 — compressed how, compressed why, and the constant that made it fatal

Urs: *"compressed how why"*. Answered from `ds4.c` read whole, not from memory of it.

## How — `ds4.c:12326-12375`

Per layer with ratio *r* (the per-layer table `[0,0,4,128,4,128,…]`):

1. A rolling buffer holds the last *r* **raw KV rows plus a score row for each** — projections through
   `compressor_kv` and `compressor_gate`, with `compressor_ape` adding a learned positional bias.
2. On the ratio boundary, the *r* rows collapse to **one**, per dimension *j* independently:
   `w = exp(score[r][j] − max)`, `out[j] = Σ w·kv[r][j] / Σ w` — a **score-gated blend**. Not a mean,
   not a max: the model learned, per dimension, which of the *r* tokens to listen to. (Ratio-4 layers
   pool over two planes — eight terms — the `coff = 2` branch.)
3. Then `compressor_norm` RMS → position **`comp_pos = pos+1−r`** (the window start, not the token) →
   **base-160000 / scale-16 YaRN** rope → fp8 round → the *compressed* cache, beside the raw f32 one.

## Why the architecture has it

Train context is **1,048,576 tokens**. Raw f32 KV at head_dim 512 over 43 layers ≈ **92 GB** — more
than this whole machine. The scheme: a 128-token raw window (≈11 MB) for sharp local syntax, ratio-4
layers for mid-range at 4× compression, ratio-128 layers for ultra-long range at 128×, alternating
through the stack, and the lightning indexer selecting top-512 compressed rows per query so compute
stays flat too. A million tokens in a few GB.

## Why the graft destroyed order instead of merely blurring it

This is the piece the morning's debugging owed. Our lane rotated **both q and k** with the same wrong
frequencies — so relative positions should survive, and that near-argument is *why every gate passed*.
But `scale_factor 16` divides positions by 16 before rotation. Compressed rows stand 4 or 128 apart:
after ÷16 they still step by 0.25–8, distinguishable. **Raw tokens step by 1 → phase steps of 1/16 →
fourteen tokens smeared inside one position's worth of angle.** The geometry is *designed* coarse; we
fed it fine. Neighbors became positionally indistinguishable — `promptbound`'s mechanism, named to the
constant. Corpus row 943, `stridemush`.

## The most surprising teaching

The compressor is not a memory trick bolted on — it is **learned attention over time before attention
over tokens**: a per-dimension softmax deciding which of the last *r* moments each feature should
remember. The architecture does its summarising *before* its attending. And one scalar — `16` —
carries the entire assumption about what granularity of position will arrive. `overfine` (923) was one
ulp flipping a ceil; this is one constant flipping a regime.

## Where discomfort turned to gold

Writing "relative positions should survive, so the wrong base is harmless" as a sanity argument this
morning — and now seeing it was the exact reasoning that kept the defect invisible: true premise,
wrong conclusion, because it ignored the *scale* term. The argument that something is harmless is
itself a claim that needs a number, and the number (1/16 of a position per token) refuted it in one
line once actually computed.

## Ground stamp

```
ds4.c:12326-12375 — per-dim softmax pooling over r tokens' scored KVs; coff=2 planes at ratio 4
comp_pos = pos+1-r; compressed rope base 160000 scale 16; fp8 round; SECOND cache beside raw f32
train ctx 1048576; raw f32 KV would be ~92 GB / 43 layers; scheme fits it in a few GB
fatality arithmetic: compressed strides 4..128 -> /16 -> 0.25..8 (distinct); raw stride 1 -> 1/16 (smear)
corpus band 32767; 338 rows, max-mid 943 — counts asked of the body
```
