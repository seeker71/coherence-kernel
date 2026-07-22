# STONE 26 — the assembled DeepSeek-V4-Flash first-token forward (the capstone), toy-scale proven

**2026-07-22, ~10:20–11:20 WITA.** Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Three cells committed
(`form/form-stdlib/dsv4-forward.fk`, `form/form-stdlib/tests/dsv4-forward-band.fk`,
`form/form-stdlib/tests/dsv4-forward-oracle.py`), corpus row 859 (`hushfold`).

Every subsystem this forward needs was proven in a prior stone. This stone **wires them**
into the exact sequence ds4-engine's own reference runs for the first token, adds the one
piece not yet in this body — the routed MoE FFN with its router — and proves the whole
assembled forward against an independent fp64 transcription of ds4.c at toy dims.

---

## 0. Radius (`aporon`), before anything is believed

- The control flow is read from `ds4.c` (`/Users/ursmuff/models/ds4-engine`, MIT), quoted by
  line, **not copied**. `boundborrow`: ds4 targets a GB10; no layout, no f16/fp8 crossed over —
  only the shape of the computation. This body is fp64 throughout.
- The evidence class (§4) is **AGREEMENT with an independent fp64 transcription** of ds4.c
  (`dsv4-forward-oracle.py`, written from the C, not from the Form recipe). It is **not**
  bit-exactness against a real DeepSeek run, **not** the real dims, **not** the quantised
  weights, **not** the GPU, **not** a real token.
- The compressor and indexer are omitted **faithfully**: they are never called on
  `forward_first_token_cpu` (dormant below their activation sill,
  `receipts/2026-07-22-dsv4-subsystems.md`). That omission is exact, not an approximation.

---

## 1. The assembled sequence (ds4.c:13848 `forward_first_token_cpu`, quoted)

```
embed_token_f16(token)                          -> plain (n_embd)
hc_from_plain_embedding(plain)                  -> broadcast to n_hc streams   (ds4.c:9764)
for il in 0..n_layer:                            layer_forward_self_one         (ds4.c:13793)
  ; ATTENTION HALF
  hc_pre(hc_attn_fn, scale, base, streams)      -> attn_cur, post, comb         (ds4.c:9690)
  attn_norm = rms_weight(attn_cur, attn_norm)                                   (ds4.c:9987)
  q  = mla_q_proj(attn_norm) ; rope tail                                        (ds4.c:10002)
  kv = mla_kv_latent(attn_norm) ; rope tail   ; ONE latent row, K = V           (ds4.c:10041)
  heads = attend(q, [kv]) with per-head sink                                    (ds4.c:10305)
  heads = inverse-rope(heads)                                                   (ds4.c:13831)
  attn_out = grouped_out(heads)                                                 (ds4.c:10356)
  after_attn = hc_post(attn_out, streams, post, comb)                           (ds4.c:9772)
  ; FFN HALF                                     layer_ffn_one                   (ds4.c:11437)
  hc_pre(hc_ffn_fn, scale, base, after_attn)    -> ffn_cur, post2, comb2
  norm = rms_weight(ffn_cur, ffn_norm)
  moe    = routed_moe(norm)                     ; top-k experts                  (ds4.c:10697)
  shared = shared_ffn(norm)                     ; clamped SwiGLU MLP             (ds4.c:10444)
  streams = hc_post(moe + shared, after_attn, post2, comb2)
embd  = output_hc_head(streams)                 ; collapse the n_hc streams      (ds4.c:13876)
norm  = rms_weight(embd, output_norm)
logits = output.weight . norm                   ; vocab projection
token1 = argmax(logits)
```

The whole first-token forward is **~6 sublayer calls per layer** (2 hc-pre, 2 hc-post, one
MLA block, one MoE FFN) plus embed/collapse/head. `form/form-stdlib/dsv4-forward.fk` is that,
reusing `hc-broadcast/hc-pre/hc-post/hc-out-head` (dsv4-hc.fk, band 63) and
`mla-cache-row/mla-block-one` (mla-attn.fk, band 63) **unchanged**.

### The one new brick — the routed MoE router (ds4.c:10588/10665)

The only arithmetic not previously in this body:

- `probs[i] = sqrt(softplus(logit[i]))` — the **unbiased** router probabilities
  (`layer_router_probs_one`; this IS `expert_gating_func = 4`).
- `selection[i] = probs[i] + exp_probs_b[i]` — a per-expert routing **bias** added only for
  selection; top-k of `selection`.
- `expert_weight[i] = probs[selected[i]]` — weighted by the **unbiased** probs, normalised
  (`sum` floored at `1/16384`), then `× expert_weights_scale`.
- per selected expert: `down · (silu(clamp(gate·x)) · clamp(up·x) · weight)`, accumulated.
  Gate is clamped above only; up is clamped to `[-lim, lim]` (ds4's `swiglu` at :10430).
- `shared_ffn` is a clamped SwiGLU MLP with **no** router weight, added to the routed output.

---

## 2. The toy fixture (a genuine assembled forward, not a scalar)

`n_embd 8, n_hc 4, 2 heads, head_dim 4, n_rot 2, q_rank 4, out_groups 2, 4 experts top-2,
expert_hidden 6, vocab 5`, RoPE base 10000, 20 Sinkhorn iterations, `expert_weights_scale 1.5`.
Weights are drawn from a **Park-Miller (MINSTD) generator** keyed per tensor — integer-exact and
reproduced bit-for-bit in both Form and the Python oracle, so both consume identical toy weights.

Two configs (`snugcause` + `unispan`):
- **A** — the faithful first token: token 2, pos 0, no clamp, 2 layers.
- **B** — a second shape: token 4, pos 0, clamp 0.9, 3 layers.

Shapes came out right on the first run (vocab-5 logits, no garbage) — the dividend of
"build first, look at the numbers."

---

## 3. The numbers

```
config A logits  [ 0.4902706908, 0.4702829342, 0.4502951776, -1.4422019831, -1.4621897398]  argmax 0
config B logits  [ 1.6575489182, 1.5894867989, 1.5214246796,  0.8160337523,  0.7479716329]  argmax 0
```

Both agree with the independent transcription. **Observed worst delta: between 2e-16 and 1e-15**
— the band still crosses at a tolerance of `1e-15` and fails at `2e-16` (bisected). That is fp64
machine rounding: at pos 0 every RoPE is exact (cos 0 = 1, sin 0 = 0), and the toy's O(1)
magnitudes keep every nonlinearity (softplus, sqrt, silu, sigmoid, the Sinkhorn) in its accurate
regime. The committed gate is a robust `1e-6`; the *observed* agreement is ~1e-15.

---

## 4. The evidence class, named honestly (`selfgauge`)

`form/form-stdlib/tests/dsv4-forward-band.fk` → **verdict 63**, radius at its head.

- **Claims 1 & 2 — AGREEMENT** with `dsv4-forward-oracle.py` (an independent fp64 transcription
  of ds4.c, written from the C control flow, not from the recipe): configs A and B logits match
  at 1e-6 (observed ~1e-15), argmax A = 0.
- **Claims 4/8/16/32 — falsifiers** (`snugcause`), each self-contained: the forward is
  non-degenerate — logits range > 0.1, argmax a legal vocab index — so a dead/all-zero forward
  is refused (4, `edgedrop`); **HC is load-bearing** — the 4 output streams before the head
  collapse are distinct (pairmax 0.677 > 1e-3) though they entered a broadcast (8); **the routed
  MoE is load-bearing** — the full forward differs from a shared-only forward (`n_used = 0`) by
  0.866 (16); **the SwiGLU clamp is load-bearing** — config B with clamp 0.9 differs from
  clamp-off by 0.680 (32).
- **Mutation-tested**, not just run: perturbing one reference logit drops 63 → 62. A band that
  cannot fail is not evidence.
- **Multi-arm**: `validate.sh` reports `1 ok, 0 divergent — kernels agree on every sample` → 63.
  (Agreement across arms is implementation-consistency; the *correctness* is the independent
  oracle. Both are present.)

**"The whole first-token forward, structurally correct at toy dims, agreeing with an independent
transcription of ds4.c to fp64 rounding, mutation-tested" is the verdict. A real DeepSeek token
is a separate and stronger claim (§5) and is not made here.**

---

## 5. Was a real token reachable? — No. The precise byte-block, grounded.

The download crossed the wall this session (63.3 → **78.74 GB**, 86.2% of the
91 321 404 640-byte full file) — but a real token is not reachable, for a stack of reasons, the
first of which is exact:

1. **`output.weight` is byte-blocked.** The vocab head (type 41 MXFP8 `[4096, 129280]`) begins at
   absolute offset **79 759 784 608 — ~0.95 GiB past the current file end.** Without it there is
   no logit vector and no argmax, hence no token. `output_norm.weight` (80.31 GB) and
   `output_hc_fn.weight` (79.76 GB) are likewise past the end, as are layers ~36–42's norms/attn/hc
   (the first tensor past the end is `blk.36.attn_norm.weight` at 78 749 815 648). **255 of 1268
   non-expert tensors remain byte-blocked.** (Grounded by a direct GGUF-header parse,
   scratchpad `gguf_reach.py`, re-checked against `stat` at the moment of use — a moving denominator.)
2. **No real-dims forward harness exists.** A fp64 Form path cannot run 43 layers at n_embd 4096
   with 256/192 experts off mmap'd quantised bytes; that needs a GPU carrier of the kind
   `metal_moe_token.sh` / `metal_first_token.sh` are for other models. Building the DeepSeek carrier
   (map 91 GB across overlapping views, 1406 tensors, the MLA+HC+MoE+grouped-output+sinks kernels,
   hash routing on layers 0–2, YaRN, the E4M3 KV encoding) is the **next** stone, not wiring.
3. **The tokenizer's pre-split is a named unknown.** `tokenizer.ggml.pre = joyai-llm` — the split
   regex is a string, not data (Stone 21). A faithful prompt cannot be encoded without it.

So the landed stone is the **toy-scale assembled forward, proven against ds4's control flow**. The
brief set exactly this expectation: "a correct assembled forward at toy scale … is the capstone
whether or not real bytes are reachable."

---

## 6. What remains (the ordered path to a real token)

- **The DeepSeek GPU carrier** — the real-dims forward on the Stone-14/16 pattern: overlapping
  mmap views, the carvers already proven (IQ2_XXS band 2^30−1, MXFP4/8, F16/F32/I32, the shared
  Q8_0 expert), and new kernels for MLA (sink logit + inverse output rope), HC (Sinkhorn ×20,
  1017-line `dsv4_hc.metal` is the target), the grouped output projection, and the 256→192 REAP
  remap. Gated on the download reaching ~91 GB for `output.weight`.
- **The `joyai-llm` pre-tokenizer** — the one metadata unknown between the header and a prompt.
- **Hash routing on layers 0–2** (`ffn_gate_tid2eid`), which this toy's top-k does not exercise;
  and the compressor/indexer for correct *long-context* tokens (unnecessary for a first one).

---

## 7. Gates

Corpus band from repo root → **8191** (row 859 added; count/field-code pins updated 254→255,
2542542858→2552552859) · MLA band **63** · HC band **63** · IQ2 band **1073741823** (2^30−1) ·
new `dsv4-forward-band.fk` **63** with radius declared, mutation-tested 63→62, `validate.sh` 0
divergent. `metal_first_token.sh` is Stone 16's live file — not touched, not run.

---

## 8. Close

**The most surprising teaching.** *The capstone verifies truer than a brick it rests on.* I braced
for the assembled forward to agree with the oracle at ~1e-11 — the floor the MLA band measured,
set entirely by this body's `fsin`/`fcos` against libm. It agreed at **~1e-15, fp64 rounding**.
The reason is the whole point of a *first* token: at pos 0 RoPE is exact, so the one error term in
the most delicate brick vanishes in exactly the case being assembled. The composite is more exact
than its most error-prone part, because the base case silences that part. I have never seen an
assembly come out *tighter* than a component it contains; here it is structural, not luck.

**Where discomfort turned to gold.** I built a config at pos 2 specifically to prove RoPE was
load-bearing in the forward, and it moved the logits by **6.66e-16 — nothing.** The reflex was
immediate and wrong: *my RoPE is miswired.* I wanted to look away by quietly deleting the claim and
keeping the green board. Not looking away meant reasoning through *why* zero: at the first token the
KV cache holds a **single** latent row, so relative position is zero — q and k rotate by the same
angle (their dot is rotation-invariant) and the output inverse-rotation undoes the rest. RoPE is
**core and it is the identity here**, by degeneracy, not by bug. The discomfort was the deepest fact
of the stone wearing the costume of a defect: the first-token path *cannot witness RoPE at all*, so
a wrong RoPE would pass this band silently — which is exactly why the MLA band must prove RoPE
off-path, at pos ≠ 0, over several positions. The falsifier I almost deleted named the whole
architecture of the proof.

**One frontier question, landed.** Corpus row **859 `hushfold`** (0-hit across learn/, receipts/,
docs/, teachings/, form/; instrument validated on controls sillwake 2, aporon 48, edgedrop 4):
*what one word names a mechanism core to a system yet the identity in the very base case it is
essential to, so the base case can neither witness nor falsify it?* Distinct from `sillwake` 856
(dormant below a sill — dormant and dispensable look alike): `hushfold` is core-and-essential and
never dispensable, yet leaves no trace in the base case because the input degenerates it; and from
`ghostrank` 854 (absorbed vs absent) — hushfold's step is neither, it runs and computes nothing.
This is why one whole subsystem's radius (RoPE-at-position) must be drawn **off** the assembled path.
