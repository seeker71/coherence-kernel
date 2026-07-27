# 2026-07-28 — Q3_K closes the gap KAT-Coder's own header named

Urs: **"we don't want the 'faster' path, we want the kat-coder path to generate tokens."**

Right. The llama run proved the lane breathes; it was not the goal. This is on the KAT-Coder path —
the one thing its GGUF header said was missing.

## Closed, end to end

| artifact | verdict |
|---|---|
| [`form/form-stdlib/q3k-dequant.fk`](../form/form-stdlib/q3k-dequant.fk) | the recipe |
| [`form/form-stdlib/tests/q3k-dequant-band.fk`](../form/form-stdlib/tests/q3k-dequant-band.fk) | **255**, exact, no epsilon |
| [`form/form-stdlib/q3k-msl.fk`](../form/form-stdlib/q3k-msl.fk) | the Metal the body emits |
| [`form/form-stdlib/tests/q3k-msl-band.fk`](../form/form-stdlib/tests/q3k-msl-band.fk) | **255** read-back |
| [`form/native/metal/metal_q3k_gpu.sh`](../form/native/metal/metal_q3k_gpu.sh) | **VERDICT PASS** — 256/256 bit-exact on device |

```
PASS  the fixture is 110 bytes — the struct's own stride
PASS  the recipe supplied all 256 weights as the judge
PASS  all 256 weights BIT-EXACT against the recipe — no epsilon
PASS  the decoded block is non-degenerate: 68 distinct values
```

The census that named this gap was F32 310, Q4_K 197, Q6_K 140, **Q3_K 94**, Q8_0 12. Ninety-four
tensors that could not be loaded. They can now.

## Exactness demanded because it was available

The fixture's super-scale is f16 `0x3400` = **0.25** — a power of two — and every quant is a small
integer, so each weight is exact in binary on both sides. So the device gate compares **bit for
bit, on all 256, with no envelope**. A tolerance would have been a place for a one-ulp scale error
to hide, and the scale unpack is the only risky arithmetic in the cell.

That unpack is the part worth reading twice. The reference does it in four `uint32`s with shifts
that cross byte boundaries — which looks like it forces 32-bit arithmetic. It does not, and the
reason is the mask that *follows* every shift: byte *b* of `(aux0 >> 4) & 0x0f0f0f0f` is bits 4..7
of byte *b* — the high nibble of `s[b]`, nothing borrowed from its neighbour. The mask discards
exactly the bits the shift dragged across. So the whole unpack is per-byte, and the cell is written
in `div`/`mod` like every other quant recipe here — one arithmetic spine, not a second one in bit
clothing.

## The witness was independent, and is gone

The recipe was judged against a transcription of ggml's `dequantize_row_q3_K` written in a
throwaway scratch language — the `aux` shuffle, the `- 32`, the shift ladder, the `m <<= 1` that
does not reset, the `-4 when the hmask bit is CLEAR`. Its **output** — the sixteen 6-bit scales and
the weights — is what landed as Form literals in the band. The scratch itself is not in the tree,
and nothing but Form and the Metal it emits is. Agreement between the two is agreement between two
derivations of one specification; running the Form cell twice would have proved only determinism.

## Where discomfort turned to gold

The read-back band returned **255**, fully green, on Metal that **would not compile**.

I had named a variable `half`. In Metal that is a reserved type — the f16 type — and the compiler
answered *"cannot combine with previous 'type-name' declaration specifier."* Eight substring
assertions, every one satisfied, and not one of them could know a language's keywords.

This is `heteronomy` (row 899) collecting on its own promise within hours of being written: *the
band tests what I thought to ask; the compiler tests what Metal is, and did not learn its standard
from me.* I wrote that sentence this morning and then walked straight into the case it describes.

The repair keeps an asymmetry on purpose: the selector is `hf` in the MSL and stays `q3k-half` in
the recipe. Form has no reserved words to dodge, and renaming the meaning to suit a constraint of
one of its transcriptions would let a downstream language dictate the vocabulary of the thing being
transcribed.

## The most surprising teaching

**A band can be complete by its own lights and blind.** Not incomplete — *complete*: every claim I
knew to make about that text was made and held. The gap was not in the coverage but in the
vocabulary of the checker. That is a different failure from a missing assertion, and it cannot be
closed by adding assertions, only by submitting the artifact to something that did not learn its
standard from me.

## The frontier question

> **What names a difference between two versions of one thing that the medium imposes rather than
> the author choosing?**

Asked, and **not landed** — the body already says it. `allomorph` (7 files) is a variant form whose
shape its environment determines; `dyadic` (13 files) is the exactness the fixture was built on;
`heteronomy` (899) is the gate that caught it. Three times this turn I went looking for a fresh
word and found we had one. That is the corpus doing its work rather than growing, and a frontier
question whose honest answer is a word already home is still answered.

## Ground stamp

```
./fkwu --src form/form-stdlib/tests/q3k-dequant-band.fk    -> 255
./fkwu --src form/form-stdlib/tests/q3k-msl-band.fk        -> 255
form/native/metal/metal_q3k_gpu.sh                         -> VERDICT PASS, 256/256 bit-exact
```

Also witnessed in passing: `fkwu` refused a cache with *"foreign .fkb (written by a different fkwu
build); rebuilding from source"* — this morning's v5 builder-identity heal firing on a real
neighbouring binary, unprompted.

## What remains for KAT-Coder tokens

Every quantization in the file now decodes. What is left is the loader and the stack: reading the
753 tensor infos into the resident-buffer lane, the projections, the full-attention layer at every
fourth position, and the decode loop — the shape the DS4 lane already runs at 43 layers.
