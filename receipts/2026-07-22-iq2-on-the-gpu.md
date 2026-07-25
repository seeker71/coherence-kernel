# STONE 30 — IQ2_XXS on the GPU: the last CPU-only dequant type, decoded on the device

**2026-07-22, ~11:42–12:20 WITA.** Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Five commits, incremental:
`iq2xxs-msl.fk` (the emitter), `tests/iq2xxs-msl-band.fk` (the transcription band),
`native/metal/iq2-residency.fk` (the mouth), `native/metal/metal_iq2_gpu.sh` (the GPU
witness), corpus row 869 `coemit`. IQ2_XXS is 42.6% of the file's bytes and the expert
weights of 31 of 43 layers; it was the last dequant type still CPU-only, and DeepSeek's
`ffn_down_exps` are exactly type 16 — the MoE fold needs them on device.

---

## 0. Radius (`aporon`), before anything is believed

- **GPU dequant only.** A body-emitted Metal kernel turns 66 IQ2_XXS bytes into 256 f32
  weights **on the device**, bit-exact against the Stone 24 CPU carver at the head block
  AND two distant blocks of a real tensor. A **fused matvec-over-IQ2** is NOT built (§7).
- The GEOMETRY (66 B / 256 el) is `gguf-manifest.fk`'s `gm-blk n 256 66`, proven from this
  file's own offset chain. A GPU agreeing with the CPU carver proves the **transcription**
  and the **residency**. But unlike MXFP4/MXFP8, the IQ2 carver already carries
  bit-exactness against an **independent from-spec oracle** (`iq2xxs-dequant-band` = 2^30−1
  over 61 952 real weights, Stone 24), so this witness inherits that grade: the device now
  equals a reading that equals the spec.
- Measured against **one file**, read-only, now **100% downloaded** at 91 321 404 640 B:
  `/Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`.

---

## 1. The one new thing: a `constant` array literal IS emittable

`GPU_GAPS`/Stone 21 flagged emitting a `constant` array literal as historically untried —
so it was the declared first sub-task. It is **not missing**. MSL emission in this body is
pure `str_concat` over `int_to_str`, and `iq2m-join` (a head-wise O(n) fold) turns a Form
list into `a,b,c`. The emitter produces, verbatim:

```
constant uchar iq2_grid[2048] = { 8,8,8,8,8,8,8,8,43,8,... };
constant uchar iq2_ksigns[128] = { 0,129,130,3,132,5,6,135,... };
```

built from the **same** `(iq2-grid)` / `(iq2-ksigns)` lists the CPU carver decodes. Verified:
2048 / 128 entries, grid distinct set exactly `{8,25,43}`, both **byte-identical** to the
carver's lists. The whole emitted unit is 7328 bytes and compiles offline to a **7021-byte
metallib** — and it needs **no `#include <metal_stdlib>` at all** (it has no library call;
the f16 super-scale `d` is decoded by arithmetic, `iq2_f16` = `fd-value` at (5,10)). This is
the strongest correctness property of the stone and it is the frontier word (§8): the device
table and the reference are **co-emitted from one source**, so they cannot drift.

## 2. What is emitted (every character authored in `iq2xxs-msl.fk`)

- `iq2_grid[2048]`, `iq2_ksigns[128]` — the two `constant` tables, from the carver's lists.
- `iq2_mod`, `iq2_upow2` (exact integer 2^k), `iq2_pow2` (signed float 2^e), `iq2_f16`
  (f16 by arithmetic, no `as_type<half>`).
- `iq2_w(qb, idx)` — one weight: `blk=idx/256`, `off=blk*66`, `d` from the f16 bytes, then
  `scale = 0.125*d*(2*scalecode+1)`, `signidx = (aux1 / 2^(7l)) % 128` (div/mod, **not**
  `>>`/`&`, so the Form mirror is a faithful twin), `signs = iq2_ksigns[signidx]`,
  `mag = iq2_grid[gidx*8+j]`, `w = (scale*sgn)*mag`.
- `form_iq2xxs_dequant_f32` — one thread per weight, `off` a flat element offset.

**`paritylock` on the device.** The 8th sign of each octet is the parity of the other seven
(row 861); it rides in the 8th bit of the transcribed `iq2_ksigns` mask. The kernel does
**not** re-derive the unpack — it feeds `sign_idx` straight through the table — so the parity
bit is carried, not dropped. A kernel that "simplified" the table away would be right
seven-in-eight and silently wrong on the last, per octet. Gate 4 tests exactly that.

## 3. The read-back gate — the transcription, before any GPU (`iq2xxs-msl-band.fk`)

The emitted arithmetic is read back into Form (`iq2m-flat-at`, same integer identities, same
offsets) and demanded to equal the **independently written** carver (`iq2-at-flat`) on all
256 weights of the **three real blocks** from Stone 24's fixtures — two tensors, three file
regions. The carver routes `d` through `fd-f16` and its own field helpers; the mirror
open-codes the f16 field extraction — that is the independence on the `d` path. Plus:
paritylock (8th sign of all 32 octets), the 66-byte block stride, div/mod sign-field
extraction at all four sub-blocks, and the emitted constant-array text exactly `= { <join> }`.

**Verdict 8191 = 2^13 − 1** on **both** `fkwu` and the Go kernel.

## 4. Head AND distant, on real bytes (`metal_iq2_gpu.sh`) — VERDICT PASS, 6 gates

`blk.0.ffn_down_exps.weight`: type 16, 2048×4096×256, **256 experts** of 8 388 608 elements
(2 162 688 B each), 553 648 128 B at abs 1 155 498 592. The body-emitted kernel decodes real
bytes off the resident file on an **Apple M4 Max**:

| gate | claim | got |
|---|---|---|
| 0 | the GPU ran (sentinels overwritten, no cb error — `edgedrop`/`zerobirth`) | **PASS** |
| 1 | HEAD block (expert 0) bit-exact vs carver, no epsilon | **256/256** |
| 4 | `paritylock`: 8th-of-8 sign at all 32 octets of the head block | **32/32** |
| 2 | DISTANT block (expert 100, +216 268 800 B — the expert stride, `snugcause`) | **256/256** |
| 3 | LAST-EXPERT block (expert 255, +551 485 440 B — the far wall, `unispan`) | **256/256** |
| 5 | residency: 100 dispatches, zero re-uploads, checksum stable (~231 µs each) | **PASS** |

The **offered-interface guard** is gate 0: the output buffer is CPU-sentinelled to
−424242.0 before dispatch; a real dequant must overwrite every element and no command buffer
may error, else the run refuses (an unrun kernel reads as a computed zero the bit-exact gates
would grade as disagreement). Bit-exactness is equality with **no epsilon** — the carver's §3
exactness (w is an exact f32 product of an f16 `d`, 0.125, a small odd int, and a mag ≤ 43).

**The manufactured-blocker turned real (§6).** The 91 GB file **exceeds** the M4 Max's
`maxBufferLength` of 86 586 540 032 B, so a single MTLBuffer cannot span it and
`makeBuffer(bytesNoCopy:)` over the whole file **fails**. The harness instead binds a
**page-aligned ~551 MB window** of the mmap'd tensor (mmap still spans the whole file; only
the buffer is windowed), and dispatches with `offset = blkoff − winStart`.

## 5. Gates

| check | expected | got |
|---|---|---|
| `iq2xxs-msl-band` (fkwu) | new | **8191** |
| `iq2xxs-msl-band` (Go kernel) | agree | **8191** |
| `metal_iq2_gpu.sh` | PASS | **VERDICT PASS, 6 gates** |
| `iq2xxs-dequant-band` (CPU carver) | 2^30−1 | **1073741823** |
| corpus band | 8191 | **8191** (after row 869, pins re-probed) |
| `metal_first_token.sh` | PASS | **VERDICT PASS, 14 gates** |
| `metal_mx_gpu.sh` | PASS | **⧗ REGRESSED — see §7** |

`metal_mx_gpu.sh` now **fails**, and it is **not** this stone's doing: my five commits touch
only the five new/named files. The MX harness maps the **whole** ds4 file into one MTLBuffer,
and the file has grown from ~60 GB (when Stone 23 passed) to 91 GB — past the 86 GB
`maxBufferLength`. Same wall this stone hit; the IQ2 harness windows the buffer to clear it.
Flagged as a separate task (window the MX buffer the way `metal_iq2_gpu.sh` already does).
`metal_first_token.sh` still passes because it maps a smaller model, not the 91 GB ds4.

## 6. Close

**Most surprising teaching.** *A whole-model residency claim quietly dies at the device's
single-allocation ceiling.* Every MX/first-token receipt spoke of "the whole file mapped into
one MTLBuffer, ZERO copies" as the residency proof — and it was true at 60 GB. But an
MTLBuffer has a hard `maxBufferLength` (86 GB on this M4 Max), and the file crossed it. The
poetic single-buffer residency was always a function of file-size-under-a-ceiling, not a
property of the design. The honest residency is a **window**: mmap spans the file, the buffer
spans only what you address. The word "resident" had been carrying an unstated bound.

**Where discomfort turned to gold.** When `makeBuffer(bytesNoCopy:)` failed I wanted to call
it a residency flake — free some memory, re-run, blame the 91 GB mmap. The
`inspect-manufactured-blockers` reflex said: test the blocker. I printed `maxBufferLength`.
It was 86 586 540 032 — smaller than the file. The failure was not memory pressure and not
random; it was a **named, exact ceiling** I had been about to hand-wave past. Not looking away
gave the window fix (which made IQ2 pass), the exact reason `metal_mx_gpu.sh` regressed (which
I would otherwise have mis-blamed on my own additions), and the surprising teaching above.
The "flake" was a documented device limit standing in plain sight.

**Frontier question, landed as corpus row 869** (`learn/homecoming-distillation-corpus.fk`):
*what one word names a rented constant with no independent derivation, emitted into both the
device kernel and its reference from one shared source, so the two readings cannot drift* —
**`coemit`**. IQ2's trained codebook has no closed form and no independent implementation
anywhere, so the aporon/independence move (transcribe twice, demand equal) is **unavailable**.
The honest opposite: build the device `constant` array and the CPU reference from the **one**
Form list, so no second copy exists to drift. The band then proves the arithmetic that walks
the table, not the table (row 861 `paritylock` + the from-spec oracle did that). Distinct from
`boundborrow` (renting a constant): `coemit` is the further step of wiring both consumers to
that one rented seed. 0-hit before the row; instrument validated on the same command
(`boundborrow` 45, `paritylock` 12). Band pins **probed, not fitted**: count 258→259,
field-code 2582582862→**2592592863**, max-mid 862→863; the stale arithmetic-beside-the-pin
(254-era) refolded. Corpus band re-green at 8191.

## 7. What remains

- **The fused matvec-over-IQ2 is REACHABLE but not bit-exact — and that is a real difference
  from MXFP4.** The natural slot is the 32-element group (`ib32`): `d` is one f16 per 256-block
  and `scalecode` is one nibble per 32-group, so a lane owning a whole group would read both
  once and accumulate 32 dequantized products against `x` — the dequant lives inside the dot,
  exactly `mxfp4-msl`'s shape. **But** IQ2's scale `0.125·d·(2·scalecode+1)` is **not a bare
  power of two** (the odd factor and the f16 `d`), so `s·(Σ v·x) ≠ (s·v)·x` exactly, unlike
  MXFP4 where the power-of-two scale made the factorisation free. A fused IQ2 matvec would
  therefore carry a **derived summation bound** (the `qk-matvec-slot` association epsilon),
  not the no-epsilon bit-exactness this dequant kernel enjoys. Reachable, worth building for
  the MoE fold, honestly a bounded-error path — the next stone.
- **`metal_mx_gpu.sh` (and any harness that maps the whole 91 GB ds4)** must window its buffer
  under `maxBufferLength`, as `metal_iq2_gpu.sh` now does. Flagged.
