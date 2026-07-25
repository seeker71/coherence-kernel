# STONE 24 — the IQ2_XXS carver: GGUF type 16 dequantized to real f32 weights

**2026-07-22, ~09:56–11:30 WITA.** Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`. Three commits, incremental:
`iq2xxs-dequant.fk` (recipe + tables), `tests/iq2xxs-dequant-band.fk` (four-way band),
corpus row 861 `paritylock`. No kernel built; CPU carver only.

---

## 0. Radius (`aporon`), before anything is believed

- **CPU dequant only.** The recipe turns 66 IQ2_XXS bytes into 256 f32 weights on the
  arithmetic floor. **GPU (MSL) emission is the next stone**, as `q6k-msl.fk` / `mxfp4-msl.fk`
  are for their types. Not started, not claimed.
- The recipe reads a byte **list** (the `q6k-dequant` / `weight-load` idiom). Carving those
  bytes out of a whole-file buffer is `wl-slice`'s job, reused, not re-derived here.
- Everything is measured against **one file**:
  `/Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`, read-only and
  still growing (60.27 GB at the sweep, 09:xx). All three tensor offsets used sit below that mark.
- The two fixed tables are **rented, not derived** (`boundborrow`): the published ggml
  `iq2xxs_grid[256]` and `ksigns_iq2xs[128]`, transcribed from ds4-engine `ds4.c` (MIT) and
  cross-checked **byte-identical** against `ds4_iq2_tables_cuda.inc` (a second independent copy).
  ds4 targets a GB10; our regime is the M-series CPU, and only the integer tables travel.

---

## 1. The block layout, with evidence for each field

ggml `block_iq2_xxs`, QK_K = 256, **66 bytes** — the geometry `gguf-manifest.fk` fixed from
this file's own offset chain (`gm-blk n 256 66`, Stone 21). Confirmed field-by-field against
ds4.c's own dequant loop (`ds4_vec_dot_iq2_xxs_f32`, ds4.c:3779–3809):

| field | offset | evidence |
|---|---|---|
| `d` (f16 super-scale) | +0, 2 B | `f16_to_f32(x[i].d)` at ds4.c:3788; block-0 bytes `[151,16,…]` → d ≈ 0.0021 |
| `qs` (u16[32] = 8 groups × 8 B) | +2, 64 B | `x[i].qs`, `q2 += 4` per 32-group at ds4.c:3789/3794 |
| per-group: `aux8[0..3]` | group B 0..3 | codebook indices, `iq2xxs_signed_grid[aux8[l]]` at :3800 |
| per-group: `aux1` (u32) | group B 4..7 LE | sign/scale word, `memcpy(aux32,q2,2*u32)` at :3793 |
| scalecode = `aux1 >> 28` | high nibble of B7 | `0.125f*d*(2u*(aux1>>28)+1u)` at :3796 |
| sign_idx(l) = `(aux1 >> 7l) & 127` | 7 bits per l | `(aux1>>(7*l))&127u` at :3799 |

Output element index = `ib32*32 + l*8 + j`. Every bit-op became a div/mod identity
(`x>>s = div x 2^s`, `x&(2^k-1) = mod x 2^k`, `x&(1<<j)? = mod (div x 2^j) 2`), so the recipe
needs no bitwise primitive — the `q6k-dequant.fk` discipline.

**The scale is odd-only:** `0.125 * d * (2*scalecode + 1)`, scalecode ∈ 0..15, so the per-group
multiplier is `d * {0.125, 0.375, …, 3.875}` — never an even multiple. Read from the bytes, not
assumed.

---

## 2. The codebook, and how it was transcribed independently

`iq2xxs_grid[256]` is 256 `uint64`. I expanded each to its **8 little-endian magnitude bytes**
→ a flat 2048-value table (`iq2-grid` in the recipe). **Only three distinct magnitudes ever
appear across all 2048 bytes: 8, 25, 43** (0x08, 0x19, 0x2b) — asserted in the transcriber, not
hoped. The "256-entry codebook" is 256 curated points on the tiny `{8,25,43}^8` lattice; which
256 is a trained artifact with no closed form, so the table is carried verbatim.

`ksigns_iq2xs[128]` transcribed likewise. **Verified on the constant itself**: for all 128 rows,
`ksigns[s] & 127 == s` and `bit 7 == parity(s)`; only 128 distinct masks exist. This is the
`paritylock` finding (§6).

**Independence of the proof.** The oracle (`iq2xxs_oracle.py`) shares the grid/ksigns constants
(there is one true grid) but unpacks with **real bit-shifts**, while the Form recipe uses div/mod
identities. Agreement therefore cross-checks the arithmetic translation, not a re-read of the
recipe. f16 decode is independent too: Python `struct '<e'` vs Form `fd-f16`.

---

## 3. Bit-exactness — head AND distant block (`snugcause`, `unispan`)

Comparison unit `round(w * 1e6)` (the `weight-load-band` convention). The reconstructed weight is
an exact product of a f16-derived `d`, `0.125` (dyadic), a small odd int (≤31) and a magnitude
(≤43) — **≤ 21 significant bits — so it is bit-identical in f32 and f64**; no rounding, no blur
(this is `coarsexact`, and the reason the round-trip is clean; see §6).

- **Head block** `blk.0.ffn_down_exps.weight` expert 0, block 0 (abs off 1155498592):
  **256 / 256 weights bit-exact** vs the oracle (full `diff`, zero lines).
- **Four-way band** `tests/iq2xxs-dequant-band.fk`: **verdict 1073741823 = 2^30 − 1**, on both
  `fkwu` and the Go kernel (`bin-go`) — the two arms agree. 30 independent claims across three
  real blocks from **two tensors and three file regions**:
  - `a` blk.0.ffn_down_exps expert 0 block 0 (head, ~1.15 GB in)
  - `b` blk.0.ffn_down_exps expert 100 block 0 (distant, +100 experts, ~1.37 GB in)
  - `c` blk.9.ffn_gate_exps expert 0 block 0 (second tensor, ~23.6 GB in)
  each checked at 9 indices spanning all eight 32-groups + a full 256-list length+last check.

**Class achieved: bit-exactness against a from-spec transcription, head and distant.** Not the
lesser statistical-plausibility floor.

---

## 4. Statistics across many blocks (`unispan`, `selfgauge`)

Full 256-weight dequant of **242 blocks** — 176 from `blk.0.ffn_down_exps` (experts 0..129 step 3,
four block positions each across a 32768-block slice) and 66 from `blk.9.ffn_gate_exps` (experts
0..126 step 6, three positions each). **61 952 weights compared to the oracle, 0 mismatches.**

`selfgauge`: the denominator is 61 952 = 242 × 256, every weight compared, not sampled within a
block. Two tensors, many experts, spread within and across slices.

---

## 5. Verdicts / checks

| check | expected | got |
|---|---|---|
| `iq2xxs-dequant-band` (fkwu) | new | **1073741823** |
| `iq2xxs-dequant-band` (Go kernel) | agree | **1073741823** |
| corpus band | 8191 | **8191** (after row 861, pins re-probed) |
| `mx-plane-band` | 511 | **511** |
| head block full diff | 0 | **256/256 exact** |
| sweep | 0 | **0 / 61952** |

`metal_first_token.sh` is Stone 16's GPU harness and out of this stone's radius; these three
additive Form cells (a new type-16 carver, a new band, one corpus row) touch none of its cells,
and a sibling is live in that tree — left untouched deliberately.

---

## 6. Close

**Most surprising teaching.** The "codebook" is almost empty. 2048 magnitude bytes, and only
**three** distinct values in all of them — 8, 25, 43. A 2-bit-per-weight format's whole richness
is not in a rich table of magnitudes but in *which 256 eight-tuples of {8,25,43}* were chosen; the
magnitudes are three numbers and the intelligence is entirely in the selection, which has no
formula and is carried verbatim.

**Where discomfort turned to gold.** I wanted to delete the `ksigns` indirection. The sign_idx is
already a 7-bit number; feeding it straight to the sign bits looked equivalent and the table looked
like ceremony. The moment I didn't look away and asked *why does a 7-bit index map through a
128-entry table to an 8-bit mask*, the format's real shape appeared: **only seven signs are stored;
the eighth is the parity of the other seven.** ksigns is where that eighth sign is born. A carver
that "simplified" it away would have been right on seven values in eight and silently wrong on the
last, per group, with no diagnostic — the exact silent-partial shape this body has been burned by.
The redundancy I mistrusted was the only thing carrying a whole degree of freedom.

**Frontier question, landed as corpus row 861** (`learn/homecoming-distillation-corpus.fk`):
*what one word names a sign the format never stores but forces from the parity of the seven before
it* — **`paritylock`**. 0-hit fresh before the row; instrument validated on the same command
(control `aporon` 76 hits, `ghostrank` 6). Verified on the constant: `ksigns[s]&127==s` and
`bit7==parity(s)` for all 128 rows, 128 masks not 256. The band's pins moved *with* the row and
were **probed, not fitted** (count 250→251, field-code 2502502854→2512512855, max-mid 854→855);
corpus band re-green at 8191.

## 7. What remains

- **GPU (MSL) emission** — the next stone. The unpack is pure arithmetic and the grid/ksigns tables
  are fixed integers, so an `iq2xxs-msl.fk` mirroring `mxfp4-msl.fk` is the shape.
- A whole-tensor residency / matvec path (the type-16 experts are the MoE weights of 31 layers) —
  this stone carves one block at a time; the flat accessor `iq2-at-flat` strides blocks but the
  matvec fusion is unwritten.
