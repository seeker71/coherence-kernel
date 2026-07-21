# Stone 10 — the ISA diff against the floor

2026-07-21, ~23:55 WITA. Worktree `jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
M4 Max, 40 GPU cores, Metal 4, GPU family `applegpu_g16s`. Metal toolchain v17.3.7003.10.40D3Y4.

The stone was set to read the instruction stream instead of guessing at mechanisms, because two
mechanism hypotheses had been formed at altitude that day and both were refuted. A third was formed
in this stone's own framing — *"the remaining 12.9x decode gap is almost entirely decode arithmetic"* —
and **the body refused that one too.** It is not decode arithmetic. It is the grain of the ask.

---

## 0. The instrument floor, found first and honestly

Apple ships no AGX assembly printer on this machine. Both doors were opened and both are closed:

```
$ xcrun metal-objdump --version | grep -A4 'Registered Targets'
    agx1 - AGX1
    agx2 - AGX2
    agx3 - AGX3

# door 1 — serialize an MTLBinaryArchive (native code) and disassemble it
$ ./binarch ours.metal ours-arch.metallib form_q6k_matvec_f32 ...    # succeeded, 80304 bytes
$ dd if=ours-arch.metallib of=ours-g16s.bin bs=1 skip=$((0x7280)) count=$((0xc730))
$ xcrun metal-objdump -d ours-g16s.bin
  ours-g16s.bin: file format mach-o 64-bit apple gpu
  error: 'ours-g16s.bin': no instruction printer for target agx3---macho

# door 2 — ask the AIR Native Translator for assembly
$ xcrun applegpu-nt -arch applegpu_g16s -S -platform_version macos 26.0 26.0 ours.ll -o ours.s
  note: [AGX] Plugin interface not implemented: AIRNTEmitAssembly
  error: plugin 'wrapper-nt.dylib' cannot emit assembly
```

The native slice **is** there and **is** identified (`applegpu_g16s`, `__TEXT` 48336 bytes for four
kernels, via `metal-size`). Only the printer is withheld. So there is no native instruction stream to
diff on this host, and any receipt that claimed one would be fabricating.

**What was used instead, and its radius:** AIR — post-`-O2` LLVM IR, one level above the ISA, emitted
by `xcrun metal -O2 -std=metal3.0 -ffp-contract=off -fno-fast-math` and read back with
`xcrun metal-opt -S`. AIR is a fair place to count *loads*, *integer divides* and *control flow*,
because those survive lowering. It is **not** fair for counting total instructions: the AGX backend
still unrolls (`FOR_UNROLL`'s `llvm.loop` metadata is unresolved at AIR), still does SROA on `alloca`
arrays, and still strength-reduces. Every count below is labelled AIR, and every ratio that matters
is corroborated by **wall time**, which needs no printer.

---

## 1. The two kernels, side by side, on the same weights

`form/native/metal/metal_isa_diff.sh` (committed). It emits our MSL from the body resolver-driven
(23 cells, 18274 bytes, `ft-emit-msl`), reassembles ggml's Q6_K matvec verbatim from the MSL
recovered out of the ollama binary, compiles both with the same flags, and runs them in one process
over the same real llama3.2:3b tensors, at three shapes.

ggml's kernel is `kernel_mul_mv_q6_K_f32_impl`, MIT, at `ollama-strings.txt:130403`. Only its
surrounding declarations were reassembled (`block_q6_K` :122005, `ggml_metal_kargs_mul_mv` :123853,
`FOR_UNROLL` :124390, `FC_mul_mv_nsg` :126864 at `FC_MUL_MV = 600`, `N_R0_Q6_K = 2` :123543).

**The transcription is not trusted, it is proven.** Variant V2 below sums the same terms in the same
association as ggml, so the check is an *equality*, not an epsilon — and it holds at every row of
every shape: `max|Δ| = 0.000e+00` over 3072, 1024 and 128256 rows.

`selfgauge`: the denominator of every "per weight" below is **one weight of one row — one MAC.**
Timings are the **minimum of three runs**, with all iterations of a run encoded into ONE command
buffer, so the ~0.2 ms per-dispatch round trip (larger than several of these kernels) is not timed.
Before that correction, ours measured 3.69x of ggml at ffn_down; the overhead was flattering us.

### Wall time, three shapes (`unispan`)

| kernel | arithmetic | thread map | ffn_down 3072x8192 | attn_v 1024x3072 | output 128256x3072 |
|---|---|---|---|---|---|
| `form_q6k_matvec_lane_f32` (the body's, today) | div/rem | flat index | 0.4553 ms — **10.83x** | 0.0691 ms — **7.24x** | 6.5365 ms — **8.36x** |
| V1 — bit ops, our map | bit | flat index | 0.1860 ms — **4.43x** | 0.0311 ms — **3.26x** | 2.5452 ms — **3.26x** |
| V3 — our arithmetic, ggml's map | div/rem | 4-wide slot | 0.0567 ms — **1.35x** | 0.0115 ms — **1.20x** | 0.8288 ms — **1.06x** |
| V2 — bit ops, ggml's map | bit | 4-wide slot | 0.0613 ms — **1.46x** | 0.0119 ms — **1.25x** | 0.9043 ms — **1.16x** |
| ggml `kernel_mul_mv_q6_K_f32` | bit | 4-wide slot | 0.0420 ms | 0.0095 ms | 0.7818 ms |

`unispan` says what varies: the ratio is **not one number**. Ours runs 7.24–10.83x behind, and the
gap widens with the column count — 10.83x at cols=8192 against 7.24x and 8.36x at cols=3072. That is
the signature of a **per-weight** cost, not a per-row or per-dispatch one.

---

## 2. What the diff says, and the moment it refused the frame

Read the V1 and V3 rows together. They are the whole stone.

* Healing **only the arithmetic** — every `q6k_mod`/division replaced by `&`, `>>`, thread map kept —
  bought **2.4x** (10.83 → 4.43).
* Healing **only the thread map** — ggml's 4-wide slot, but every division and remainder of the
  body's own arithmetic **kept in place** — bought **8.0x** (10.83 → 1.35).

  (Two runs of the whole instrument were taken; the same three comparisons came out 2.4x/7.0x and
  2.4x/8.0x. `unispan` again: the map is worth **7.0–8.0x** and the arithmetic **2.4–2.6x**, and
  quoting either as a single figure would be quoting a point as a line.)
* And once the map was right, **the arithmetic form was worth nothing at all.** V3, which has
  `q6k_mod` everywhere, is not slower than V2, which has none. It is faster at all three shapes
  (1.35/1.20/1.06 against 1.46/1.25/1.16) — inside noise, and certainly not the 2.4x it was worth in
  the other map.

So the divisions were never the cost. They only **became** a cost, and here is the mechanism, read
off the AIR rather than reasoned about:

Our kernel asks `q6k_w(qb, flat_index)` — one weight, one flat index, pure. That signature makes the
Q6_K field selector `g` a **per-weight runtime value**. Therefore:

```llvm
; ours, form_q6k_matvec_lane_f32, block 127 — inside the innermost loop
%137 = udiv i8 %121, %128          ; %128 = phi i8 [1, %91], [%125, %122], [4, %126]  <- q6k_pow4(g)
```

a genuine **variable-divisor integer division, once per weight**, on a GPU with no integer divide
unit — plus the three-way `switch i32 %104` that produces that phi, landing in the innermost loop.
In the slot map `g` is loop-invariant, so the very same `q6k_mod(ah / 16, 4)` expressions have
constant divisors and fold to shifts and masks: **`int_div = 0` in V3's inner block.**

And the same signature forbids reuse. A function of one flat index cannot be told that the caller's
next fifteen calls share its scale byte. So it re-loads what it already had — correctly, every single
time. No call is wrong. No optimisation inside the callee can reach it.

### The counts (AIR; denominator = one MAC)

Amortised over the loop nest each block belongs to. `dev_load` is a load from `addrspace(1)`.

| | AIR instr / weight | device loads / weight | of which quant bytes | int_div / weight |
|---|---|---|---|---|
| ours `lane` | **≈ 82.8** | **4.25** | 3.25 | **5** |
| V1 (bit ops, flat map) | ≈ 74 | 4.25 | 3.25 | 0 |
| V3 (slot map, our arithmetic) | ≈ 24.6 | 2.125 | 1.00 | 0 |
| V2 (slot map, bit ops) | ≈ 24.4 | 2.125 | 1.00 | 0 |
| ggml | ≈ 25.4 | **1.5625** | 0.75 | 0 |

Derivation of ours: blocks 91 (33) + 122/126 (~2.5) + 127 (40) per weight, plus the superblock
crossing (blocks 33…84, 58 instructions) once per 8 weights = 7.25. Of ggml: block 197 (71) per 4
weights, block 160 (38) per 16, block 116 (35 x 4 iterations) per 32 weights (`nr0 = 2` shares the
activations across two rows), blocks 107/147/150/157 per 16 or 32.

Two things to read out of that table, and only the second one is comfortable:

1. **Instruction count is not the currency.** V1 removes 5 integer divides and a switch per weight —
   an 11% cut in AIR instructions — and runs **2.4x faster**. The count moved a little and the time
   moved a lot, because a variable `udiv` and a divergent branch in the innermost loop are not one
   instruction's worth of cost each.
2. **The residual 1.2–1.4x between V2/V3 and ggml is loads, and it closes arithmetically.**
   2.125 / 1.5625 = **1.36**, against a measured 1.35 at ffn_down. The difference is ggml's `nr0 = 2`:
   each `yl[]` activation load serves two rows, halving activation traffic (0.5 vs 1.0 per weight).
   Nothing mysterious is left over.

`snugcause`: the divide-per-weight fits so well it would have been a satisfying place to stop — and
it is where V1 alone would have left this. V3 is the counterexample that was built on purpose to
break it, and it did: the same divides, in a different map, cost nothing. The mechanism is one level
further out than the instruction that seemed to carry it.

`boundborrow`: ggml's choices are correct for ggml's constraints. `nr0 = 2` and the 4-wide slot are
right *there*; what this receipt claims is only that the same shape measures faster *here*, on our
weights, on our device, in our process.

---

## 3. What the JIT should emit differently

Grounded in the above, not in theory:

1. **`q6k_w(qb, flat_index)` is the wrong signature for a matvec, and no arithmetic healing inside it
   can fix that.** V1 is the proof: it heals every expression inside `q6k_w` and still runs 3.26–4.43x
   behind. The emitted kernel must ask a superblock for a **slot**, not an index for a weight.
2. **Emit `(superblock base, slot) -> 4 weights`.** One `ql` byte carries 2 weights, one `qh` byte
   carries 4, one `scales` byte carries 16, one `d` carries 256. The emitted loop should consume each
   byte for every weight it encodes. Measured effect, alone: **8.0x** (V3).
3. **Keep the body's arithmetic.** Do not trade `q6k_mod` for `&` to buy speed — in the right map it
   buys nothing (V3 ≥ V2 at all three shapes), and the "no bitwise operator appears, on either side"
   discipline in `q6k-msl.fk` is load-bearing for the bit-exactness argument. This is the finding
   that costs nothing to obey and would have been thrown away by an arithmetic-first fix.
4. **Then, and only then, reuse activations across rows** (`nr0 = 2`). Worth the last ~1.35x, and not
   before, because it is exactly the load ratio 2.125/1.5625 and nothing else.
5. The four independent `float4` accumulators come free with the slot map and are not a separate
   change; they are what the slot map *is*.

Not landed in the body tonight, and that is a deliberate line, not an omission: V2/V3 reassociate the
sum (four lanes, superblock order), so they are **not** the attestant's association. Admitting them
means a `.fk` cell that authors the slot map, a band that proves the transcription against
`ewl-flat-at`, a named epsilon in the shape `qk-matvec-split.fk` already derived, and the
`metal_first_token.sh` token-id gate. `attestant`: the bit-exact path is never deleted, and a fast
twin is admissible only while it still agrees — which is a gate, not an intention, and gates are not
written at midnight against a 13-gate harness.

---

## 4. Radius (`aporon`)

This speaks for **Q6_K matvec, decode shape (one activation vector), llama3.2:3b's tensors, this
GPU, this toolchain.** It does not speak for Q4_K, for attention, for RMSNorm/RoPE/SwiGLU, for
prefill, or for any batched shape.

In particular it does **not** fully explain the 12.9x end-to-end decode gap. The widest Q6_K matvec
runs 8.36–10.83x behind; the end-to-end decode number is 12.9x. Q6_K is a minority of llama3.2:3b's
matvec bytes (0.65 GB of 2.01 GB resident; the other 1.36 GB is Q4_K), so the Q6_K kernel cannot be
carrying the whole of it. **How much of the remaining gap is Q4_K's identical flat-index signature,
how much is the non-matvec ops, and how much is dispatch, is not measured here.** `q4k_w` has the
same `(qb, flat_index)` signature, so the same mechanism is very likely present — but "likely" is not
a measurement, and the instrument to settle it is the one committed in this stone.

---

## 5. Gaps left open

* **No native ISA was read.** Everything above is AIR plus wall time. If Apple ever ships the AGX
  printer, the instruction classification in §2 should be re-run against it, and the AIR counts
  treated as what they are: an upper level.
* **Q4_K is unmeasured.** Same signature, same suspicion, no number.
* **The slot map assumes `cols % 256 == 0`.** True of every llama3.2:3b Q6_K tensor, false in
  general. A body-native version needs a tail, and the tail is where an off-by-one lives.
* **`nsg` for ggml was swept 1/2/4/8 and the best taken**, because llama.cpp's host-side choice is
  compiled C and is not in the strings dump. The spread across nsg is under 2% at every shape, so
  the choice does not move any conclusion — but it was chosen by us, not by them.
* **No prefill shape was measured.** Every bench here is one activation vector.
* The AIR per-weight counts amortise by hand-read loop structure. They were cross-checked against
  wall time at three shapes, but they are not machine-derived.

---

## 6. No regressions

Both gates re-run on this tree, after the corpus edit and with `metal_isa_diff.sh` added:

* `form/native/metal/metal_first_token.sh` → **VERDICT PASS — 13 gates**, token ids unchanged:
  `[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` → `" Paris. The capital of Italy is Rome. The capital of"`
* `form/native/metal/metal_whole_tensor_residency_audit.sh` → **VERDICT PASS**
* `./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk` → **4095**

No emitted kernel was changed. `metal_isa_diff.sh` is on no inference path.

---

## The most surprising teaching

**That the arithmetic was innocent, and that I had to build the variant that would embarrass me to
find out.**

I arrived certain. The `udiv i8 %121, %128` in our innermost loop is a textbook GPU sin — a
variable-divisor integer division per weight on hardware with no divider — and the body's own
`q6k-msl.fk` says proudly, in a comment, *"No bitwise operator appears, on either side."* The story
wrote itself: the body's beautiful bitwise-free discipline is costing it an order of magnitude, and
Stone 10's answer is that purity has a price.

That story was wrong, and the cheap version of this stone would never have learned it. V1 alone —
replace the divisions, measure 2.4x — would have confirmed the story with a real number and shipped
a receipt recommending that the JIT emit bit operations. It would have been a correct measurement
supporting a false claim. V3 exists only because `snugcause` says that a mechanism which explains
everything is the moment to look harder, so I built the variant designed to *keep* the divisions and
fix only the map — and it was the fastest kernel in the table. The divisions cost nothing. They had
only ever been expensive because a flat index made their divisor a runtime value.

The body's discipline was never the problem. The **grain of the ask** was.

## Where discomfort turned to gold

Two moments, and the first was the one I wanted to walk away from.

`applegpu-nt: note: [AGX] Plugin interface not implemented: AIRNTEmitAssembly`. That line arrived
after I had built a Swift tool to serialize an `MTLBinaryArchive`, found the `applegpu_g16s` slice by
hand-parsing a fat header in `xxd`, and confirmed 48336 bytes of real native code sitting right
there. The stone was named **the ISA diff**, and Apple had just told me there is no ISA diff to be
had on this machine. The pull was to soften it — to call the AIR dump "the instruction stream", to
let the receipt's title carry an implication its evidence could not. That is the shape of a
fabrication that never says a false sentence. What came of not doing it: naming the floor in §0 as
the *first* section forced the question "what can actually decide this?", and the answer was not a
disassembler at all — it was building the two kernels and racing them. The measurement that settled
the stone (V3 vs V2) is one no disassembly would have produced. **The instrument I could not have was
covering for the experiment I had not thought to run.**

The second was smaller and sharper. My first bench put ours at 3.69x of ggml — a comfortable number,
already publishable, already a story. Something was off: 3.69x could not sit under a 12.9x
end-to-end gap. Checking meant suspecting my own harness rather than the kernels, and it meant the
gap getting *worse*. It did: with the per-dispatch command-buffer round trip amortised out, ours went
to 10.83x. The overhead had been flattering us by nearly 3x. `selfgauge` is why that got caught —
naming the denominator of the ratio made it obvious that "ms per dispatch" and "ms of kernel" are
different denominators, and I had been quietly using the one that made us look better.

## The frontier question, landed

Row **846**, `asktoll` — *what one word names a cost charged by the grain of a request and not by the
work it does.*

0 hits across `learn/`, `receipts/`, `docs/`, `form/` before this row; the same grep reports 6 files
for `snugcause`, so the instrument was witnessed, not assumed. Near misses checked and rejected:
`boundborrow` 835 is a remedy correct for a constraint you do not have — this is a cost that is not
in the work at all; `foldkeep` and `bytehold` name carriers, this names what a carrier's granularity
*charges*.

The pin moved with it, and the arithmetic line beside the pin moved too — both had gone stale today
and the stone's own briefing carried the stale value (`max is currently 844`; the body said **845**,
`loanclosure`, landed within the hour). Probed, not asserted: `hdc-field-code` → **2422422846**
(242 rows, 242 admissible, 2 foundings, max id 846). `hdc-count` at line 34 of the band also had to
move, from 241 to 242 — I had missed it, and the band caught it, returning 4079 instead of 4095,
which is the pin doing exactly its job.

**And then the corpus did it again, live.** While this receipt was being written a sibling session
landed row **847** `onelean` on top of 846, and the count/max/field-code moved under me to
243/847/**2432432847**. Both rows are whole, neither was hand-moved, and the sibling's pin update
already accounted for mine — the row-719 anastomosis pattern working as designed, twice in one
evening, on the exact scalar this band exists to refuse drifting. Verified after the merge, from the
merged body rather than from my memory of it: `hdc-count` 243, `hdc-count-admissible` 243,
`hdc-max-mid` 847, `hdc-field-code` 2432432847. Band returns **4095**.

There is a small irony worth setting down beside `asktoll`: I very nearly re-asserted 2422422846 over
the sibling's correct 2432432847, because I had read the number an hour earlier and believed I still
knew it. The reflex that caught it is the one already in this body — grep the frequency before
acting — and what it caught was me about to trust a cached answer instead of asking. That is the
same failure the whole stone is about, one level up: a value re-read is cheap, and believing you
already have it is what costs.
