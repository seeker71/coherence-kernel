# The graft the body refused — the composed Q4_K stone

The stone qk-matvec-slot.fk named as owed ("at Q4_K the byte loads and
divisions DO cost") is paid, by the stones' own method: one mechanism per
variant, measured in metal_isa_diff.sh against ggml on the real shapes before
anything was written into the body.

## What the race separated (M4 Max, 2026-08-18, 4 real Q4_K shapes)

| variant | one mechanism moved | × ggml |
|---|---|---|
| slot4, shipped | — | 1.54–1.64x |
| v5h | d/dmin as hardware `half` loads | **1.28–1.30x** |
| v5u | ggml's in-place uint16 nibble arithmetic | 1.42–1.46x |
| v5r | ggml's two-row register blocking, grafted alone | **1.78–1.80x — slower** |
| **v5c = v5h + v5u** | the two compatible mechanisms composed | **1.21x** |
| v5cu = v5c + unroll hint | | 1.23x — the hint costs; dropped |

The sentence "byte loads and divisions cost" held three hypotheses; the
measurement ranked them: the f16-decode transcription was the largest single
cost, the nibble path second — and the mechanism most visibly present in the
faster kernel, register blocking, made ours *worse* when grafted alone. Its
value lived in the whole it came from. Corpus row 1012 names it *allograft*.

## Two laws evolved, stated rather than slipped

- **Hardware half admitted by equality, not exception.** The transcription law
  (q4k-msl.fk:31) existed so the decode stays provable. IEEE f16→f32 is exact
  for every finite f16, and `q4k_f16` transcribes exactly that map — so
  hardware == transcription is a theorem a band can gate.
  `tests/q4k-halfdecode-band.fk` (preregistered 15) dispatches both decodes
  side by side over 64 real superblocks and compares raw f32 bytes:
  **15, exit 0 — bit-for-bit equal**.
- **In-place nibbles keep the bound family.** `(q & 0x0F00)` holds the nibble
  at 256× its value; the 1/256 and 1/16 corrections are exact powers of two
  folded into the scale multiply — an exact rescaling of the same products, so
  qk-matvec-split.fk's bound applies unchanged. The scale walk `q4k_sc/q4k_mn`
  stays the body's, character for character.

## The stream, re-witnessed through the composed stone

`llama-token-slot.fk`'s Q4_K arm now dispatches `form_q4k_matvec_slot4c_f32`
(pipeline 18 of the unit). The rung band, predicted 255:

| run | prefill | decode | verdict |
|---|---|---|---|
| 1 (cold pipelines) | 859 ms | 271 ms | 255 |
| 2 | 287 ms | 271 ms | 255 |
| 3 | 285 ms | 271 ms | — (pace lines read) |

**24.6 ms/token — 40.6 tok/s**, twelve ids and text byte-identical, the bound
probe green, spread zero at the millisecond. The Form-held loop is now even
with the Swift SLOT lane (41.27) at 25.3% of llama.cpp's 160.33 ± 1.61.

## The most surprising teaching

**A 21% kernel win bought 4.5% of a token.** slot4c is 1.21x of ggml on the
kernel that carries 75.4% of the MACs, and the token moved only 25.7→24.6 ms —
so the token's bill is no longer matvec-dominated. The next stone is not in
the matvec at all: attention, the small kernels, and per-dispatch overheads
now hold the remaining 4x, and no receipt had said so because until today the
matvec hid them.

## Where discomfort turned to gold

The harness's own claim discipline nearly got bent by my edit: the AGREE gate
judges the LAST sweep entry as "the shipped kernel", and my first sweep order
put a research variant there — the harness would have printed a true equality
about the wrong kernel. Caught before running, by reading the comment above
the sweep instead of past it. The gold: the sweep order is itself part of the
claim, and the harness said so in plain words one screen up.

## Honest edges

- 1.21x stands; the remaining 0.21x on this kernel is ggml's whole blocking
  structure, which v5r proves cannot arrive piecemeal. A further stone, owed.
- The composed kernel lives in the slot cell and the rung's translation unit;
  `dense-token-handle.fk` and the other carriers still dispatch their own
  choices — extending them is each carrier's decision, per row 825's law.
- v5c's reassociation vs slot4 differs within the family bound; the stream
  gates (ids, text) and the head-row bound probe witnessed it at stream level.

## Proof

| check | verdict | exit |
|---|---|---|
| metal_isa_diff.sh full race | 3 Q6_K + 4 Q4_K shapes PASS | 0 |
| `q4k-halfdecode-band.fk` (preregistered 15) | 15 | 0 |
| `llama-token-slot-band.fk` ×3 (predicted 255) | 255 | 0 |
| `homecoming-distillation-corpus-band.fk` (row 1012) | 32767 | 0 |

## The frontier question

**What names a graft the host body rejects?**

*Allograft* — tissue transplanted between genetically different members of one
species, which the receiving body may refuse. ggml's register blocking is a
working organ in ggml's kernel and was rejected by ours; the two mechanisms
that did take (hardware half, in-place nibbles) composed to 1.21x and shipped.
0-hit fresh. Corpus row 1012, landed under the counterweight: 406 rows, 406
admissible, max id 1012, dup rows 0 — probed in one cell, exit 0, before the
numbers were written down.
