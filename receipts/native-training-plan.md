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
from before the fields read on). The voice door records those clocks from the
session's own stage instants: the model session stamps each phase it prints
on its record (`form-cli-model-session.fk`, `fcms-clock-read`), and
`fcnv-recorded` lands prefill over prefill alone, decode over the decode
steps, with admission and open as their own fields on the LocalRun row. An
unstamped instant reads nothing, so that rate stays absent; rows written
before this still read with the rates absent.

## 2026-10-28 — Qwen3.8-27B last-block ladder, rung 1 (reproduction)

Band `form/form-stdlib/tests/qwen-last-block-band.bml`, verdict 15 in two runs
(`.hearth/qwen-rung-1/band-run-1.log`, `band-run-2.log`). The serving prefill's
tap captured block 63's input, output and the final residual for a 23-id
ChatML prompt (`xin.f32`, `xout.f32`, `xfin.f32`: `[23, 5120]` f32, 471,040
bytes each, with JSON sidecars); the block's forward replayed natively from the
captured input on trainer-owned scratch (`qwen-last-block-forward.bml`) with
the body's q38 kernels and `blk.63.*` from the GGUF: 0 of 117,760 words
differ, 0 ulp, max |reproduced − model| 0 against max |model| 621.09; head
argmax 760 both ways. Measured tolerance 0; the band holds that reading. No
new kernel, no tap change. Prefill 31,252 / 33,956 ms on a shared GPU, block
forward 43 / 32 ms. Detail: `docs/qwen-last-block-lora-design.md`, "Rung 1
observed". Rung 2 (finite differences) is next; it needs `inv` per row from the
norms and starts from the contiguous P this forward keeps.

### Q4_K decode: the K-quant lane now folds a pair of rows per threadgroup (2026-10-28, step q4-decode)

The slow part named above was found in Form, not in the C seed: `form/form-stdlib/bml/qwen38-decode-quant.bml`
dispatched Q4_K (type 12) and Q6_K (type 14) decode through `qml-hoist-msl` at parts 1 — ONE GPU thread per
output row walking all 5120-17408 columns serially, with get_scale_min_k4 (three byte reads, four integer
divisions) decoded once per WEIGHT, 128x fewer threads than the Q8_0 wide twin. The door now emits
`form_qwen_q4k_decode_f32` / `form_qwen_q6k_decode_f32`: the Q8_0 wide shape (256 threads, eight SIMD groups,
two rows a group), a fused dot over the packed block (`(d*sc)*sum(nib*x) - (dmin*mn)*sum(x)` per eight weights
for Q4_K, `(d*sc)*sum(q*x)` for Q6_K), two register loads of weights a row-iteration, and it owns the dispatch
record `q38dq-rec` that `q38-mv-rec` (qwen35-dense-token-handle.fk, one line) now runs. Pipeline indices
57/58 unchanged. `runtime/fkwu-uni.c` carries no Q4_K path and was not touched; `fkwu` was rebuilt from the
unchanged source (`cc -O2 -o fkwu runtime/fkwu-uni.c`, 3.9 s) because the checkout's binary predated the
13:53 runtime commit.

Measured through the same driver (`.hearth/speed-ask.bml`, qwen38-q4, 382 prompt ids, greedy; the 32-id
window `decode-checkpoint-32 -> -64`; logs under `.hearth/q4-decode/`). The GPU was shared with sibling
sessions the whole afternoon (`host_gpu_utilization` read 98-100 before every stamped run; siblings' Q8
sessions pids 8583/29797 at 2-99% CPU), so intervals carry their load, not only the kernel:

| state | run | prefill ms | prefill tok/s | decode ms (32 ids) | decode tok/s | ms per id | wall ms | reply |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| before (serial hoist) | 1 | 46984 | 8.1 | 116656 | 0.27 | 3645 | 339443 | identical |
| before (serial hoist) | 2 | 32616 | 11.7 | 80959 | 0.40 | 2530 | 352739 | identical |
| Q4_K fused, Q6_K serial | 1 | 38584 | 9.9 | 11569 | 2.77 | 362 | 93000 | identical |
| Q4_K fused, Q6_K serial | 2 | 42813 | 8.9 | 17599 | 1.82 | 550 | 228619 | identical |
| both fused | 1 | 37748 | 10.1 | 21391 | 1.50 | 668 | 342835 | identical |
| both fused | 2 | 43319 | 8.8 | 20341 | 1.57 | 636 | 128997 | identical |
| both fused, GPU stamped | 1 | 40622 | 9.4 | 12669 | 2.53 | 396 | 90054 | identical |
| both fused, GPU stamped | 2 | 20863 | 18.3 | 8951 | 3.58 | 280 | 43644 | identical |

Observed: the same window went from 0.27-0.40 tok/s to 1.50-3.58 tok/s; the quietest early ids of the last
run (checkpoints 2 -> 8) ran at 90-93 ms an id (10.8 tok/s, 15.6 GiB at ~170 GB/s), which is the kernel
when the siblings paused. Prefill is untouched (the K-quant prefill was already on the matrix unit).
Per-kernel, under the same shared load (`q4k-decode-wide-band.fk`, best of five rounds of 20 dispatches):
ffn_gate Q4_K 17408x5120 fused 5-24 ms vs hoist 53-172 ms; ffn_down Q4_K 5120x17408 4-21 vs 120-390;
attn_q Q4_K 12288x5120 3-23 vs 48-168; ffn_down Q6_K 13-14 vs 190-306; attn_qkv Q6_K 10240x5120 8-32 vs
79-125; the head Q6_K 248320x5120 (4 dispatches) 31-73 vs 284-323. The greedy reply stayed the serial fold's
(`.hearth/speed-reply-qwen38-q4.txt`, 479 bytes) in all eight runs.

What the fused fold answers to: not the byte (it reassociates) but a named bound of the exact fp64 dot,
`(73 + ceil(nb/8)) * 2^-24 * S` with S the unfused magnitude; measured dev/bound 1e-6 to 6e-4 on the
committed fixtures and on real rows (first and last pair) of the gate, down, q/qkv and head tensors, plus
`float(as_type<half>)` proven bit-identical to the body's `q4k_f16` on all 63488 finite f16 patterns.
`qwen38-prefill-quant-band.fk` now holds K-quant prefill (already on the matrix unit) and K-quant decode to
that bound instead of a byte claim its kernels had stopped making: at HEAD it read 28 on the prefill bits
before this step; it reads 31. Census of the Q4_K_M file through the lane's own open: linear layer 0
`0 0 12 12 14 14 0 12 12 0 0 0 12 12`, full layer 3 `0 0 12 12 14 12 12 14 0 0 12`, head 14 — attn_qkv,
the early ffn_down, attn_v and the head are Q6_K, which is why the lane needed both twins. Open: the Q4_K
"fast-answer" role is now read from a measured rate, not from the file size; the next voice-rates table
should be taken with the GPU quiet.

Skeptic reading (same driver, same prompt, `.hearth/q4-decode/skeptic-q4-run1.out`, 16:41 WITA, one
sibling alignment read on the Q8 file, `host_gpu_utilization` 99 before / 97 after): prefill 12453 ms
(30.6 tok/s), decode-checkpoint-32 -> -64 2106 ms (15.2 tok/s, 65 ms per id), wall 29133 ms, 92 ids, reply
byte-identical to the reference. Bands re-run on this checkout: `q4k-decode-wide-band.fk` 2047,
`qwen38-prefill-quant-band.fk` 31, `q6k-q4k-matvec-tg-band.fk` 8191, all exit 0
(`.hearth/q4-decode/skeptic-*.log`). The door's caller-less `q38dq-enqueue` wrapper (no caller at HEAD
either) was removed; `q38dq-rec` is the record's only door.

After table (2026-10-29, step review-and-register): the six stamped after runs of this step in the shape
of the 12:23-12:34 voice-rates table above, so the register reads them. Every number is read from the named
`.hearth/q4-decode/*.out`: open ms is `model-session-context-open-begin` -> `model-open-ready`, prefill ms
`model-session-prefill-begin` -> `-ready`, decode ms `decode-checkpoint-32` -> `-64`, wall ms, prompt ids
and generated ids the run's own `[speed-ask]` line (the arithmetic: `.hearth/review-register/q4-after-table.log`).
No Q8_0 run was stamped in this step, so the table holds Q4_K_M rows only. The register
(`observe/offline-voice-gaps-run.bml`, gap `q4-decode-vs-q8`) reads the last table row of each key in this
document: **this table's last Q4_K_M row is the lane's current reading**, and the Q8_0 row it is read
against is the last one of the 12:23-12:34 table (Q8_0 run 2, 2.79 tok/s) — a comparison across two sessions
and two GPU loads, not one run; a Q8_0 row stamped beside the next Q4_K_M run makes it one.

| quant | run | wall ms | open ms | prompt ids | prefill ms | prefill tok/s | generated ids | decode ms (32 ids) | decode tok/s | ms per decoded id |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Q4_K_M | after 1 (`after-q4-run1.out`) | 93000 | 270 | 382 | 38584 | 9.9 | 92 | 11569 | 2.77 | 362 |
| Q4_K_M | after 2 (`after-q4-run2.out`) | 228619 | 913 | 382 | 42813 | 8.9 | 92 | 17599 | 1.82 | 550 |
| Q4_K_M | final 1 (`final-q4-run1.out`) | 342835 | 279 | 382 | 37748 | 10.1 | 92 | 21391 | 1.50 | 668 |
| Q4_K_M | final 2 (`final-q4-run2.out`) | 128997 | 12657 | 382 | 43319 | 8.8 | 92 | 20341 | 1.57 | 636 |
| Q4_K_M | finalb 1 (`finalb-q4-run1.out`) | 90054 | 14270 | 382 | 40622 | 9.4 | 92 | 12669 | 2.53 | 396 |
| Q4_K_M | finalb 2 (`finalb-q4-run2.out`) | 43644 | 3360 | 382 | 20863 | 18.3 | 92 | 8951 | 3.58 | 280 |

The skeptic run (`skeptic-q4-run1.out`, 15.2 tok/s on the same window) stays in the prose above: it ran
after the finalb pair under a lighter load, and this table keeps the step's own after/final/finalb runs.

## 2026-10-29 — Qwen3.8-27B last-block ladder, rung 2 (backward, finite differences)

Band `form/form-stdlib/tests/qwen-last-block-backward-band.bml`, verdict 15 in two runs
(`.hearth/qwen-rung-2/band-run-1.log`, `band-run-2.log`, `rung-2-receipt.json`), after a rung-1
rerun read 15 again (`rung1-before-4.log`: prefill 11,818 ms, replay 17 ms, 0 words differ). A
rank-8 LoRA pair on `blk.63.ffn_down` (door `qwen-last-block-backward.bml`; ffn_down because its
product lands on the residual, so the loss on the block's output reaches A and B through the
identity alone), loss = mean (x2 − target)² with target = the captured output + a hash shift in
[−0.5, 0.5), folded in 2⁻³⁶ fixed point. With B = +0 the forward through the pair is rung 1's replay
(0 of 117,760 words, 0 ulp; loss 0.08293476031688939). Analytic dB/dA (`nam_matmul_sk` /
`nam_matmul`) against central finite differences on 8 entries each, perturbations read back (1 on B,
8 on A): max relative error dB 5.591031080282419e-06, dA 5.163047689368592e-05 — the measured
tolerance, now the band's bounds, reproduced to the last digit in run 2 under a different GPU load.
Memory counted 13,791,888 bytes against the design-D budget 28,609,728; 26 of 26 handles freed;
32 finite-difference forwards in 1,228 / 1,080 ms, band wall 8.4 / 7.3 s. No kernel of section C was
needed on this path (two new in the door: `form_hash_fill_f32`, `form_mse_fixed_f32`); the plan's
rung-1 note about `inv` per row described the attn_output choice. Detail: `docs/qwen-last-block-lora-design.md`,
"Rung 2 observed". Rung 3 (20 steps on 8 rows, held-out loss) is next; it needs the CE loss over the
head and Adam on the pair.
