# STONE 22 — an MLA attention recipe for this body

2026-07-22, ~11:30 WITA. Worktree `jovial-aryabhata-3751d7`.

`form/form-stdlib/mla-attn.fk` · `form/form-stdlib/tests/mla-attn-band.fk` (verdict **63**, radius declared).

This body had GQA, RoPE, RMSNorm, SwiGLU, a KV cache and a full llama3.2:3b token at 13.24 tok/s, and
**no MLA**. It has one now, on the CPU, at rank-generic dims, with its evidence class named.

---

## 1. What the recipe computes

One MLA attention block for one token at one position, against a cache of latent rows. In order —
each line carries where it was read, not remembered:

| step | what | read at |
|---|---|---|
| 1 | `norm = RMSNorm_w(x, attn_norm)` over `n_embd` | ds4.c:9981 `layer_attn_norm_one` |
| 2 | `qr = W_q_a · norm` — **down** to the q rank | ds4.c:10002 |
| 3 | `qrn = RMSNorm_w(qr, q_a_norm)` — a norm **in rank space** | ds4.c:10012 |
| 4 | `q = W_q_b · qrn` — **up** to `n_head · head_dim` | ds4.c:10013 |
| 5 | per-head **unweighted** RMSNorm on q | ds4.c:10014 `head_rms_norm_inplace` |
| 6 | RoPE the **tail** `n_rot` dims of each q head at `pos` | ds4.c:10116 |
| 7 | `raw = W_kv · norm` — **down** to the latent; ONE row, `n_head_kv = 1` | ds4.c:10041 |
| 8 | `kv = RMSNorm_w(raw, kv_a_norm)` | ds4.c:10047 |
| 9 | RoPE the **tail** `n_rot` dims of that single row — one rotation for every head | ds4.c:13825 |
| 10 | attend: per head, `score = (q_h · row)/√head_dim`, **K and V are the same row**, plus a per-head learned **sink** logit in the denominator only | ds4.c:10305 |
| 11 | **inverse** RoPE the attention output, per head, at `pos` | ds4.c:13829 (`inverse = true`) |
| 12 | grouped low-rank out: 8 groups → rank 1024 each → concat 8192 → `n_embd` | ds4.c:10356 |

Entry points: `mla-cache-row` (steps 1,7,8,9 — the whole of what the cache stores for a token) and
`mla-block-one` (the rest). `mla-c-heads` carries the **classic** factored form (per-head `k_b`/`v_b`
up-projection plus a shared rope'd key tail, ds4.c:14043's shape) — the general MLA that V4-Flash is
the identity case of.

**Provenance.** ds4-engine is MIT, at `/Users/ursmuff/models/ds4-engine`. Nothing is copied. The
arithmetic is re-derived on this body's own proven primitives (`tb-matvec`, `tb-dot`, `ln-rmsnorm`,
`rope-rot-pair`'s rotation, `tn-softmax`'s max-shift). `boundborrow`: ds4 targets a GB10, so no layout
and no quantisation crossed over — only the shape of the computation.

## 2. The RoPE split, and how it was established

Not guessed, and not derived from one reading. Three independent statements, all agreeing:

1. **ds4.c:10116** `const uint32_t n_nope = head_dim - n_rot;` and **:10125**
   `float *tail = x + h*head_dim + n_nope;` — the rotation starts at `n_nope` and runs to the end.
   The front is bit-untouched.
2. **ds4.c:14140-14142**, a different function, assembles a key row as
   `[k_nope (head_dim − n_rot)] ++ [k_rot (n_rot)]` by two `memcpy`s. Nope first, rope last —
   confirmed by construction, not by reading the same line twice.
3. **The GGUF**, via Stone 21's manifest: `rope.dimension_count = 64` against
   `attention.key_length = 512`. 64 rotated, 448 not.

Reading 3 fixes the **widths**; Stone 21 marks the **byte position** (leading 64 or trailing 64) as
derived-but-unsettled from the file alone. Readings 1 and 2 settle it: **trailing**. For V4-Flash,
**seven eighths of every head is not rotated**.

Within the tail, pairs are **adjacent** `(i, i+1)` (ds4.c:10141), and the angle for pair *k* is
`pos · base^(−2k/n_rot)` — ds4 carries it as `theta_scale = base^(−2/n_rot)` multiplied in per pair.
That is exactly this body's `rope-freq-theta` at `HD = n_rot`, `hd = 2k`: the same convention
`rope.fk` already proves four-way. No new rotation kernel was needed.

**Not implemented:** YaRN. The recipe is ds4's `rope_tail_ext_inplace` with `ext_factor = 0` and
`freq_scale = 1`. V4-Flash declares `rope.scaling.type = yarn` (factor 16, beta 32/1) and a *second*
base of 160 000 on the compressor path. Both are named gaps, in the recipe header and in the band's
radius.

## 3. The worked example, and its numbers

E=4, R=3, `nh`=2, `hd`=4, `nrot`=2, `ng`=2, three positions, invented weights. Position 0's cache row
and position 2's block output, this body against the independent transcription:

```
r0  form   0.12177083434262709   0.91328125756970335   1.5221354292828386   -0.91328125756970313
r0  oracle 0.12177083434262706   0.9132812575697031    1.5221354292828386   -0.9132812575697034
o2  form  -0.10171868446165808   0.05634827138518593   0.47156254217054527   0.19090032546723301
o2  oracle-0.10171868446008428   0.05634827138540402   0.47156254217532556   0.19090032547105806
```

Worst disagreement across all six vectors: **1.07e-11**, at `r1[2]`. It is this body's `fsin`/`fcos`
against libm, not the decomposition — the un-rotated components agree to 3e-17, and only components
that passed through a rotation disagree at all.

Shapes came out right first run: cache rows `hd` wide, outputs `n_embd` wide, no garbage. That is what
"build first, prove after" bought.

## 4. The evidence class, named honestly

There is **no independent MLA implementation in this repo** to be bit-exact against. So, in the
descending order the brief set:

- **Claim 1 — agreement at 1e-9 with an independent fp64 transcription** written from ds4.c's C
  control flow, not from the Form recipe: `mla_oracle.py` in this session's scratchpad
  (`.../6f04f4fa-.../scratchpad/mla_oracle.py`), transcribed function-by-function from
  `rms_norm_weight` / `matvec` / `rope_tail_ext_inplace` / `layer_attention_rows_one` /
  `layer_grouped_out_one`. Observed 1.07e-11.
- **Claims 2, 4, 16, 32 — structural properties that must hold, chosen as falsifiers** for readings
  that would otherwise sit beautifully (`snugcause`): that RoPE covers the whole head (2), that
  `pos = 0` is the identity and the inverse rotation is an inverse (4), that attention weights sum to
  1 (16 — they do **not**, the sink sees to that), that the output un-rotation is decoration (32).
- **Claim 8 — the latent round-trips**: the classic up-projection path with identity up-projections
  reproduces the absorbed path's heads at 1e-9. The up-projection is exercised, and V4-Flash is
  placed as the identity case of general MLA.

**"Structurally correct at rank 4 with a worked example, agreeing with an independent transcription at
1.07e-11" is the verdict. "Bit-exact" is not claimed and would be false.**

The band was **mutation-tested**, not just run: rewriting `mla-rope-tail` to rotate the whole head
instead of the tail drops the verdict 63 → 60 (claims 1 and 2 fall). A band that cannot fail is not
evidence.

The declared radius (`aporon`) is at the head of the band file: not bit-exact, not the real dims, not
the quantisation, not YaRN, not the layer, not the GPU.

## 5. What the KV cache must now hold

**One row of `head_dim` floats per token per layer — the rope'd latent. No per-head K. No V at all.**

- `n_head_kv = 1`: the single latent row is shared by all 64 query heads.
- **K and V are the same stored row.** ds4.c:10305 dots against `kv` and accumulates `kv`. There is no
  second tensor to store.
- At V4-Flash's shape: **512 floats/token/layer**, against GQA-llama's `2 · n_kv · head_dim`.
- The rotation happens **before** the row is stored, at the token's own position — so a cached row is
  already rope'd and is never re-rotated. The query's position enters only through the query, and
  through the **inverse** rotation on the way out.
- ds4 additionally stores the row as **E4M3 on the non-rotated 448** and f16 across the whole 512
  (ds4.c:3211). This body's recipe is fp64: the cache **shape** is proven, the cache **encoding** is
  not. `kv-cache.fk`'s `kv-append` is the right shape for it — a list of rows — but a latent row, not
  a k/v pair.

## 6. The next stone: GPU emission

**Not this stone, and named as the next one.** The CPU recipe and its band exist; Metal emission does
not. What it will need, in this body's own terms: `gqa-attn-emit-band.fk` is the pattern for lifting an
attention recipe to a Metal kernel, and ds4's own kernel names are already sighted
(`kernel_dsv4_attn_out_low_q8_0_f32`, `kernel_dsv4_flash_kv_stage_f16`,
`kernel_dsv4_fp8_kv_quantize_f32`). The sink logit and the inverse output rotation are the two pieces
with no existing counterpart in this body's emitted kernels.

After that, and before a token: YaRN, the E4M3 cache encoding, the real dims, the residual and the MoE
half of the layer, and the 42.6% of the file this body still cannot read (row 859 `mutewide`).

---

## Closing

**Most surprising teaching.** *The up-projection is not there.* The architecture names a KV
up-projection — latent → per-head key and value — and DeepSeek-V4-Flash does not carry one. No
`attn_kv_b` tensor in the file under any spelling (Stone 21, independently), and ds4's own reference
reads the single latent row as **both** the key it dots against and the value it accumulates. The step
has been absorbed into the weights around it. A recipe written faithfully from the *architecture*
would have spent this run hunting a tensor that was never meant to be there. Second, and nearly as
sharp: the **inverse RoPE on the attention output**. Nothing in this body's GQA experience predicts
it, and the reason is clean — the output is a mixture of rows rotated at *their* positions, consumed
by a position-free matrix.

**Where discomfort turned to gold.** I wanted to write this recipe from what I already knew about
DeepSeek MLA. `ds4.c` is 64 525 lines; opening it felt like the expensive option and reciting the
paper's `kv_a → kv_b` shape felt like the cheap one. What I would have written is a recipe with a
`kv_b` this model does not have, at a rank that does not exist, and it would have looked entirely
right until it met a weight file. Not looking away cost forty minutes of grep and bought
`layer_forward_self_one` — 40 lines that name the whole block in order. The smaller one: the band came
back **59**, not 63, and the reflex was to loosen every tolerance and move on. Splitting the failing
claim into its two halves instead found that `fcos 0.0` returns an exact `1` here (so `pos = 0` really
is the identity, bit-for-bit) and that the round-trip residual is **5.1e-11** — the body's trig, at 3
radians. The tolerance moved because of a measurement, not because of a deadline.

**Frontier question, landed.** Row **854 `ghostrank`** (0-hit checked; instrument validated on
`aporon` 74 and `boundborrow` 32, controls that *should* hit): *what one word names a rank the
architecture names and the model does not carry, so absorbed and lost look the same?* From the file
alone there is no signature that distinguishes a step folded into its neighbours from a step that went
missing. Here two independent readings agreed — the header's tensor list and the engine's reference
path — which is how it was settled, and one reading could not have settled it (`unispan`).

**Gates.** Corpus band **8191** · `metal_first_token.sh` **VERDICT PASS, 14 gates**, ids
`[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` · `mla-attn-band.fk` **63** with
its radius declared. Cites: row 858 `apposition`, row 859 `mutewide`, row 860 `ghostrank`;
`receipts/2026-07-22-deepseek-manifest.md` (Stone 21).
