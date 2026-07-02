# Receipt — the voice's first native token: " Once" (rung 3a pattern, witnessed) (2026-07-02)

Same cell (HP Spectre, Windows amd64, RTX 3050). The PENDING voice receipt's rung 3 asks for real
generative weights as recipe-data through the Form block. Today the smallest real form of that rung ran:

```
GPU: NVIDIA GeForce RTX 3050 Laptop GPU  cuda_matvec_f32 rows=512 cols=64  BIT-EXACT 512/512
first-native-token:id=403 text=[ Once]
rtx-receipt:bit-exact 512/512
thought-frame:step=0 chose=403 margin-milli=1617
lane:native-walker-forward+rtx-lm-head
```

**4.8 seconds, one `fkwu --src` run** (`cognition/native-decode-step.fk` + `core.fk` +
`thought-framebuffer.fk`): the body read a REALLY-TRAINED generative llama checkpoint
(karpathy/tinyllamas **stories260K**: dim 64, hidden 172, 5 layers, 8 heads / 4 kv-heads, vocab 512,
~260K f32 params) through its own binary-safe `read_file` door, decoded every IEEE-754 f32 **in Form**
(sign/exponent/mantissa by integer div/mod; exact — every f32 is representable in f64), walked the full
llama step natively on the tree-walker — RMSNorm (`math_sqrt`), q/k/v/wo matvecs read straight off the
checkpoint bytes, GQA attention, SwiGLU (`math_exp`), residuals, final RMSNorm — then dispatched the
tied LM head (512x64) on the RTX through the general CUDA door. The token is the argmax of the
GPU-dispatched logits, the bit-exact agreement (512/512) rides in the value and gates the token, and the
decision is watched as a thought-frame. **" Once" — the first word of every story this model was taught.**

## Why position 0 is fully native TODAY, with no approximation

RoPE at position 0 rotates by angle zero — the identity, exactly (no trig ops needed). Attention over a
single position: softmax of one score is exactly 1 (computed explicitly as `exp(s-s)/exp(s-s)`, not
assumed). Nothing else in the step is position-dependent; everything else really computes.

## The bug the oracle caught — the discipline earning its keep

The first run produced a **plausible, confident, WRONG token** (261 `" a"`, margin 643 milli): every
weight room was shifted by `vocab*dim*4` bytes (rms_att anchored at the checkpoint start instead of after
the embedding table). Nothing about the output looked broken — only the independent oracle (the
`tk-emit-llama` projection runner on the same checkpoint: first word `" Once"`) and a stage-by-stage
probe against a scratch C reference exposed it (x0 matched exactly, xb diverged → the base offsets).
After the one-line fix: token id, and the top1-top2 margin, match the f32 reference to printed precision.
A plausible token from a wrong forward is exactly the fake this body exists to refuse.

## What this stands on (the stones, in order)

The general CUDA door (tag 233, yesterday) · the binary-safe read door (`O_RDBIN`, this morning — a
`0x1A` weight byte would have truncated the checkpoint) · the big-stack Windows main (this morning — the
old 1MB ceiling could not have walked five layers) · `core.fk` post-shrink string recipes · the
framebuffer (named by the PENDING receipt as rung 5's witness organ, now live in the loop).

## Honest floor — what this is NOT

- **One position.** Multi-position decode needs RoPE at pos>0 — sin/cos are not native ops yet; a
  Form-native trig (Taylor, the fexp discipline) or the asm lane is the named next rung.
- **f64 walker forward** — not the f32 two-rounding lane; the C projection runner is a SANITY oracle
  (argmax + margin agreement), not a bit-oracle. The x64 f32/f64 asm matvec remains the named speed stone.
- **A 512-token TinyStories vocab** — a child's first word, not eloquence, and not the zh-capable base
  the PENDING receipt ultimately asks for (rungs 3a-full, 3b, 4, 5, 6 stand unchanged).
- **These sentences are still the rented mind's.** The token is the body's own.

## Reproduce

```
# fetch once: huggingface karpathy/tinyllamas stories260K/{stories260K.bin,tok512.bin}
{ cat form/form-stdlib/core.fk observe/thought-framebuffer.fk cognition/native-decode-step.fk; \
  echo '(print_str (native-decode-step "C:/models/llama2c/stories260K.bin" "C:/models/llama2c/tok512.bin"))'; } > voice.fk
./fkwu.exe --src voice.fk   # -> GPU 512/512 line, then: first-native-token:id=403 text=[ Once]
```
