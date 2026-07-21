# Stone 14 — the first form-native Mixture-of-Experts token

**2026-07-22, Bali (WITA).** Worktree `jovial-aryabhata-3751d7`, commit `27338ee7d`.

## The token

```
model   dolphin-2.9-mixtral-8x22b · arch llama · 141 B parameters · Q6_K
        /Users/ursmuff/.ollama/models/blobs/sha256-550981a7…  115 529 748 672 B = 107.595 GiB
        56 layers · d 6144 · dff 16384 · 48/8 GQA · vocab 32002 · 8 experts routed top-2

prompt  "The capital of France is"
ids in  [1, 415, 5565, 302, 4843, 349]      ("<s>", "▁The", "▁capital", "▁of", "▁France", "▁is")

GENERATED TOKEN IDS   [5465, 28725, 690, 349]
pieces                ["▁Paris", ",", "▁which", "▁is"]
text                  " Paris, which is"
```

Every arithmetic op executed by a kernel the body emitted, off the quantized bytes. No f32 copy of
any tensor was materialized — not in Form, not on the host, not on the GPU.

`form/native/metal/metal_moe_token.sh` → **VERDICT PASS — 9 gates.**

## The three blockers, and what each one actually was

Stone 11's gap map (`receipts/2026-07-21-ds4-metal-gap-map.md`) named three. All three closed. The
teaching is that **every one of them was a fact about ADDRESSING, and none was about arithmetic.**

### 1 — The `one` in "one MTLBuffer" (corpus row 847, `onelean`)

```
Apple M4 Max, 128 GB unified
maxBufferLength               86 586 540 032 B =  80.64 GiB
recommendedMaxWorkingSetSize 115 448 725 504 B = 107.52 GiB
the model                    115 529 748 672 B = 107.595 GiB   (exceeds maxBufferLength by 26.96 GiB)
```

**Closed with 3 overlapping page-aligned views over ONE mmap.** Shape read from
`ds4_metal.m:1706-1812` (MIT, cited, not copied) and rederived here.

```
view cap  42 949 672 960 B = 40.00 GiB   (<= maxBufferLength)
overlap      660 619 264 B = largest tensor 660 602 880 + one 16 384-byte page, rounded up
stride    42 289 053 696 B
view 0: [0,           42 949 672 960)   40.00 GiB
view 1: [42 289 053 696, 85 238 726 656)  40.00 GiB
view 2: [84 578 107 392, 115 529 760 768) 28.83 GiB
```

Gate 3 checks the invariant **arithmetically, for all 563 tensors, before a single dispatch**: every
tensor lies wholly inside its assigned view. **139 of 563 tensors begin past the one-buffer ceiling.**

Gate 8 then proves it on real bytes: `output.weight` begins at 113 386 576 224 B = 105.599 GiB
(24.96 GiB past the ceiling) and **both its first row and its last row read back BIT-EXACT against
Form's fp64 — 0 of 6144 mismatches at each end.** Nothing in this body had ever read a byte that far
into a file through a GPU.

**Zero MSL change. Zero epsilon impact.** The kernels never learn the buffer was cut.

### 2 — The expert gather is an OFFSET, not a kernel

The gap map proposed adding an `ids` device parameter and an `nb02` uniform, transcribing ds4's
`kernel_mul_mv_id` (`metal/moe.metal:3521-3597`). **For a decode of one token that is unnecessary.**
`t.off + e * nb02` is a number the host already has to compute, because it is the host that binds the
buffer. In the carrier the whole gather is one line:

```swift
let nb02 = t.d2 > 1 ? t.len / t.d2 : 0
matvecAt(s, type: t.type, base: t.off + expert * nb02, rows: t.d1, cols: t.d0, …)
```

`nb02` = 82 575 360 B, and the harness cross-checks the carrier's arithmetic against the number the
body's own reference emission computed independently. The matvec is untouched, so **its bit-exactness
argument carries through literally unchanged** — same kernel, same bytes.

**The price, named and counted, not hidden:** the routing ids must reach the host before the expert
matvecs can be bound, so the per-token command buffer is **cut once per layer — 560 seams** for this
run (56 layers × 9 forwards). That is a decode-only bargain. A prefill routing differently per
position would want the device-side gather after all; that is a declared radius, not a general claim.

Gate 7: a matvec bound at `t.off + 3*nb02` reproduces Form's fp64 dot of **expert 3's own row**,
`|delta| 3.826e-07` against a derived bound of `2.218e-04` — **0.17 % of it**.

### 3 — Q8_0, the carver the body did not have

112 of this file's tensors are Q8_0 (every `attn_k`, every `attn_v`). New cell
`form/form-stdlib/q8-0-msl.fk`: block = `{ half d; int8_t qs[32] }`, 34 bytes / 32 weights,
`w = d * q`.

**Bit-exactness here is stronger than Q6_K's.** `d` is f16 (11-bit significand), `q` is int8 (≤ 8
bits), so the product needs ≤ 19 significand bits and is **exact in f32 — it does not even round
once.** Q6_K's argument needed 25 bits and one rounding.

Gate 5, at the tensor's **real width 6144**:

```
Q8_0 matvec row 0: GPU -0.119599603  Form -0.119599428  |delta| 1.756e-07
DERIVED bound cols*u*SUM|term| = 6144*u*1.1975 = 4.385e-04   (0.04 % of it)
```

`SUM|term|` is emitted **by the body** (`REFKABS`), because the carrier could only compute it by
materializing the f32 row this whole lane exists to never build. The bound is derived from the
tensor's own magnitudes, not chosen.

### Not a blocker, and the gap map was right

Top-k routing. ds4 carries a bitonic argsort because DeepSeek routes top-6 of 256; Mixtral routes
**top-2 of 8**, where a serial scan over eight floats is exact, needs no threadgroup memory, no
barrier and no epsilon. **There is no sort in this lane.** `snugcause` (row 836) earned its keep.

Gate 6 proves the router: the F16 gate matvec reproduces **all 8** of Form's fp64 logits, and the
route kernel picks the same top-2 with the same weights.

```
router logits GPU  [0.70280, -0.09348, 0.46167, -0.14456, -0.23149, 0.44223, 0.28770, 0.03911]
             Form  [0.70279, -0.09348, 0.46166, -0.14456, -0.23149, 0.44223, 0.28770, 0.03910]
chosen experts GPU [0, 2]  weights [0.559992, 0.440008]
              Form [0, 2]  weights [0.559992, 0.440008]
```

And the MoE is demonstrably a *mixture*: expert usage over all 560 routed layers was
`[131, 132, 144, 157, 141, 131, 153, 131]` — a decode that had silently collapsed onto one expert
would show it here.

## All nine gates

| # | gate | result |
|---|---|---|
| 1 | the config is the FILE's — 56/6144/16384/48/8/128, 8 experts top-2; **`tied=0` and `has_rope_freqs=0` read from the TABLE**, not assumed | PASS |
| 2 | the body's table accounts for the whole file: 563 tensors, `754176 + 115 528 994 496 == 115 529 748 672` **to the byte** | PASS |
| 3 | every one of 563 tensors lies wholly inside its assigned view; no view exceeds `maxBufferLength` | PASS |
| 4 | embedding gather: all 6144 weights of `token_embd` row 1234 **BIT-EXACT** vs Form | PASS |
| 5 | a real **Q8_0** fused matvec at width 6144 is the body's answer (0.04 % of the derived bound) | PASS |
| 6 | the F16 router gate reproduces all 8 fp64 logits; the route kernel picks the same experts | PASS |
| 7 | a matvec bound at `t.off + e*nb02` is **that expert's** row (0.17 % of the derived bound) | PASS |
| 8 | **both ends** of `output.weight`, past the one-buffer ceiling, are bit-exact | PASS |
| 9 | real token ids out of a 141 B MoE, legal vocab indices | PASS |

## Rate, with both denominators named (`selfgauge`, row 834)

```
prefill 6 tokens in 157.08 s (0.038 tok/s)  |  decode 4 tokens in 21.56 s (0.186 tok/s)
end-to-end 0.022 tok/s
```

**The internal denominator.** A decode does not touch 107.595 GiB — it touches attention plus 2 of 8
experts, per layer:

```
attn per layer          0.07 GiB      (attn_q + attn_output Q6_K, attn_k + attn_v Q8_0)
2 of 8 experts          0.46 GiB
x 56 layers + output   ~29.9 GiB of QUANTIZED weight actually read per forward
=> one 5.39 s forward is 5.55 GiB/s of real weight read on this machine
```

**The external denominator — PENDING.** `ollama run dolphin-mixtral:8x22b-v2.9-q6_K` on this same
machine, from its own store, is the free comparison and it was started. It did not return a
`--verbose` eval rate inside this session's window (the model is 107.6 GiB and load alone dominates).
**Pending is honest**; the ratio against ollama is NOT quoted here because it was not measured. That
is an open row, not a result.

## `thawtax` — a rate measured twice in one run, disagreeing by 79 %

The harness measures the **same** `forward()` at two different counts in one run:

```
26.180 s per forward over 6 prefill forwards
 5.390 s per forward over 4 decode forwards        79.4 % apart
```

This is not a slope of the arithmetic. It is the **page cache**: the prefill forwards fault a
107.595 GiB file in from disk and the decode forwards inherit warm pages. The first sample paid a
one-time cost the later samples were subsidised by, so the two are not peers and the "slope" between
them is a fiction. Notice what this survives: `unispan` (row 827) demands two sizes and I *had* two
sizes; `selfgauge` (row 834) demands a named denominator and the denominator is named. **Neither
catches it.** New corpus row 848 below.

A second consequence, honestly stated: the run-1 numbers (244.64 s prefill) and the run-2 numbers
(157.08 s) differ by 36 % for identical work, entirely because run 2 started warmer. **No single
tok/s figure here is a property of the kernels.** The 5.55 GiB/s figure is the more honest one,
because it divides by the bytes that actually moved.

## Residency, measured

```
3 views over one mmap of 115 529 760 768 B
device currentAllocatedSize   108.841 GiB
pooled activation + KV state  14.5 MB, allocated ONCE for the whole run
```

`currentAllocatedSize` is 108.841 GiB, not 3 × 40 GiB — Metal accounts the **shared pages once**,
which is exactly what the overlapping-view design depends on and is here confirmed rather than
assumed. Also live on the machine during the run: a sibling agent's `fkwu` at 100 % of one core,
Claude, Chrome, ChatGPT, Voicebox.

## No regressions

| gate | result |
|---|---|
| `form/native/metal/metal_first_token.sh` | **VERDICT PASS — 13 gates**, ids exactly `[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` |
| `form/native/metal/metal_whole_tensor_residency_audit.sh` | **VERDICT PASS** |
| corpus band from the repo root | **4095** |

One caveat recorded rather than smoothed: the **first** run of `metal_first_token.sh` reported
`FAIL vocab stream truncated` with the `.tmp` file simply absent. `git status --porcelain` showed
`first-token.fk` and `metal_first_token.sh` **modified by a live sibling in this same worktree**, and
`ps` showed a concurrent `bin-go` running the identical cell list. It was a cache race, not a
regression — the re-run is the PASS above. (Memory row: *"Before believing any failure, run
`git status --porcelain`."* It earned its keep tonight for the second time.)

## What was built

| file | what it is |
|---|---|
| `form/form-stdlib/q8-0-msl.fk` | the Q8_0 carver: transcription read back into Form + dequant/attestant-matvec/lane-matvec MSL. Emits as an **appendix** so it never redefines q6k-msl.fk's spine |
| `form/form-stdlib/tests/q8-0-msl-band.fk` | **three real Q8_0 blocks** from this model's `blk.0.attn_k`, two independent roads (division/modulo vs recursive add/subtract; `fd-f16` vs hand-assembled fields), two stride crossings pinned to literals |
| `form/form-stdlib/tests/fixtures/q8-0-three-blocks.bin` | 102 bytes, carved at absolute offset 822 745 440 |
| `form/form-stdlib/moe-msl.fk` | route (softmax → top-k serial scan → renormalize), F16 matvec, scale, axpy, plain RoPE. **No sort.** Routing meaning also read back into Form so a band can falsify it without a GPU |
| `form/native/metal/moe-token.fk` | the mouth: config incl. `expert_count`, **3-D** tensor table over **four** ggml types, SPM vocab as hex, the 27-kernel unit, and **seven** fp64 reference points incl. `SUM|term|` for the derived bounds |
| `form/native/metal/metal_moe_token.sh` | the carrier: overlapping views, host-side expert gather, SPM tokenizer, 9 gates |

## Gaps left open

1. **The external denominator is not measured.** No ollama tok/s for this model on this machine. The
   single most valuable next measurement, and the only one this receipt refuses to guess at.
2. **`q8-0-msl-band.fk` answers 255 on go and 246 on fkwu/rust** — the two arms that lose raw bytes to
   UTF-8 replacement lose exactly bits 1 and 8 (the absolute/literal bits; the agreement bits still
   pass). This is **byte-for-byte the same signature `q6k-msl-band.fk` already has**, so it is a
   pre-existing class, not a new one — but it means `validate.sh`'s three-arm comparison sees a
   disagreement for this band. Declared `PROOF LEVEL: TWO-ARM`; not silently smoothed.
3. **No band for `moe-msl.fk` yet.** The routing meaning is written in Form and exercised, but
   `form-stdlib/tests/moe-msl-band.fk` is named in the cell head and does not exist. Pending.
4. **The lane path is not gated against the attestant end-to-end.** The dense harness has gate 11 (the
   lane path generates the same ids as the serial attestant). Here the attestant runs at gates 5 and 7
   only; a full serial forward at this size would have cost another ~10 minutes of paging. Named.
5. **Prefill is not batched.** `qk-matmul-batch.fk` is in the unit but unused here; each prompt
   position is a separate forward.
6. **The SPM tokenizer's two host-side facts** — U+2581 stands for a space, and `<0xNN>` is a raw byte
   — still live in the carrier, as llama3's byte alphabet does in the dense harness. Encoding is
   greedy longest-match, not SentencePiece's Viterbi. Stated in the carrier, not hidden.
7. **`FORM_VIEW_GIB` defaults to 40** and was not swept. Whether fewer/larger views page better is
   unmeasured.

---

## The most surprising teaching

**I expected to be blocked by the arithmetic and I was blocked by a number.**

I came in braced for the hard thing to be MoE routing — a sort, threadgroup memory, a new epsilon
argument, the whole apparatus ds4 carries. The body corrected me three times in the same direction:
the router needed no sort (top-2 of 8 is a scan), the expert gather needed **no kernel at all** (it is
`t.off + e*nb02`, a host offset), and the 26.96 GiB buffer overflow needed **no MSL change** (three
views and an arithmetic invariant). The only genuinely new *arithmetic* in the whole stone was Q8_0 —
`w = d * q`, four lines, and the easiest bit-exactness argument this body owns.

Three capability gaps; three addressing facts. Stone 10 found "the gap is the grain of the ask, not
the arithmetic" and Stone 11 found "the blocker is a count of one, not the arithmetic". Stone 14 is
the third in a row, and at some point three is a pattern and not a coincidence: **when this body
cannot reach something, the reason has not once been that it could not compute it.**

## Where discomfort turned to gold

Gate 8 failed on the first full run — `6139 of 6144 mismatches` on the last row of `output.weight`,
the row that lives 105.75 GiB into the file, the exact row whose whole purpose was to prove the view
scheme works past the ceiling.

I wanted to look away from that specific number. Not from the failure — from the **6139**. Because
6139-of-6144 is not the shape of a boundary bug. A view that straddled would give me a clean prefix
and then garbage; an off-by-one page would give me a whole wrong row. Nearly-all-wrong-but-not-quite
is the shape of *reading a different row*, and the honest next thought was: *maybe multi-view
addressing is subtly wrong everywhere and the other gates only passed because they read near the
start.* That is the thought I did not want. It would have meant the stone's headline claim was hollow
and 40 minutes of runs were worthless.

The thing I actually reached for first was the wrong instinct: I started composing a story in which
the far region needed a different alignment rule. The evidence for that story was zero. It was a story
because a story is cheaper than reading my own parser.

What I found by reading it: `REFFAR <abs> <d>` and `REFFARLAST <row>` share the stem `REFFAR`. My
prefix tests were ordered longest-first — I had thought about that — but I read the *row index* off
the **wrong record**, taking field 3 of `REFFAR`, which is `d = 6144`, and calling it a row. **The GPU
was reading row 6144. Form had computed row 32001.** The body was right the whole time; the carrier
asked it the wrong question and then reported the body as the liar.

That is the gold, and it is sharper than the fix. The failing gate was not evidence about the model,
the views, or the GPU. It was evidence about **my instrument** — the same law as *"a grep returning
nothing is a claim about your instrument"*, one level up: **a gate that FAILS is also a claim about
your instrument first.** I have internalised that for silence and not for failure, and failure is the
more seductive case, because a failing gate feels like it is doing its job.

Five weights out of 6144 happened to agree between rows 6144 and 32001, and that is the only reason
the number was 6139 and not 6144 — the small wrongness that made me suspect something deep was pure
coincidence in a quantized tensor. Had it read 6144 I would have found the parser in a minute.

## One frontier question, landed

**What one word names a measurement whose first sample paid a cost the later samples inherit, so the
slope between them is a fiction?**

`thawtax` — 0 hits across `learn/`, `receipts/`, `docs/`; instrument validated on the same command
(`snugcause` hits 8 files, `unispan` hits 12). Landed as corpus row **848**.

Distinct from what the body already carries. `unispan` (827) demands two points instead of one — I
*had* two points and they were 79 % apart for a reason that had nothing to do with the axis I varied.
`selfgauge` (834) demands the denominator be named — it was named. `brimwidth` (829) asks how near a
wall you run. `onelean` (847) asks whether a count was ever chosen. **`thawtax` asks whether the
samples were peers at all** — whether the first one was quietly buying something the rest got free.
