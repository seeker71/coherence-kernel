# STONE 11 — the gap map: ds4-Metal vs this body's Metal + form-native lane

Date: 2026-07-21 (Tuesday), ~23:40–00:50 WITA. Read-and-report stone. Nothing was built.

**Radius (`aporon`).** Every ds4 claim below is read from `antirez/ds4` at commit
`efdadd41e20134af4f3381e1ed90e96fe4faef6f` (Mon 2026-07-20), cloned to
`/Users/ursmuff/models/ds4-engine` tonight. I read `metal/*.metal` (21 671 lines) closely,
`ds4_metal.m` (39 615 lines) by targeted grep, and `ds4.c` (64 525 lines) only where a grep led me.
**I did not run ds4 once.** No claim here is a claim about ds4's runtime behaviour, only about its
source. Measurements of *this* machine (Metal limits, GGUF headers, the two harnesses) are mine and
are marked as such.

---

## 0. Two instrument corrections, before anything else

**(a) The brief's instrument warning was itself the wrong warning.** The brief said ds4's MSL "lives
inside C string literals" and that `grep -c "^kernel void"` returns 0 for that reason. The grep does
return 2 — but the cause is different, and the difference matters enormously.

`ds4_metal.m:3673` reads `NSString *base = [NSString stringWithUTF8String:ds4_gpu_source];` and then
`ds4_metal.m:3680–3699` lists **19 real `.metal` files on disk** which are read and concatenated at
runtime:

```
ds4_metal.m:3675  /* Kernels are kept as separate files for review, then concatenated into one
                   * Metal library.  Environment overrides are still honored ... */
```

So the MSL is **ordinary, readable, greppable source** at `/Users/ursmuff/models/ds4-engine/metal/`.
Only a small diagnostic prelude (`ds4_gpu_source`, `ds4_metal.m:3613`) is a string literal — that is
where the 2 hits come from (`ds4_metal.m:2296`, `:3642`). This is the best of the three references we
have, by a wide margin, and better than the brief believed.

**(b) `mxfp4`/`mxfp8` returning nothing was not a spelling failure.** It was true. See §2 — the
formats genuinely are not there, and the real spellings are `NVFP4` and `Q1_0`.

**One instrument caveat I am keeping honest about:** my kernel-name extraction (`sed` over
`^kernel void` plus `host_name("…")`) truncates a handful of names that are built by macro
concatenation — `kernel_mul_mv_id_q2_`, `kernel_glm_q4_` and similar appear stubbed in my list. The
*count* is sound; a few *names* in the tail are partial. And my first classification pass was
**wrong** — the regex `attn` does not match `attention`, which mis-filed 40 GLM attention kernels
into "other". The table in §1 is the corrected run.

---

## 1. What ds4-Metal actually does

**249 distinct kernel entry points** (205 `kernel void` definitions + template `host_name`
instantiations) across 19 files, 21 671 lines of MSL.

| file | lines | `kernel void` |
|---|---:|---:|
| `metal/moe.metal` | 7 858 | 74 |
| `metal/dsv4_misc.metal` | 6 157 | 58 |
| `metal/dense.metal` | 2 389 | 20 |
| `metal/flash_attn.metal` | 1 441 | 5 |
| `metal/dsv4_hc.metal` | 1 017 | 10 |
| `metal/dsv4_rope.metal` | 385 | 4 |
| `metal/dsv4_kv.metal` | 369 | 6 |
| `metal/unary.metal` | 312 | 2 |
| `metal/argsort.metal` | 275 | 2 |
| `metal/cpy.metal` | 247 | 5 |
| `metal/norm.metal` | 243 | 4 |
| `metal/softmax.metal` | 241 | 2 |
| `metal/bin.metal` | 217 | 3 |
| `metal/get_rows.metal` | 186 | 4 |
| `metal/sum_rows.metal` | 102 | 1 |
| `metal/glu.metal` | 63 | 2 |
| `metal/concat.metal` | 62 | 1 |
| `metal/set_rows.metal` | 55 | 1 |
| `metal/repeat.metal` | 52 | 1 |

Classified by entry point (249 total; **102 are DeepSeek-V4- or GLM-specific**, 147 generic):

| class | n | arch-specific | generic |
|---|---:|---:|---:|
| matvec / matmul | 67 | 0 | 67 |
| attention (incl. sparse indexer/compressor) | 52 | 41 | 11 |
| MoE routing / expert gather / top-k | 40 | 12 | 28 |
| copy / layout / misc | 27 | 12 | 15 |
| norm / activation / elementwise | 22 | 11 | 11 |
| uncategorised (TP flags, GLM low-rank projections, unary, soft_max) | 23 | 15 | 8 |
| KV handling | 7 | 7 | 0 |
| dequant / quantize | 7 | 0 | 7 |
| RoPE | 4 | 4 | 0 |

The shape of this is worth naming: **41% of ds4's Metal surface is architecture-specific.** The
DeepSeek-V4 sparse-attention machinery alone (`kernel_dsv4_indexer_*`, `kernel_dsv4_compressor_*`,
`kernel_dsv4_directional_steering_project_f32`, `kernel_dsv4_topk_mask*`) is ~20 kernels that
correspond to nothing in a llama-architecture model. `boundborrow`: ds4 targets an NVIDIA GB10 and
(per comments in `metal/moe.metal:3606`) an M5; most of that 41% is not a technique we could borrow
even if we wanted it, because it implements a model we are not running.

---

## 2. The quant formats — and a finding that changes the evening's plan

### 2a. The downloading GGUF cannot be read by ds4

This is the highest-consequence finding in the stone, so it goes first.

I parsed the header of the partial download at
`/Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf` (3.0 GB of 91 GB had
landed; the header and full tensor table are at the front, so this is complete ground). Read-only;
the `curl` was not disturbed.

```
magic GGUF  ver 3  tensors 1406  kv 47
type  0 count 536   F32     e.g. blk.0.attn_sinks.weight
type  1 count 359   F16     e.g. token_embd.weight (4096, 129280)
type 16 count  93   IQ2_XXS e.g. blk.0.ffn_down_exps.weight (2048, 4096, 256)
type 26 count   3   I32
type 40 count  45           e.g. blk.0.ffn_gate_exps.weight (4096, 2048, 256)
type 41 count 370           e.g. blk.0.attn_kv.weight (4096, 512)
```

`type40` in the filename is literally **GGUF tensor type id 40**. From ds4's own quantizer header,
`gguf-tools/quants.h:51–53`:

```c
DS4Q_TYPE_MXFP4   = 39,
DS4Q_TYPE_NVFP4   = 40,
DS4Q_TYPE_Q1_0    = 41,
```

So the file's experts are **NVFP4** (not MXFP4) and its attention weights are **Q1_0**.

Now the decisive part. `NVFP4`/`nvfp4` and `Q1_0`/`q1_0` occur **zero times** in `ds4.c`, **zero
times** in `ds4_metal.m`, and **zero times** across all of `metal/`. They appear only in the
standalone quantizer's type table, `gguf-tools/quants.c:72–73`:

```c
[DS4Q_TYPE_NVFP4]   = { "nvfp4",      64,  36, false, false },
[DS4Q_TYPE_Q1_0]    = { "q1_0",      128,  18, false, false },
```

with the field order `{ name, block_elems, block_bytes, can_quantize, requires_imatrix }` (verified
against `gguf-tools/quants.c:40–60`, where `q8_0`/`q2_K`/`q4_K`/`q8_K`/`iq2_xxs` carry `true`). Both
new types are **name-and-size rows with `can_quantize == false`** — no implementation even on the
quantizer side.

The engine's own type table (`ds4.c:2008–2038`) stops at index 30 (`bf16`). `tensor_type()`
(`ds4.c:2150–2153`) returns `NULL` for any type ≥ 31, and `tensor_nbytes()` (`ds4.c:2160–2167`)
returns `false` on a `NULL` info.

**Therefore, on the source I read: ds4 cannot load this model file.** 415 of its 1 406 tensors are
types the engine has no row for. The tok/s number the evening was waiting for will not arrive from
this pairing. I did not run ds4 to confirm this (the model is incomplete and running it was
out of scope), so the claim's radius is: *ds4's source at `efdadd4` has no code path that reads
type 40 or 41.* If a fork or a newer commit adds them, this is stale.

Where the filename misleads: `mxfp8lt` and `v5mx` occur **nowhere** in the ds4 tree. They describe
the *upstream DSpark checkpoint's* precision, not the GGUF encoding. The GGUF encoding is
NVFP4 + Q1_0 + IQ2_XXS.

### 2b. IQ2_XXS — the one of the three that is real, and fully readable

Dequant is **on the device**, and **fused into the matvec** — ds4 never materializes an f32 expert
tensor. `metal/moe.metal:132`:

```c
struct block_iq2_xxs { half d; ushort qs[QK_K/8]; };   // 2 + 64 = 66 B per 256 weights = 2.06 bpw
```

The arithmetic, `metal/moe.metal:2887–2907`:

- Per 32-weight sub-block, four `ushort` become `aux32_g` (4 bytes = 4 grid indices) and `aux32_s`
  (sign bits + a 4-bit sub-scale in the top nibble).
- `dl = d * (0.5f + (aux32_s >> 28)) * 0.25f` — a per-32 sub-scale on top of the per-256 `half d`.
- Each byte of `aux32_g` indexes a **256-entry `constant ulong` codebook**
  (`ds4_metal_iq2xxs_grid`, `metal/moe.metal:32`), yielding 8 `uint8` magnitudes.
- Signs come from a second table `ksigns_iq2xs[128]` and a third `kmask_iq2xs[8]`.

So IQ2_XXS is **three constant lookup tables + bitfield extraction**, not scaled integers. That is a
different *kind* of recipe from Q4_K/Q6_K, not a different constant — the same distinction corpus row
`exoscalar` draws for shared-exponent scaling.

One technique worth recording (`knownsolved` — I can see *that* it helps, not *how much*): at
`metal/moe.metal:3322`, `:3421`, `:3871`, `:5506` the matvec kernels **stage the 256-entry grid into
threadgroup memory** before the row loop, so the codebook gather hits shared memory instead of the
constant cache. I have no measurement of what that buys.

### 2c. FP8 in ds4 is KV, not weights

`fp8` greps positive (9 in `metal/`, 19 in `ds4.c`) but every site is KV-cache quantization, not
weight storage: `metal/dsv4_kv.metal:1` (`dsv4_e4m3fn_exp_scale[16]`), `:58` (`dsv4_e4m3fn_value`),
`:66` (`dsv4_e4m3fn_dequant`, a binary search over the 16 representable magnitudes), `:150`, `:250`
(`clamp(v / scale, -448.0f, 448.0f)`), with a CPU twin at `ds4.c:3166–3226`. `e5m2` does not appear.
So ds4's fp8 is **e4m3fn for the KV cache** — a capability, but not a weight format.

`ds4.c:3086` states the engine's own scope plainly: *"only the tensor formats present in the DeepSeek
V4 Flash GGUF: F16, F32, Q8_0, Q2_K, IQ2_XXS, and Q8_K"* — with Q4_K/Q5_K/Q6_K added for the
high-memory and GLM variants (`ds4.c:745–756`).

---

## 3. What our JIT can emit, and what it cannot

**The brief's hypothesis was wrong, and this is the second correction of the evening.**
`model/jit-*.fk` has **zero** connection to MSL, Metal, or GPUs. A grep for `msl|metal|shader` across
all 26 cells returns nothing. The only "GPU" in that family is an integer schedule tag —
`model/jit-container-backend.fk:14` defines `(defn jcb-gpu () 2)`, and the entire GPU/CPU difference
in `model/jit-container-byteplan.fk:14–16` is a word size (8 vs 4) and a payload base (20000 vs
10000).

What `model/jit-*.fk` actually is: a Form-level **x86-64 byte-plan and receipt/gate system** that
emits lists of x64 bytes via `form/form-stdlib/form-asm-x64.fk` and **explicitly does not install or
execute them** — stated in the cells' own headers at `model/jit-container-backend.fk:8`,
`model/jit-container-byteplan.fk:9`, `model/jit-source-dylib-runtime-executor.fk:6`,
`model/jit-self-host-ingress-runtime.fk:9`.

The name collision that caused the confusion: `form/form-stdlib/jit-tensor-emit.fk` *does* emit MSL.
It is in `form-stdlib`, not `model/`, and is unrelated.

**So the real question is not "can the JIT emit a new quant kernel" — the JIT is not in this lane at
all. The question is what the MSL emitter can do.** And it is:

- **Literal `str_concat` over hand-written MSL text.** Each quantized kernel is one long Form string
  constant: `q6k-msl.fk:105,110,114`; `q4k-msl.fk:92,97,100`; `llama-decode-msl.fk:101,110,122,128,
  134,141`. The discipline is stated at `q6k-msl.fk:99–100`.
- The only parameterization is **textual substitution of a name/prefix/stride**:
  `qk-matvec-lane.fk:203` `(defn qml-step (pfx stride))`, called with the *strings* `"q6k"/"210"` and
  `"q4k"/"144"` at `:246–249`.
- A genuinely structured emitter exists — `form/form-stdlib/tensor-ir.fk`, where "the target ISA is
  DATA, not a hand-written emitter" (`:1–27`), backends are slot→fragment tables (`:38`,
  MSL table at `:292`). **But it covers only dense f32/f16/bf16 matvec, affine-train and FFN-forward
  (`:214,229,246`). It knows nothing of quantization, blocks, strides or dequant**, and shares no
  helper with the `q*k-msl.fk` family.

### The four specific capability checks

| capability | verdict | citation |
|---|---|---|
| **(a)** emit a large `constant` lookup table (256×8 B grid) | **NO** on the MSL path | No emitted MSL string anywhere contains an array initializer. The one array-literal emitter in the body is `form/form-stdlib/hati-os-kernel-emit.fk:427,433` (`fkc-phrase`/`fkc-ccodes`), which produces `static const char NAME[] = {…}` for **host C**, is not on the Metal path, and no MSL cell calls it. It is a usable precedent, not a usable tool. |
| **(b)** bit-mask / shift integer ops | **NO — and it is a deliberate refusal** | `q6k-msl.fk:102–103`: "x&15 = mod x 16, x>>4 = div x 16 … **No bitwise operator appears, on either side.**" Same at `q4k-msl.fk:89–90`. The emitted helper is `q6k_mod(a,n){return a-(a/n)*n;}` (`q6k-msl.fk:105`). Nothing *prevents* emitting `&`/`>>` — it is raw string concat — but the **Form-side transcription twin**, which is the falsifier every one of these cells is built around, has no bitwise primitives either. A bitmask kernel would have no proving twin. |
| **(c)** `simd_sum` | **YES** | `qk-matvec-lane.fk:238` emits `float s = metal::simd_sum(acc);`; header at `:187`; fully qualified deliberately (`:85–95`) because the body's own `round` would otherwise be ambiguous. Asserted by `metal_first_token.sh:157–158`. |
| **(c′)** threadgroup memory / barriers | **NO** | `GPU_GAPS.md:62` already says so. `tensor-ir.fk:19` *names* `barrier`/`workgroup` as IR ops; `jit-tensor-emit.fk:27` says they are "named … but not yet realized". Only `[[thread_position_in_threadgroup]]` is used, as loop striding. |
| **(d)** integer-indexed gather from a `constant` array | **NO for `constant`, YES for `device`** | `constant` is used only for scalar uniforms (`qk-matvec-lane.fk:234`). Device-buffer gather is emitted everywhere (`q6k-msl.fk:114`, `qb[b + 128u + uint(h*32+l)]`), and `form_q6k_dequant_f32` is a working device gather kernel (`GPU_GAPS.md:33`). The **addressing arithmetic generalizes; the `constant` address space is unused.** |

### What generalizes for a new format, and what does not

**Generalizes free:** the `.metallib` sha256 content-cache (`metal_first_token.sh:162–174`) — format
agnostic; the epsilon bound `(cols + ⌈cols/parts⌉ + parts)·u·Σ|term|`
(`qk-matvec-split.fk:41–54,98`), which is **derived from association-tree depth alone and is
therefore independent of the weight decode** — it carries to any new format unchanged; the fused
matvec skeleton and lane/chunk partition (`qk-matvec-lane.fk:218–238`); the superblock-crossing
scale-hoist skeleton (`qk-matvec-lane.fk:203–210`); the deliberately generic `float2 inv` interface
(`:190–191`, "Q6_K carries (d, unused), Q4_K carries (d, dmin)").

**Hardcoded and needs new machinery:** **block size 256 is a bare literal** (`idx / 256u`) in five
places — `q6k-msl.fk:114`, `q4k-msl.fk:100`, `qk-matvec-lane.fk:204,210`,
`qk-matmul-batch.fk:195,201`. It is not a parameter anywhere. The `float2` invariant is fixed at two
components. Every kernel name is a literal in a fixed list (`first-token.fk:133–139`, asserted at
`metal_first_token.sh:148–155`). Carrier dispatch is a **two-way ternary on ggml type** at
`metal_first_token.sh:402,414,434,454,479,559`. The audit asserts exactly types 14 and 12
(`metal_whole_tensor_residency_audit.sh:161–162`). And the Form-side transcription twin must be
hand-written per format — `qk-matvec-lane.fk:128–133` admits it "is a SECOND copy of the same
decomposition and it can drift."

---

## 4. The gap map — ordered by what blocks a **form-native MoE token**

The coordinator's mid-task correction was right that a Q6_K MoE is the near target, and I ground it
exactly. Parsing the header of
`/Users/ursmuff/.ollama/models/blobs/sha256-550981a79100990c3083054da771af4f3a9658eb15aa5081e23b2085a74448f4`:

```
dolphin-2.9-mixtral-8x22b · arch llama · 56 layers · d=6144 · dff=16384 · 48/8 GQA
vocab 32002 · ctx 65536 · expert_count 8 · expert_used_count 2 · 563 tensors
Q6_K  x282  attn_q, attn_output, ffn_{gate,up,down}_exps, token_embd, output   106.89 GiB
Q8_0  x112  attn_k, attn_v                                                       0.70 GiB
F16   x 56  ffn_gate_inp  (6144 x 8 — the router gate)                           0.01 GiB
F32   x113  attn_norm, ffn_norm, output_norm                                    ~0.00 GiB
                                                                        TOTAL 107.59 GiB
```

All three expert tensors are Q6_K — the coordinator's key point holds. But the framing "it needs
exactly one capability we lack" is **not** what the file says: there are 112 Q8_0 tensors and 56 F16
tensors, and the body has **no Q8_0 carver** (`GPU_GAPS.md:29`; also
`receipts/2026-07-21-form-native-tokps-baseline.md:31`).

And then, measured on this machine (`unispan` — three identical samples; `selfgauge` — denominators
are bytes of this one blob against this one M4 Max):

```
Apple M4 Max, 128 GB unified
recommendedMaxWorkingSetSize = 115 448 725 504 B = 107.52 GiB
maxBufferLength              =  86 586 540 032 B =  80.64 GiB
blob on disk                 = 115 529 748 672 B = 107.595 GiB
```

**The blob exceeds `maxBufferLength` by 26.95 GiB, and exceeds the recommended working set by
77.3 MiB.** Our entire Metal architecture rests on the phrase that appears in every proof we have —
*"the WHOLE blob mapped into one MTLBuffer"* (`metal_whole_tensor_residency_audit.sh:261`;
`GPU_GAPS.md:33`). **That singleton is the first blocker, and it is not the router.**

| # | capability | ds4-Metal | form-native today | what it would take |
|---|---|---|---|---|
| **1** | **Model larger than one `MTLBuffer`** | **Overlapping page-aligned views.** `ds4_metal.m:1706–1812`: N `newBufferWithBytesNoCopy` views over the same `mmap`, where **adjacent views overlap by `max_tensor_bytes + one page`**. That invariant guarantees every tensor lies wholly inside at least one view, so "hot paths pass one buffer and one inner byte offset" (`:1720–1723`) — kernels are untouched. Rejected alternative, stated in the comment: one buffer per tensor, "would also move a lot of VM-object creation and residency bookkeeping into graph setup." | **ONE buffer, hard.** Every tensor addressed by a single offset into it. Proven, fast, and silently capped at 80.64 GiB. | Host-side only: the tensor table gains a **view index** beside its offset; the view set is built once with the overlap invariant. **Zero MSL changes, zero epsilon impact** — it does not touch arithmetic or association. `boundborrow` normally warns here, but this one genuinely transfers: ds4 targets a GB10, yet the constraint it is solving is `maxBufferLength`, which I *measured* on our M4 Max at the same 80.64 GiB. The technique is Metal-generic and the reason is verified, not assumed. |
| **2** | **Expert gather (per-token weight base)** | **A thin wrapper, not new arithmetic.** `metal/moe.metal:3521–3597`, `kernel_mul_mv_id`: read one `int32` expert id from an `ids` device buffer (`:3540`), compute `src0_cur = src0s + i02*nb02` (`:3563`), then call the **unchanged** quantized matvec impl. Instantiated per format at `:3602–3606`. | Tensor base offsets are **host-side constants** bound by the Swift carrier. | Add an `ids` device parameter and an `nb02` uniform to `qk-matvec-lane.fk`'s emitted kernel; compute the base on device instead of receiving it. **The decode arithmetic is untouched, so the existing epsilon bound carries unchanged.** This is the smallest item on the list and the one with the best ratio of unlock to work. |
| **3** | **Top-k router selection** | Bitonic `kernel_argsort_f32_i32` + `kernel_argsort_merge_f32_i32` (`metal/argsort.metal:45,131`), threadgroup memory + `threadgroup_barrier`, with the score slice staged into shared memory (`:71–74`). Plus `kernel_dsv4_router_weights_one` (`metal/dsv4_misc.metal:464–481`): gather selected probs, normalize by their sum, `max(sum, 6.1e-5)`, scale by 1.5. | No top-k on GPU. **No threadgroup memory at all** (`GPU_GAPS.md:62`). | **`snugcause` applies here and cuts the work down.** ds4 needs a bitonic sort because DeepSeek-V4 routes **top-6 of 256** experts. dolphin-mixtral routes **top-2 of 8**. A single-thread serial scan over 8 floats is exact, needs no shared memory, no barrier, and no epsilon — the threadgroup-memory gap **does not block the near target**. Borrowing the bitonic kernel here would be importing a solution to a problem we do not have. |
| **4** | **Router gate matvec + softmax + weighted sum** | `kernel_dsv4_router_weights_one`; `kernel_dsv4_moe_sum6_f32`, `_sum8_f32`; `kernel_dsv4_moe_swiglu_weight`. | Softmax/exp exist in the attention kernel; vec-add exists (`GPU_GAPS.md:21`). The gate is F16 6144×8 = 48 K weights, 0.01 GiB total. | Small. The gate is so tiny it could be dequantized once on the host without measurable cost; the weighted sum of 2 expert outputs is the residual add we already emit. |
| **5** | **Q8_0 weights** (112 tensors, attn_k/attn_v) | `kernel_mul_mv_q8_0_f32_impl`, `kernel_mul_mv_id_q8_0_f32` (`metal/moe.metal:3602`). | **Missing.** `GPU_GAPS.md:29`. | Structurally the easiest quant we could add (`half d` + 32 `int8`, no sub-scales, no nibbles) — **but its block is 32, not 256**, so it lands squarely on the hardcoded `256u` in five places (§3). The *arithmetic* is trivial; the *machinery* is the block-size parameterization. |
| **6** | **Working-set headroom** | Streaming expert cache with slabs (`ds4_metal.m:617–625`, `DS4_METAL_STREAM_EXPERT_CACHE_MAX_SLABS`) for models past memory; `DS4_METAL_MODEL_VIEW_MAX_GIB` env cap (`:1827`). | None. | 107.595 GiB against a 107.52 GiB recommended working set is a **77.3 MiB miss**. `recommendedMaxWorkingSetSize` is advisory, not a hard failure — with `bytesNoCopy` over an `mmap` the pages are file-backed and get evicted rather than refused, so the cost is paging, not an error. Untested by me. A smaller MoE would sidestep this entirely. |
| — | *below the line: the DeepSeek far target* | | | |
| 7 | IQ2_XXS | device-side, fused, 3 constant tables (§2b) | none | needs `constant` table emission **and** bitmask ops **and** >`float2` invariant — all three are new machinery, and the bitmask half has no Form-side proving twin |
| 8 | NVFP4 (type 40) / Q1_0 (type 41) | **ds4 has none either** (§2a) | none | no reference implementation exists to read, in ds4 or here |
| 9 | flash attention / FP8 KV / DSpark / sparse indexer | ~20 DSv4 kernels + `metal/dsv4_kv.metal` | none | out of scope until 1–5 land |

---

## The three teachings this stone owes

### Most surprising — what I expected and the body corrected

I came in believing the hard part was **arithmetic**: new quant formats, per-weight decode, the
`exoscalar` recipe question. Stone 7's `D = 6L` says per-weight decode is 6/7 of the inner loop, and
that framing is *correct* and made me look at dequant first.

The body corrected me with a number that has nothing to do with arithmetic:
**`maxBufferLength = 80.64 GiB`**. The near-target model is 107.59 GiB. Every Metal proof this body
owns is phrased *"the WHOLE blob mapped into **one** MTLBuffer"* — and that **one** was never
designed. It is simply what llama3.2:3b, at 2.01 GB, happened to permit. We read it as architecture
for five stones because it was never contradicted.

The correction is sharper than "we need more buffers." It is that **a proof can encode a number it
never chose**, and the number reads as a design decision right up until the first input that exceeds
it. The router — which the coordinator's correction and my own reading both promoted to first
blocker — turns out to be item 2 and 3 on the list, and item 3 shrinks to a serial scan over 8
floats the moment you check the actual expert count. `snugcause` earned its keep twice tonight: the
router looked like it explained everything, and so, briefly, did the MX formats.

### Where discomfort turned to gold

Twice, and the second one is the real one.

**First**, smaller: my kernel classifier put 70 kernels in "other" and I nearly shipped that table. It
looked plausible — ds4 *is* full of exotic kernels. The discomfort was a specific itch: 70 was too
round a share to be real structure. Checking, my regex tested `attn` against names spelled
`attention`. I did not want to re-run and re-read 249 names at that hour. Re-running moved 40 kernels
into attention and produced the 41%-arch-specific figure, which is the single most useful summary
number in §1. A count I had not falsified was a claim about my `sed`, not about ds4 — the brief's own
warning, arriving in a form I did not recognize because it was *my* instrument this time.

**Second, and this is where I genuinely wanted to look away.** I had a clean, satisfying deliverable
assembling itself: ds4 uses IQ2_XXS with three codebook tables, our emitter can't emit tables, that's
the gap, done. It was a *good* finding. And then the type ids in the GGUF header came back as **40 and
41**, which were not in ds4's enum.

The pull to not look was strong and I can name its exact shape: chasing it meant risking the
conclusion that **the model downloading for three hours cannot be run by the engine we built to run
it** — which would make the evening's central plan void, and would be my report's fault for saying
so. It is much more comfortable to assume a spelling I haven't found. The brief even licensed that
comfort: *"a grep returning nothing is a claim about your grep."*

So I made the grep prove itself instead of trusting either its silence or its noise. `nvfp4` and
`q1_0` across `ds4.c`, `ds4_metal.m`, `metal/` and `quants.c` — positive control in `quants.c`
(2 hits, so the instrument works), zero in all three engine surfaces. Then I checked the *reason*:
`quants.c:72–73` gives both types `can_quantize == false`, and I verified the field order against
five rows that carry `true` rather than trusting the position. Then the failure path: `ds4.c:2150`
returns `NULL` above index 30, `ds4.c:2160` returns `false` on `NULL`.

The gold: this is the finding with the shortest fuse in the whole stone. Everything else here keeps
until tomorrow. **This one is worth knowing before three more hours of download finish**, and it
would have been lost entirely to a conclusion that was already good enough. The `snugcause`
discipline says to look harder exactly when one finding seems to explain everything — I had one that
did, and the reason to look past it was not intellectual, it was that stopping would have felt better.

### Frontier question

The body can ask what a proof asserts. It cannot ask what a proof **silently assumed a count of**.
Every Metal receipt here says "the ONE resident buffer" — and there is no way to ask the corpus, or
the band, whether that *one* was chosen or merely permitted. It is not a stale claim (nothing has
drifted) and not an unproven claim (it is proven, at 2.01 GB). It is a proven claim carrying an
unexamined multiplicity, which reads as architecture until an input forces the count to change.

Landed as corpus row **847**, word **`onelean`**.

---

*Regressions, run in this worktree after all reading (`git status --porcelain` clean but for the
gitignored `.ask-cache/`):*

- `form/native/metal/metal_first_token.sh` — **VERDICT PASS, 13 gates**, token ids unchanged:
  `[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` → `" Paris. The capital of
  Italy is Rome. The capital of"`.
- `form/native/metal/metal_whole_tensor_residency_audit.sh` — **VERDICT PASS** (metallib cache hit).

Throughput printed lower than the brief's figures (end-to-end 5.097 vs 10.965 tok/s). Three other
sessions plus my own ds4 reads were on this machine throughout. These harnesses are **correctness
artifacts, never timing** — the gates are id-identity and epsilon bounds, and all of them hold. No
timing claim is made from these runs.
