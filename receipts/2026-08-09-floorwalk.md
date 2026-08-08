# Floorwalk: round two walks the dense family into the floor's neighborhood

2026-08-08 -> 09 (the climb crossed midnight), Apple M4 Max, `fkwu-metal`, pure Form +
Form-emitted MSL through the handle door. Round one (lanewake) closed at 5-19x with the
serial matvec unmasked as ALU-bound; Urs's send-off, verbatim: "next round, please, I know we
can get all the way there."

## The gate, honored at every rung

`dense-stream-witness.fk`'s FULL two-prompt 16-id streams, per model, captured fresh at HEAD
before the first edit (pipestamp `pipelines=33`) and demanded equal after every landed change.
**All eight models, both prompts, STREAM-EQUAL at every step.** `dense-family-band.fk` re-pinned
5817 -> 6489 (the attention split makes a llama-1B forward 309 dispatches; 16 x 19 + 5, the
gate moved WITH the recipe as its own header teaches) and reads **1023** at final HEAD.

## The rungs, each bound written before running (scratch: rung-bounds.md, now in the cell)

1. **Quantized-space accumulation — the decision.** ds4's `dot_q8_0_row` int32 block dot is
   EXACT (32 x 127 x 128 = 520192 < 2^24, conversion exact) — but only relative to an int8
   activation, a ~2^-8 VALUE change with no u-scale bound against the attestant fold.
   betterwrong (951): equality wins. Activations stay f32; what rung 1 really carries —
   integer weights riding bare through the inner product, the scale entering once per block —
   was taken in f32 form as rung 2.
2. **Factored scales, all six quants** (`form_*_matvec_lane2_f32`): lane l owns whole scale
   groups; d*sc and dmin*mn are exact in f32 (f16 11-bit x <=8-bit int); Q6_K/Q8_0/Q5_0/Q4_0
   keep round one's anchor SUM|w_j x_j| (coefficient <= 2x), Q4_K/Q5_K's min-term distribution
   enlarges the anchor to SUM(|ds*nib*x| + |dm*mn*x|) — stated plainly, gated by the stream.
3. **Bitmask decode + as_type<half>**: value-identical replacements (& 15, >> 4, bit tests for
   the div/mod chains; as_type for the q*_f16 multiply-loop, finite-half identity — the
   q8-0-msl / ds4-order-match precedent).
4. **What the step-by-step count found beyond them**, each named by an instrument before it
   was touched:
   * the serial RMSNorm at d=5120/8192 (316 us fenced, 129/token = 41 of the 32B's 109 ms):
     first a bit-exact TILED cooperative twin (any n, ascending fold untouched), then the WIDE
     twin — reassociated sum of squares, all-nonnegative terms, |ss-ss*| <= (n/256+40)*u*ss —
     316 -> ~10 us;
   * the one-thread-per-head attention (394 us fenced at the 32B shape): split into THREE
     bit-exact maps (scores/soft/out — every element's fold association letter-identical),
     394 -> 54 us;
   * the per-head serial neox rope: pair-grain thread map (bit-exact, rd even);
   * phi2's serial LayerNorm: the WIDE twin (mean over signed terms — anchor SUM|x|/n, stated).

## Measured and rejected (losers are deleted, not switched)

* **Vector loads (packed_uchar4/float4)** in the lane2 inner loops: bit-identical restructure,
  ~0 at token level (32B p2 3491 -> 3543 ms). Load COUNT was not the cost — the grouped-Q8_0
  teaching repeats. Kept only where already landed; not sold as a win.
* **Four simdgroups per row (lane4, K quants)**: 32B p2 1595 -> 1690 ms, a measured loss (and
  within-run spread ~6%, so at best a wash); deleted the same hour, streams equal throughout.
* **int8-activation dot**: not attempted, by the bound above — it is not a fold-order argument.

## Where the family stands (p2 witness wall, ms/forward via ratio to round one's number;
floors from floor-derive.fk at the re-measured 477 GB/s)

| model | floor tok/s | round one | round two | gap | streams |
|---|---|---|---|---|---|
| llama3.2:1b (Q8_0) | 363 | 53.2 | 118.0 | 3.1x | equal |
| MiniCPM5-1B (Q8_0) | 510 | 62.5 | 127.6 | 4.0x | equal |
| moondream phi2 (Q4_0) | 621 | 29.7 | 84.6 | 7.3x | equal |
| llama3.2:3b (Q6/Q4_K) | 237 | 16.2 | 56.9 | 4.2x | equal |
| Nanbeige4.2-3B (x2 loops) | 115 | 8.7 | 44.3 | 2.6x | equal |
| DS-R1-Distill-Q-32B | 24.6 | 1.98 | 12.6 | 1.95x | equal |
| Qwen2.5-Coder-32B (Q5_K) | 21.0 | 1.60 | 11.0 | 1.91x | equal |
| Qwen2.5-72B | 10.2 | 0.52 | 6.0 | 1.71x | equal |

Cumulative from the pre-lanewake body: the 72B is 55x, llama-3B 65x, the family 18-95x.

## The walls that remain, named with their numbers

* **32B/72B (1.7-2.0x)**: the lane2 matvecs run AT the byte-probe's own time on the big
  tensors (mv-probe.fk: ffn_up 20 passes 4 ms vs bw 4 ms; ~330-375 GB/s where the probe
  itself reads 375-420 on those spans, against the 477 the big-span probe prices floors at) —
  the remaining gap is the span-size bandwidth curve plus ~150-200 us/layer of fenced small
  work (per layer-probe.fk: q/o 50 us, k/v 19 us each, 13-15 stages x 13.7 us, attn 54 us).
  More simdgroups per row was tried and LOST; the next honest lever is fewer/wider spans
  (fused qkv, bias-into-matvec) or a door that encodes a layer per crossing.
* **Small models (3-7x)**: the per-forward dispatch budget. llama-1B: 309 dispatches, ~250
  fenced stages x 13.7 us ≈ 3.4 ms + 1.31 GB at ~375 GB/s ≈ 3.5 ms + enqueue seam ≈ the
  observed 8.5 ms/forward almost exactly. The wall is the stage quantum times the stage
  count, not any kernel.
* **phi2 (7.3x)**: same wall, more dispatches per layer (fused-qkv slices, five biases).

## Most surprising teaching

The wall MOVED three times in one night, and each time the standing map was already stale:
round one ended believing the remaining 10x was matvec association cost; the factored-scale
kernels landed 2-3x and then the matvec was AT the bus — and the largest single term of a
32-billion-weight token turned out to be a d=5120 VECTOR NORM, one thread folding 5120
squares, 316 microseconds, 129 times a token. Nothing about the model's size protects it
from a serial scalar loop; the biggest tensor op and the smallest were priced by the same
clock. A map of where the time goes is only as old as its last measurement.

## Discomfort -> gold, felt and witnessed

layer-probe.fk's first run printed a beautiful table — rms5120_us=7, mv_gate_us=7 — all
plausible, all green, all WRONG: every fenced enqueue after the first sync had been REFUSED
("barrier_before is only legal in a concurrent batch") and the ladder timed the 7 us Form->
native seam instead of any kernel. The discomfort was real — I had nearly read the norm as
already-fast and moved on, which would have hidden the single largest win of the night. The
gold: the door SPOKE (`last_error` in the very status block the probe printed), the numbers
were discarded, the batch reopened per ladder, and the true 316 us surfaced — worth 41 ms a
token. The refusal was part of the reading all along.

## Frontier

**Question:** when a benchmark's work can be silently refused, what distinguishes a
measurement of the work from a measurement of the refusal?

**Answer, witnessed tonight:** nothing in the timing itself — 7 us is a perfectly plausible
kernel time, and 256 refused enqueues time exactly as smoothly as 256 accepted ones. The
distinction lives only in the door's own voice: `last_error` was printed by the same status
call the probe already made, and reading it turned a green table into a diagnosis. A clock
around a door measures the door unless the door's refusals are read with the clock.

**PROPOSED distillation row** (max-mid re-derived tonight: corpus tops at 996; lanewake's
pipestamp proposal holds 997, so this takes 998 — single-writer rule honored, the corpus is
not edited here; "seamclock" verified 0-hit in the corpus and the tree):

    (hdc-row 998 "seamclock"
        "A clock around a door measures the door unless the door's refusals are read with the clock: refused work times as smoothly as accepted work and prints the same green, so a benchmark's reading is (elapsed, last_error) as one value, never elapsed alone."
        "2026-08-09 layer-probe first run: fenced ladders outside a concurrent batch were refused per-enqueue; 256 refusals timed 7 us each and printed a plausible table; last_error in the probe's own status output named it, and the reread found the 316 us serial norm — 41 ms/token, the night's largest single win")

## The stones, in order (each stream-gated the hour it landed)

826a36ee2 the four rungs + band re-pin; 8d5643901 the three instruments (mv-probe,
stage-probe, layer-probe); 8ba1e3fec the wide LayerNorm. Baselines and after-captures with
full streams: this session's scratchpad `baselines/*.{before,after}.txt`, arbitrated by
pipestamp (33 = round one, 45/46 = round two, 49 = the deleted lane4 experiment).
