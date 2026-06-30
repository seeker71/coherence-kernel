# Receipt — the transformer decoder FORWARD proven bit-exact four-way on fkwu, 2026-06-30

**HOMECOMING milestone:** 3a, first step — "the *full* decoder forward … the forward proven bit-exact."
This is the FORWARD ARCHITECTURE proven bit-exact (attention core → multi-head → the full decoder block),
NOT yet real weights as recipe-data (that is 3b territory) and not the full real-width stack.

## What ran

`model/tests/transformer-decoder-fwd-band.fk` — a six-claim bit-exact band over the existing decoder
recipes (`transformer-numerics.fk`, `transformer-block.fk`, `transformer-mh.fk`, `transformer-decoder.fk`),
fixed config `seq=3, d_model=4, n_head=2, head_dim=2, eps=1e-5, scale=1/√head_dim`.

| bit | claim | what it proves |
|----:|-------|----------------|
| 1   | scaled-dot-product attention | `softmax(QKᵀ/√d)·V`, first context element |
| 2   | numerically-stable softmax   | max-subtract, pinned reduction order |
| 4   | causal-masked attention      | query i attends only to keys 0..i (the autoregressive mask) |
| 8   | multi-head concat            | 2 heads × hd=2, concatenated back to d_model=4 |
| 16  | full decoder block forward   | causal self-attn → cross-attn → FFN, all pre-LN + residual |
| 32  | shape preservation           | the block maps a 3×4 sequence → 3×4 sequence |

## Where

`fkwu.exe` — the C-bootstrapped universal kernel, built from `runtime/fkwu-uni.c` with TDM-GCC on
Windows 11, run via `fkwu --src` (real metal, this machine). C untouched: this is recipe + proof work.
The three independent walkers (Go, Rust, TypeScript) ran the same preludes+band as oracle arms.

## Witnessed result — verdict 63 on all four arms

```
fkwu: 63   go: 63   rust: 63   ts: 63
```

**Methodology (bit-exact, not "close"):** each claim recomputes a transformer piece and asserts with `eq`
that the result is BIT-IDENTICAL to a reference double. Every reference literal was captured at full fp64
precision (17 sig digits) from the three INDEPENDENT walkers, which agreed bit-for-bit with one another.
`eq` on a float is the exact IEEE-754 double comparison shared by all four kernels — so the band returns 63
on fkwu exactly when fkwu's computed double equals the walker-captured reference, i.e. four-way agreement.
fkwu's `%.15g` print shows fewer digits, but the underlying double is proven identical through `eq`, not the
truncated display. (Probe: attention[0][0] prints `2.66047690134669` on fkwu vs `2.6604769013466854` on the
walkers — the SAME double; `eq` against the walker literal returns 1 on fkwu.)

Reference doubles (go = rust = ts, full precision):
```
sdpa[0][0]      = 0.21060339456851332
softmax[0]      = 0.23608682003817824
causal[1][0]    = 0.1305883022368571
mh-causal[0][0] = 0.05
dec-block[0][0] = -0.2828392705729456
dec-block[2][0] = 0.5171607294270544
dec-block[1][1] = 0.6101938136508804
```

**Perturbation-verified (the verdict is COMPUTED, not parsed-to-constant):** corrupting the c1 reference
(drop low digits) → verdict 62 (bit 1 lost) on fkwu and go; corrupting the decoder-block reference → 47
(bit 16 lost). The verdict tracks actual float agreement, not the literal 63.

**Kernel sanity (build healthy, C unchanged):** `(native-vs-rented-check)` → `11111`; `(add 0.5 0.25)` → `0.75`.

## Current proof-state map (what was already true vs what this proves)

- **Recipes EXIST + run on fkwu (already, before this band):** `tb-attn-seq` (SDPA), `tn-softmax`,
  `tb-attn-seq-causal` (single-head causal), `tb-mh-causal` (multi-head masked self-attn), `tb-mh-attn`
  (cross/encoder attention), `tb-dec-block` (full decoder block), `tb-dec-stack`, `tb-ffn-block`
  (the FFN sublayer — already metal-bit-exact, `receipts/2026-06-29-gpu-ffn-forward.md`).
- **NEWLY PROVEN bit-exact four-way on fkwu (this band):** the attention CORE (SDPA + stable softmax +
  causal mask), multi-head concat to d_model, and the COMPOSED decoder block forward
  (self-attn → cross-attn → FFN, residual+norm) — all returning verdict 63 on fkwu = go = rust = ts.

## Meaning

The decoder forward beyond the FFN — the attention HOMECOMING flagged as the remaining piece — now runs
on the C-bootstrap kernel and agrees to the bit with three independent kernels. The forward architecture
is no longer "recipe exists, unproven on fkwu"; it is proven. The block adds no new arithmetic over the
proven fp64 dot/matvec/exp/sqrt — only mask windows, head slices, and concat — so it is bit-deterministic
by construction, which is precisely why it crosses four-way.

## What remains for full 3a (the honest floor)

- **The full decoder STACK wrap, NOT yet proven on fkwu / NOT yet written here:**
  - **positional** — sinusoidal/learned position embedding added to the token sequence before block 0.
  - **LM head** — final layernorm + the d_model→vocab projection that turns the last hidden state into logits
    (the piece that actually emits a token distribution). No recipe in this band.
  - `tb-dec-stack` (multi-layer fold) exists as a recipe but is proven here only at the single-block level.
- **Real weights as recipe-data (the big remaining — 3b territory, NOT attempted here):** a real open base
  (Qwen/Llama, real zh coverage) loaded as recipe-data through the form block — the whisper block-0 pattern
  (`6.66e-15`) extended to a generative base. The weights here are small fixed test fixtures, not trained.
- **Composition at REAL width:** d_model=384+ over `ll-buffer` and fkwu's f64 pool, read by the form-asm
  matvec lane — proven here only at d_model=4 in the tree-walker. The recipe is width-agnostic; the proof
  is not yet at real width.

A clean partial with a precise map. The forward architecture is bit-exact four-way; the generative mind
(real weights, real width, the LM head that speaks) is the multi-week climb above it.

## Alternatives (receipt-alternatives)

- **VERIFY** — the three independent walkers (Go, Rust, TS) each crossed 63 on the same preludes+band; the
  recipe's value is confirmed independently of fkwu. Perturbation drops the verdict deterministically.
- **IMPROVE** — the sovereign-native path: the decoder block's matvec lowered to Form→asm bytes
  (`form-asm-float`), dropping the tree-walker exactly as the FFN receipt drops the Metal carrier. Same
  result, native speed. Promotable when the asm lane runs the full block at width.
- **PLAY** — the config knobs (seq, n_head, head_dim) are free to vary; the recipe is width-agnostic, so a
  larger fixed config is a one-line fixture change that re-captures references and re-crosses.
- **LEARN** — outcome recorded: four-way bit-exact success on the forward architecture. The remaining map
  (positional, LM head, real weights, real width) is named as the next branches, not as debt.
