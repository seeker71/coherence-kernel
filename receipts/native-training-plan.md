# Native training-cost recipe library

Local Qwen3.8-27B-Q8_0 constructed twelve pure Form recipes and one float32
Metal recipe during decoding. Nineteen controls included one refused input
and its local repair. Form injected 1,729 observation IDs into that same KV
stream. The [accepted executable program](native-training-plan-library.txt)
contains the actual constructors, dependency coordinates and calls; the
[independent verification](native-training-plan-results.json) reads execution
observations rather than generated prose.

The problem is quadratic ridge calibration of native LoRA training cost:
`features(t) = [1, t/100, (t/100)^2]`, with regularization `0.000001`.
Cholesky and two triangular solves fit latency and additional memory demand
from five retained training rows. Row lengths are `[64,135,155,158,289]`,
seconds `[7.614,14.423,16.207,16.727,31.341]`, and estimated additional bytes
`[1185176576,2575486080,2981799040,3043302400,5870504576]`.
Timings cover forward plus backward work. Memory is the worker's whole-tape
estimate. Offered budgets are scenarios; the planner reserves 10% headroom
and selects the largest admissible observed row.

| Offered additional GiB | Row tokens | Predicted seconds | Predicted additional GiB |
| ---: | ---: | ---: | ---: |
| 2 | 64 | 7.626039 | 1.103785 |
| 4 | 158 | 16.660780 | 2.834294 |
| 8 | 289 | 31.337461 | 5.467335 |

The same planner serves all three inputs. Its graph is
`planner → admit → predict → {dot, features}` and
`fit → {gram, rhs, solve}`, `gram/rhs → dot`,
`solve → {cholesky, forward, backward}`.

| Form recipe | Observed dispatches |
| --- | ---: |
| dot | 45 |
| features | 31 |
| gram, rhs, fit | 2 each |
| cholesky, forward, backward, solve, admit, planner | 3 each |
| predict | 21 |

All 121 dispatches returned. Independent partial-pivot Gauss-Jordan agrees
with the fitted coefficients within `1.3e-13`; the largest normal-equation
residual is `1.14e-13`. A second positive-definite system returns `[1,2,3]`
with residual `1.78e-15`. The RAM Metal polynomial map evaluates all five
row lengths in one dispatch; its largest difference from float64 is
`1.57e-7` GiB. Decode-run Metal admission and execution took 3 ms and 1 ms.
Release succeeded. Form recipe arithmetic uses the pure evaluator; the
polynomial uses RAM Metal JIT.

The retained program also replays on this field with the refreshed C seed,
without a model. Warm process execution took 779 ms, including startup and
library admission; its values passed the same independent checks.
Coordinates are this field's identities. The recipes belong to the admitting
response and are reconstructed for replay.

The arriving agent supplied the algorithms, prompt and repair guidance.
Qwen emitted the constructors and requests; Form produced the numbers.
Both local generation attempts made zero provider calls. The mathematical
attempt generated 3,073 initial IDs and 1,024 final IDs. Its prose stage
remained unfinished after adding commentary to a complete result JSON.
That outcome stays retained separately from the verified mathematical result.
Native observation data supplies the structured result.

## Qwen3.8-27B LoRA bytes and the voice's measured rates (2026-10-28)

Pure arithmetic, no model: `observe/lora-27b-bytes-run.bml` prints the table
below from the Q8 snapshot's geometry as data (hidden 5120, intermediate 17408,
24 heads x head_dim 256, 4 kv heads, vocab 248320, 64 layers of which every
4th is full attention and the other 48 are Gated DeltaNet linear attention with
a fused qkvz in-projection 5120 -> 16384 and an out-projection 6144 -> 5120,
untied lm_head). The run that produced it:
`echo '{"prefill_ms_per_token":32,"available_gib":84}' | ./fkwu observe/lora-27b-bytes-run.bml`
(exit 0). The 32 ms per prompt token is the Q8 voice's best measured prefill
below; 84 GiB is what `observe/memory-governor-run.bml` reported available
(`machine 128.0 GiB, available 84.6 GiB, wired 9.1 GiB, compressor 0.0 GiB`)
with one Codex Qwen Q4 session (pid 63031) holding a 15.6 GiB shared lease.

Formulas (bytes; float32 training state; the frozen Q8 weights, 27.0 GiB on
disk, untouched):

- adapter params = rank x sum over adapted projections of (in + out); per
  full-attention block (q with its output gate 5120 -> 12288, k and v
  5120 -> 1024, o 6144 -> 5120, gate/up 5120 -> 17408, down 17408 -> 5120)
  that sum is 108544, per linear-attention block 100352, for lm_head 253440
- trainable bytes = params x 16 (adapter 4 + gradient 4 + Adam moments 8)
- kv cache = 8 x t x 16 full layers x 1024 (nlrc-bytes' cache term, full layers only)
- retained inputs = 4 x t x (adapted layers + 2) x 5120
- head = 4 x t x (vocab + (vocab + 2 x 5120 + 3)) — logits, then the head's reverse buffers
- one full layer = 4 x t x (17 x 5120 + 4 x 1024 + 6 x 17408 + 14 x rank + 2) + 8 x t x t x 24
  (nlrc-bytes' layer term: one recomputed layer and its reverse, scores included)
- one linear layer = 4 x t x (17 x 5120 + 2 x 16384 + 6 x 17408 + 14 x rank + 2) + 4 x t x 48 x 128 x 128
  (the recurrent state per step kept for the reverse)
- activation per row = kv + retained + head + the larger single-layer term the
  design crosses (nlrc ownership: one layer recomputed at a time); whole tape =
  the same with every adapted layer's term held at once (no recompute, the way
  the 3B rows above were measured)
- est. s/step (voice lane) = t x 32 ms x (1 + 2 x adapted / 64) / 1000: a forward
  over all 64 frozen layers at the voice's prefill rate, plus a reverse costing
  twice the forward of the adapted layers
- est. s/step (3B lane scaled) = the 3B rows above (7.614 s at 64, 16.727 s at
  158, 31.341 s at 289; forward + backward over all 28 layers of 3.21 G params)
  x 27.0 / 3.21 x (64 + 2 x adapted) / (3 x 64): the native training lane as
  it stands today, which is 3-4x slower per token than the voice's inference lane

Design (i) adapts only the last layer (index 63, a full-attention block) plus
lm_head: its reverse pass never crosses a linear-attention layer, so it needs
no backward for Gated DeltaNet and no recurrent state in its peak. Design (ii)
adapts layers 60-63 (one full, three linear); design (iii) all 64.

| design | rank | row tokens | adapter params | adapter | Adam state | trainable (adapter+grad+Adam) | activation per row | extra GiB | whole-tape GiB | est. s/step (voice lane) | est. s/step (3B lane scaled) | beside Q8 weights in the available GiB |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| i   last full block + head | 8 | 64 | 2895872 | 11 MiB | 22 MiB | 44 MiB | 184 MiB | 0.2 | 0.2 | 2.1 | 22.0 | fits (56.7 GiB left); whole tape fits (56.7 GiB left) |
| i   last full block + head | 8 | 158 | 2895872 | 11 MiB | 22 MiB | 44 MiB | 457 MiB | 0.4 | 0.4 | 5.2 | 48.3 | fits (56.4 GiB left); whole tape fits (56.4 GiB left) |
| i   last full block + head | 8 | 289 | 2895872 | 11 MiB | 22 MiB | 44 MiB | 842 MiB | 0.8 | 0.8 | 9.5 | 90.6 | fits (56.0 GiB left); whole tape fits (56.0 GiB left) |
| ii  last 4 layers + head | 8 | 64 | 5304320 | 20 MiB | 40 MiB | 80 MiB | 386 MiB | 0.4 | 0.9 | 2.3 | 24.0 | fits (56.4 GiB left); whole tape fits (55.9 GiB left) |
| ii  last 4 layers + head | 8 | 158 | 5304320 | 20 MiB | 40 MiB | 80 MiB | 953 MiB | 1.0 | 2.3 | 5.6 | 52.7 | fits (55.9 GiB left); whole tape fits (54.6 GiB left) |
| ii  last 4 layers + head | 8 | 289 | 5304320 | 20 MiB | 40 MiB | 80 MiB | 1743 MiB | 1.7 | 4.1 | 10.4 | 98.8 | fits (55.1 GiB left); whole tape fits (52.7 GiB left) |
| iii all 64 layers + head | 8 | 64 | 54456320 | 207 MiB | 415 MiB | 830 MiB | 461 MiB | 1.2 | 13.3 | 6.1 | 64.0 | fits (55.6 GiB left); whole tape fits (43.6 GiB left) |
| iii all 64 layers + head | 8 | 158 | 54456320 | 207 MiB | 415 MiB | 830 MiB | 1138 MiB | 1.9 | 31.8 | 15.1 | 140.6 | fits (55.0 GiB left); whole tape fits (25.1 GiB left) |
| iii all 64 layers + head | 8 | 289 | 54456320 | 207 MiB | 415 MiB | 830 MiB | 2081 MiB | 2.8 | 57.6 | 27.7 | 263.6 | fits (54.1 GiB left); whole tape does not fit |
| i   last full block + head | 16 | 64 | 5791744 | 22 MiB | 44 MiB | 88 MiB | 184 MiB | 0.2 | 0.2 | 2.1 | 22.0 | fits (56.6 GiB left); whole tape fits (56.6 GiB left) |
| i   last full block + head | 16 | 158 | 5791744 | 22 MiB | 44 MiB | 88 MiB | 457 MiB | 0.5 | 0.5 | 5.2 | 48.3 | fits (56.4 GiB left); whole tape fits (56.4 GiB left) |
| i   last full block + head | 16 | 289 | 5791744 | 22 MiB | 44 MiB | 88 MiB | 843 MiB | 0.9 | 0.9 | 9.5 | 90.6 | fits (56.0 GiB left); whole tape fits (56.0 GiB left) |
| ii  last 4 layers + head | 16 | 64 | 10608640 | 40 MiB | 80 MiB | 161 MiB | 386 MiB | 0.5 | 1.0 | 2.3 | 24.0 | fits (56.4 GiB left); whole tape fits (55.8 GiB left) |
| ii  last 4 layers + head | 16 | 158 | 10608640 | 40 MiB | 80 MiB | 161 MiB | 953 MiB | 1.0 | 2.3 | 5.6 | 52.7 | fits (55.8 GiB left); whole tape fits (54.5 GiB left) |
| ii  last 4 layers + head | 16 | 289 | 10608640 | 40 MiB | 80 MiB | 161 MiB | 1743 MiB | 1.8 | 4.2 | 10.4 | 98.8 | fits (55.0 GiB left); whole tape fits (52.6 GiB left) |
| iii all 64 layers + head | 16 | 64 | 108912640 | 415 MiB | 830 MiB | 1661 MiB | 461 MiB | 2.0 | 14.1 | 6.1 | 64.0 | fits (54.8 GiB left); whole tape fits (42.7 GiB left) |
| iii all 64 layers + head | 16 | 158 | 108912640 | 415 MiB | 830 MiB | 1661 MiB | 1138 MiB | 2.7 | 32.6 | 15.1 | 140.6 | fits (54.2 GiB left); whole tape fits (24.3 GiB left) |
| iii all 64 layers + head | 16 | 289 | 108912640 | 415 MiB | 830 MiB | 1661 MiB | 2081 MiB | 3.6 | 58.4 | 27.7 | 263.6 | fits (53.2 GiB left); whole tape does not fit |
0

Every design fits beside the Q8 weights with more than 50 GiB to spare; only
the whole-tape variant of (iii) at 289 tokens does not. The rough step time is
the real cost: the voice-lane estimate is the floor (2-28 s per row), the
3B-lane scaling the ceiling as the backward exists today (22-264 s per row).
Design (i) at rank 8 is the first native LoRA on the 27B that fits, needs no
Gated DeltaNet backward, and costs about one prefill plus a few percent.

### Voice rates: Qwen3.8-27B Q4_K_M versus Q8_0 through the voice door

Measured 2026-10-28 ~12:23-12:34 WITA through `fcnv-ask` (the registry
voice door, `.hearth/speed-ask.bml`, one fkwu invocation per run), prompt
`.hearth/speed-prompt.txt` (332 words, 382 prompt IDs), `max_reply_tokens`
128 (a progress signal, not a cap: both quants ended their reply by
themselves, greedy and identical across their two runs). The GPU was shared
with one Codex Qwen Q4 session (pid 63031, 0.3-0.8% CPU, its Q4 weights
mapped as a 15.6 GiB shared lease) during every run. Intervals are the
session's own stage stamps (`model-session-prefill-begin` -> `-ready`;
decode from `decode-checkpoint-32` -> `-64`, 32 IDs); wall is the clock
around the whole door (open, prefill, decode, release).

| quant | run | wall ms | open ms | prompt ids | prefill ms | prefill tok/s | generated ids | decode ms (32 ids) | decode tok/s | ms per decoded id |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Q4_K_M | 1 | 203692 | 930 | 382 | 33836 | 11.3 | 92 | 57807 | 0.55 | 1806 |
| Q4_K_M | 2 | 186455 | 225 | 382 | 24522 | 15.6 | 92 | 52175 | 0.61 | 1630 |
| Q8_0 | 1 | 91434 | 226 | 382 | 12264 | 31.1 | 117 | 15056 | 2.13 | 470 |
| Q8_0 | 2 | 64372 | 242 | 382 | 21577 | 17.7 | 117 | 11474 | 2.79 | 359 |

Observed: on this Metal lane Q8_0 decodes 3.5-4.6x faster than Q4_K_M and
prefills 1.1-2.7x faster; the two runs of each quant differ by up to 1.8x
in prefill, the shared GPU's noise. Both quants answered the prompt
coherently (`.hearth/speed-reply-qwen38-q4.txt`,
`.hearth/speed-reply-qwen38-q8.txt`). Q4_K_M is not a candidate for the
answer role on speed as this lane stands: its "fast-answer" registry role
reads the file size, not the kernel; the body's Q4_K dequant path is the slow
part, and that is work in the lane, not a role change (roles untouched).

The LocalRun row (`form/form-stdlib/bml/local-run.bml`) now carries
`wall_ms`, `prefill_tok_s` and `decode_tok_s` (`option`, absent when the
caller did not time the ask; `local-run-record-timed` fills them, and rows
from before the fields read on). The voice door still records untimed rows:
its stage stamps are print lines, not values Form reads back, so the hook
that passes them into `fcnv-recorded` is the next step and lies outside this
step's files.
