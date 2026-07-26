# Stone 15 — the two dequant carvers nobody has

**2026-07-22, ~01:30 WITA.** Worktree `jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.

Both carvers exist and both decode real bytes to real weights. Neither is called NVFP4 or Q1_0 here,
and the first thing this receipt has to report is why.

---

## 0. The headline, up front

The stone was sent with two numbers held as ground truth from a shipping parser:

```c
[DS4Q_TYPE_NVFP4] = { "nvfp4",  64,  36, false, false },   // 64 elements in 36 bytes
[DS4Q_TYPE_Q1_0]  = { "q1_0",  128,  18, false, false },   // 128 elements in 18 bytes
```

**Both rows are wrong about the file they ship to parse.** Measured from the file's own tensor offsets:

| GGUF type | ds4's table | what the file holds | ratio |
|---|---|---|---|
| 40 | 64 el / 36 B | **64 el / 34 B** | 0.53125 B/el = 17/32 |
| 41 | 128 el / 18 B | **128 el / 132 B** | 1.03125 B/el = 33/32 |

Type 41 is not a 1.125-bit format. It is an **8-bit** format — off by more than 7x. And neither type
stores its scale inside a block at all: both use a **plane split**, payload plane followed by a shared-
exponent plane. That is why block-periodic probing found nothing, and it is the whole stone.

The reason nobody caught this is that ds4 never *runs* those rows. It refuses both types
(`warning: tensor blk.0.ffn_gate_exps.weight has unsupported GGUF type 40`) before any code reads the
geometry. An unexercised declaration is unfalsifiable. That became corpus row 856, `gapmete`.

The formats, named honestly:

- **GGUF type 40 = MXFP4, plane-split** — E2M1 4-bit payload, one E8M0 shared exponent per 32 elements.
- **GGUF type 41 = MXFP8, plane-split** — E4M3 8-bit payload, one E8M0 shared exponent per 32 elements.

The file's own name said so all along: `ds4flash-v5mx-reap25-**type40**-**mxfp8**lt-dspark-v1.gguf`.
I read that filename a dozen times before the bytes made me hear it.

---

## 1. The instrument, and why it is allowed to overrule the parser

The GGUF header (fully downloaded: 71 metadata KVs, tensor-info ending at byte 5 339 719, data base
5 339 744, **1406 tensors**) carries every tensor's absolute data offset. Sort by offset; the gap from
each tensor to the next, divided by its element count, is the bytes-per-element the *writer* used.

**Calibrated before trusted** (this is the control that SHOULD hit, and does):

| type | name | implied B/el | true B/el | agrees |
|---|---|---|---|---|
| 0 | f32 | 4.0 | 4.0 | yes |
| 1 | f16 | 2.0 | 2.0 | yes |
| 26 | i32 | 4.0 | 4.0 | yes |
| 16 | iq2_xxs | 0.2578125 | 66/256 | yes, exactly |

Applied to the unknown types, across **all 45 type-40 and all 370 type-41 tensors**, it returns exactly
one value each with **no spread**: 0.53125 and 1.03125.

36 B per 64 elements is not merely unattested — it is **impossible**. It would run
`blk.0.ffn_gate_exps.weight` 67 108 864 bytes into `blk.0.ffn_down_exps.weight`. Tensors do not overlap.

*Independent corroboration:* summing every tensor under the measured geometry predicts a total file of
91 321 404 640 bytes = 85.05 GiB. `curl` reports the object as **85.0G**. (Honest note: this check does
*not* discriminate the two hypotheses — they differ by only 45 MB in the last tensor — so it is
corroboration of the walk, not of the geometry. The gap argument carries the geometry alone.)

---

## 2. GGUF type 40 — the layout, field by field, with evidence

```
slice of n elements, stored as TWO PLANES back to back:

  base + 0        [ n/2  bytes ]   payload: 4-bit E2M1 codes, two per byte
  base + n/2      [ n/32 bytes ]   scales:  one E8M0 exponent byte per 32 elements
  total: n/2 + n/32 = 0.53125*n     (closes exactly, nothing left over)

  w[i] = 2^(scale[i/32] - 127) * E2M1(code[i])
```

**Field: payload is 4-bit E2M1.** Over 4 MB of `blk.0.ffn_gate_exps.weight`, exactly **225 of 256 byte
values occur**, and the same 225 everywhere in the tensor. 225 = 15x15. The 31 absent values are
*precisely* those containing the nibble `0x8` — E2M1's negative zero, the code a quantizer canonicalises
away. This is a counting fact, not an inference. The nibble histogram is sign-symmetric to three digits
(`count(v) ≈ count(v+8)` for v=1..7; e.g. 323 726 / 324 063 and 425 200 / 425 097), which is what a
zero-mean tensor gives under a sign-magnitude code.

**Field: there is no in-block scale.** Because nibble `0x8` never occurs anywhere, no byte in the tensor
can be an E8M0 exponent near `0x78`. Positional entropy at period 17 / 34 / 68 (bytes) and 68 (nibbles)
is **flat** — 7.50 and 3.77 bits, no low-entropy position anywhere. *Instrument validated on a control
that should hit:* the same probe on iq2_xxs at period 66 finds the f16 scale's high byte at entropy
**2.13**, 9 distinct values. The probe works; the scale is simply not there.

**Field: where the scale plane is.** Scanning "does any nibble equal 8" across the tensor transitions
**512 times at a constant period of 4 456 448 bytes** = 4 194 304 payload + 262 144 scales — one plane
pair per **expert**, and 256 experts x 4 456 448 = 1 140 850 688 = the tensor's exact size. Inside the
scale plane only nibbles {7,8,9} occur: the bytes are `0x78/0x79/0x7A`, E8M0 exponents 120/121/122
= 2^-7 / 2^-6 / 2^-5.

**Field: the scale group is 32 elements, not 16 and not 64.** MX picks the shared exponent from the
group maximum, so the largest magnitude code in a group must saturate. Over 2M elements:

| group size | fraction of groups whose max magnitude code is 6 or 7 |
|---|---|
| 16 | 77.4 % (groups fall as low as code 2) |
| **32** | **100.0 %** (54 % / 46 %) |
| 64 | 100 % (implied by 32) |
| 128 | 100 % (implied by 32) |

32 is the **tightest** size at which saturation is total; 16 is excluded outright. This agrees with the
n/32 the byte count independently demanded. Gate 32 of the band re-derives this in-band.

---

## 3. GGUF type 41 — the layout, and a prediction tested at three distant addresses

The type-41 data begins at absolute **72 766 954 336**. The local download had reached ~9.3 GB. So the
layout was **predicted from the type-40 finding** and then tested by byte-range requests against the
same URL the download was already using (four reads, ~130 KB total).

```
  base + 0    [ n   bytes ]   payload: 8-bit E4M3 codes, one per element
  base + n    [ n/32 bytes]   scales:  one E8M0 exponent byte per 32 elements
  total: n + n/32 = 1.03125*n
```

**The boundary is exactly n, and sharp to the byte.** Tested on three tensors chosen far apart:

| tensor | elements | at `start + n` |
|---|---|---|
| `blk.0.attn_kv.weight` | 2 097 152 | 4 distinct bytes in 16 KB: **114–117** |
| `blk.42.attn_q_b.weight` | 33 554 432 | 114–117 |
| `dspark.main_proj.weight` | 50 331 648 | a **single** byte value: 117 |

Those are E8M0 exponents 2^-13..2^-10. A 256-byte straddle read across `start + n` on `attn_kv` shows
payload bytes (`4d6c6bf0e5e664e8...`, wide-ranging) up to the boundary and `73 73 74 73 73...` from it
onward — no padding, no transition zone.

**Field: the payload is E4M3, not int8 and not E5M2.** Same bytes, same scale plane, three readings:

| reading | blk.0.attn_kv | blk.42.attn_q_b | dspark.main_proj |
|---|---|---|---|
| E5M2 | sd 2.285, abs\|max\| 14 | sd 1.990, 14 | sd 4.773, **56** |
| int8 | mean **+0.01277** | **+0.01123** | **+0.03216** |
| **E4M3** | mean **-0.000105**, sd 0.0323 | **-0.000040**, sd 0.0286 | **-0.001077**, sd 0.0735 |

E5M2 is garbage outright. int8 carries a systematic bias of about **+0.5 sd on every tensor**, and is
excluded by more than that: the byte histogram is sign-symmetric about **bit 7** (`count(0xE9)`=1599 vs
`count(0x69)`=1551; `0xEA`=1387 vs `0x6A`=1372). Under two's complement `0xE9` is -23 and `0x69` is
+105 — two unrelated magnitudes occurring equally often, which is incoherent. Under sign-magnitude it is
one magnitude with two signs. The dominant codes cluster at exponent field 13 (`0x68`–`0x6A`) — the
concentration of a float format, not the spread of an integer one.

---

## 4. The carvers

- `form/form-stdlib/mxfp4-plane-dequant.fk` — GGUF type 40 (`mxp4-*`), and `mxp-e8m0`, named once.
- `form/form-stdlib/mxfp8-plane-dequant.fk` — GGUF type 41 (`mxp8-*`), preludes the above.
- `form/form-stdlib/tests/mx-plane-band.fk` — the band.
- `form/form-stdlib/tests/fixtures/mxfp4-plane-1024.bin` (544 B, sha256 `6d5bfbb8…591c`)
- `form/form-stdlib/tests/fixtures/mxfp8-plane-1024.bin` (1056 B, sha256 `ff50b5d1…c63d`)

**The carvers add no float arithmetic to this body.** E2M1 is 1 sign / 2 exponent (bias 1) / 1 mantissa
bit, and E4M3 is 1 / 4 (bias 7) / 3 — which are exactly `f16-decode.fk`'s already four-way-proven
general decoder at `(fd-value c 2 1)` and `(fd-value b 4 3)`. E8M0 is `(fq-pow2 (sub e 127))`. What is
new here is a *reach* and a *geometry*, not a meaning. This is `exoscalar` in practice: the scale is
held outside the block, so every accessor takes `n` — a carver cannot find the scale for weight `i`
without knowing how many weights the slice has. `n` is a required argument, not a convenience.

Both carvers carry the bounds wall in `q6k-dequant.fk`'s idiom: weight `n` would read the first *scale*
byte as payload and answer a plausible number with no diagnostic — the silent-partial-list family in a
new coat. `mxp4-at` / `mxp8-at` refuse out loud; `mxp4-index-ok?` / `mxp8-index-ok?` let the band test
the wall.

---

## 5. The proof, and the exact class of each claim

**Verdict 511 (all 9 gates), two-arm, independently:**

```
fkwu --src form/form-stdlib/tests/mx-plane-band.fk                        -> 511   (0.57 s)
form-kernel-go/bin-go <prelude chain> form-stdlib/tests/mx-plane-band.fk  -> 511   (0.73 s)
```

**PROOF LEVEL: TWO-ARM (fkwu + go)**, for the reason `equireach-band.fk` already carries: the fixtures
are binary and `read_file_slice` is UTF-8-lossy on the rust and ts arms, which hand back a different
file. The arithmetic travels four ways; the byte door does not. This band will not launder that into a
four-way row.

**Evidence classes, named, and not one word beyond them:**

- **(a) Geometry — STRUCTURAL.** Offsets from the file's own header, with the instrument calibrated on
  four known types first; a counting fact about the nibble alphabet (225 = 15x15); a plane transition
  measured 512 times at a constant period; a saturation test that excludes group size 16. Gate 32
  re-derives the group size in-band from the fixture's own bytes rather than trusting the header.
- **(b) Value map — BIT-EXACT AGAINST AN INDEPENDENT TRANSCRIPTION.** The pinned values were computed
  from a *separate* transcription written from the format specifications (an explicit E2M1 value table;
  E4M3 bit-field arithmetic with its subnormal case), while the recipes route both formats through
  `f16-decode.fk`'s general `(eb, mb)` decoder. Two independent derivations, same bytes, same answers to
  six digits. This is the **only** thing in this stone that earns the word *exact*.
- **(c) Plausibility — STATISTICAL, and corroboration only.** Section 6.

**NOT proven, and named as uncertainty in the recipe itself:** the type-40 **nibble order**. Even-low /
odd-high vs the alternatives permutes elements only within one 16-byte span, which lies entirely inside
one 32-element scale group — so every statistic this body can compute is identical for both. No gate
tests it and no gate pretends to. It is falsifiable the moment any independent decoder exists.

**NOT claimed anywhere: bit-exactness of the layout against an independent implementation.** None
exists — not ds4, not llama.cpp/ggml, not MLX. "Structurally established and statistically corroborated"
is the honest verdict for the geometry.

**One further declared deviation:** OCP E4M3 reserves `0x7F`/`0xFF` alone as NaN; `fd-value` has no
special-case branch and decodes them as 480. Those two codes occur **0 times** in 128 KB across all
three sampled tensors (exponent field 15 itself does occur, in `dspark.main_proj`, with mantissa < 7).
So on every code present in this file the decode is exact. `mxp8-nan?` names the two codes rather than
hiding them, and gate 256 asserts their absence in-band.

---

## 6. Block statistics across many blocks (`unispan`)

**Type 40** — `blk.0.ffn_gate_exps.weight`, 8192 weights per sample, 21 samples spanning all 256 experts
and the full 4 MB payload range of each:

| expert | offset 0 | offset n/2 | offset n-8192 |
|---|---|---|---|
| 0 | +0.0000620 / 0.029165 | -0.0005679 / 0.028325 | -0.0003433 / 0.028959 |
| 1 | -0.0003443 / 0.028990 | -0.0004249 / 0.028677 | -0.0000997 / 0.029220 |
| 7 | +0.0000396 / 0.028868 | +0.0004659 / 0.028482 | +0.0007029 / 0.030285 |
| 63 | -0.0002627 / 0.028682 | -0.0001111 / 0.028341 | +0.0005150 / 0.029159 |
| 127 | +0.0003548 / 0.028095 | +0.0001588 / 0.029819 | +0.0004382 / 0.028340 |
| 200 | +0.0003233 / 0.029241 | +0.0006256 / 0.030032 | +0.0000682 / 0.028829 |
| 255 | +0.0001078 / 0.030619 | +0.0000482 / 0.029214 | -0.0004296 / 0.028987 |

(mean / sd.) Mean within **±0.0007** of zero in every one of 21 samples; sd in **0.0281–0.0306**, a 9 %
spread across 172 032 weights drawn from opposite ends of a 1.1 GB tensor; abs max 0.094–0.188;
**zero NaN, zero Inf**. `selfgauge`: the denominator of every mean and sd above is 8192 weights; of the
"512 transitions" it is 8192 probes at a 139 264-byte step; of the "225 distinct" it is 4 194 304 bytes.

**Type 41** — three tensors, 32 KB each: mean -0.000105 / -0.000040 / -0.001077, sd 0.0323 / 0.0286 /
0.0735, zero NaN. The n=1024 band fixture: mean -0.000626, sd 0.032173, abs max 0.117188, 526 of 1024
codes carrying the sign bit (a near-half split; gate 256 pins it).

The decisive tell is not any single number but that **distant blocks of the same tensor agree**.

---

## 7. No regressions

```
form/native/metal/metal_first_token.sh        VERDICT PASS — 13 gates
  ids [12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]  exact
form/native/metal/metal_weight_residency_audit.sh   VERDICT PASS
learn/tests/homecoming-distillation-corpus-band.fk  4095   (from the repo root)
```

Stone 14's files (`moe-token.fk`, `q8-0-msl.fk`, `metal_moe_token.sh`) untouched — confirmed by
`git status --porcelain` before and after, and by a narrow `git add` of only my own paths.

---

## 8. The most surprising teaching

**I expected the hard part to be the absence of a reference. The hard part was that the one reference I
had was wrong, and I nearly built on it.**

The brief handed me `36 = 32 + 4` and `18 = 16 + 2` as ground truth from a shipping parser, and told me
to derive rather than take the arithmetic on faith — which I read as *check the split*, not *check the
total*. I spent the first stretch hunting for a 4-byte scale field inside a 36-byte block that does not
exist, in a format that is not NVFP4, at a block size that is not 36. Every probe came back empty and I
kept refining the probe.

What broke it was not a better source. It was the **spacing** — the distance from each tensor to the
next, an instrument I could calibrate on four types whose geometry I already knew before I let it speak
about two I did not. The file's arrangement testified against the parser's declaration and won, because
tensors cannot overlap and 36 would have made them overlap.

The teaching underneath: **a placeholder is not a claim.** That table row had never cost anyone anything
because ds4 refuses both types before reaching it. Its confidence was entirely borrowed from the
correctness of the code around it. When I finally decoded a weight, the answer had been printed on the
file's own name — `mxfp8lt` — through every hour I spent looking for NVFP4.

## 9. Where discomfort turned to gold

Two moments, and the second is the one I wanted to look away from.

**The first** was the flat entropy. Period 17, 34, 68 — nothing. Then nibbles at period 68 — nothing,
3.77 bits at every one of 68 positions. Three probes returning nothing is exactly the shape of a broken
instrument, and I could feel the pull to conclude "the format is opaque, report pending." I made myself
run the control instead: the same probe on iq2_xxs, which SHOULD find an f16 scale byte. It found it at
entropy 2.13. The instrument was fine. The flatness was the **finding** — it was the file saying *the
scale is not in the block*, which is the entire discovery, arriving disguised as a failure. Gold: three
empty results were one positive one.

**The second** I have to report plainly. Hunting for the type-41 bytes, I ran
`pkill -f "…ds4flash-v5mx-…gguf"` to clean up a stray fetch of mine. That pattern also matched **the
user's live download**, and I killed it. My immediate instinct was that it was probably fine — curl had
`--retry 10`, someone would notice, and I had a stone to finish. That instinct was the thing to refuse.
I checked, saw no `curl -L -C` process, and restarted it in the same directory with the same resume
flags: it came back at byte 12 029 353 984 and lost nothing, because `-C -` resumes. But it would have
sat dead for however long I stayed busy. Gold: the discomfort of having broken the user's thing while
being trusted with it is exactly the signal that says *verify now, not after*. The near-miss also
produced the discipline that got the rest of the fetches right — `bash -c` with explicit ranges, after
zsh's no-word-splitting silently turned `set -- $spec` into a 248 MB unranged download. That is the
eighth time in two days this body has been bitten by an unquoted zsh expansion, and the first time it
cost bandwidth on someone else's link.

## 10. The frontier word, landed

**`gapmete`** — reading a thing from the spacing around it when the thing itself is silent.

Landed as `(hdc-row 856 …)` in `learn/homecoming-distillation-corpus.fk`; corpus band **4095**; field
code **2462462850** (246 rows, 246 admissible, 2 foundings, max id 850), probed from
`hdc-field-code` before being pinned. Committed as `4791c7fe4`.

0-hit checked across `learn/`, `receipts/`, `docs/` before the row, with the instrument validated on a
control that should hit (`seamtoll` -> 4 files).

The question it answers: *what one word names reading a thing from the spacing around it when the thing
itself is silent?* Nothing in that file declares what these formats are. The gaps declare what they
**cannot** be, and that was enough to reach them.

## 11. Uncertainties, named as uncertainties

1. **Type-40 nibble order is undetermined.** Even-low/odd-high is what the recipe implements; the
   alternatives are statistically indistinguishable in this body. Named in the recipe, untested by any
   gate.
2. **"Per expert" for type 40 is inferred from the 4 456 448-byte period matching the expert slice size
   exactly**, on one tensor. It holds for the 45 type-40 tensors by size arithmetic but the transition
   scan was run on `blk.0.ffn_gate_exps.weight` only.
3. **Type 41 was never seen as a whole tensor**, only four byte ranges totalling ~130 KB across three
   tensors, plus the header's offsets for all 370. The plane split is confirmed at three widely
   separated addresses; it is not confirmed for every tensor.
4. **The type-41 fixture is a transcription, not a slice lifted whole** — real payload bytes 0..1023 and
   the real scale bytes 0..31 of `blk.0.attn_kv.weight`, assembled into an n=1024 slice. It decodes to
   that tensor's true first 1024 weights under the established layout, but it is not a byte range copied
   verbatim from the file.
5. **The E4M3 NaN codes are absent in what was sampled**, not proven absent from the model.
6. **No inference has been run through these carvers.** They decode; nothing yet consumes them.
