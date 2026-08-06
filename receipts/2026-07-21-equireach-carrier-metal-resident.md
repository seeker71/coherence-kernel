# equireach — a byte source whose reach does not grow with position, and real weights resident on Metal

Tuesday 2026-07-21, Hati Suci (WITA). Apple M4 Max, 128 GiB unified memory.
Worktree: `.claude/worktrees/jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
Body: root `fkwu` (194040 bytes, Jul 18 01:12); `form/form-kernel-go/bin-go` (Jul 18 01:11).

**STONE 2** of the 4-stone program. Stone 1 (`eb3ea92a7`,
`receipts/2026-07-21-form-native-tokps-baseline.md`) measured 43.8 tok/s on real llama3.2:3b weights at
reduced width and named the wall: **the byte-list carrier is quadratic**, and it named the word for what
was missing — `equireach`. This stone builds it, and drives the ⬜ it unblocks.

Every timing below cleared its own `.fkb` before every run and ran **three times** unless the row says
otherwise. Every harness is `bash -c`. Nothing is projected unless it says PROJECTED.

---

## 0. The short of it

| | before (byte list) | after (equireach) |
|---|---|---|
| reach one byte at position p, 16 MB window | O(p) | **0.043 µs, flat** |
| build a 1 MB window | 165.5 s | **0.012 s** (no build — the window IS the source) |
| dequant 65 536 Q6_K weights, base 11 760 B deep | **788–807 s** | **3.55 s** (222×) |
| the same, base 16 723 350 B deep | not attempted (the build alone is PROJECTED hours) | **3.52 s** (unchanged) |
| locate `blk.0.ffn_down.weight` **by name** in the real 2 GB blob | PROJECTED ~2.7 h | **4.7 s** |
| llama3.2:3b weights resident on the Metal GPU | ⬜ | **✅ bit-exact, 200 dispatches, 0 re-uploads** |

---

## 1. Part A — the bounds seam: a silent `-0` made loud, and the wall crossed

### The defect, restated from the body

`q6k-at i d ql qh scales` decomposes `i` assuming `0 <= i < 256`. Past the wall, `q6k-h` returns 2,3,…
and the ql/qh/scales indices walk off the ends of the 128/64/16-byte fields. `nth` past the end of a
list answers **0**, so `q6k-at` returned `d * 0 * (0-32)` = **-0** — a plausible-looking weight, no
diagnostic, full green verdict. `block-join.fk`'s `bj-row-n` / `bj-matrix` reach through that door, so
**`bj-matrix` was only ever correct for `dim <= 16`** (`dim*dim <= 256`) while a real llama3.2:3b row is
**3072** weights wide. Grepping `ge i 256` / `lt i 256` / `bound` / `clamp` across `q6k-dequant.fk`,
`weight-load.fk`, `block-join.fk` returned **zero hits** — there was no guard anywhere.

`q4k-dequant.fk` carries the identical defect (`q4k-c` walks off `scales[12]`/`qs[128]`).

### What changed

| cell | change |
|---|---|
| `form/form-stdlib/q6k-dequant.fk` | `q6k-block-n` / `q6k-index-ok?` name the wall in-recipe; `q6k-at` refuses out loud (`form_error`) instead of answering `-0` |
| `form/form-stdlib/q4k-dequant.fk` | same shape: `q4k-block-n` / `q4k-index-ok?` / a refusing `q4k-at` |
| `form/form-stdlib/weight-load.fk` | `wl-slice` now **checks its own length** — `take` returned short at the buffer's end (the silent-partial-list shape) and a short `ql` dequants to plausible zeros; `wl-q6k-stride` / `wl-q6k-block-of` / `wl-q6k-within` / **`wl-q6k-at-flat`** give random access across superblocks at the 210-byte stride |
| `form/form-stdlib/block-join.fk` | `bj-row-n-from` reaches through `wl-q6k-at-flat`, so `bj-row-n` / `bj-matrix` are **correct at any width**; `bj-matrix-max-index` states the reach instead of leaving it to be discovered as zeros |

The refusal, witnessed (`fkwu`, exit 1):

```
fkwu: form_error: q6k-at: weight index outside the 256-weight Q6_K superblock --
      stride to the next block (210-byte stride) instead of indexing past it
fkwu: form_error: wl-slice: window runs past the end of the byte buffer --
      the buffer is short of the block, not the block short of bytes
```

### The band that would have failed before — and did

`form/form-stdlib/tests/q6k-bounds-band.fk`, **verdict 255**. It carries **two consecutive REAL
llama3.2:3b Q6_K superblocks** (420 bytes, from the blob at 331 055 328) as a transcribed byte list —
the ~1 KB committed Q6_K fixture Stone 1 named as missing, so the stride claim travels without the 2 GB
blob. Ground truth is an independent transcription of ggml's `dequantize_row_q6_K` over those same
bytes (it reproduces Stone 1's pins exactly: `w[0]*1e6 = 9149.5514`, `w[256]*1e6 = 19241.3330`).

A/B, `git stash` on the four cells only, `.fkb` cleared both sides:

```
BEFORE (cells reverted):  verdict 4    (12 unresolved-call: q6k-index-ok?, q4k-index-ok?,
                                        wl-q6k-at-flat, bj-matrix-max-index)
AFTER  (cells healed):    verdict 255
```

Bits **32** and **64** are the ones that matter: `bj-row-n` over 257 weights and `bj-matrix` at
`dim=17`. Both recipes *existed* before and both answered `-0` at full green. Now they answer
19241.3330 and 25485.5156 — ggml's own values.

**Four-way**: `q6k-bounds-band` returns **255 on fkwu, go, rust and ts**.

---

## 2. Part B — `equireach`: the carrier

### The word, and what it is made of

> **equireach**: a byte source whose cost of reaching a position does not grow with the position.

`form/form-stdlib/equireach.fk` gives the body one out of parts it already owned: `read_file_slice`
hands back a host string, `str_byte_at` indexes it in O(1). Vocabulary: `eqr-of-file` / `eqr-of-string`
/ `eqr-len` / **`eqr-at`** / `eqr-in?` / `eqr-at-checked` / `eqr-le` / `eqr-u16` / `eqr-u32` / `eqr-u64`,
plus `eqr-bytes` — the escape hatch back to a list, **named as a cost and capped at 65 536 bytes**,
because a list is not equireach and the door refuses to be the quiet way back into the defect.

`form/form-stdlib/equireach-gguf.fk` rewrites the reach path: `egg-*` mirrors `gg-*` (header, KV walk,
tensor-info walk, find-by-name, data base, absolute offset) and `ewl-*` mirrors `wl-*` (superblock
fields, `ewl-at`, **`ewl-flat-at`**, `ewl-weights`, `ewl-mat`, `ewl-f32`, `ewl-gain`). Nothing
re-derives the format: the index math is `q6k-dequant.fk`'s own (`q6k-h`/`q6k-l`/`q6k-g`/`q6k-is`/
`q6k-qlidx`/`q6k-s8`/`q6k-pow4`), so there is still exactly **one** Q6_K spine in the body.

### THE CURVE — reach cost vs position

10M reads of the byte at the **last** position of the window, `.fkb` cleared each run, three runs, minus
the identical program with zero reads (0.00 s at 64 KB, 0.02 s at 16 MB).

| window | 64 KB | 256 KB | 1 MB | 4 MB | 16 MB |
|---|---|---|---|---|---|
| `eqr-at`, wall (s) | 0.48 / 0.44 / 0.44 | 0.43 / 0.43 / 0.43 | 0.44 / 0.44 / 0.43 | 0.46 / 0.45 / 0.44 | 0.45 / 0.46 / 0.45 |
| reads/s | 22.7M | 23.3M | 23.3M | 22.7M | 23.3M |

> **Flat over a 256× window growth.** Not merely faster — *flat*. That is what places the stone.

The other side, the same shape on the list carrier — 100 000 `nth` reads at the last index, minus the
build:

| list length | 4 096 | 16 384 | 65 536 |
|---|---|---|---|
| reach per read | 4.8 µs | 8.5 µs | **50.7 µs** |
| `eqr-at` at the same size | 0.043 µs | 0.043 µs | 0.043 µs |
| ratio | 112× | 198× | **1 180×** |

And the bridge that gets bytes into a list at all (`host-abi-string-to-bytes`), three runs each:

| window | 64 KB | 256 KB | 1 MB |
|---|---|---|---|
| build (s) | 0.96 / 0.96 / 0.95 | 11.59 / 11.51 / 11.06 | 165.46 / 166.38 / 164.94 |
| ratio per 4× | — | **12.0×** | **14.5×** |

Quadratic, confirming Stone 1's 0.91 s / 167 s to two figures.

### THE CURVE THAT MATTERS — dequant cost vs depth, matched windows

65 536 dequantized weights (256 Q6_K superblocks = 53 760 bytes), the block base placed at the **end**
of the window so the reach is as deep as the window is large. Checksummed (a nonzero, base-dependent
sum, so the arithmetic is forced and not elided). Setup subtracted.

| window | base depth | **equireach** `ewl-weights` | rate | **byte list** (`wl-load-q6k` per block) |
|---|---|---|---|---|
| 64 KB | 11 760 B | **3.55 s** (3.66/3.64/3.70 − 0.11) | 18 460 w/s | **788.4 / 807.0 s** *(2 runs)* → 82 w/s |
| 1 MB | 994 770 B | **3.49 s** (3.60/3.64/3.60 − 0.12) | 18 778 w/s | not attempted — the 1 MB build alone is 165 s and the reach then grows |
| 16 MB | 16 723 350 B | **3.52 s** (3.67/3.76/3.62 − 0.16) | 18 617 w/s | not attempted — PROJECTED hours |

> **222× at the shallowest matched point, and thereafter one lane is flat while the other cannot be run.**
> Stone 1's list lane peaked at 15 754 w/s in a ~6.7 KB window and fell to 903 w/s in a 26.9 KB window.
> equireach holds 18.5k w/s with its base **16.7 MB** deep.

*(Honesty note on the list cell: **two** runs, not three — each takes ~13 minutes, and a concurrent
sibling `fkwu` process (another agent's `cognition/scripture-model-merge-round3.fk`) held 100% of a core
on this host throughout. Contention of that kind is worth at most ~2×; the conclusion survives it by two
orders of magnitude. It is the one row here that is not three-run.)*

### What equireach makes possible that was not possible at all

Locating a tensor **by name** in the real 2 GB blob — walking its own 7 837 658-byte header (30 metadata
KVs including the tokenizer's 128 256-element string arrays, then 255 tensor-info records):

```
fkwu:  4.69 / 4.78 / 5.24 s
  ->  blk.0.ffn_down.weight   type 14 (Q6_K)  dims 8192 x 3072  abs offset 331 055 328
  ->  blk.0.attn_norm.weight                                    abs offset 331 043 040
  ->  data base 7 837 664, tensor count 255, magic ok, version 3
```

Both offsets match Stone 1's independently hand-derived values. **The list lane cannot do this at all**:
the header must become a list first, PROJECTED ~2.7 h at the measured ~1.5e-10·n² before the first field
is read.

### The band

`form/form-stdlib/tests/equireach-band.fk`, **verdict 511 on fkwu AND on the Go kernel**.

The claim that earns the carrier the right to replace the reach is bit 64: **all 512 weights, both
carriers, `eq` with no epsilon and no sampling**. Plus: the window equals the transcribed list in all
420 bytes (the fixture arrives twice, independently — as a byte list in the band and as the committed
`form/form-stdlib/tests/fixtures/q6k-two-superblocks.bin`, sha256 `d44993f2…e8c1`); the edge is named;
`eqr-le` agrees with `gg-le` at four widths; `ewl-d` agrees with `wl-q6k-d`; `ewl-flat-at 256` crosses
the wall; `ewl-weights` and `ewl-mat` carve identically.

`; PROOF LEVEL: TWO-ARM (fkwu + go)` — declared, with the reason, because of §5's defect 1.

---

## 3. Part C — GPU_GAPS §C closed for one tile: real weights resident on Metal

`form/native/metal/q6k-device-tile.fk` (the body) + `form/native/metal/metal_weight_residency_audit.sh`
(the carrier, which writes its own Swift runner). The `; preludes:` directives are walked recursively by
the script — the same expansion `validate.sh` uses — never hand-catted.

Who decides what: the body picks the tensor, finds its bytes, dequantizes every weight, and states the
answer; the body also **emits the MSL kernel text** (`jte-matvec-msl`, 438 bytes, not one character
authored by the carrier). The carrier parses, uploads, dispatches, reads back, compares.

```
$ form/native/metal/metal_weight_residency_audit.sh 256 256 200

emitted MSL: 438 bytes, every byte authored by the Form recipe
locating blk.0.ffn_down.weight by name in the real blob (walking its own 7.8 MB header) ...
TENSOR blk.0.ffn_down.weight
TYPE 14
D0 8192
D1 3072
ABS 331055328
body time (locate + dequant 256x256 + fp64 reference): 118s
resident: weights 262144 bytes on Apple M4 Max, uploaded once
PASS  gate 1 residency bit-exact: all 65536 f32 weights on the device equal Form's
PASS  gate 2 kernel bit-exact: all 256 GPU rows equal the f32 right-fold over the resident buffer
      value-relative max 8.902e-05 (worst row condition number 6180.8 — cancellation, not error)
PASS  gate 3 derived bound: max |gpu-form| is 0.008 of the cols*u*SUM|term| fp32 summation bound
resident dispatches: 200 in 0.0617 s (308.4 us each), zero re-uploads; checksum after == checksum before: yes
VERDICT PASS
```

Also PASSes at 16×16 / 50 iters (max relative 1.827e-07, 160.6 µs/dispatch).

**On gate 3, because this is where a receipt can lie.** The GPU accumulates in fp32 and Form in fp64, so
equality is impossible and a round tolerance picked to make it pass would be a fudge. The first gate the
audit ran used a 1e-5 value-relative tolerance and **it FAILED at 256×256** (8.902e-05). Rather than
loosen the number, the gate was replaced with the arithmetic's own bound for a sequential fp32 sum,
`cols · 2⁻²⁴ · Σ|term|`; the measured deviation is **0.008 of it** — 125× inside. The large
value-relative number is the worst row's **condition number 6180.8**: its products cancel, so a tiny
`|y|` amplifies an error that never grew. Both numbers are printed, always.

### The GPU_GAPS.md row that changed

`form/native/GPU_GAPS.md` §C, from:

> ⬜ **Weight load → device**: GGUF dequant recipes exist; loading + keeping resident on GPU not wired.

to **🟡** — wired and proven on Metal **for one tensor tile**, with the offsets, the three gates, the
200 zero-re-upload dispatches, and the explicit remaining ⬜: whole-tensor residency (25.2M weights ⇒
~23 min of Form dequant at the measured 18.5k w/s), multi-layer weight sets, KV-cache device buffers,
and a `.metallib` cached across runs. §E's Metal row gained a pointer to the audit. Nothing wider was
claimed.

---

## 4. Everything that changed

| path | what |
|---|---|
| `form/form-stdlib/q6k-dequant.fk` | bounds seam: `q6k-block-n`, `q6k-index-ok?`, refusing `q6k-at` |
| `form/form-stdlib/q4k-dequant.fk` | the same seam for Q4_K |
| `form/form-stdlib/weight-load.fk` | `wl-slice` length check; `wl-q6k-stride` / `-block-of` / `-within` / **`wl-q6k-at-flat`** |
| `form/form-stdlib/block-join.fk` | `bj-row-n-from` strides; `bj-matrix-max-index` |
| `form/form-stdlib/equireach.fk` | **new** — the carrier and its vocabulary |
| `form/form-stdlib/equireach-gguf.fk` | **new** — `egg-*` / `ewl-*`, the reach path rewritten |
| `form/form-stdlib/tests/q6k-bounds-band.fk` | **new** — verdict 255, four-way |
| `form/form-stdlib/tests/equireach-band.fk` | **new** — verdict 511, two-arm (declared) |
| `form/form-stdlib/tests/fixtures/q6k-two-superblocks.bin` | **new** — 420 real bytes, sha256 `d44993f2…e8c1` |
| `form/native/metal/q6k-device-tile.fk` | **new** — locate, dequant, reference, emit |
| `form/native/metal/metal_weight_residency_audit.sh` | **new** — the Metal residency witness |
| `form/native/GPU_GAPS.md` | §C row ⬜ → 🟡; §E Metal row |

Regression sweep, `.fkb` cleared, all on fkwu:

```
gguf-read-band 127 · q6k-dequant-band 4095 · q4k-dequant-band 4095 · weight-load-band 4095
weight-load-q4k-band 4095 · block-join-band 255 · block-join-causal-band 15
block-join-gqa-causal-band 15 · real-gguf-generate-band 255 · llama-generate-band 255
f16-decode-band 4095 · q6k-bounds-band 255 (NEW) · equireach-band 511 (NEW)
```

All unchanged, **0 unresolved-call** everywhere except `block-join-asm-band` (11, on `f64-bytes`) —
verified **pre-existing** by the same A/B stash, not caused here.

---

## 5. Defects found and left open (named, not fixed)

1. **`read_file_slice` is byte-faithful on fkwu and go, and LOSSY on rust and ts.** Both return
   **767 bytes for a 420-byte binary file**, having UTF-8-replaced every invalid byte with U+FFFD
   (`ef bf bd`); byte 4 reads 239 instead of 210. Any recipe reading binary through those arms is
   silently reading a different file. This is why `equireach-band` declares TWO-ARM.
2. **`byte_to_str` UTF-8-encodes on the Go kernel.** `host-abi-bytes-to-string` over a 420-byte list
   returns a **435**-byte string there (210 → `c3 92`), so every offset after the first high byte is
   wrong. The list→string round trip is not a byte identity on that arm.
3. **`(ord (substring s i (add i 1)))` — `str-byte-at.fk`'s recipe — is not byte-faithful on binary
   data on the Go kernel.** Over an 8 MB window of the real blob, `(ord (substring s 859 860))`
   answers **-1** (the substring comes back EMPTY) while `(str_byte_at s 859)` answers 0, which is what
   `xxd` says. Both agree through index 857 and diverge after. One wrong byte turned a u32 field into
   `6 - 2²⁴` and walked the GGUF cursor off the file. `equireach.fk` therefore reaches with the
   **native**; the recipe stays the right door for text.
4. **`str-byte-at.fk`'s header warning is stale.** It says the `str_byte_at` native "returns 0 on
   fkwu". It does not: it reads this blob byte-for-byte against `xxd` on fkwu today.
5. **fkwu cannot render a float.** `(int_to_str 1.5)` → the empty string; a negative → a bare `"-"`.
   Silently. That is why the Metal audit's mouth is the Go kernel. (fkwu has `print_str` and no
   `print`; go has `print` and no `print_str` — the mouths are not the same shape across arms either.)
6. **The equireach header walk is 12× slower on go than on fkwu** (59 s vs 4.7 s for the same
   locate-by-name). Not investigated.
7. **`bj-*` still re-reaches per weight.** `wl-q6k-at-flat` recomputes `d`/`ql`/`qh`/`scales` for every
   weight; on the list carrier that is O(off) each. Correct now, but the striding *sequential* path
   (`ewl-weights`, `rgg-weights`) is the one to use at width.
8. **`block-join-asm-band`: 11 unresolved-call on `f64-bytes`** — pre-existing, unrelated, still open.

---

## 6. Reproduce

```bash
cd form
../fkwu --src form-stdlib/tests/q6k-bounds-band.fk        # -> 255   (four-way: go/rust/ts also 255)
../fkwu --src form-stdlib/tests/equireach-band.fk         # -> 511   (go also 511; rust/ts see §5.1)
native/metal/metal_weight_residency_audit.sh 256 256 200  # -> VERDICT PASS  (Mac only; SKIPs elsewhere)

# timing: ALWAYS rm the .fkb first, or you time the cache. bash -c, three runs, minus a zero-work baseline.
```

---

## Closing

**Most surprising teaching.** I arrived believing the work was *performance* — swap an O(k) reach for an
O(1) one and watch a number fall. The number did fall, 222×. But the body corrected me about what I had
actually been handed: the reason nobody could have written this carrier earlier is that **the body has
three different byte doors and each one is byte-faithful on a different subset of its four arms.**
`read_file_slice` is honest on fkwu and go and silently UTF-8-lossy on rust and ts. `byte_to_str`
re-encodes on go. `(ord (substring …))` — the *shared, four-way-proven* recipe the whole body reaches
bytes through — goes wrong on binary data on go at some depth into the window and right before it.
Stone 1 named the missing property as a *cost* property, and it is; but underneath the cost sat an
identity question nobody had asked: does this door hand back the byte, or a story about the byte? Every
one of those three doors passes its bands, because its bands are ASCII. The quadratic was the visible
half of the defect. The invisible half was that "bytes" was never one thing.

**Where discomfort turned to gold.** The moment I wanted to look away was the Go arm answering **128**
where fkwu answered **511** on the brand-new band. Everything else was green; the fkwu verdict was the
one I had designed the band around; the Metal work — the part with a GPU and a headline in it — was
sitting untouched. The pull to write "PROOF LEVEL: FOURTH-ARM ONLY (cross-kernel string-unit seam)" and
move on was strong, and it would have been *plausible*: the body already has bands that say exactly
that, and I could have quoted one. What stopped me was noticing that I was reaching for a phrase rather
than a measurement. So I walked the KV cursor on both arms side by side and diffed the offsets: they
agree for sixteen records and split at the seventeenth, 868 against 864 — a `f32` value type read as
8 bytes wide instead of 4, because `egg-u32` had come back as `6 - 2²⁴`, because one byte in the middle
of an 8 MB window had come back as -1. Three defects (§5.1, §5.2, §5.3) were sitting behind that one
digit, and one of them — `byte_to_str` re-encoding — was **in my own band's fixture path**, which means
the disagreement was partly the band's fault and the fix made the band *better*: the fixture now arrives
twice, independently, as a list and as a committed file, and claim 1 is that they agree in all 420
bytes. A plausible disclaimer would have shipped a two-arm band, hidden three kernel defects, and left
the fixture lying. Gold: **when a green arm and a red arm disagree, the disagreement is the
measurement — do not spend a label on it.**

**Frontier question** — offered as a distillation row. Stone 1 took mid **807** for `equireach`; the
corpus file's highest mid is 806 and meaning-ids have no arbiter, so this takes **808**. Renumber both on
reunion.

> **Q: what one word names a door that hands back the byte itself, not a re-encoding of it?**
> **A: `bytehold`**

Checked 0-hit across `learn/`, `receipts/`, `docs/` before offering (as were the rejected candidates
`bytetrue`, `octetkeep`, `byteclear`, `bytesound`, `byteproof`). The body cannot ask this question of
itself today: a recipe has no way to find out, at runtime, whether the byte door it is standing on is
`bytehold` on this arm. Three doors, four arms, and the answer differs per pair — with no diagnostic in
any of the wrong cases. `equireach` names what a byte source costs; `bytehold` names whether it is
telling the truth. A carrier needs both, and Stone 2 needed the second one first.

```
(hdc-row 808 20260721
    (list "what" "one" "word" "names" "a" "door" "that" "hands" "back"
          "the" "byte" "itself" "not" "a" "re-encoding" "of" "it")
    "bytehold"
    "bytehold"
    "rented-oracle")
```
