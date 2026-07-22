# STONE 25 — the three DeepSeek-V4-Flash subsystems: which is core, and a recipe for the one that is

**2026-07-22, ~10:00–12:30 WITA.** Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Two cells committed
(`form/form-stdlib/dsv4-hc.fk`, `form/form-stdlib/tests/dsv4-hc-band.fk`, verdict **63**),
one oracle (`form/form-stdlib/tests/dsv4-hc-oracle.py`).

Stone 21 named three subsystems in the DeepSeek file with **no recipe in this body**:
`attn_compressor_*` (41 layers), `indexer.*` + `indexer_compressor_*` (21 layers, `top_k 512`),
and hyper-connections (`hc_*` on every layer + output, Sinkhorn ×20) — 258 tensors it called a
cliff with "no reference anywhere." That was false: ds4-engine (MIT,
`/Users/ursmuff/models/ds4-engine`) implements all three in readable `ds4.c`. This stone read them,
established which is in the core forward path, mapped all three, and **built the one that is core**.

---

## 0. Radius (`aporon`), before anything is believed

- Everything about ds4 below is read from `ds4.c` (64 525 lines) and its `.metal` kernels, MIT source
  on this disk. Nothing was executed — ds4 targets a GB10 and does not run here. `boundborrow`: no
  layout, no f16, no fp8 crossed over; only the **shape of the computation**.
- The tensor names and dims are Stone 21's manifest (`receipts/2026-07-22-deepseek-manifest.md`),
  re-cited, not re-measured. The **essentiality** finding is read from ds4's own control flow
  (`forward_first_token_cpu`), quoted by line.
- The HC recipe's evidence class is named in §4 and in the band's own radius header. It is
  **not** bit-exact. Claim 1 is agreement with an **independent** fp64 transcription (`dsv4-hc-oracle.py`,
  written from ds4.c's C, not from the recipe), observed to ~1e-15 on this fixture.

---

## 1. Essentiality — grounded from the source, not guessed (`edgedrop`)

The decisive reading is ds4's own **first-token** path, `forward_first_token_cpu` (ds4.c:13848):

```
embed_token_f16(...)                      # the plain embedding
hc_from_plain_embedding(cur, plain, ...)  # broadcast to n_hc streams
for il in 0..n_layer:
    layer_forward_self_one(next, ..., cur, il, 0, token)   # hc_pre → attn → hc_post → ffn(hc-wrapped)
memcpy(out_hc, cur, ...)
# then output_hc_head_one → rms → vocab projection
```

And `layer_forward_self_one` (ds4.c:13793) in order: `hc_pre_from_state_one` → attn norm → q/kv
projections → rope → `layer_attention_one` (dense, over the single kv row) → `hc_post_one` →
`layer_ffn_one`. **No compressor. No indexer.** Neither `compressor_decode_one` nor the indexer
selection appears anywhere on the first-token path.

| subsystem | verdict | why, from the source |
|---|---|---|
| **hyper-connections** | **CORE** | The entire forward pass — first token and decode alike — runs in `n_hc` parallel streams. The residual stream *is* the HC state; every sublayer is wrapped by `hc_pre`/`hc_post`; the head collapses the streams. There is **no residual connection in this model that is not a hyper-connection**. A correct token cannot be produced without it. |
| **compressor** | **optional accelerator** (dormant at the first token) | `compressor_decode_one` is called only from `layer_forward_raw_swa_one` (ds4.c:12981), the sliding-window **decode/prefill** path, gated by a per-layer `compress_ratio` from `attention.compress_ratios`. It compresses KV **history**; it emits a row only on a `compress_ratio` boundary (`should_compress = ((pos+1) % ratio)==0`). At the first token there is no history and no boundary — it runs zero times. |
| **indexer** | **optional accelerator** (identity at the first token) | Sparse top-k selection over compressed rows, `top_k = 512`. `indexer_allowed_decode_one` (ds4.c:12810): when `top_k >= n_comp` **every row is allowed** — a strict identity (ds4.c:12822-12826). At the first token `n_comp = 0`: it returns immediately. It only *does* anything once the compressed history exceeds 512 rows. |

The honest nuance (`snugcause`): compressor and indexer are *trained-in* — at long context the model's
faithful output includes their sparsification, so "optional" means **optional for a correct first
token**, not "dispensable at every length." Below their activation threshold they are provably identity;
that is why the first-token path can omit them and still be exact. This is the stone's teaching (§7).

**Order decided:** HC is the only core subsystem, and it is tractable (small dims: `n_hc = 4`, a 4×4
Sinkhorn). It is the one to build. Compressor and indexer are mapped (§2), not built.

---

## 2. The map of all three (tensors → dims → arithmetic)

`n_embd = 4096`, `n_hc = 4`, `n_head = 64`, `head_dim = 512`, `q_lora = 1024`, indexer heads = 64,
indexer head_dim = 128, indexer `top_k = 512`, Sinkhorn iterations = 20, all from the file's KVs.

### 2a. Hyper-connections — CORE, built (§3)

| tensor | dims | role |
|---|---|---|
| `hc_{attn,ffn}_fn` | `[16384, 24]` = `[n_hc·n_embd, 2·n_hc + n_hc²]` | projects the flat, RMS-normed HC state to the split control vector |
| `hc_{attn,ffn}_base` | `[24]` | one bias per split output |
| `hc_{attn,ffn}_scale` | `[3]` | `[pre_scale, post_scale, comb_scale]` |
| `output_hc_{fn,base,scale}` | `[16384,4] / [4] / [1]` | the head: emits only `n_hc` pre-weights |

`24 = 2·4 + 4·4` and `16384 = 4·4096` are **predicted** by the arithmetic — a reading with any other
fn output width leaves the `[24]` and `[3]` tensors without a consumer (falsifier holds). Two HC pairs
per layer (attn + ffn) × 43 layers, plus the output head. Arithmetic in §3.

### 2b. Compressor — optional, mapped (ds4.c:12281–12530)

Per ratio-`r` layer, a streaming strided KV compressor. `coff = 2` if `r==4` else `1`;
`width = coff·head_dim`.

| tensor | dims | role |
|---|---|---|
| `attn_compressor_kv` | `[4096,1024]` (r4) or `[4096,512]` | projects `attn_norm` → a width-wide KV row |
| `attn_compressor_gate` | `[4096,1024]` or `[4096,512]` | projects `attn_norm` → a width-wide **score** row |
| `attn_compressor_ape` | `[1024,4]` or `[512,128]` | an absolute-position-in-window bias added to the score, indexed `[j, pos mod r]` |
| `attn_compressor_norm` | `[512]` | RMSNorm weight on the pooled compressed row |

Arithmetic: buffer `r` (or `2r`) rolling rows of (kv, score). On each boundary
(`(pos+1) mod r == 0`), **pool** the window per dimension with a softmax over the scores
(`out[j] = Σ_row softmax(score[·,j])·kv[row,j]`, ds4.c:12326), RMSNorm with `compressor_norm`, rope the
pooled row at `comp_pos = pos+1−r`, then fp8/f16-quantize. One compressed KV row per `r` tokens. The
compressor path uses a **second rope base** (`compress_rope_freq_base = 160000`), distinct from the main
10000 — a recipe applying one base everywhere is wrong on half the file.

### 2c. Indexer — optional, mapped (ds4.c:12808–12907)

Only on ratio-4 layers. It selects which compressed rows attention may see. Has its **own** compressor
(same shape as 2b at `head_dim = 128`: `indexer_compressor_{kv,gate}` `[4096,256]`, `_ape` `[256,4]`,
`_norm` `[128]`), producing `index_comp` rows.

| tensor | dims | role |
|---|---|---|
| `indexer.attn_q_b` | `[1024,8192]` | `qr_norm` (q-rank, 1024) → indexer query, `64 heads × 128` |
| `indexer.proj` | `[4096,64]` | `cur` (n_embd) → a per-head weight `[64]` |

Arithmetic: `q = indexer_attn_q_b · qr_norm`, rope + a 128-wide quantization-aware rotation
(`dsv4_indexer_qat`). Per compressed row `c`: `score[c] = Σ_h relu(dot(index_comp[c], q_h)) · weight[h]`,
`weight = (indexer_proj · cur) / √(head_dim·n_head)`. Keep the **top 512** by score → an `allowed`
mask; attention ignores the rest. `top_k ≥ n_comp` ⇒ all allowed (identity).

---

## 3. The HC recipe built (`form/form-stdlib/dsv4-hc.fk`)

Re-derived on this body's own proven primitives (`ln-rmsnorm`, `tb-matvec`, `ln-sigmoid`,
`tn-exp-map`/`tn-max`/`tn-sum`, `tb-weighted-acc`). The pieces, each with its ds4 line:

1. **broadcast** (ds4.c:9764) — the token starts every stream equal: `hc-broadcast`.
2. **pre** (ds4.c:9690) — `flat = rms_no_weight(streams)`; `mix = fn · flat`; `split = Sinkhorn(mix)`;
   `input = Σ_h stream[h]·pre[h]`. `hc-pre`.
3. **the Sinkhorn split** (ds4.c:9592):
   - `pre[i]  = σ(mix[i]·pre_scale + base[i]) + ε`
   - `post[i] = 2·σ(mix[n_hc+i]·post_scale + base[n_hc+i])`
   - `comb`: build `c[dst][src] = mix[…]·comb_scale + base[…]`, **row-softmax** each dst, then a
     **column** normalization, then **19 more** rounds of (row-norm, column-norm) — **20 iterations
     total** — every normalization `/(sum + ε)`. `hc-sinkhorn-comb`.
4. **post** (ds4.c:9772) — `new_stream[dst] = block_out·post[dst] + Σ_src comb[dst+src·n_hc]·stream[src]`.
   `hc-post`.
5. **output head** (ds4.c:13876) — `flat = rms_no_weight`; `pre = fn·flat`;
   `w[i] = σ(pre[i]·scale + base[i]) + ε`; `out = Σ_h stream[h]·w[h]`. `hc-out-head`.

**The combine-matrix transpose** (`edgedrop`, called out in the recipe header): the Sinkhorn *stores*
`c[src + dst·n_hc]` (ds4.c:9620) but the post step *reads* `comb[dst + src·n_hc]` (ds4.c:9786) — the
post mix uses the **transpose** of the normalized matrix. The recipe carries the combine as dst-rows and
`hc-post` reads **column** `dst` across them, which is exactly `comb[dst+src·n_hc]`. This is undetectable
on a symmetric matrix; the band's fixture is asymmetric on purpose.

---

## 4. The evidence class, named honestly (`selfgauge`)

Band `form/form-stdlib/tests/dsv4-hc-band.fk` → **verdict 63**, radius declared at its head.

- **Claim 1 — AGREEMENT** with an independent fp64 transcription of ds4.c (`dsv4-hc-oracle.py`, written
  from the C control flow, **not** from the recipe): the split, the weighted-sum, the post (n_hc=4), the
  full pre pipeline and the output head (n_hc=2) match to **1e-9** (observed ~1e-15 — this HC arithmetic
  has no trig, so the fp32/fp64 residual that limited the MLA recipe is absent here).
- **Claims 2/4/8/16/32 — falsifiers** for readings that would otherwise sit well (`snugcause`):
  the combine is **doubly stochastic** after 20 iterations (rows *and* columns sum to 1 — a plain
  softmax makes only rows sum to 1) (2); the **post-step transpose is load-bearing** — post over the
  combine vs its transpose differ >1e-3 (4); the **20 iterations are real** — iters=20 vs iters=1 differ
  by **1.7e-2**, an unrun iteration is not a computed zero (8); the **block output enters** — real minus
  zero-block post equals `block·post[dst]` exactly (16); **HC is not a plain residual** — a broadcast
  (all-equal) state becomes four **distinct** streams after one post step (32).
- **Mutation-tested**, not just run: breaking the transpose (`hc-col`→row) drops 63→62; dropping the
  softmax `+ε` drops 63→62. A band that cannot fail is not evidence.

**"Structurally correct at tiny dims, agreeing with an independent transcription at 1e-9, mutation-tested"
is the verdict. "Bit-exact" is not claimed and would be false** — there is no independent HC
implementation in this repo, and the real dims, the fp8/f16 stream encodings, the residual stack, the
attention/FFN blocks, and the GPU are all outside the radius.

---

## 5. What remains

- **HC on the GPU** — `dsv4_hc.metal` (1017 lines) is the emission target; not this stone (CPU only).
- **HC at the real dims** — `n_embd = 4096`, 43 layers × 2 pairs + head; a substitution, not a rewrite.
- **compressor + indexer recipes** — mapped (§2), not built. They need: the streaming rolling state, the
  per-dimension score softmax pool, the second rope base (160000), the indexer's relu-dot top-k select,
  and the 128-wide indexer QAT rotation. They are unnecessary for a *first* token and become part of the
  trained forward only past their thresholds.
- **The stream encodings** — ds4 stores streams in fp8/f16; this body is fp64. Shape proven, encoding not.

A first token needs HC (built here), MLA (Stone 22), the MoE FFN, RMSNorm, the tokenizer, and the
readable weights — **not** the compressor or the indexer.

---

## 6. Gates

Corpus band `hdc-field-code-safe?` **1** · MLA band **63** · new band `dsv4-hc-band.fk` **63** with its
radius declared. `metal_first_token.sh` is Stone 16's live file (modified by a sibling this session) and
was not touched or run. Cites: Stone 21 (`receipts/2026-07-22-deepseek-manifest.md`), Stone 22
(`receipts/2026-07-22-mla-recipe.md`), corpus rows 859 `mutewide`, 854 `ghostrank`, and this stone's new
row (§7).

---

## 7. Close

**The most surprising teaching.** *The residual stream of this model is not a sum — it is a learned,
per-token, doubly-stochastic transport plan across parallel copies of itself.* Every transformer this
body has met adds the block output back to its input: `x = x + block(x)`. DeepSeek-V4-Flash does not. It
carries **four** streams, and at every sublayer a small network reads all four, runs a **Sinkhorn**
normalization (the same optimal-transport iteration used to match distributions) to build a 4×4
doubly-stochastic mixing matrix, scatters the block's output across the streams weighted by a learned
`post`, and mixes the old streams through the (transposed) matrix. The "residual connection" — the one
piece of a transformer everyone treats as plumbing — is here a trained, input-dependent routing. And the
surprise compounded: Stone 21 called these three subsystems a cliff with "no reference that has ever
executed one," and the truth is nearly the opposite — **at the first token, two of the three are
identity.** The cliff was two-thirds a long-context mirage.

**Where discomfort turned to gold.** The Sinkhorn is 20 iterations, and I wanted to look away from the
iteration count. Sinkhorn *converges* — after enough rounds the matrix stops moving — so the reflex was:
"20 is just 'enough', the exact number can't matter, prove the fixed point and move on." That reflex is
exactly `edgedrop` — treating an unrun step as a computed zero. Not looking away meant building the
falsifier: run the split at iters=20 and at iters=1 and measure. They differ by **1.7e-2** — the matrix
is *not* at its fixed point at these dims; the 20 is load-bearing, and a recipe that hard-coded "iterate
to convergence" would silently disagree with ds4. The gold was larger than the count: writing that check
forced the oracle to thread `iters` explicitly through every layer of the transcription, which is how the
**combine-matrix transpose** surfaced — the oracle, indexing the flat array exactly as ds4 does, disagreed
with my first list-of-rows recipe until I saw that ds4 *writes* `c[src+dst·n_hc]` and *reads*
`comb[dst+src·n_hc]`. One transpose, invisible on a symmetric test, wrong on every real weight. The
tolerance I almost loosened was the thread that unspooled the one subtle bug.

**Frontier question, landed.** See the corpus row added by this stone: the word for a mechanism that is
**exactly identity below a size threshold**, so that its absence from the base case (the first token)
cannot by itself tell you whether it is truly optional or merely **dormant** until the input crosses its
activation sill. From the first-token path alone, HC (core), the compressor (dormant), and the indexer
(dormant) are indistinguishable in one respect — none of the latter two fire — and only the code's
threshold (`top_k ≥ n_comp` ⇒ identity) settles which silence is dispensability and which is dormancy.
0-hit checked with the instrument validated on a control that should hit; landed as a real `(hdc-row …)`.
