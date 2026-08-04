# KAT-Coder speaks whole: 40 blocks in the body, and a fibonacci that closes

2026-08-04, third landing of the day on this lane (after `2026-08-04-form-held-decode-loop.md`
and `2026-08-04-block0-door-and-the-256.md`). Apple M4 Max, `fkwu-metal` rebuilt from source.
No Swift ran anywhere in this landing; the one recorded oracle used is the exit-path argmax 3637,
carried from the receipts.

## What landed (commits b14a1514d, 44e7e30af)

| rung | verdict | mutations (predicted → actual) |
|---|---|---|
| conv repair home in `gated-deltanet-msl.fk` | 255 | M-R1 broadcast-revert 253 → 253 |
| block0 band, roles flipped (shared cell corrected, witness local) | 511 | M-R2 witness-corrected 383 → 383 |
| **the whole forward**: `kat-token-handle.fk` + band | **262143** (18 gates) | table below |
| pipeline map's expert rows corrected to the census | 255 | — |

Whole-forward mutation table:

| mutation | predicted | actual |
|---|---|---|
| M-T1 key-head mapping h mod 16 → h div 2 (first run) | 261119 | **262143 — survived** |
| M-T1 again, after the anchor was rebuilt head-normalized | 261119 | 261119 |
| M-T2 gated-norm gate moved before the scale (multiplies only) | 260095 | **262143 — survived, and rightly**: the mutation was a pure reassociation, the same product |
| M-T2b RMS computed over the GATED values (the real Mamba2 order) | 260095 | 260095 |
| M-T3 partial rope re-paired adjacent | 229375 | 229375 |
| M-T4 expert offset-matvec rows shifted one | 245759 | 245759 |

## The finish: verbatim continuations, text to text

`kat-run-continuation.fk` — tokenize (greedy longest-match over the file's own gpt2/qwen35
vocabulary, the stated llama3-tokenize radius), prefill, greedy decode, detokenize. Both
witnessed on this machine today:

**"The capital of France is"** →

```
 Paris.
The capital of France is Paris.
The
```

**"def fibonacci(n):\n"** →

```
    if n <= 1:
        return n
    else:
        return fibonacci(n-1) +
```

ids for the second: prompt `[727, 73111, 1393, 1590, 198]`, continuation
`[262, 413, 307, 2564, 220, 16, 25, 198, 285, 460, 307, 198, 262, 745, 25, 198, 285, 460,
73111, 1393, 12, 16, 8, 478]` — 24 tokens of correct recursion, indentation carried, cut only
by the token budget. No recorded end-to-end oracle exists for these and none is claimed; the
stages are gated severally (18 fp64/recorded gates) and the text is the body's own computation.

## What had to be read before it could be built

Four semantics that this repo's own Form cells would have built wrong, all read 2026-08-04 from
the references that CONSUME this exact file (llama.cpp `qwen35moe.cpp` + `delta-net-base.cpp`,
the converter `conversion/qwen.py`, and `modeling_qwen3_5.py`):

1. **The conv output is silu'd** before the q/k/v split. `gated-deltanet-layer.fk` does not
   carry this stage at all.
2. **Value head h borrows key head h mod 16 — in this FILE.** The HF text repeat_interleaves
   (h div 2). Both are true: the GGUF converter REORDERS every V-side tensor (v, z, alpha,
   beta, ssm_a, dt_bias, the conv's V channels, ssm_out's columns) from grouped to tiled order
   so ggml's modulo broadcast lands right. The file speaks permuted coordinates, and reading
   the model's own source against it gives the wrong mapping.
3. **ssm_a holds -exp(A_log) already** (probed: all sampled values negative), and the gated
   norm is **norm before gate** — the cousin order is Mamba2's, and only the reference's own
   comment ("Norm before gate") settles it.
4. **The routed experts are MIXED QUANT**: blocks 5–35 Q3_K, the rest Q4_K. The pipeline map
   had sampled one linear block into a claim about thirty. The failure shape was the loud kind
   only by luck: Q3_K bytes through the Q4_K decode gave 1e24 — finite, and green to every
   nonzero/presence gate. The body's first Q3_K matvec exists because of this.

## Most surprising teaching

**A gate can be blind by SCALE, not by construction.** The delta-rule anchor compared
pointwise with the band family's usual 1e-4 absolute floor — and the delta outputs at
position 0 are ~2e-5. The floor was larger than the whole signal, so the anchor passed
EVERYTHING, including the key-head-mapping mutation it existed to refuse. Three other anchors
with the same floor bit their mutations fine, because their signals stood above it. Nothing
about the gate's text was wrong; its scale was. The repair: compare the worst deviation
against the head's own max |ref|, so the tolerance is the signal's, not the family's. Only
the predict-then-mutate discipline surfaced this — the band was green either way.

## Where discomfort turned to gold

Watching M-T2 come back green after predicting it would fall. The urge was to call the
mutation "close enough" and move on; sitting with it showed the mutation itself was wrong —
moving a multiply through other multiplies is the same product, so the band OUGHT to pass it,
and a band that failed it would be gating rounding noise. The real Mamba2 order changes the
normalizer's input, and M-T2b fell exactly as predicted. The discomfort was the difference
between mutating the TEXT and mutating the MEANING, and the table now records both runs
rather than the flattering one.

Second seam, felt and kept: alpha's interval. Written open (0,1) from the mathematics, and
the file refused it — head 11's decay is 1.7e-40 in fp64, under binary32's floor, so the
device answers exactly 0.0 and is RIGHT. The 07-28 receipt pinned the same boundary from
above (`swamping`, decay saturating to 1); this is its floor-side twin, now witnessed in the
band's own words.

## UNFINISHED, named

1. **Rate.** ~2.5 s/token serial with 41 syncs/token (40 routing readbacks + argmax). The
   llama lane's concurrent batches and cooperative twins are expressible here too; unmeasured.
2. **Merge-order BPE** for prompts where longest-match diverges — same rung llama named.
3. **The attestant shell** `metal_gdn_gpu.sh` is text-updated for per-channel taps but has
   not run (it is a Swift lane; the standing word holds). The handle band is the live gate.
4. **`gated-deltanet-layer.fk`'s missing stages** (conv silu, gated norm, q scale) — the cell
   is internally consistent and its band is honest about what it computes, but it is not the
   reference path; a reader taking it as KAT's layer will miss three stages the header of
   `kat-token-handle.fk` now names.
5. **The MTP head** (block 40) sits unread on disk — speculative decoding, one block class.

## Frontier question

**What names a gate whose tolerance floor stands taller than the signal it guards, so every
mutation of that signal walks under it unseen?**

Answer: **`floorblind`** — verified 0 hits in corpus and tree before proposing. Proposed row
(NOT applied; corpus max was 995 at proposal time, and the row number belongs to the body at
reunion):

```
; 996 — floorblind. The delta anchor compared f32 to fp64 under the band
; family's 1e-4 absolute floor, and position 0's delta outputs are ~2e-5:
; the whole signal lived beneath the tolerance, so the anchor blessed the
; key-head mutation it was written to refuse. A floor is a claim about the
; signal's scale, and it was inherited, not measured. The repair compares
; the worst deviation against the head's own max reference — the gate now
; wears the signal's units. Kin: swamping (895, a value lost beside a
; larger one); this is the gate-side twin — a TEST lost beneath its own
; tolerance. "floorblind" — 0 hits in corpus and tree before this row.
(hdc-row 996 20260804
    (list "what" "names" "a" "gate" "whose" "floor" "stands" "taller"
          "than" "the" "signal" "it" "guards")
    "floorblind"
    "floorblind"
    "rented-oracle")
```

## Re-witness

```sh
cc -O2 -o fkwu-metal runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
   -framework Metal -framework Foundation -fobjc-arc
rm -f form/native/metal/*.fkb form/native/metal/tests/*.fkb form/form-stdlib/*.fkb form/form-stdlib/tests/*.fkb
./fkwu --src form/form-stdlib/tests/gated-deltanet-msl-band.fk          # 255
./fkwu-metal --src form/native/metal/tests/kat-block0-handle-band.fk    # 511
./fkwu-metal --src form/native/metal/tests/kat-token-handle-band.fk     # 262143, ~35 s
./fkwu-metal --src form/native/metal/kat-run-continuation.fk            # the two continuations
```
