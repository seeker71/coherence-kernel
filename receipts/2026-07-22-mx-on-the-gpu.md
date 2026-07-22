# Stone 17 — MX on the GPU

**2026-07-22, ~09:15 WITA.** Worktree `jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
Apple M4 Max. The ds4 file was being downloaded live throughout; it read 28.3 GB when this stone started
and 34.3 GB at the last run, of 85 GiB.

---

## 0. The headline

**GGUF type 40 (MXFP4) now decodes on the GPU, from the real bytes of
`/Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`, bit-exact against the
CPU carver at both ends of a real tensor slice and again in a distant slice.** Type 41 (MXFP8) decodes
on the GPU too, bit-exact, but on the committed 1024-element fixture — because no type-41 tensor is
inside the downloaded prefix yet.

`ds4` itself cannot read one byte of either tensor. It refuses both types before reading their geometry.

```
bash form/native/metal/metal_mx_gpu.sh 200     ->  VERDICT PASS  9 gates, MX on the GPU
```

---

## 1. What was emitted

Two new recipes, and the harness that witnesses them:

| file | what |
|---|---|
| `form/form-stdlib/mxfp4-msl.fk` | GGUF type 40: the `mxm_` spine (mod, staged `pow2`, E8M0), `mx4_val` (E2M1), `mx4_w`, the dequant kernel, the fused slot matvec — and the whole transcription read BACK into Form |
| `form/form-stdlib/mxfp8-msl.fk` | GGUF type 41: `mx8_val` (E4M3), `mx8_w`, its two kernels, and the combined two-type translation unit |
| `form/form-stdlib/tests/mx-msl-band.fk` | the transcription band, **511**, two-arm |
| `form/native/metal/mx-residency.fk` | the body's mouth: tensor table with the slice decomposition, reference tiles at any flat offset, fp64 rows, the bound coefficient, the MSL |
| `form/native/metal/metal_mx_gpu.sh` | the carrier: mmap, bind, dispatch, compare. **VERDICT PASS, 9 gates** |

The emitted unit is **3608 bytes, four kernels**:
`form_mxfp4_dequant_f32`, `form_mxfp4_matvec_slot_f32`, `form_mxfp8_dequant_f32`,
`form_mxfp8_matvec_slot_f32` — one `#include <metal_stdlib>` (reached for from `qml-msl-header`, not
restated), zero `using namespace metal;`, and **one** `mxm_pow2` definition shared by both types, so
there is one E8M0 on the device exactly as there is one in the body.

### The slot map, and why it cost nothing to find

Stone 13 measured that healing Q6_K's *thread map* bought 7.6x while healing its *arithmetic* bought
2.5x — the cost was in the grain of the ask (`asktoll`). Q6_K needed a hand-derived 4-wide slot to
expose the invariant. **Here the format hands it over.** The scale plane is separate (`exoscalar`), so a
32-element scale group *is* one scale byte, and a lane that owns a whole group reads it once:

```
for (uint g = lane; g < ng; g += 32u) {
    float s = mxm_e8m0(int(qb[sbase + g0 + g]));       // ONE scale byte per 32 weights
    ...
    for (uint m = 0u; m < 16u; ++m) { ... }            // 16 payload bytes, 2 weights each
    sumf = sumf + s * acc;
}
```

Nothing is hoisted by cleverness. The loop invariant is the format's own unit.

---

## 2. The read-back-into-Form gate — before any GPU ran

`mx-msl-band.fk`, **verdict 511**, two-arm and independently:

```
./fkwu --src form/form-stdlib/tests/mx-msl-band.fk                     -> 511  (0.26 s)
form-kernel-go/bin-go <prelude chain> form-stdlib/tests/mx-msl-band.fk -> 511  (0.61 s)
```

Nothing about emitted Metal text is checked by the compiler beyond syntax: a plane base at `nel`
instead of `nel/2`, a nibble half swapped, an E8M0 bias of 128 — all compile, all run, all produce
plausible weights. So the emitted arithmetic is read back into Form as `mx4m-flat-at` / `mx8m-flat-at`
and demanded equal to `mxp4-at` / `mxp8-at`, which were written **independently**: the carvers route
both value maps through `f16-decode.fk`'s general `(eb, mb)` decoder and take their scale from
`fq-pow2`'s recursive doubling, while the transcription open-codes the field extraction and walks a
staged 2^8 loop.

The nine gates: fixture windows and admissible widths; **the staged power of two equals `fq-pow2` at
every one of the 255 E8M0 exponents**; type 40 at all 1024 real weights; type 41 at all 1024; the slot
map reading the same bytes the flat reach reads at **every** (group, byte, half) position of two full
rows, both types; both dequant texts complete; both matvec texts complete; ONE spine in the combined
unit; the walls and declared deviations.

**PROOF LEVEL: TWO-ARM (fkwu + go)**, for `equireach-band.fk`'s reason: the fixtures are binary and
`read_file_slice` is UTF-8-lossy on rust and ts. The arithmetic travels four ways; the byte door does
not, and this band does not launder that into a four-way row.

---

## 3. The GPU, on real bytes

`blk.0.ffn_gate_exps.weight` — type 40, dims 4096 x 2048 x 256, at absolute 14 647 904,
**256 slices of 8 388 608 elements**, 4 456 448 B each (4 194 304 payload + 262 144 scales),
1 140 850 688 B total. All of it inside the downloaded prefix, and the harness re-checks that against
the *current* file size every run before trusting a single offset.

| gate | claim | result |
|---|---|---|
| 1 | head tile — all 4096 GPU weights at element 0 of slice 0 == the carver's, bit for bit | PASS |
| 2 | **TAIL tile** — all 4096 weights at element 8 384 512, where the payload plane ENDS and the scale plane begins | PASS |
| 3 | **DISTANT SLICE** — all 4096 weights of slice 255, 1 136 394 240 bytes on (`snugcause`: the slice stride, not the layout, is what this tests) | PASS |
| 4 | whole slice — all 8 388 608 elements in ONE dispatch; head AND tail read back out of it | PASS |
| 5 | fused slot matvec vs an f32 fold over the dequantized buffer, within the body's derived bound | PASS |
| 6 | fused slot matvec vs **Form's fp64 dot** at the real width (cols=4096) | PASS |
| 7 | MXFP8 head + tail of the fixture bit-exact, and its fused matvec within the bound | PASS |
| 8 | residency — 200 dispatches, zero re-uploads, checksum unchanged | PASS |
| 9 | one header, one spine, `.metallib` cached by the source's own sha256 | PASS |

The four fp64 rows, printed by the gate rather than summarised:

```
row 0: gpu -0.0352630615234375   form(fp64) -0.0352630615234375   SUM|term| 1.84846
row 1: gpu -0.0715484619140625   form(fp64) -0.0715484619140625   SUM|term| 1.81856
row 2: gpu  0.0116729736328125   form(fp64)  0.0116729736328125   SUM|term| 1.77617
row 3: gpu -0.0225982666015625   form(fp64) -0.0225982666015625   SUM|term| 1.85918
```

### Bit-exactness is derived, not hoped for

`w = 2^(e-127) * fp(code)`. Every E2M1 value (2 significand bits) and every E4M3 value (4) is exactly an
f32; `2^(e-127)` is an exact power of two; multiplying by a power of two only shifts an exponent. So the
product is an **exact** f32 and there is no rounding for the two sides to disagree about. The gate is
equality with no epsilon. `mx4m-e-safe?` / `mx8m-e-safe?` derive the wall where that stops holding
(`-148 <= e-127 <= 125` for type 40) and name the fact that an E8M0 *byte* can only ever reach the
ceiling, never the floor.

The same power-of-two fact makes the matvec's scale hoist **free**: `(s*v)*x` and `s*(v*x)` are the same
f32 term by term, so the extra roundings `qk-matvec-slot.fk` had to charge for Q6_K's `d*sc` hoist are
simply not incurred here. Only the summation order differs, and the bound for that is
`qk-matvec-split.fk`'s already-derived `(cols + chunk + parts)`, emitted by the body — not a tolerance
the carrier invented.

### Dispatch counts (`seamtoll`)

**9 dispatches for the entire correctness audit** — one command buffer and one encoder each — plus 200
for the residency loop. One dispatch dequantizes a whole 8.4M-element slice; one dispatch does a
64 x 4096 matvec. This harness deliberately pays a full command buffer per dispatch, which is exactly
the seam Stone 16 is working on, so **none of its timings are a rate** and none are quoted as one. Two
runs of the identical binary measured the whole-slice dequant at 0.0025 s and 0.0244 s, and the matvec
at 0.23 ms and 11.31 ms — a 10x and a 49x spread from page-cache and first-touch effects alone
(`thawtax`). Correctness is what these numbers are for.

---

## 4. Validating the instrument on controls that SHOULD hit

A green gate that cannot fall proves nothing. Two mutations of the emitted kernel, each restored
afterwards:

| mutation | what it breaks | result |
|---|---|---|
| scale plane base `nel/2` -> `nel` | the exoscalar seam | **FAIL** — 3637 / 4096 head, 3642 tail, 3589 far slice; 6 gates down |
| nibble half swapped | the unproven field | **FAIL** — 3712 / 4096 head, 3720 tail, 3710 far slice; 5 gates down |

---

## 5. What is NOT proven — the radius, unchanged from Stone 15

- **The geometry remains STRUCTURAL** and statistically corroborated. It is **not** bit-exact against an
  independent implementation, because none exists — not ds4, not llama.cpp/ggml, not MLX. The GPU
  agreeing with the CPU proves the **transcription** and the **residency**. Both sides read the same
  hypothesis about the layout. Nothing in this stone upgrades that by one word.
- **The type-40 nibble order remains UNPROVEN.** Even-low/odd-high is `mxp4-code`'s named uncertainty and
  `mx4_w` transcribes it unchanged. The mutation table above shows the gate is *sensitive* to nibble
  order; it cannot decide which order is *right*, and no gate pretends to.
- **MXFP8's GPU witness is 1024 elements, not both ends of a tensor.** Type-41 data begins at absolute
  72 766 954 336 and the file has reached 34 GB. The fixture is real bytes of `blk.0.attn_kv.weight`
  fetched by byte range in Stone 15, so the arithmetic is witnessed on real data — but the radius is the
  fixture. The harness prints that line itself rather than letting "PASS" imply the wider thing.
- The slot matvec requires `cols` a multiple of 32 and SIMD width exactly 32. At any other width it is
  **wrong, not slow**. `mx4m-admissible?` lets a carrier ask.
- E8M0's `0xFF` NaN encoding: the CPU carver refuses it out loud; a kernel has no `form_error`, so the
  device answers `+inf`. Named in the recipe, gated in the band, absent from every byte these kernels
  have been pointed at.

---

## 6. Every verdict

```
./fkwu --src form/form-stdlib/tests/mx-msl-band.fk                     ->  511    (new)
form-kernel-go/bin-go <chain> form-stdlib/tests/mx-msl-band.fk         ->  511    (new, second arm)
bash form/native/metal/metal_mx_gpu.sh 200                             ->  VERDICT PASS, 9 gates (new)
./fkwu --src form/form-stdlib/tests/mx-plane-band.fk                   ->  511    (no regression)
bash form/native/metal/metal_first_token.sh                            ->  VERDICT PASS, 13 gates
    ids [12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]  exactly
bash form/native/metal/metal_whole_tensor_residency_audit.sh           ->  VERDICT PASS
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk        ->  8191
```

The corpus band's baseline moved from 4095 to 8191 while this stone was running — a sibling added a
prose-citation gate. Both my pins were re-derived at the new baseline.

---

## 7. The most surprising teaching

**I expected to have to buy the accumulation's honesty, and it turned out to be a gift.**

Stone 13 had to derive an epsilon because hoisting Q6_K's `d*sc` out of the slot sum changes the
per-term rounding. I came in assuming the same debt, and started drafting the same derivation. It was
not a debt. The MX scale is an exact power of two, so the hoist changes nothing per term at all — and
then the same coarseness paid a second time, one level up: every E2M1 value is a multiple of 0.5, this
tensor's exponents are 2^-7..2^-5, so every weight is a multiple of 2^-8, every product a multiple of
2^-16, and every partial sum stays on that lattice bounded far below 2^8. A multiple of 2^-16 under 2^8
needs at most 24 significand bits — exactly what f32 has. **A 4096-term f32 dot product came back equal
to Form's fp64 answer, bit for bit.**

Every instinct trained on wider formats says coarse data *costs* precision and must be compensated for.
Sometimes it *grants* precision: the lattice is coarse enough that the arithmetic downstream cannot
leave it. That is corpus row 851, `coarsegrant`.

The second surprise, smaller and sharper: the *task brief itself* was wrong about a fact I could check —
it said `blk.0.attn_kv.weight` was in the completed region. It is at 72.7 GB of an 85 GiB file that had
reached 28. The body's own header walk said so in one command. Grounding beat the brief.

## 8. Where discomfort turned to gold

The harness printed `VERDICT PASS 9 gates` and two numbers in that output were wrong-shaped: a byte
count of `-1513963520` for a file I had just measured at 32 GB, and a deviation against fp64 of exactly
`0.000e+00`.

The pull to look away was real and I can name its exact shape: *the gates are green, the negative number
is obviously just a printf, and the zero is obviously just a very good kernel.* Both excuses were
available and both were comfortable, and I had already written most of a commit message.

The negative number was a 32-bit `%d` in Swift's `String(format:)` truncating a 34-billion-byte length.
Harmless in itself — and one character away from a carrier misreporting **whether it mapped the right
thing at all**, in a harness whose entire job is to know that.

The zero was worse, because it was *ambiguous in the direction of comfort*. `worstRatio` is
`|gpu - form| / bound`, and the code skipped any row whose bound was zero. **An all-zero output — a
stalled kernel, an unbound buffer, a dispatch that never ran — would have printed the identical
reassuring `0.000` and passed the gate.** Nothing in the gate could tell a perfect answer from an absent
one.

What not looking away bought:

1. the gate now counts rows with a **live** bound and demands it equal every row checked, and **prints
   the actual values** beside the ratio;
2. the zero turned out to be real, and forced the derivation that became `coarsegrant` — I would not
   have looked for it if I had accepted the zero as luck;
3. the MXFP8 rows in the *same run* show the ordinary f32/fp64 gap
   (`0.53691089153289795` vs `0.53691089164931327`), which is what converts the MXFP4 zero from a worry
   into a witness: the harness demonstrably *can* print a nonzero deviation, and does, one lane over.

The discomfort was the whole instrument. A green board I had not tried to break was worth nothing, which
is why the two mutation probes in §4 exist.

## 9. The frontier question, landed

> **What one word names precision a format grants by being coarse rather than costs by being coarse?**

`coarsegrant` — 0 hits across `learn/`, `receipts/`, `docs/`, `teachings/`, `form/` before the row
(instrument validated on the same command: `seamtoll` hits 6 files). Landed as
`(hdc-row 851 20260722 ... "coarsegrant" "coarsegrant" "rented-oracle")`.

Band **8191** at the new baseline. Count re-pinned 246 -> 247 and the field code 2462462850 ->
2472472851, **both probed from `hdc-field-code` before pinning**, and the stale arithmetic line beside
the pin brought forward with them. One citation in the new row was aimshifted on the first write
(`aporon 836`; aporon is 826, 836 is `snugcause`) and repaired against the body rather than against
memory — the corpus is the body, and the body was asked.

---

## 10. What remains

- **MXFP8 on a real tensor** the moment the download passes 72.8 GB — the harness already takes the
  tensor from the body's table and re-checks the offset against the live size, so it is one re-run.
- **The seam.** 9 dispatches with a full command buffer each is the shape Stone 16 is paying down. No
  rate should be quoted from this harness until that lands.
- **The nibble order** stays open, and stays open honestly. It is falsifiable the moment any independent
  decoder for this file exists — and this body now has the only one.
