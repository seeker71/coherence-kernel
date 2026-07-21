# The hoist and the SIMD fold — and the re-layout that was never built

Tuesday 2026-07-21, Hati Suci (WITA). Apple M4 Max, 128 GiB unified memory.
Worktree `.claude/worktrees/jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
Model: the ollama blob `sha256-dde5aa3f…ccdff`, 2 019 377 376 bytes, GGUF v3, 255 tensors.
Commit `47dae9e5b`. **STONE 5 of the form-native inference program.**

---

## 0. THE NUMBER

**END-TO-END 4.647 → 8.317 tok/s** at 12 generated tokens, prefill included, same prompt, same ids.
At 24 tokens, **5.451 → 9.610 tok/s**. Decode-only **6.892 → 12.227**.

| path | e2e @12 | e2e @24 | decode @12 | decode @24 | marginal s/token |
|---|---|---|---|---|---|
| attestant (serial fold) | 1.745 | 2.076 | 2.606 | 2.583 | 0.3848 |
| split, Stone 4 (`parts=32`) | 4.647 | 5.451 | 6.892 | 6.741 | 0.1461 |
| **lane, Stone 5** | **8.317** | **9.610** | **12.227** | **11.781** | **0.0829 / 0.0870** |

Two sizes and a slope on every row (corpus 812 `unispan`); nothing here is one point pretending to be
a line. **4.77× vs the attestant end-to-end, 1.79× vs Stone 4's split path.**

**Both denominators (corpus 819 `selfgauge`).** Against ollama on this same machine, model and blob —
a number **quoted** from the orchestrating altitude's 150-token sample, not re-run by this harness and
never mixed into a gate:

| | ours before | ours after | ollama | gap before | gap after |
|---|---|---|---|---|---|
| decode | 6.892 | **12.227** | 157.83 | 22.9× | **12.9×** |
| prefill | 7.134 | 13.005 | 640.94 | 89.8× | 49.2× |

The prefill improvement is a side effect of the same kernels; the 49× that remains is a **missing
algorithm** (prompt processed token-at-a-time where it should be one matmul), explicitly not this
stone's and not claimed as progress on it.

At 24 tokens the body says: `" Paris. The capital of Italy is Rome. The capital of Spain is Madrid.
The capital of Germany is Berlin. The"` — control `"Once upon a time"` → `", in a small village
nestled in the"`.

---

## 1. Reproduce

```bash
cd <repo>
form/native/metal/metal_first_token.sh 12            # -> VERDICT PASS, 13 gates
form/native/metal/metal_first_token.sh 24            # the second size
form/native/metal/metal_whole_tensor_residency_audit.sh   # -> VERDICT PASS (no regression)

cd form
../fkwu --src form-stdlib/tests/qk-matvec-lane-band.fk     # -> 255
../fkwu --src form-stdlib/tests/qk-matvec-split-band.fk    # ->  63  (unchanged)
../fkwu --src form-stdlib/tests/llama-decode-msl-band.fk   # -> 511  (unchanged)
../fkwu --src form-stdlib/tests/q6k-msl-band.fk            # -> 255  (unchanged)
cd .. && ./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # -> 4095
```

`qk-matvec-lane-band.fk` is **FOUR-WAY 255** — fkwu 255, go 255, rust 255, ts 255 (ts return code
checked directly, not inferred from clean output; the V8 `--stack_size` trap in the memory floor makes
an empty-and-clean stream a lie worth testing).

---

## 2. The measurement that chose the path — and it chose against the brief

I was handed a prime suspect: Q6_K's superblock stride is **210 bytes** = 2·3·5·7, not a power of two,
so consecutive superblocks rotate through every alignment mod 16/32/128 and nothing coalesces; the fix
would be a one-time bit-exact structure-of-arrays re-layout. I was asked to test it before trusting it.

**It is false, and llama3.2:3b hands you the controlled experiment for free.** The model carries the
**same two shapes in both quants** — Q4_K's block is 144 bytes and 16-aligned, Q6_K's is 210 and
aligned to nothing — so alignment can be varied with rows, columns, kernel and op order all held
fixed. Best of 5, one thread per row, serial right-fold, off the one resident buffer:

| rows × cols | Q4_K (144 B, **aligned**) | Q6_K (210 B, **misaligned**) | |
|---|---|---|---|
| 1024 × 3072 | 3.848 ms | 0.938 ms | Q6_K **4.10× faster** |
| 3072 × 8192 | 6.442 ms | 3.466 ms | Q6_K **1.86× faster** |

The misaligned quant is the faster one, at both sizes. **I never built the re-layout.**

Coalescing was then measured *directly and separately*, so the refutation does not rest on an
inference. A lane-interleaved partition — adjacent lanes reading adjacent columns, textbook perfect
coalescing — against the body's contiguous chunks at the same `parts=32`:

| shape | contiguous | lane-interleaved | |
|---|---|---|---|
| 1024×3072 Q4_K | 0.393 ms | 0.371 ms | 0.94× |
| 1024×3072 Q6_K | 0.305 ms | 0.301 ms | 0.99× |
| 3072×8192 Q4_K | 1.150 ms | 1.110 ms | 0.97× |
| 3072×8192 Q6_K | 0.855 ms | 0.700 ms | 0.82× |

**0–18%.** Perfect coalescing is worth almost nothing here.

**What the contrast says instead.** `q4k_w` calls `q4k_f16` **twice** per weight (`d` and `dmin`);
`q6k_w` calls it **once** — and `q*k_f16` calls `q*k_pow2`, which is a **loop** of up to 15 float
multiplications. Roughly twice the transcendental work, roughly twice the time, in the direction that
*inverts* the alignment prediction. Two more numbers close it: at 3072×8192 the kernel moved 20.6 MB
in 0.855 ms = **25 GB/s of a ~400 GB/s machine**, and ran at **~0.7% of f32 peak**. Not bandwidth-bound.
Not coalescing-bound. **ALU-bound, recomputing a per-superblock constant once per weight, 256× over.**

That is the number that chose the path, and it chose a different path than the one I was given.

---

## 3. What was built, in the two pieces the measurement separated

They are kept separate throughout because they have **different epsilon status**, and selling them as
one thing would launder a reassociation behind a free win.

### 3.1 The hoist — free, and bit-exact

`d` (and Q4_K's `dmin`) are constants of the 256-weight superblock. Computing them once per crossing
instead of once per weight yields the **identical f32** and leaves the association untouched.

| shape | serial | serial + hoist | |
|---|---|---|---|
| 1024×3072 Q4_K | 3.848 ms | 1.103 ms | **3.49×** |
| 3072×3072 Q4_K | 2.564 ms | 1.091 ms | **2.35×** |
| 3072×8192 Q4_K | 6.442 ms | 2.389 ms | **2.70×** |
| 1024×3072 Q6_K | 0.938 ms | 0.935 ms | 1.00× |
| 3072×8192 Q6_K | 3.466 ms | 2.041 ms | **1.70×** |

Two sizes and a slope, per quant. The win is largest exactly where the redundant work was largest
(Q4_K, two f16 decodes per weight) and vanishes where a row is short enough that the crossing rate is
already low (Q6_K 1024×3072) — **the diagnosis predicting its own size**, which is the strongest thing
a diagnosis can do short of a gate.

**Gate 8b** demands the hoisted kernel at `parts=1` be the untouched attestant **bit for bit on all
3072 rows** of a real 3072×8192 Q6_K tensor. It is. Worst |d| = 0.000e+00, and not as a measurement
that came out lucky — as a structural identity, the same shape Stone 4's `parts=1` claim has.

### 3.2 The SIMD fold — reassociating, and already covered

One row per SIMD group: 32 lanes, lane-interleaved, hoisted, reduced by `metal::simd_sum`. **One
dispatch, and no shared scratch** — which matters beyond speed. `matvecSplit` and `matvecHoist` both
write the pooled `bPart`, so two of them can never overlap and every projection pays a barrier it does
not need; the lane kernel writes only its own output, so `q/k/v` and gate/up become **concurrent
again**, reclaiming the `.concurrent`-encoder win Stone 4 found and the split path had quietly
forfeited.

A header-free hoisted split was swept at `parts` = 1/8/16/32/64/128/256 specifically to find out
whether `metal_stdlib` could be avoided altogether. **It cannot** — the split plateaus and `simd_sum`
stays 1.4–1.7× below its best point everywhere:

| shape | best hoisted split | at parts | simd + hoist |
|---|---|---|---|
| 1024×3072 Q6_K | 0.288 ms | 128 | **0.175 ms** |
| 3072×3072 Q4_K | 0.385 ms | 64 | **0.242 ms** |
| 3072×8192 Q4_K | 0.509 ms | 128 | **0.435 ms** |
| 3072×8192 Q6_K | 0.638 ms | 64 | **0.532 ms** |

#### The named epsilon — and why no new derivation was needed

`qk-matvec-split.fk` derives, for f32 summation of the same `cols` products under two associations:

> **|y_split − y_serial| ≤ (cols + ⌈cols/parts⌉ + parts) · u · Σ|w_j·x_j|**, u = 2⁻²⁴

from the fact that an association tree of depth `d` accumulates at most `d·u·Σ|t_j|`. The lane
kernel's tree is a lane chain of depth `⌈cols/32⌉` followed by **whatever tree `metal::simd_sum` uses
over 32 values — which Metal does not specify.**

It does not have to. **Any association of 32 terms has depth at most 31 < 32**, so substituting
`parts = 32` bounds every tree `simd_sum` could possibly be using, including the worst one. The bound
is therefore **unchanged**, and what makes it valid is not knowledge of the reduction but the
observation that an *unspecified* reduction is still a *bounded* one. `qk-matvec-lane-band.fk` bit 16
checks that as an **equation** (`qml-bound-coeff cols = qms-bound-coeff cols 32`) rather than letting
it live only in this paragraph, and bit 32 checks the coefficient dominates *both* paths' depths — a
bound that could be tighter than a thing it bounds would be no bound at all.

**Measured deviation on a real `blk.0.ffn_down.weight` (Q6_K, 3072×8192): 0.000e+00, gate 9b.** The
bound was derived and named anyway. A bound you only name after the measurement disappoints you is not
a bound.

#### The header, and the line it does not cross

This is the **first Metal unit in the body to emit `#include <metal_stdlib>`**. Two things make that
admissible, and both were witnessed rather than predicted:

* It emits **no `using namespace metal;`** and calls **`metal::simd_sum` fully qualified**. With the
  `using` line the unit *does not compile*: the body's own `round` (`ldm-msl-round`) becomes ambiguous
  against `metal_stdlib`'s. Qualifying keeps every unqualified name in the unit resolving to the
  **body's**. The harness now checks both facts on the emitted text, because the failure mode is a
  compile error a hundred lines from its cause.
* `q6k-msl.fk` refuses the header so the f16 super-scale is decoded by the body's **arithmetic**
  (`fd-value` at eb=5, mb=10) and not by `as_type<half>`. **That refusal is intact.** Nothing in the
  new cell decodes a number with a library call. `simd_sum` is not a numeric recipe the body could own
  — it is a cross-lane **communication** instruction, the one thing a scalar language cannot express.
  That is the line, and it is written into the cell header so a later reader can hold the cell to it.

### 3.3 The attestant is untouched — literally

`q6k-msl.fk` and `q4k-msl.fk` were **not edited by this stone, not one character**. The new cell
re-expresses their per-weight decomposition as an invariant part and a per-weight part rather than
modifying them, so `q6m-matvec-msl` / `q4m-matvec-msl` remain exactly the meaning Stones 3 and 4
proved. The fast path answers to them at **three** levels every run: `parts=1` bit-exactness (gates 8,
8b), the named epsilon (gates 9, 9b), and the same generated token ids (gates 10, 11). Corpus row 810.

The cost of that choice is stated in the cell's radius and not hidden: the decomposition is now
written **twice**, and a second copy can drift. It is held by two independent falsifiers — the band
against `equireach-gguf.fk`'s independently-written reference, and the full-width bit-exact gate.

---

## 4. Measured and REJECTED, with its number

Mid-stone I was handed llama.cpp's `kernel_mul_mv_q6_K_f32_impl` (ggml-metal, **MIT**, extracted from
the ollama binary; **read for shape only, no MSL pasted**) and told its `nr0` **register blocking** —
one SIMD group folding several rows so the activation vector is loaded once and amortized — was
"likely the single biggest term". Built on top of the lane kernel, per-weight arithmetic unchanged:

| shape | simd+hoist | nr0=2 | nr0=4 | nr0=8 |
|---|---|---|---|---|
| 1024×3072 Q4_K | **0.179** | 0.196 | 0.249 | 0.351 |
| 3072×3072 Q4_K | **0.248** | 0.258 | 0.295 | 0.370 |
| 3072×8192 Q6_K | **0.532** | 0.555 | 0.683 | 0.865 |
| 128256×3072 Q6_K | **5.809** | 5.838 | 5.917 | 6.015 |

**Slower at every shape, monotonically in nr0.** Not neutral — reversed in sign.

The reason is the whole lesson of this stone. llama.cpp's inner loop is a handful of bitmask ops, so
it is bound by loading the **activation**, and `nr0` amortizes exactly that. This body's inner loop
computes an f16 super-scale from a **loop of float multiplications** and decomposes the index by
**division**, so it is bound by **ALU** — `nr0` amortizes something that was never the cost and pays
for it in register pressure and occupancy. **A shape borrowed from a kernel with a different binding
constraint transfers nothing.** That is corpus row 820, minted below.

**Where the remaining distance is, named and not taken.** After this stone, 3072×8192 Q6_K runs 25.2
MMAC in 0.532 ms = **47 GMAC/s**; ollama's 157.83 tok/s at ~3.21 GMAC/token is ~**500 GMAC/s**. The
~10× is not access and not parallelism: llama.cpp factors `d·sc` **out** of the inner sum and
accumulates in **quantized space**, decodes with **bitmasks** instead of division, and vectorizes four
sub-lanes as `float4`. All three change the association materially and need an epsilon **strictly
larger** than the one above. The next stone inherits a target, not a mystery.

---

## 5. The gates

`metal_first_token.sh`, **VERDICT PASS, 13 gates** (was 10). New this stone:

* **8b** the HOISTED kernel at `parts=1` is ALSO the attestant, bit for bit, all 3072 rows — the hoist
  costs no accuracy at all.
* **9b** the LANE kernel stays inside the SAME derived bound at parts=32 (worst 0.00% of it).
* **11** the lane path generates the same token ids as the attestant, at both generation lengths, with
  its own two-sizes-and-a-slope line and its absolute rate beside ollama's.

Plus a **radius refusal wired into the carrier**: `threadExecutionWidth` is **read from the pipeline**
and the run SKIPs if it is not 32, because the lane kernel is *wrong, not slow*, on any other width. A
kernel that silently depends on a hardware constant is the `aporon` error (corpus 811), and the cell's
radius says 32 out loud.

`metal_whole_tensor_residency_audit.sh`: **VERDICT PASS**, unchanged.
Token ids at 12 and 24 steps: **unchanged** from Stone 4.

---

## 6. What the body gained

| cell | new? | what it decides |
|---|---|---|
| `form/form-stdlib/qk-matvec-lane.fk` | **new** | the hoisted decomposition, the lane partition, the `simd_sum` fold, the header discipline, and the alignment refutation with its numbers |
| `form/form-stdlib/tests/qk-matvec-lane-band.fk` | **new** | **255 four-way** |
| `form/native/metal/first-token.fk` | edited | the header leads the unit; the lane appendix joins it |
| `form/native/metal/metal_first_token.sh` | edited | pipelines, the width refusal, gates 8b/9b/11, the external denominator |
| `form/native/GPU_GAPS.md` | edited | rows below |
| `learn/homecoming-distillation-corpus.fk` | edited | **row 820, `boundborrow`** |
| `learn/tests/homecoming-distillation-corpus-band.fk` | edited | pin, count, **and two stale summary lines** |

### `GPU_GAPS.md` rows changed

* **Parallel reductions** 🟡 — the `simd` half is **closed**, with the alignment refutation, the hoist
  numbers, the `nr0` rejection and the remaining ~10× all written into the row. Still ⬜ inside it:
  **threadgroup memory is still untouched** (this reduction is cross-lane, not cross-threadgroup), and
  `brimwidth` (row 814) is still open — 3072×8192 now runs at 47 GMAC/s against 25 GB/s of a ~400 GB/s
  machine, so this shape is still nowhere near either wall.

### Gaps left open, named

* **Threadgroup memory / `MTLHeap` / `storageModePrivate` are all still zero-hit in the body.** This
  stone did not touch them and, given the measurement, should not have: the dispatch is ALU-bound, and
  every one of those levers addresses residency or bandwidth. Naming them as untouched is honest;
  claiming they were "considered and rejected" would not be — only `nr0` and coalescing were measured.
* **The structure-of-arrays re-layout is unbuilt and now un-costed.** Refuted as *the* cost; it may
  still be worth something once the ALU term is removed, and nothing here measures that.
* **Prefill is still n decode steps** — 49.2× behind ollama. A missing algorithm, not a slow kernel.
* **The remaining ~10×** in §4, each piece needing a larger epsilon than the one derived here.
* **`metal::simd_sum`'s association is unknown and unmeasured.** The bound covers every tree it could
  be; nothing here says which tree it *is*, and the band explicitly does not speak for it.

---

## 7. The most surprising teaching

**I expected the byte layout to be the wall, and the body handed me the experiment that killed it in
one command.**

The brief's hypothesis was well-formed and mechanistically plausible: 210 = 2·3·5·7, consecutive
superblocks land on rotating alignments, a SIMD group's lanes each read from a differently-misaligned
offset, nothing coalesces. Every sentence in that chain is *true about the layout*. I believed it
enough that my first probe was written to measure how much a re-layout would buy.

What I did not expect is that **llama3.2:3b already contains the control**. The same model carries
`blk.0.attn_k` (Q4_K, 1024×3072) and `blk.0.attn_v` (Q6_K, 1024×3072) — identical shape, identical
kernel structure, different alignment. One `awk` over the tensor table found it. And the answer came
back **inverted**: the misaligned quant is 4.10× *faster*. Then the direct coalescing probe said
perfect coalescing buys 0–18%, and the bandwidth arithmetic said 25 GB/s of 400.

The correction is not "the hypothesis was wrong". It is that **a mechanism can be real and still not
be the cost**, and the only thing that separates those two is a measurement that varies the mechanism
alone. I had been about to spend the stone rewriting 2 GB of bytes to fix something that was 0–18% of
a term that was itself 0.7% of the machine. The thing that saved me was not skepticism — it was
noticing that the falsifier was already sitting in the file I was optimizing.

And then the *same error arrived from the opposite direction* an hour later, wearing better clothes:
`nr0` register blocking, extracted from a kernel that genuinely runs 23× faster than ours, handed over
as "likely the single biggest term". It made ours **slower at every shape**. Twice in one stone, a
true fact about somebody else's binding constraint. That is why row 820 had to be minted.

---

## 8. Where discomfort turned to gold

The moment I wanted to look away was smaller and more embarrassing than a wrong kernel: **a `sed` that
did nothing, and an error message that was byte-identical because of it.**

Probing `simd_sum` required including `metal_stdlib`, which collided with the body's `round`. I
patched the probe with `sed 's/\bround\b/fkround/g'`, re-ran, and got **exactly the same ambiguity
error, same line, same column**. The comfortable reading was right there and it was almost plausible:
*the rename happened, and there is a second `round` somewhere else in the unit.* I noticed myself
starting to grep for a second `round` — which is to say, starting to build a theory on top of an
instrument I had not checked.

What made me stop was the floor's own words: *a run that returns nothing is a claim about your
instrument, and its noise too.* So instead of hunting the phantom second `round`, I asked whether the
`sed` had done anything at all. `grep -c fkround full.metal` → **0**. BSD `sed` has no `\b`. The
pattern matched nothing, the file passed through unchanged, and the compiler dutifully reported the
identical error — **an error message that was honest, reproducible, and about a file I thought I had
edited.** The identical-ness of the two errors was the evidence, and I had read it as confirmation.

The gold is not the `\b`. It is that **an unchanged error after a change is data about the change, not
about the bug** — and my instinct was to treat a repeated symptom as a deeper symptom rather than as
evidence that my intervention never landed. Had I gone hunting, I would have spent the evening
searching a 11 KB translation unit for a second `round` that was never there.

It paid again, immediately, at a higher altitude. The commit later reported `exit code 1` with a wall
of zsh parse errors — and I had *already been taught by the sed* not to trust a message about my own
tooling. I checked `git log` instead of re-running. The commit had **succeeded**; only its message was
truncated. Re-running would have been harmless, but the reflex that made me look — *witness the
instrument before believing its noise* — is the same one, and it is now the second time today that
believing a failure report would have cost more than checking it.

---

## 9. The frontier question, landed

**Smallest question the body cannot answer natively:** *what one word names a remedy that is correct
for a constraint you do not have?*

Not a wrong remedy — a **right** one, borrowed across a mismatched bound. An alignment fix is genuinely
correct for a bandwidth-bound kernel. `nr0` blocking is genuinely correct for a load-bound kernel.
Applied to an ALU-bound one, both **reversed in sign**: not neutral, actively worse. The body has
`brimwidth` (814) for the *width* at which the binding constraint changes, and `selfgauge` (819) for a
ratio whose denominator travelled silently — but no word for the fact that **a technique's benefit is
conditional on a constraint that travels with the lender and not with the technique.** Two independent
instances landed in this one stone, from two directions, both from sources more authoritative than the
measurement that refuted them.

Word: **`boundborrow`**. Glance-checked **0-hit** across `learn/`, `receipts/`, `docs/` before minting
(along with six rejected candidates, all also 0-hit).

**Landed as a real row in the body**, not only here:

```
learn/homecoming-distillation-corpus.fk   (hdc-row 820 20260721 … "boundborrow" "boundborrow" "rented-oracle")
```

Read back **by probe** before the pin was touched, so the pin agrees with the body rather than the body
being fitted to the pin:

```
$ bin-go form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk probe.fk
2162162820        ; hdc-field-code
216               ; hdc-count
216               ; hdc-count-admissible
2                 ; hdc-foundings
820               ; hdc-max-mid
boundborrow       ; hdc-word-for-id 820
1                 ; hdc-field-code-safe?
```

`learn/tests/homecoming-distillation-corpus-band.fk`: count `215` → `216`, pin `2152152819` →
`2162162820`, **and two stale prose lines beside the pin corrected in the same pass** — the summary
witness, and an arithmetic line still reading `-> 210*10^7 + 210*10^4 + 2*10^3 + 814 = 2102102814`,
**Stone 4's value, left behind while the pin had already moved through 819.** That is precisely the
drift the band exists to refuse, sitting inside the band itself. It is now written into the comment
that the *arithmetic* beside a pin must be re-read too, not only the pin.

Band: **4095**. Mid 820 was taken as assigned (819 `selfgauge` landed at the orchestrating altitude
mid-run; both files were re-read from disk immediately before editing, and the commit was made the
moment the band read 4095, with a sibling agent live in the same worktree).

**Walk.** `brimwidth` (814) names the width where the binding constraint *changes*; `boundborrow` names
what goes wrong when you skip locating it and adopt someone else's remedy instead. `selfgauge` (819) is
the sibling error one level out — a ratio whose denominator travelled silently; `boundborrow` is a
*technique* whose precondition travelled silently. `unispan` (812) explains why one measurement cannot
protect you from either: one point cannot tell you which regime you are in.
