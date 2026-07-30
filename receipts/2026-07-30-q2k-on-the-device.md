# 2026-07-30 — Q2_K on the device, and a gate that had to be split rather than loosened

Continuing the enablement while the 86.7 GB mainline DeepSeek downloads (37% at the time of writing).
The CPU carver landed yesterday; this is the GPU side, and it can be proven now because the fixture is
two seeded blocks on disk rather than the model.

## What was built

`form/form-stdlib/q2k-msl.fk` — the Q2_K decode as MSL the body emits. Same shape as its siblings
`iq2xxs-msl.fk` and `q6k-msl.fk`, deliberately: a spine (mod, a power of two, **f16 decoded by hand**),
then one `q2k_w(qb, idx)` answering a single weight at a flat index. The f16 is hand-rolled rather than
`as_type<half>` because both siblings do it that way — the arithmetic in a carver is the body's, or the
carver is not the body's. And no bit operators: div and mod, matching the CPU cell character for
character, on the settled ground that the map is what costs and the arithmetic is not
(`metal_isa_diff.sh`, 2026-07-28 — v3 keeps every division and beats v2 which has none).

`form/native/metal/metal_q2k_gpu.sh` — dispatches it and judges all 512 fixture weights against
`q2k-dequant.fk` itself. Output is NaN-sentinelled, so a kernel that never runs cannot pass by leaving
zeros.

## The gate was wrong, and the fix was to narrow it

First run: **135 of 512 disagreed.** The instinct is to look at the kernel. The measurement said
otherwise:

```
block0 (d = 0x3C00 = 1.0, dmin = 0x3800 = 0.5)    0 of 256 differ
block1 (d = 0x3555, dmin = 0xB400 NEGATIVE)     135 of 256 differ
max |d| 1.000e-06   max relative 4.616e-07   f32 eps 1.1920929e-07
```

Every disagreement is in the block whose constants are *not* exactly representable, and the worst is
about four f32 ulps. The CPU carver runs Form's floats; the kernel runs f32. **I had demanded that a
lower-precision carrier reproduce a higher-precision one bit-for-bit** — `overfine` (row 923) arriving
from the other direction: there the *reference* was too precise to be faithful; here the *gate* was.

The obvious repair is a tolerance everywhere. That would have thrown away the 256 weights where
exactness is real and meaningful. So the claim is split where the evidence splits it:

- **block 0** — both constants exact in f16 *and* f32, so every product is exactly representable.
  **Exact equality demanded.** A difference here is a decode bug and nothing else.
- **block 1** — awkward constants, so the two carriers *must* round differently. **f32's own
  granularity, 8 ulps.**

That is *narrower* than a blanket tolerance, not looser.

```
PASS  block 0, exact constants: all 256 weights bit-identical to q2k-dequant.fk
PASS  block 1, awkward f16 d and NEGATIVE dmin: 135 of 256 differ, max rel 4.616e-07 within 9.537e-07
=== VERDICT PASS ===
```

**And the split gate has teeth**, mutation-proven on the MSL itself:

```
M1  contiguous stride, qbase = group*16   -> FAIL, 161 of 256 differ in BLOCK 0
M2  scale the min by q (per-element)      -> FAIL, 186 of 256 differ in BLOCK 0
```

Both land in block 0, where the demand is exact. A blanket 8-ulp tolerance would have caught these too
— but only because they are large. An error of one ulp in the fold would have hidden inside a uniform
tolerance and cannot hide from block 0.

## The most surprising teaching

**When two carriers disagree because they carry differently, the repair is to find the sub-case where
the property still holds exactly and keep it exact there — not to relax the whole claim to the weakest
case.** A uniform tolerance is the easy move and it silently converts a bit-exact check into an
approximate one across the entire surface, including everywhere the exactness was true and free.
`exactwhere` — 0 hits before this row (`partexact` also 0; `splitgate` already appears twice in the
tree, so it was rejected).

The fixture made this available by accident: I put an awkward `d` and a negative `dmin` in block 1 to
exercise the sign path, and that choice is what produced two blocks with *different* exactness
properties — which is what made the split possible to state. A fixture built only from convenient
constants would have passed the wrong gate and taught nothing.

## Where discomfort turned to gold

Seeing `FAIL 135 of 512` and reaching for the kernel. The kernel was correct; the harness was wrong,
and it was wrong in the direction that feels most rigorous — demanding exactness. **Strictness is not
the same as correctness, and a gate can be wrong by asking for more than the world can give.** The
thing that stopped me rewriting a correct kernel was printing the distribution instead of the first
failure: `block0 0, block1 135` is a sentence about f16 representability, and `first disagreement at
256` alone is not.

## Ground stamp

```
form/form-stdlib/q2k-msl.fk — new; helpers + form_q2k_dequant_f32 + form_q2k_matvec_slot_f32
form/native/metal/metal_q2k_gpu.sh -> VERDICT PASS
  block 0: 256 of 256 bit-identical to q2k-dequant.fk
  block 1: 135 of 256 differ, max rel 4.616e-07, bound 9.537e-07 (8 f32 ulps)
  mutations M1/M2 both FAIL in block 0 (161 and 186 of 256)
form/form-stdlib/tests/q2k-dequant-band.fk -> 511
download: DeepSeek-V4-Flash IQ2XXS 86.7 GB, 37%
```
