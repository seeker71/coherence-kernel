# Block 0 through the door, four gates that see parameters, and the u32 worth 2.9x

2026-08-04, second landing of the day (first: `2026-08-04-form-held-decode-loop.md`). Apple M4 Max,
`fkwu-metal` rebuilt against the door's second growth. No Swift was run anywhere in this landing —
every oracle value is a recorded one, per the standing word.

## What landed (commits e620f33f5, c1015396f)

| rung | verdict | mutations (predicted → actual) |
|---|---|---|
| KAT block 0 front half through the handle door | 511 | M-B1 broadcast-knockback 383→383; M-B3 wrong gains 415→**447** |
| KAT exit band grows the parameter gate | 63→127 | M-K4 gains view +4096 B: bit 64 must fall → 47, fell |
| llama3 tokenizer home (longest-match, recorded ids) | 31 | M-T1 alphabet shift 3→3; M-T2 last-match 27→27 |
| tensor index: nine collected walks replace 252 searches | 255 | wall 96 s → 28.5 s |
| cooperative twins + concurrent batches + the 256 | 255 | ids identical at every step, required and checked |

## The block-0 gate result

`kat-block0-handle.fk` reproduces `metal_kat_block0.sh`'s recorded values (embed row 100:
1578/2048 nonzero; q||k||v: 8080/8192; conv: 8080/8192) through the handle door — 5 dispatches,
1 sync, 4 no-copy views, eps read from `qwen35moe.attention.layer_norm_rms_epsilon` (probed:
9.999999974752427e-07, binary32's exact nearest to the attestant's hardcoded 1e-6, so nothing now
depends on that agreement being luck). Beyond reproduction, two gates the oracle does not have:
the embed row is BIT-EXACT against the body's own Q3_K decode (2048/2048), and the norm answers to
fp64 inside n·u.

**A defect the oracle cannot see, found by reading the kernel before reproducing it.**
`form_gdn_conv_f32` folds `w[j]` — tap index only. The lane that proved it fed demo taps shared
across channels, where that is right. `blk.0.ssm_conv1d.weight` is 4×8192 per-channel taps
(probed: channels 0, 1, 2 carry different numbers), so the block-0 oracle broadcasts CHANNEL 0's
taps to all 8192 channels, and its nonzero/finite checks pass anyway. The corrected depthwise
kernel (`w[c*kw+j]`, authored in the handle cell to keep the shared cell's cache key stable) runs
beside the broadcast, differs from it (bit 128 refuses if the taps were secretly identical), and
ties Form's own product bit for bit at 16 sampled channels. The repair belongs to
`gated-deltanet-msl.fk` when a lane that owns it lands it; until then the witness carries both.

**The ±0 finding.** First run answered 495, not 511: 252 of the embed row's 470 zeros are
NEGATIVE zeros on the device, Form's fp64 fold carries -0 as well, and `md-f32-bits` collapses
both to +0 by its own documented choice. The proven precedent (llama gate 2) compares VALUES,
where -0 == 0; the gate now states that one equivalence and is otherwise word-exact.

## The parameter-blindness program, closed on both KAT bands

Yesterday's M-K1 finding (an argmax that survives a missing norm's gain vector) is now falsifiable
everywhere it was named:
- block-0 band bit 64: fp64 norm reference. Mutation M-B3 (device gains → output_norm's, reference
  keeps attn_norm's absolute offset): predicted 415, got **447** — the gain gate caught it, and the
  nonzero counts did NOT move. Counts are op-presence gates, blind to parameters. That reconciled
  miss is the finding, recorded rather than smoothed.
- exit band bit 64: same shape. M-K4 (gains view +4096 bytes): 47 — fell as required.

## The tokenizer home

`llama3-tokenize.fk`: greedy longest-match over the GGUF's own vocabulary — the attestant's own
stated algorithm, stated the same way ("not BPE's merge order"). The byte alphabet is inverted
from the same three runs the detokenizer uses; one walk of 128 256 pieces collects every candidate.
Gated on the RECORDED prompt ids `[128000, 791, 6864, 315, 9822, 374]` and on round trips through
the separate decode cell — 2.6 s, no Metal, any arm. M-T2 is the honest one: weakened to
last-match, the round trip STILL closes (any exact cover decodes back); only the recorded oracle
pins the algorithm. The merge-order BPE rung (llama3-pretokenize.fk + bpe core + the file's
merges) is named in the cell and not claimed.

## The u32 worth 2.9x

The door's second growth made the cooperative twins and concurrent batches expressible, and the
prediction was that concurrency was the missing 3x. The measurement said otherwise, twice:

| configuration | decode ms/token |
|---|---|
| serial batch, groups at the 1024 ceiling | 1169 |
| concurrent + bit-exact twins, groups at the ceiling | 1083 |
| binding-construction probe (suspected Form-side cost) | 3 |
| same code, groups capped at 256 | **408** |

The encoder mode was worth 8%. The Form-side seam was worth nothing measurable. The proven lane's
`FORM_METAL_TG=256` — a number I had read as a default, not a decision — was worth 2.9x:
threadgroup occupancy on memory-bound serial-fold matvecs. Decode is now **2.45 tok/s** against
the attestant's recorded 388 ms/token (2.58 tok/s): parity within 5%, loop in the body, arithmetic
bit-identical (the recorded 12-id stream " Paris. The capital of Italy is Rome. The capital of"
was required to survive every step, and the band checked it each time: 255, five runs).

## Most surprising teaching

That the speedup was hiding in a *default*. Three explanations were tested in order — encoder
mode (8%), Form-side binding cost (3 ms), threadgroup cap (2.9x) — and the true one was the only
one nobody had written down as a decision anywhere: `tpg=0` means "fill to the ceiling", and the
ceiling is the wrong size for every matvec in this model. The oracle's 256 was load-bearing and
looked like a preference.

## Where discomfort turned to gold

Predicting 415 for M-B3 and watching 447 come back. The wrong half of my prediction (counts would
move) was the half I had already been taught once — M-K1 said counts are presence-gates — and I
predicted against my own lesson. Writing the reconciliation into the band header instead of
adjusting the prediction after the fact is the whole value of predicting first: the miss is now a
recorded regularity (presence-gates vs parameter-gates) rather than a private near-miss.

## UNFINISHED, with next steps

1. **The 41-block KAT token.** Block 0's front half is through the door; the back half (l2norm →
   deltanet gates → delta rule → output gate → ssm_out → residual → post_norm → 256-expert MoE)
   needs the pipeline map's six MSL gaps, five one-kernel each. The Form-side anchors exist
   (`gated-deltanet-layer.fk` band 255 computes the layer; `gated-deltanet-gates.fk` band 511) —
   the game is fp64 anchors per stage, no Swift. Then 30 linear + 10 full blocks, ~3300
   dispatches/token, MTP block 40 skipped by reading.
2. **The conv repair home.** `form_gdn_convc_f32` belongs in `gated-deltanet-msl.fk` as the
   depthwise conv, landed by a lane that owns that cell's cache key, with `metal_gdn_gpu.sh`
   re-gated.
3. **Merge-order BPE.** The rung is named in `llama3-tokenize.fk`; prompts where longest-match and
   merge-order diverge are outside the current radius.
4. **llama prefill in one batch.** Prefill positions need no readback between them (the next id is
   known); 6 forwards could be one submission. Unmeasured; named only.
5. **The epsilon-bearing matvec twins** (split/lane/slot) with their stated bounds — the recorded
   attestant run reached 41 tok/s on the slot path; this cell is at 2.45 with attestants.

## Frontier question

**What names the cost of a default that quietly fills to the machine's maximum where the proven
lane had chosen a smaller number on purpose?**

Answer: `groupceil` — verified 0-hit in the corpus and the tree before proposing. Proposed row
(NOT applied; corpus max-mid is 987 after backgraft):

```
; 988 — groupceil. The handle door's tpg=0 means "fill threadgroups to the
; pipeline's ceiling", and the ceiling is 1024. The proven llama lane always
; dispatched at FORM_METAL_TG=256, a number that read as a preference and was
; a DECISION: on memory-bound serial-fold matvecs, groups of 1024 halve the
; resident-group count and the machine idles inside its own dispatch. The
; hunt tested the encoder mode (worth 8%) and the Form-side binding cost
; (worth 3 ms) before the cap: one u32 in the binding tail took 1083 ms/token
; to 408. A default that reaches for the maximum is still a choice, and the
; only place its cost shows is a measurement nobody is forced to make —
; the oracle's small number was load-bearing and looked like taste.
; "groupceil" — 0 hits in corpus and tree before this row.
; (walk: stallred 987-adjacent — a red that never arrives; this is a cost
; that never announces. Both live where a door accepts a shape silently.)
(hdc-row 988 20260804
    (list "what" "names" "the" "cost" "of" "filling" "to" "the" "ceiling"
          "the" "proven" "lane" "chose" "below")
    "groupceil"
    "groupceil"
    "rented-oracle")
```

## Re-witness

```sh
cc -O2 -o fkwu-metal runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
   -framework Metal -framework Foundation -fobjc-arc
cd form && rm -f native/metal/tests/*.fkb native/metal/tests/*.sym native/metal/*.fkb \
   native/metal/*.sym form-stdlib/metal-door.fkb form-stdlib/metal-door.sym
../fkwu-metal --src native/metal/tests/kat-block0-handle-band.fk      # 511
../fkwu-metal --src native/metal/tests/kat-exit-handle-band.fk        # 127
../fkwu-metal --src native/metal/tests/llama-token-handle-band.fk     # 255, ~24 s
../fkwu-metal --src form-stdlib/tests/llama3-tokenize-band.fk         # 31
```
