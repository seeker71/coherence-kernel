# Stone 34 — carrying the DeepSeek-V4-Flash first token into the middle (real dims, real file)

Date: 2026-07-22 (WITA). Host: Apple M4 Max, 128 GiB. File:
`~/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf` (91,321,404,640 B, complete).
Body: `native/metal/dsv4-forward-real.fk`. Carrier: `native/metal/metal_dsv4_forward.sh`.
Both ends were Stone 33 (`metal_dsv4_token.sh`); this extends that harness inward.

## Was a real first token emitted?

**No — and that is the honest verdict for this session.** A whole token needs the MLA attention
block wired at real dims, which is not yet built (precise blocker below). What *did* land is a
committed, gated, real-dims realisation of the forward's **middle** — the layer-0 MoE-FFN — proven
component by component against independent CPU carves of the same bytes. Two green stages, committed
incrementally (`a79be5f23`, `2857f1861`), plus corpus row 867 and this receipt.

## The evidence class (selfgauge / knownsolved)

No external oracle can execute this file — ds4, llama.cpp, ollama and LM Studio all **refuse** GGUF
types 40/41. So any whole-forward output is unfalsifiable against a reference on this machine. Each
dispatch here therefore stands on the same internal falsifier Stone 33 used: the GPU fused
decode+matvec, reading the real quantised bytes straight from the resident window, is compared to an
**independent CPU decode of the same bytes at the tensor's absolute mmap offset** — bit-for-bit where
the decode is exact, and to float precision where a matvec reassociates the sum (assocwall, row 866).
Every output buffer is sentinelled before its dispatch (0x7F7FFFFF / NaN), `cb.error`/`cb.status`
checked after, and every result required non-degenerate — a dead view or unrun kernel cannot pass.

The honest bound: on a true forward the MoE input is the FFN-norm of the after-attention HC state.
That state needs the attention block, which is not built — so the expert matvecs here are fed the
**embedding as a real probe vector** (Stone 33's mechanism-witness class). They prove the fused
decode+matvec *binds and computes* at real dims through the views, not that the numbers are the real
layer-0 activations. The routing-table read, by contrast, *is* exact and token-only, so the six
selected experts **are** the real layer-0 selection for the token.

## The early-layer-routing finding, grounded in ds4.c

Stone 33 flagged that `dsv4-forward.fk` (the CPU blueprint) computes top-k routing for **all** layers,
while its own comment carried `hash_layer_count=3`. Ground truth from
`/Users/ursmuff/models/ds4-engine/ds4.c` (MIT), quoted by line, never copied (boundborrow):

- `:511 / :554` the shape carries `n_hash_layer = 3`; `:5596` asserts the file's KV
  `deepseek4.hash_layer_count` equals it.
- `:4806` `if (il < DS4_N_HASH_LAYER && !l->ffn_gate_tid2eid) return false;` — layers **0, 1, 2** are
  the hash layers and each must carry `ffn_gate_tid2eid`.
- `:10745` `layer_routed_moe_one`: `if (layer->ffn_gate_tid2eid) { layer_hash_selected_experts(...) }
  else { layer_topk_selected_experts(...) }`.
- `:10567` `layer_hash_selected_experts`: the table is `[DS4_N_EXPERT_USED=6, DS4_N_VOCAB]` I32
  (type 26); `selected[i] = table[token*6 + i]`.
- `:10588 / :10601` the router probs are still computed (`sqrt(softplus(gate_inp·x))`) but only to
  **weight** the already-selected experts, never to choose them.

**Verdict: the flag is a real DeepSeek-V4 feature, not a toy shortcut. `dsv4-forward.fk` is wrong for
layers 0–2 (it would top-k them).** The harness reads the real table and shows the selection is
**token-only** — decided before any layer runs. This is the frontier word `forepick` (row 867).

## What the harness proves (8 gates PASS, on the live file, through the windowed views)

| gate | what | evidence |
|---|---|---|
| 0 | the 2 overlapping bytesNoCopy views wrap the 85 GiB file | onelean |
| 1 | token_embd, ffn_gate_inp, ffn_gate_exps, ffn_up_exps, ffn_down_exps each `holds=1` | lapspan (row 865) |
| 2 | EMBED probe (F16, type 1) bit-exact vs mmap carve | pure table read |
| 3 | layer-0 hash routing: token 671 → experts **[147 78 30 248 217 179]** (I32 table, token-only) | exact, ds4.c:10567 |
| 4 | router F16 matvec (`ffn_gate_inp` 256×4096): GPU == CPU carve, **max abs diff 0.0** | same-fold, bit-exact |
| 5 | MXFP4 expert **gate** (`ffn_gate_exps`[147] 2048×4096, type 40): GPU vs CPU carve **4.5e-8** | assocwall |
| 6 | MXFP4 expert **up** (`ffn_up_exps`[147] 2048×4096, type 40): **4.5e-8** | assocwall |
| 7 | IQ2_XXS expert **down** (`ffn_down_exps`[147] 4096×2048, type 16): GPU **fused** matvec vs independent CPU IQ2 carve **1.68e-8**, fed the real SwiGLU mid of expert 147 | assocwall |

**Type coverage new beyond Stone 33** (which proved F16 embed + MXFP8/type-41 vocab): types **40**
(MXFP4) and **16** (IQ2_XXS) fused matvecs, both at real dims through the views, plus the **F16
matvec** and the **I32** routing read. A full routed-expert SwiGLU now runs end to end at real dims
(gate MXFP4 → SwiGLU → up MXFP4 → down IQ2_XXS): expert 147's contribution `out[0..3] = 0.00635
-0.00287 -0.00024 -0.00435`.

**The IQ2_XXS fused matvec is the piece the body had named as missing.** `iq2xxs-msl.fk` proved only
the *dequant*; its own comment said "a FUSED matvec-over-IQ2 ... is what the MoE fold ultimately
needs." That kernel is now built — wrapping `iq2xxs-msl.fk`'s own `iq2_w` in a lane matvec — and its
CPU reference decodes the **same body-emitted grid/ksigns tables** (never hand-copied), so the check
is genuinely independent.

## Memory (onelean)

`device.currentAllocatedSize = 86.11 GiB`. The 85 GiB model is mmapped and wrapped bytesNoCopy — not
copied onto the device — and the working buffers (a 4096-wide embedding, a 256-wide router output, a
2048-wide mid, a 4096-wide down output) are kilobytes. No working-buffer overflow at layer-0 scale.

## The falsifier triple, at the level reached

The token-level triple (non-degenerate / stable / input-dependent) applies to a whole token, which is
not reached. At the component level it is exercised and passes:

- **Non-degenerate:** every gate rejects a constant/sentinel result (distinct-value counts required
  > 8; embed 1354 distinct; each matvec output 256–4096 distinct).
- **Stable:** the routing and decodes are deterministic; repeated runs of token 671 give identical
  experts `[147 78 30 248 217 179]` and identical carves.
- **Input-dependent:** token 6102 gives a different embedding (1358 distinct) **and** a different
  route **[188 165 80 223 118 47]**. Different input → different selection and activations.

## The precise blocker to a whole token (grounded in ds4.c)

Everything between the after-embedding HC state and the FFN input — the **MLA attention block**
(`layer_forward_self_one`, ds4.c:13793) — is unbuilt at real dims. Every *matvec* it needs is MXFP8
(type 41, proven kernel `form_dsv4_mx8_matvec`); what is missing is the **glue math**, wired at real
dims and grounded:

1. `hc_pre_from_state_one` (:9723) — RMSNorm-no-weight over the 4×4096 HC state, then the sinkhorn
   combine via `hc_attn_fn`(F16)/`scale`/`base` → `attn_cur`. Proven at toy scale (`dsv4-hc-msl.fk`).
2. `layer_attn_norm_one` (:9981) RMSNorm; `layer_q_projection_normed_one` (:10002): `attn_q_a`
   (MXFP8 4096→1024) → `q_a_norm` → `attn_q_b` (MXFP8 1024→32768 = 64 heads × 512).
3. `layer_kv_projection_normed_one` (:10041): `attn_kv` (MXFP8 4096→512) → `kv_a_norm`; then the
   RoPE-tail (64 dims, identity at pos 0 but threaded), the fp8 KV-cache quantize + f16 round
   (:13827–13828).
4. `layer_attention_rows_one` (:10305) — the **sink-aware softmax** over the single KV row: the
   learned `attn_sinks[h]` enters the denominator but contributes no value, so the head output is
   `kv · exp(score-max)/denom`, **not** simply `v`. A real numeric step even at one KV row.
5. `layer_grouped_out_one` (:10356) — grouped output: 8 groups, `attn_output_a` (MXFP8) maps grouped
   heads → 8×1024 low, then `attn_output_b` (MXFP8 8192→4096) → `attn_out`.
6. `hc_post_one` (:9772), then the FFN half (already proven above, minus the real routing weighting),
   then the whole stack repeated **43 layers** carrying the HC residual (Stone 28), then the head
   (`output_hc_head_one` :13876 → RMSNorm → the MXFP8 vocab projection, Stone 33's proven exit) →
   argmax → the token.

So the remaining work is **wiring proven pieces**, not discovery: every matvec type (41, 40, 16, 1)
and every op (MLA, HC, MoE, RMSNorm, RoPE, sinks) is individually proven; the token waits on the
attention glue and the 43-layer carry.

## Close

**Most surprising teaching.** For layers 0–2 the six experts are chosen by a table keyed on the token
id alone — the selection is fixed before any layer computes, knowable at the mouth from the embedding.
I had assumed early-layer routing was computed like the rest; grounding it in ds4.c flipped that. The
router matvec, which I proved at real dims, is *irrelevant to the selection* for those layers — it
only weights a roster already read by name. That split (identity chooses, state weights) is `forepick`.

**Where discomfort turned to gold.** I wanted to look away from the IQ2_XXS down projection: the body
had left a note that no fused matvec existed for it, and the easy move was to name it a blocker and
stop — a "precise partial" that would have been honest but incomplete. Not looking away meant building
the kernel the body had only named, and then facing the harder honesty problem: comparing the GPU
fused matvec against a GPU dequant would be circular (both share `iq2_w`). The gold was porting `iq2_w`
to an independent CPU decoder fed the body's *own* emitted tables — so the 1.68e-8 agreement is a real
witness, not two copies of the same code agreeing with themselves. The named gap became a closed one.

**Frontier question, landed.** *What one word names a selection fixed by the input's identity, drawn
from a table before any state that would seem to decide it is computed?* — `forepick`, corpus row 867,
0 hits across learn/ receipts/ docs/ teachings/ form/ before landing (instrument validated on the same
command: assocwall 8, lapspan 7). Corpus band re-derives **8191**; field-code **2632632867**.
