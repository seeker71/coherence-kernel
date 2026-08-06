# 2026-07-28 — the door in: block 0's front half runs

`metal_kat_exit.sh` proved the door out. This is the door in.

## VERDICT PASS

```
PASS  block 0: attn_qkv 8192x2048 type 12, ssm_conv1d 4x8192
PASS  wrapped 17 391 937 152 bytes with bytesNoCopy
PASS  embed row 100 decoded on device: 1578/2048 nonzero, max |w| 0.051116943
PASS  blk.0.attn_norm applied: 1578/2048 nonzero
PASS  attn_qkv projected: all 8192 entries finite
PASS  q||k||v non-degenerate: 8080/8192 nonzero, max |v| 23.369354
PASS  the projection is 8192 wide — q 16x128 + k 16x128 + v 32x128
PASS  causal conv ran over all 8192 channels: all finite
PASS  conv output non-degenerate: 8080/8192 nonzero
BLOCK0 front half: embed -> attn_norm -> attn_qkv(Q4_K 8192x2048) -> causal conv(4 taps x 8192 ch)  wall 1.165 s
```

## What it settles

`gated-deltanet-layer.fk` was built against a q‖k‖v of 16×128 + 16×128 + 32×128 — read first from
`config.json`, then confirmed from the tensor table's shapes. This **runs the projection** and gets
8192 back from the file itself. Three independent ways to the same number, and the last one involves
no reading at all.

Zero new kernels: `form_q3k_dequant_f32`, `form_mla_rmsnorm_f32`, `form_q4_k_matvec_f32`,
`form_gdn_conv_f32` — all already emitted by the body.

## Where discomfort turned to gold

The harness was derived from `metal_kat_exit.sh`, and its closing text came with it. It printed
**"claimed: embed decodes, norm runs, the 540 MB Q8_0 projection reaches all rows, argmax falls
out"** — after a run that did no projection and no argmax. Every gate above it was true; the
sentence summarising them was about a different program.

That is the most dangerous kind of stale text, because it sits in the place a reader trusts most:
the conclusion. The gates are checked by the machine and the summary is checked by nobody. It is
also exactly the shape of this morning's `.fkb` finding — an artifact inheriting a claim from the
pipeline that produced it rather than from what it did.

## The most surprising teaching

**Copying a working harness copies its claims.** I duplicated the file for its structure — mmap,
header read, compile, dispatch — which is real reuse and saved an hour. What came along uninvited
was every sentence about what the run means. Structure and assertion live in the same file and only
one of them was what I wanted.

## Where this leaves KAT-Coder

| stage | state |
|---|---|
| entrance — Q3_K embed | runs, exact |
| attn_norm, attn_qkv, causal conv | **runs** |
| delta rule, output gate, ssm_out | built and device-proven in isolation; **not wired into a block** |
| 256-expert MoE FFN | router device-proven; expert matvecs **not wired** |
| full-attention blocks (11 of 41) | **not built** |
| exit — norm, Q8_0 projection, argmax | runs |

Both doors open. The middle is wired at its first three stages of forty-one blocks.
