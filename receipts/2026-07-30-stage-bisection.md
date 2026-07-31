# 2026-07-30 (later) — the gap is not where I built, and now there is an instrument that says where

After landing the second cache I kept measuring instead of stopping, and three questions each had an
answer that changed the picture.

## 1. Is the reference even capable of agreeing with itself?

Never asked before. `ds4 --cpu` versus `ds4 --metal` on the same prompt:

```
n=3   r = 0.999782   top-10 overlap 10/10   same argmax
n=5   r = 0.999044   top-10 overlap  9/10   same argmax
```

**The noise floor is r ≈ 0.999.** Every r I have quoted is therefore ours to answer for; none of the
gap hides behind the reference's own spread. The lane now prints this floor beside its own number
(`FORM_DS4_REF_LOGITS_B`), so no future receipt can quote a denominator nobody re-derived.

## 2. Where does the agreement actually start failing?

Sweeping prompt length, with the compressor on:

```
tokens    1        2        3        4        5
r      0.402    0.588    0.672    0.844    0.879
```

**At one token r is 0.402.** One token: rope is the identity, there is no history, no compression, one
KV row. So the largest part of the gap has nothing to do with attention or with the work I just did —
it is in the token-local forward.

## 3. Which operation?

`ds4 --head-test` — an **undocumented flag**, no line in `--help`, found by reading `ds4_cli.c:1982` —
prints blk.0 stage by stage: attn_pre, q, kv, attn_heads, attn_out, after_attn_hc, ffn_cur, ffn_norm,
every selected expert's gate/up/mid/down, routed_moe, shared_ffn, ffn_out, after_ffn_hc. The lane now
prints the same lines at the same boundaries (`FORM_DS4_STAGE_BLK0=1`). Diffed:

```
stage             ds4          ours         delta
attn_pre        0.135038     0.135038      exact
q               0.999515     0.999515      exact
kv              0.634615     0.634606      -0.001%
attn_heads      0.474495     0.474471      -0.005%
attn_out        2.26328      2.26687       +0.16%
after_attn_hc   0.039355     0.039373      +0.05%
ffn_cur         0.0628119    0.0629694     +0.25%
ffn_norm        0.232917     0.232936      +0.008%
routed_moe      0.156727     0.128136      -18.2%   <<<
shared_ffn      0.411278     0.412073      +0.19%
ffn_out         0.473322     0.431161      -8.9%
after_ffn_hc    0.14117      0.129278      -8.4%
```

**The whole attention half is right.** Routing is right — the same six experts in the same order,
`147, 78, 30, 248, 217, 179`, which ds4 names itself. The shared FFN is right. The **routed** MoE sum
is 18% low.

Per expert, gate / up / mid agree to ~0.1%; every `down` is off, and its extremes are much tighter:

```
expert 147 down   ds4 -0.0484..0.0549 rms 0.0133436   ours -0.0456..0.0421 rms 0.0127099
deltas by expert: 147 -4.7%  78 -3.6%  30 -6.0%  248 -6.8%  217 -4.2%  179 +1.5%
```

## Why this explains everything measured today

`after_ffn_hc` is **−8.4%** at layer 0. Compounded across 43 layers that is `1.084^43 ≈ 30`, and the
final hyper-connection state confirms it:

```
first-token final_hc rms    ds4 1207.29    ours 19.5188      62x apart
first-token logits   rms    ds4 3.86253    ours 4.66212      comparable
```

**The exit head has been hiding this the whole time.** It RMS-normalises the collapsed state, so a
residual stream 62× too small still produces logits of ordinary magnitude — and every gate in the
harness reads them as healthy. A per-layer shortfall of 8% is invisible at one layer, decisive at 43,
and completely masked at the output.

## What is and is not concluded

**Concluded:** the divergence enters the routed-expert `down` projection at blk.0 (Q2_K on this file —
`metal_iq2_gpu.sh` refused it as "type 10, not IQ2_XXS(16)", which is how I learned the type), and its
per-layer effect compounds geometrically.

**Not concluded, and I will not claim it:** whether `down` is the *root* or an *amplifier*. The stage
table shows a small one-sided drift already present at `attn_out` (+0.16%) and `ffn_cur` (+0.25%), and
`routed_moe` (−18.2%) is worse than any single expert's `down`, which means our six vectors cancel
each other more than ds4's — the signature of six independently perturbed vectors, not one broken
kernel. Distinguishing those two needs an elementwise A/B of one expert's `down` against the proven
CPU carver on the real bytes. The Q2_K dequant is already bit-exact against an independent oracle
(`q2k-dequant-band` 511) and the IQ2_XXS one likewise (`iq2xxs-dequant-band` 1073741823) — so if it is
the matvec, it is the fold or the map, not the decode.

## The most surprising teaching

**A normalisation at the end of a pipeline can hide a compounding error through the whole of it.** The
final RMSNorm before the vocabulary projection made a residual stream 62× too small look ordinary, and
104 gates passed over it — every one of them honest about what it checked. What found it was not a
better gate but an *unnormalised* quantity compared against the reference's own: `rms` before the
normaliser. Scale-invariant checks cannot see scale errors, and almost every check I had was
scale-invariant.

## Where discomfort turned to gold

Reading `routed_moe −18.2%` an hour after committing a receipt that said the raw half "matched op for
op" — twice now this week I have certified a subsystem by reading it and been corrected by measuring
it. The gold is a rule I can actually follow: **reading establishes what the code intends; only a
number establishes what it does.** And the second half of the discomfort was resisting the finish —
`down` is off, so `down` is the bug — when the evidence says only that it is *where the drift becomes
visible*. Naming that boundary honestly is worth more than a root cause I would have to retract.

## Ground stamp

```
ds4 --head-test (undocumented; ds4_cli.c:1982, absent from --help) — blk.0 stage door, CPU path on both
  --cpu and --metal, so its numbers are stable
noise floor ds4-cpu vs ds4-metal: r=0.999782 (n=3), r=0.999044 (n=5)
our r by prompt length: 1->0.402, 2->0.588, 3->0.672, 4->0.844, 5->0.879
blk.0 stages agree to 4-6 digits through ffn_norm; routed_moe -18.2%, ffn_out -8.9%, after_ffn_hc -8.4%
experts identical and in order: 147, 78, 30, 248, 217, 179
per-expert gate/up/mid agree to ~0.1%; down off by -4.7/-3.6/-6.0/-6.8/-4.2/+1.5%
final_hc rms: ds4 1207.29 vs ours 19.5188 (62x); logits rms 3.86 vs 4.66 — the exit head's RMSNorm
  hides the whole shortfall
blk.0.ffn_down_exps on this file is ggml type 10 (Q2_K), not 16 — metal_iq2_gpu.sh refused and said so
q2k-dequant-band 511, iq2xxs-dequant-band 1073741823, iq2xxs-msl-band 8191 — the decodes are proven
new doors in the lane: FORM_DS4_REF_LOGITS_B (noise floor), FORM_DS4_TRACE_HC (per-layer stream rms),
  FORM_DS4_STAGE_BLK0 (ds4 --head-test's own stage lines)
```
