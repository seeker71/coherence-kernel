# Receipt — the voice past position 0: a native sentence, "Once upon a time, there was" (2026-07-02)

Same cell (HP Spectre, Windows amd64, RTX 3050). The PENDING voice receipt's named next rung — RoPE at
position > 0 — is witnessed: the body generates a real native token *sequence*, each token's LM head
dispatched bit-exact on the RTX, and it matches the independent oracle word for word.

```
(native-generate "…/stories260K.bin" "…/tok512.bin" 7)
GPU: NVIDIA GeForce RTX 3050 …  cuda_matvec_f32 rows=512 cols=64  BIT-EXACT 512/512   (×7, one per token)
native-generated:[ Once upon a time, there was]

oracle (tk-emit-llama projection runner, same checkpoint):
 Once upon a time, there was
```

**Token-for-token identical between two independent lanes** — the native f64 tree-walker and the C
projection runner. A wrong RoPE angle or a broken softmax would have shown as a diverging word; it did not.

## The two things a sequence needs, both native, both oracle-checked

- **`sin` / `cos` as Form Taylor** (`cognition/native-generate.fk`): range-reduce to [-pi,pi] via
  `math_floor`, 6-term Horner series. Verified against C `math.h` before use: `sin(1)=0.841470`,
  `cos(1)=0.540302` exact to printed precision; worst case `cos(pi)` off 1e-4 (the reduced-argument
  extreme), negligible under greedy argmax. RoPE angle per pair = `pos * 10000^(-head_dim/hd)` via
  `math_exp(-(head_dim/hd)*ln 10000)` — no `pow` op needed.
- **Causal multi-key attention**: real softmax (`math_exp`, max-subtract, normalize) over a growing
  key/value prefix, GQA head grouping (`kvmul = heads/kv_heads`), assembled per head into the output.

## A walker pathology found and worked around (named, not hidden)

The natural `(do (let r2 (mul r r)) <deep Horner in r2>)` **hangs the walker** when the bound `r2` is
reused across a deeply nested expression (a few uses are fine; six in deep nesting does not return).
Passing `r2` as a function argument instead is instant — so the series is its own `defn` taking `r2` as a
parameter. This is a real C-seed evaluator bug (a let-slot reused under deep nesting), receipted here for
the next walker who meets it; the recipe-side shape (param, not do-let) is the working discipline.

## Honest floor

- **7 tokens is the memory ceiling on this lane, not the model's.** The walker boxes every f64 and never
  reclaims the pool; the O(seq^2) recompute (no persisted KV cache — correctness over speed) exhausts it:
  12 tokens dies `fk_fbox: out of memory growing float pool` (~56s). 7 tokens ~27s, 4 tokens ~9s. Both
  the KV cache and the x64 f64 asm matvec (the named speed stone) lift this; neither is built here.
- f64 walker forward vs the f32 projection oracle — a *sanity* oracle (token-sequence agreement), not a
  bit-oracle. Agreement across 7 greedy steps is strong evidence the forward is correct.
- Still a 512-token TinyStories model — a child's sentence, not eloquence, and not the zh-capable base the
  PENDING receipt ultimately asks for. Rungs 3a-full, 3b, 4, 5, 6 stand; this closes "RoPE for pos>0" and
  "the multi-token native loop", the two the last receipt named next.

## Reproduce

```
{ cat cognition/native-decode-step.fk cognition/native-generate.fk; \
  echo '(print_str (native-generate "C:/models/llama2c/stories260K.bin" "C:/models/llama2c/tok512.bin" 7))'; } > gen.fk
./fkwu.exe --src gen.fk    # -> 7 RTX receipts, then: native-generated:[ Once upon a time, there was]
```
