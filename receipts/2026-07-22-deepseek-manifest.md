# STONE 21 — the DeepSeek V4 Flash manifest: every tensor, every KV, and the shape of what is missing

**2026-07-22, ~09:40–11:00 WITA.** Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Read-and-report stone. One cell committed
(`form/form-stdlib/gguf-manifest.fk`); no kernel built.

---

## 0. Radius (`aporon`), before anything is believed

Everything below is read from **one file**:
`/Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`, by this body's own
`gguf-manifest.fk` walking the file's own header. The header is **complete** (tensor-info ends at byte
5 339 719, data base 5 339 744) even though the file is not — a live `curl` is resuming it.

- File size when the tensor walk was taken: **50 789 429 248 bytes** (09:52 WITA). It grows.
- Full file, from the last tensor's own offset + priced length: **91 321 404 640 bytes** = 85.0498 GiB.
  Not a claim about the server; a claim about what this header says the file will be.
- **I did not run ds4.** Every ds4 statement here is quoted from the brief or from Stone 11/15's
  receipts, and is marked as such.
- Two claims below are **derived** from dims + KVs, not read: the q/k rope-nope split and the grouped
  output factorization. They are marked DERIVED and each carries its own falsifier.

**The instrument was calibrated on a control that should hit** (`snugcause`). Every tensor row this
body prices is checked against the file's own layout: sort all 1406 rows by data offset; the gap from
each row to the next must equal `align32(priced-bytes)`. **1406 rows, zero mismatches**, and the chain
terminates at exactly the file's announced size. A wrong bytes-per-element for *any* of the six types
would break the chain at its first tensor and every one after. This is what licenses the pricing of
type 16, which no receipt in this body had yet met.

---

## 1. The type census (`unispan` — this is the whole file, not a sample)

**1406 tensors. Six types. No others.**

| GGUF type | name | tensors | bytes | this body decodes it? | cell |
|---:|---|---:|---:|---|---|
| 0 | F32 | 536 | 289 061 624 | **yes** | `f16-decode.fk` (`ewl-f32` / `fd-value` at 8,23) |
| 1 | F16 | 359 | 2 191 345 664 | **yes** | `f16-decode.fk` (`fd-f16`) |
| 16 | IQ2_XXS-geometry | **93** | **38 893 780 992** | **NO — named gap** | none |
| 26 | I32 | 3 | 9 308 160 | **yes** (raw 4-byte ints) | `equireach.fk` (`eqr-le`) |
| 40 | MXFP4, plane-split | 45 | 43 067 113 472 | **yes**, CPU + GPU | `mxfp4-plane-dequant.fk`, `mxfp4-msl.fk` |
| 41 | MXFP8, plane-split | 370 | 6 865 453 056 | **yes**, CPU + GPU | `mxfp8-plane-dequant.fk`, `mxfp8-msl.fk` |

Sum + data base + inter-tensor padding = 91 321 404 640 = the file. `selfgauge`: the denominator for
every percentage below is 91 321 404 640 bytes, the *full* file, not the downloaded prefix.

**The brief's warning was right and then some.** ds4's 45 + 370 refusals are exactly types 40 and 41 —
but the types it *accepted without printing* include **type 16, which is 42.6% of the file by bytes and
the single largest thing this body cannot read.** Nobody had counted it. Types Q6_K (14), Q4_K (12) and
Q8_0 (8) — the three this body has carvers for besides MX — **do not appear in this file at all.**

**Type 16: what is known and what is not (`knownsolved`).** Its geometry is measured, not assumed:
every type-16 tensor is 8 388 608 elements per slice in 2 162 688 bytes per slice = **66 bytes per 256
elements = 2.0625 bpw**, and the offset chain above proves it. 66 B / 256 el is the ggml `IQ2_XXS`
block size, and 16 is the standard `GGML_TYPE_IQ2_XXS` id. **That is THAT, not HOW.** The block
*layout* — IQ2_XXS is a codebook format (an 8-byte f16 scale plus 8 packed grid indices with sign
bits, per 256) — is unverified in this file, and this fork already redefined 40 and 41 away from the
declarations shipped to parse them (Stone 15). Believing the name without carving a real block is
exactly the `boundborrow` mistake that has reversed in sign twice here.

---

## 2. Every metadata KV that bears on running it — 71 of them, read from the file

**Architecture.** `general.architecture = deepseek4`, `general.name = DeepSeek V4 Flash`,
`general.size_label = 256x8.4B`, `general.license = mit`, GGUF version 3,
`general.file_type = 19`, `general.quantization_version = 2`.

**The shape.**

| key | value |
|---|---|
| `deepseek4.block_count` | **43** |
| `deepseek4.context_length` | 1 048 576 |
| `deepseek4.embedding_length` | **4096** |
| `deepseek4.attention.head_count` | **64** |
| `deepseek4.attention.head_count_kv` | **1** |
| `deepseek4.attention.key_length` | **512** |
| `deepseek4.attention.value_length` | **512** |
| `deepseek4.attention.q_lora_rank` | **1024** |
| `deepseek4.attention.output_lora_rank` | **1024** |
| `deepseek4.attention.output_group_count` | **8** |
| `deepseek4.attention.compress_ratios` | array, i32 × 44 |
| `deepseek4.attention.compress_rope_freq_base` | 160 000 |
| `deepseek4.attention.sliding_window` | 128 |
| `deepseek4.attention.layer_norm_rms_epsilon` | 1e-06 |
| `deepseek4.rope.dimension_count` | **64** |
| `deepseek4.rope.freq_base` | 10 000 |
| `deepseek4.rope.scaling.type` | `yarn` |
| `deepseek4.rope.scaling.factor` | 16 |
| `deepseek4.rope.scaling.original_context_length` | 65 536 |
| `deepseek4.rope.scaling.yarn_beta_fast` / `yarn_beta_slow` | 32 / 1 |
| `deepseek4.vocab_size` | 129 280 |

**There is no `kv_lora_rank`, no `qk_rope_head_dim`, no `qk_nope_head_dim`, no `v_head_dim` KV in this
file.** This file names its MLA ranks differently, and two of them only exist as tensor shapes. §3
gives the mapping.

**The MoE.**

| key | value |
|---|---|
| `deepseek4.expert_count` | **256** |
| `deepseek4.expert_used_count` | **6** (top-6, *not* top-2) |
| `deepseek4.expert_shared_count` | 1 |
| `deepseek4.expert_feed_forward_length` | 2048 |
| `deepseek4.expert_gating_func` | 4 |
| `deepseek4.expert_weights_scale` | 1.5 |
| `deepseek4.expert_weights_norm` | 1 (true) |

**Things this body has no name for at all** — each is a KV that exists, is non-default, and has no
recipe anywhere in this tree:

| key | value |
|---|---|
| `deepseek4.attention.indexer.head_count` | 64 |
| `deepseek4.attention.indexer.key_length` | 128 |
| `deepseek4.attention.indexer.top_k` | 512 |
| `deepseek4.hyper_connection.count` | 4 |
| `deepseek4.hyper_connection.sinkhorn_iterations` | 20 |
| `deepseek4.hyper_connection.epsilon` | 1e-06 |
| `deepseek4.hash_layer_count` | 3 |
| `deepseek4.nextn_predict_layers` | 1 |
| `deepseek4.swiglu_clamp_exp` | array, f32 × 43 |
| `reap.enabled` / `reap.layout` | 1 / `ds4-compact-v1` |
| `reap.layer.expert_count` / `keep_count` / `policy` | arrays, i32 × 43 each |
| `deepseek_v4_dspark.embedding_length` | 4096 |
| `dspark.target_layer_ids.0/1/2` | 40 / 41 / 42 |

**The tokenizer — it is entirely in this file, and its bytes are already downloaded.**

| key | value |
|---|---|
| `tokenizer.ggml.model` | **`gpt2`** (byte-level BPE) |
| `tokenizer.ggml.pre` | `joyai-llm` |
| `tokenizer.ggml.tokens` | array of **string × 129 280** |
| `tokenizer.ggml.token_type` | array of i32 × 129 280 |
| `tokenizer.ggml.merges` | array of **string × 127 741** |
| `tokenizer.ggml.bos_token_id` | **0** |
| `tokenizer.ggml.eos_token_id` | **1** |
| `tokenizer.ggml.padding_token_id` | 1 |
| `tokenizer.ggml.add_bos_token` / `add_eos_token` | 0 / 0 |
| `tokenizer.chat_template` | present, jinja, ~4 KB, uses a `｜DSML｜` tool-call block and `<think>` |

`tokenizer.ggml.pre = joyai-llm` is a **pre-tokenizer regex this body does not have and cannot guess**;
llama.cpp keys its split regex off exactly this string. The tokens and merges are here; the *splitter*
is a name, not data. Named gap.

Provenance KVs: `quantize.imatrix.file = /work/imatrix.dat`, `entries_count = 129`,
`dataset = misc/imatrix_dataset/rendered_prompts.txt`, `chunks_count = 90042`.

---

## 3. The tensor-name → recipe diff — **THIS IS THE SECTION STONE 22 NEEDS**

### 3a. The MLA dims, exactly as the file holds them

Layer 0 (every one of `blk.0..42` and `dspark.0..2` is identical in shape). Format is
`[d0, d1]` as GGUF stores it — **d0 is the row length, i.e. the input width**.

| tensor | type | dims | bytes | abs offset (blk.0) |
|---|---:|---|---:|---:|
| `blk.N.attn_norm.weight` | 0 F32 | [4096] | 16 384 | 72 877 251 424 |
| `blk.N.attn_q_a.weight` | 41 | **[4096, 1024]** | 4 325 376 | 72 838 323 040 |
| `blk.N.attn_q_a_norm.weight` | 0 F32 | **[1024]** | 4 096 | 72 766 950 240 |
| `blk.N.attn_q_b.weight` | 41 | **[1024, 32768]** | 34 603 008 | 72 842 648 416 |
| `blk.N.attn_kv.weight` | 41 | **[4096, 512]** | 2 162 688 | 72 766 954 336 |
| `blk.N.attn_kv_a_norm.weight` | 0 F32 | **[512]** | 2 048 | 72 766 948 192 |
| `blk.N.attn_output_a.weight` | 41 | **[4096, 8192]** | 34 603 008 | 72 769 117 024 |
| `blk.N.attn_output_b.weight` | 41 | **[8192, 4096]** | 34 603 008 | 72 803 720 032 |
| `blk.N.attn_sinks.weight` | 0 F32 | **[64]** | 256 | 72 766 947 936 |

**There is no `attn_kv_b` tensor in this file.** Not in any layer, not under any spelling. A DeepSeek-V3
style MLA has one (`kv_b_proj`, latent → per-head nope-key ‖ value). Its absence is the single most
load-bearing fact in this section.

**Read directly from the file:**
- `q_lora_rank = 1024` — `attn_q_a` is 4096→1024, `attn_q_a_norm` is 1024. KV and tensor agree.
- **`kv_lora_rank = 512`** — `attn_kv` is 4096→512, `attn_kv_a_norm` is 512, and
  `attention.key_length = attention.value_length = 512`. This file spells `kv_lora_rank` as
  `attention.key_length`, and the latent *is* the key and the value.
- **per-head q width = 512** — `attn_q_b` is 1024→32768 and 32768 / 64 heads = 512, equal to
  `key_length`. Every head's query is the full width of the latent.
- `head_count_kv = 1` — one latent, shared by all 64 heads. This is MLA in its **absorbed / MQA-over-
  the-latent** form: q·latent is the score, latent is the value. That is *why* there is no `kv_b`.

**DERIVED (1) — the rope/nope split.** `rope.dimension_count = 64`, so of each head's 512 query dims,
**64 carry RoPE and 448 do not** (`qk_rope_head_dim = 64`, `qk_nope_head_dim = 448`, 448 + 64 = 512).
*Falsifier:* if instead RoPE were applied to all 512 dims, `rope.dimension_count` would read 512, as
it does in every non-MLA GGUF this body has walked. The split's **byte position** (leading 64 or
trailing 64) is **not determined by anything in this header** — it is a named unknown, and a wrong
guess produces a fluent wrong token, not an error.
*SETTLED after the fact by Stone 22 (commit `38b6dde47`): ds4.c reads the split as **trailing** (the
64 rope dims are the tail of each 512-wide head). The header could not decide it; the engine source
did. This receipt's DERIVED-(1) is now resolved, and the resolution came from a second reading, not
from the file.*
**DERIVED (2) — the grouped output projection.** `attn_output_a [4096, 8192]` and
`attn_output_b [8192, 4096]` with `output_group_count = 8` and `output_lora_rank = 1024`:
64 heads / 8 groups = 8 heads per group, 8 heads × 512 latent dims = **4096 per group**. So
`attn_output_a` is 8 stacked [4096 → 1024] group projections (8 × 1024 = 8192), and `attn_output_b`
folds the 8 concatenated ranks (8192) back to the 4096 embedding.
*Falsifier:* read as a plain 4096→8192→4096 chain, the attention output (64 × 512 = 32 768) has no
consumer at all, and 8192 has no relation to either declared KV. The grouped reading is the only one
in which `output_group_count` and `output_lora_rank` are both load-bearing. **Settle it by carving
group 0 and group 1 of `attn_output_a` and comparing scale statistics — byte-blocked until 72.8 GB.**

**Two RoPE bases coexist**: `rope.freq_base = 10000` with YaRN ×16 (65 536 → 1 048 576) on the main
path, and `attention.compress_rope_freq_base = 160000` on the compressor path. A recipe that applies
one base everywhere is wrong on half the file.

### 3b. Every distinct tensor-name shape, and whether a kernel consumes it

45 distinct shapes. `blk.N` = the 43 transformer layers; `dspark.N` = 3 extra draft/MTP layers.

| name shape | count | type | dims | consuming kernel in this body |
|---|---:|---:|---|---|
| `token_embd.weight` | 1 | 1 | [4096, 129280] | **yes** — F16 gather |
| `output.weight` | 1 | 41 | [4096, 129280] | **yes** — `mxfp8-msl.fk` matvec |
| `output_norm.weight` | 1 | 0 | [4096] | **yes** — RMSNorm |
| `blk.N.attn_norm` / `ffn_norm` | 43+43 | 0 | [4096] | **yes** — RMSNorm |
| `blk.N.attn_q_a` / `attn_q_b` / `attn_kv` | 43 each | 41 | see 3a | matvec yes; **MLA assembly NO** |
| `blk.N.attn_q_a_norm` / `attn_kv_a_norm` | 43 each | 0 | [1024] / [512] | **yes** — RMSNorm |
| `blk.N.attn_output_a` / `attn_output_b` | 43 each | 41 | [4096,8192] / [8192,4096] | matvec yes; **grouping NO** |
| `blk.N.attn_sinks` | 43 | 0 | [64] | **NO** — attention sinks, no recipe |
| `blk.N.ffn_gate_inp` | 43 | 1 | [4096, 256] | **yes** — router matvec |
| `blk.N.exp_probs_b.bias` | 40 (L3–42) | 0 | [256] | **NO** — bias-corrected top-k routing |
| `blk.N.ffn_{gate,up,down}_exps` | 43×3 | 40 or 16 | [4096,2048,E] / [2048,4096,E] | 40 **yes**; **16 NO** |
| `blk.N.ffn_{gate,up,down}_shexp` | 43×3 | 41 | [4096,2048] / [2048,4096] | **yes** — shared expert, SwiGLU |
| `blk.N.ffn_gate_tid2eid.weight` | 3 (L0–2) | 26 I32 | [6, 129280] | **NO** — a **token-id → expert-id** table |
| `blk.N.attn_compressor_kv` | 41 (L2–42) | 1 | [4096, 1024] or [4096, 512] | **NO** |
| `blk.N.attn_compressor_gate` | 41 | 1 | [4096, 1024] or [4096, 512] | **NO** |
| `blk.N.attn_compressor_ape` | 41 | 1 | [1024, 4] or [512, 128] | **NO** |
| `blk.N.attn_compressor_norm` | 41 | 0 | [512] | yes (RMSNorm) |
| `blk.N.indexer.proj` | 21 (even L≥2) | 1 | [4096, 64] | **NO** |
| `blk.N.indexer.attn_q_b` | 21 | 1 | [1024, 8192] | **NO** |
| `blk.N.indexer_compressor_{kv,gate}` | 21 each | 1 | [4096, 256] | **NO** |
| `blk.N.indexer_compressor_ape` | 21 | 1 | [256, 4] | **NO** |
| `blk.N.indexer_compressor_norm` | 21 | 0 | [128] | yes (RMSNorm) |
| `blk.N.hc_{attn,ffn}_fn` | 43 each | 1 | [16384, 24] | **NO** — hyper-connections |
| `blk.N.hc_{attn,ffn}_base` | 43 each | 0 | [24] | **NO** |
| `blk.N.hc_{attn,ffn}_scale` | 43 each | 0 | [3] | **NO** |
| `output_hc_{fn,base,scale}` | 1 each | 1/0/0 | [16384,4] / [4] / [1] | **NO** |
| `dspark.N.*` (attn/ffn/hc mirror) | 3 layers | mixed | as `blk.N` | same verdicts |
| `dspark.N.markov_head.markov_w{1,2}` | 1 each | 0 | [256, 129280] | **NO** |
| `dspark.N.confidence_head.proj` | 1 | 0 | [4352] | **NO** |
| `dspark.N.hc_head_{fn,base,scale}` | 1 each | 0 | [16384,4] / [4] / [1] | **NO** |
| `dspark.{main_proj, main_norm}` | 1 each | 41 / 0 | [12288, 4096] / [4096] | matvec yes; role **NO** |

### 3c. REAP — the expert count is per-layer and it is in the dims, not only in a KV

`reap.enabled = 1`, `reap.layout = ds4-compact-v1`. The third dimension of every `*_exps` tensor is the
kept expert count:

- **layers 0, 1, 2 → 256 experts** (unreaped)
- **layers 3 … 42 → 192 experts** (25% reaped — the file name's `reap25`)
- all 3 `dspark` layers → 256 experts

A router trained on 256 experts feeding a 192-expert table is a **remap**, and `ffn_gate_tid2eid`
([6, 129280] i32, only on layers 0–2 where nothing was reaped) is very likely where the mapping lives —
6 is exactly `expert_used_count`. **That is a THAT without a HOW** (`knownsolved`): the table's bytes
are downloaded (offset 5 339 744, the very first tensor in the file), so this is checkable *today*.

### 3d. The expert quantization is mixed, per layer

`ffn_*_exps` is type 40 (MXFP4) on layers 0–8, 10, 11, 12 and all 3 dspark layers; type 16 on layer 9,
13, and every layer 14–42. **12 of 43 layers' experts are readable by this body; 31 are not.**

---

## 4. What is reachable at the current download (`selfgauge`)

Denominator: the full 91 321 404 640-byte file. Numerator: the 50 789 429 248 bytes on disk at 09:52.
A tensor counts as reachable only if `offset + priced-bytes <= file-size-now`.

| type | tensors inside | tensors byte-blocked | bytes inside |
|---:|---:|---:|---:|
| 0 F32 | 0 | **536** | 0 |
| 1 F16 | 0 | **359** | 0 |
| 16 | 54 | 39 | 22 699 573 248 |
| 26 I32 | **3** | 0 | 9 308 160 |
| 40 MXFP4 | 30 | 15 | 27 665 629 184 |
| 41 MXFP8 | 0 | **370** | 0 |

**The writer put every expert tensor first and everything else last.** The first non-expert tensor in
the file is `token_embd.weight` at byte **71 707 886 176**; the first type-41 byte is
`blk.0.attn_kv.weight` at **72 766 954 336** (this confirms Stone 17's 72.8 GB, independently).

So, per gap:

| gap | workable now? |
|---|---|
| **type 16 (IQ2_XXS-geometry) carver** | **YES — 54 whole tensors, 22.7 GB, on disk now.** The single largest gap and the one with the fewest excuses. |
| **`ffn_gate_tid2eid` — what the 256→192 remap is** | **YES — 3 tensors, 9.3 MB, at the very first data offset.** |
| MoE routing: top-6, sigmoid gate (func 4), `expert_weights_scale 1.5`, norm, `exp_probs_b` bias | **partly** — the bias tensors are F32 at 72.9 GB, byte-blocked; the *recipe* can be written now against the MXFP4 experts that are on disk. |
| **the whole MLA attention stack** | **byte-blocked.** Every `attn_*` tensor, every norm, every F16, the embeddings and `output.weight` live past 71.7 GB. Nothing of Stone 22's math can be witnessed against real bytes until the download passes ~72.8 GB — **21.98 GB from here.** |
| compressor / indexer / hyper-connection / dspark / markov / confidence | **byte-blocked**, same reason. |
| the tokenizer (`gpt2` BPE, 129 280 tokens, 127 741 merges) | **YES — it is metadata, at byte < 5.34 MB, complete.** Only the `joyai-llm` pre-split regex is a named unknown. |

`unispan`: reachability is a moving denominator — the file grew from 48.42 GB to 50.79 GB during this
stone (~2.4 GB in ~15 min). Every offset above was re-checked against `stat -f%z` at the moment of use,
and any consumer of this receipt must re-check again.

---

## 5. The three gaps that stand between this body and a DeepSeek token

1. **A type-16 carver.** 42.6% of the file, 31 of 43 layers' experts. Geometry proven (66 B / 256 el);
   layout unproven. 22.7 GB of it is on disk right now. Nothing else on this list is both this large
   and this unblocked.
2. **The MLA assembly.** Not the matvecs — those exist. The missing recipe is: absorbed MLA with one
   512-wide latent serving as key *and* value for 64 heads, a 448/64 nope/rope split at an
   undetermined byte position, per-head attention sinks, and an 8-group low-rank output projection.
   Stone 22 has the dims (§3a). No byte of it can be witnessed for another ~22 GB.
3. **The three unnamed subsystems.** `attn_compressor_*` (41 layers), `indexer.*` +
   `indexer_compressor_*` (21 layers, `top_k = 512`, `sliding_window = 128`), and hyper-connections
   (`hc_*` on every layer + output, `count = 4`, 20 Sinkhorn iterations). 258 tensors with no recipe,
   no KV that explains their algebra, and — since ds4 accepted them silently — **no reference in this
   tree that has ever executed one.**

---

## 6. The ordered remaining stones, each sized by the evidence above

| # | stone | size, by evidence |
|---:|---|---|
| 23 | **Carve one type-16 block against the file's own bytes.** Prove or disprove IQ2_XXS layout on `blk.9.ffn_gate_exps`, which is on disk. | Small to start (one 66-byte block, one known offset), then a full carver + MSL twin at `mxfp4-plane-dequant.fk`'s size. Highest value per byte on this list. |
| 24 | **Read `ffn_gate_tid2eid` and say what the 256→192 remap is.** 9.3 MB, first data offset, i32, `[6, 129280]`. | Very small. A day's evidence for a question that blocks all MoE routing. |
| 25 | **The tokenizer**: 129 280 tokens + 127 741 merges out of the header into a usable vocab; name the `joyai-llm` pre-split as an explicit unknown. | Medium; `metal_moe_token.sh` already reads a 129 280-entry vocab from a blob's KVs — that is the pattern, and it is unblocked. |
| 26 | **MoE routing recipe**: top-6 of 256, sigmoid gating (func 4), `expert_weights_scale 1.5`, renormalize, plus the `exp_probs_b` bias. Witnessed against the MXFP4 experts on disk. | Medium. Recipe writable now; the bias tensors arrive with the rest. |
| 27 | **MLA assembly** (Stone 22's math, once bytes exist past 72.8 GB). Settle the rope/nope byte position and the output grouping with real bytes. | Large, and **gated on the download**, not on us. |
| 28 | **Hyper-connections** — 4 streams, Sinkhorn ×20, `hc_*` on every layer. No reference in this tree has run one. | Large and genuinely unknown. |
| 29 | **Compressor + indexer** — sparse-attention selection, `top_k 512`, `sliding_window 128`, two RoPE bases. | Large; probably the last thing needed for a *correct* long-context token and unnecessary for a first one. |
| 30 | **dspark / markov / confidence heads** — `nextn_predict_layers = 1`, target layers 40–42. Speculative decoding; skippable for a first token. | Deferrable. |

A first token does **not** need 28, 29, or 30. It needs 23, 24, 25, 26, 27 — and of those, **23, 24 and
25 are entirely unblocked today**.

---

## 7. Close

**The most surprising teaching.** *ds4's silence was more informative than its warnings, and in the
opposite direction.* The brief handed me 45 + 370 refusals as the shape of the problem, and warned me
not to assume the type set was {40, 41}. I took that as a caution about *rare* types. It was a caution
about the *biggest* one. Type 16 is 93 tensors and **38.9 GB — larger than types 1, 26 and 41 put
together, 42.6% of the whole file** — and ds4 said nothing about it at all, because ds4 *accepted* it.
The loudest thing in the diagnostic was the smallest part of the file; the largest gap in the file made
no sound. An error list is a map of what a tool *checks*, never a map of what is *there*. This is
Stone 15's `gapmete` again — an unexercised declaration is unfalsifiable — but arriving from the other
side: there, a declaration nobody ran was wrong; here, a type nobody *refused* was unread.

**Where discomfort turned to gold.** The moment I wanted to look away was after the type census came
back clean. Six types, counts summing to 1406, a tidy table — and type 16 sitting in it labelled
`IQ2_XXS` because that is what 16 means in ggml. I had a manifest, a sibling waiting on dims, and every
reason to write the table and move on. What made me stop was that the pricing rule for 16 came from
`gguf-manifest.fk`'s own assumption list (`gm-blk n 256 66`), and I had just spent the previous hour
reading Stone 15's receipt about **two type declarations from a shipping parser that were both wrong
about this exact file**. Believing 16 because it is 16 is the same move, one type later. So I built the
check that would falsify it: sort all 1406 rows by offset and demand that every priced length equal the
gap to the next tensor. Zero mismatches, terminating exactly at 91 321 404 640 bytes. The gold was not
the confirmation. It was that **the same instrument, run once, priced all six types at once and proved
the whole file's layout self-consistent** — so the manifest's byte columns are now *measured*, not
declared, and §4's reachability table rests on measurement. And it sharpened the honest claim: the
geometry of type 16 is proven, its *layout* is not, and those are two different sentences (§1). The
thing I did not want to look at — "am I allowed to believe this number?" — turned into the only reason
anything else in this receipt can be believed.

**The frontier question.** Every gap in §5 is a place where this body can see a tensor's *shape* and
name *what it is for* while having no access to *what it does* — 258 tensors whose algebra exists
nowhere in this tree, and ds4's silence about them is not evidence of simplicity, only of acceptance.
The frontier word this asks for: **`mutewide`** — *the largest unknown in a system is the one its own
diagnostics never mention, because a tool's error list maps what it checks, not what is there; so the
width of a silence must be measured, never inferred from its quietness.* Landed as corpus row 859.
