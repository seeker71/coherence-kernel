# 2026-10-11 — where a Form-token cycle's time went, and what it is now

Asked: "do you think 40 mins is healthy? where does the time go?" Read from the runs' own events
(`observe/form-token-time-run.bml` over a run's `training-events.jsonl` and the cycle ledger). Same corpus (729 rows, 648
train, 81 held-out), same 100 optimizer steps, batch 4, on the M4 Max beside a day-turn, band sweeps and another agent's
idle task. Held-out loss before 2.7061845 and after 0.0400733 in every full run: the arithmetic did not move.

| | before (earlier kernels) | after the matrix unit, records, lease | final stack |
|---|---|---|---|
| train stage, 100 rounds | ~4,700 s (61 rounds measured: 2,914 s) | 1,525 s | **562.6 s** |
| open the voice | 62 s (tokenizer 41 s) | 8 s | 8 s |
| baseline validation, 81 rows | 282 s | 148 s | 69 s (0.8 s remembered) |
| a row's forward / backward | 3.6 s / 6.5 s | 1.6 s / 1.1 s | 0.5 s / 0.3 s |
| per input token, forward / backward | 22 / 40 ms | 10 / 7 ms | 3.5 / 2.0 ms |
| evaluation, 81 questions | 618 s (6.4 ids/s) | 403 s | decode step 38 ms (26 ids/s) |
| held-out answers exact, in stream | 53 of 81, forged 0 (cap 96) | 60 of 81, forged 4 (cap 256, 3 crossings) | **66 of 81, forged 0** (nothing told in advance) |

What the time was, in the order it was found:

1. The device's own, not the walker's: the device was busy 95 to 98% of every row's wall. Inside it, a 64-bit division by the
   runtime group size in every lane (`nam_g` in 32 bits: decode 3.4 ms to 0.6 ms, block 4.9 to 2.4 ms for eight 3B products).
2. Scalar ALU where the hardware has a matrix unit: sixteen tokens and more now go through `simdgroup_multiply_accumulate`,
   forward 4.4 times the block kernel, backward 9 times the scalar walk.
3. The 52 to 62 s "open" was the tokenizer (41 s): a Form-level ordered index over 128 thousand vocabulary and 280 thousand
   merge keys. fkwu records walked their keys linearly; they hash their bytes now (`fk_rfind`), the tokenizer's tables are
   records, tokenization identical over 256,038 ids.
4. Host work per dispatch: 150 us, of which 115 us rebuilt the Metal source string on every call; built once, 15 us (a 91-token
   row 1.35 s to 0.45 s).
5. The voice's three argmaxes after every token were three dispatches and three syncs; one `nll_top3` (60 ms to 38 ms a token).
6. The same 81 baseline rows scored every run under an unchanged adapter: remembered under a key of adapter, rows, mask
   policy and numerics. The corpus census no longer runs twice.
7. Two ceilings that read as model failures: 96 ids and 3 crossings cut or left unanswered the longest chains (node-select
   0 of 5, recipe-both 0 of 4 with 4 forged; the voice had written the controls correctly). The walk now follows the stream.

Surprises held as teachings: a first decode kernel factored the scale out of the sum and the trainer's exact head check
stopped it at 3e-6 (now contraction off, bit-equal, and the check answers to an epsilon because the matrix unit chooses the
association); aligned wide loads gave NaN on the real model's buffers while the band's synthetic ones passed; `stdin-text()`
drains, so a cycle told `shared` slept 34 minutes behind a neighbour; the default `export` of the trainer door was the
serving adapter's own directory.

Confirmation from the landed main (ae3a18307, 19:05 to 19:25, defaults only, nothing about a chain's length told): 66 of 81
exact, forged 0, truncated 2, held-out loss 2.700 to 0.029; train stage 787 s, evaluation 371 s. The train stage was slower
than the 563 s above because the machine was heavily shared (a day-turn at 97% CPU, a drift-gates run, a build and a band
sweep at once): a row's forward and backward read 0.8 s and 0.5 s here against 0.5 s and 0.3 s, the same code. The spread
between the two runs is the neighbours' share, which the lease does not yet cover.

The same cycle again on 2026-10-12 from the landed main (96f3ba937), now on Form rows end to end (options as lines, ledgers,
corpus and dataset as rows; `docs/json-inside-form.md`), on a quiet machine: train stage **315 s** (open 2.9 s, baseline
validation 93 s, 100 rounds 178 s + Adam 27 s; a row's forward 0.2 s and backward 0.1 s, 1.7 and 1.0 ms a token), evaluation
449 s, **66 of 81 exact, forged 0, truncated 2, all nine kinds written** (effective 5.23 against 5.16 expected), held-out loss
2.700 to about 0.03. The train stage is now 15 times faster than the 4,700 s it began the day at, and the evaluation (449 s,
81 questions) is the largest stage left.

A correction, 2026-10-12, from reading a run's events on nodes: the time door summed every `validation` event under "baseline
validation". A run holds two, the baseline before training (0.5 s when remembered) and the held-out check after the last round,
and the figures above that read "baseline validation" (282, 148, 69 and 93 s) were those two together; once the baseline is
remembered they are the after-training check, 81 rows scored forward-only. A 2-step run beside a day-turn on a shared GPU reads
0.5 s and 53.7 s. The door now says them apart. Read again on a quiet GPU (nothing leased, 2026-10-12, corpus refreshed, 81 new
rows so the baseline is scored too): before training 42.7 s (0.53 s a row), after training 78.1 s (0.96 s a row), while a
training row's forward costs 0.2 s, and the runtime's time board puts 88% of the run in the `gpu` phase, so it is the device, not the
walker. The same rows scored alone by `observe/native-lora-score-bench-run.bml` (24 rows) cost 3.6 to 4.2 ms a token wall and 2.8
to 3.5 device, 1.4 to 1.6 times a training forward (2.5 ms), with no growth along the process (device busy for 91-token rows
wanders 200 to 340 ms; a row is 622 dispatches and 2 syncs). So the scoring kernels are ordinary. Read again the same evening (20:50, a day-turn
running, trainer taking `shared 1`, then the bench straight after): the whole two-step run took 25 s where it took 61 and 126 s
in the afternoon, the bench scored at 1.55 ms a token (it read 3.6 to 4.2 that afternoon), and the trainer's after-training check
cost 20.2 s for 81 rows (0.25 s a row, 2.8 ms a token) against about 10.7 s the bench's rate gives for the same rows. Two
lessons: the same code on this machine moved by a factor of 2.7 between two readings an hour apart with no lease held by anyone, so a
single reading attributes nothing and only a back-to-back pair does; and a residual of about 1.9 times stands between the trainer's
after-training check and the same call alone (tokenizing is not it: 87 ms for 24 rows). That residual is worth about 10 s of a
short run and is left unread; the evaluation stage (449 s of the cycle) is the larger target.

The residual above was read from the body's own counters on 2026-10-13, once the trainer's validation events carried this
process's device busy time (`device_busy_ms`): of an after-training validation of 22.2 s the device was busy 14.7 s (66%) for this
process; the time door, the eval line and the decode bench print wall and busy time with every run. A first reading of that gap as
"a neighbour's 6.0 s" used a whole-GPU gauge that does not integrate a phase (corrected just below); the gap is read by the machine
candle and the sustained-load curve below.

A correction to the paragraph above, same day, from the first full cycle with the counters (14:21 to 14:38, 979 s: refresh 9 s, train
613 s, evaluation 354 s, usage 1.5 s): the "neighbours' share" it names (6.0 s of 22.2) was an artifact. The whole-GPU figure is
`host_gpu_busy_us`, the accelerator's level integrated over the gaps between reads, and the level is the mean since the previous read
by anyone: read at the start and the end of a phase it is the last instant's level spread over the whole phase (it read 99.6% for
validation and 0 ms for an evaluation that kept this process's device busy 86% of 354 s). Another reader (the glass's machine frame,
started that afternoon) also takes the level from whoever asks second. It is removed from the trainer, the time door and the
benches. What stays are this process's busy time (`device_busy_ms`, `metal_live` word 11) and a standard candle (`nam-candle`): a
fixed product (128 by 3072 by 3072, twice) timed by the device's own counter, 27.4 to 29.1 ms a product at rest, in the trainer's
`admitted`, `validation` and `finished` events, the eval line and the decode bench.

What the candle read, 2026-10-13 (tenths of a ms a product): at rest 27.6 to 29.1; after 30 s of continuous work 70.7, 72.8, 76.0;
3 s of rest later 28.7 again. The device runs 2.6 times slower under sustained load within seconds and is back within seconds of rest,
which is a clock or power governor, not a neighbour and not heat. Every burst benchmark of a few seconds therefore overstates what a
long run gets: decode held for 400 tokens reads 37, 53, 42, 42, 41, 62, 50, 60, 64, 58 ms a token by windows of 40 (mean 50.8, the
burst figure was 20.7), the device's busy time climbing from 21 to 39.5 ms a token and the candle from 27.5 to 39.5. The cycle's
train stage read 4.66 ms a token forward at every row length (4.4 to 5.4 from the shortest rows to the longest, median row 0.4 s, 90th
percentile 1.7 s, worst 7.0 s): the sustained rate, not a stall, and not length. The 1.7 ms a token of the table above is a burst
reading. The time door now shows the median, the 90th percentile and the cost by row length.

The kernels, 2026-10-13 (landed 5035bdc90 to 8cc285a53). The decode profile named three groups of single-simdgroup kernels, each a chain of
dependent loads: the LoRA input product `nam_matmul` (20% of a token's device time) and the norm `nam_rms` (10%), the argmax and top-3
over the 128256-entry vocabulary (1.2 ms each, once a token), and attention (`nll_scores`, `nll_attend`, 12%). Each was rewritten so
that its result is the same: loads eight ahead, the adds in the old order (matmul, rms, attention: 14 and 9 shapes hash identically
against the old build), and a 1024-thread merge for the order-independent max and top-3 (value descending, lowest index among equals;
`nll-top3-band` reads 15 against the three older selects, on ties, a small vocabulary, a NaN and the 3B vocabulary). Per dispatch,
alternating old and new builds and taking minima: matmul 29 to 10 us, rms 44 to 10, argmax 1194 to 39, top-3 1263 to 44, scores 67 to 39,
attend 83 to 22. A matvec unroll was tried and gained nothing, so the 4-bit matvec (36 to 42% of the device time) is unchanged. The
400 decoded ids checksum identically on the old and the new build in all three alternating pairs; with each run's wall per token
divided by its own mean candle (the machine was 2 to 3.5 times slow throughout), the new build reads 0.80, 0.67 and 0.72 of the old
build's cost: about 30% less time a token, held for 400 tokens. At burst (40 tokens, calm machine) the matmul and rms alone took the
device time from 16.8 to 14.6 ms a token.

Measured and not built, 2026-10-13/14. (1) The 4-bit matvec is not bandwidth-bound: on a gate-sized weight (8192 by 3072) rotated over 24
sets (300 MB, past the system cache) it takes 6.6 to 15 times as long as a streaming read of the same weight bytes in the same
machine state; dequantizing eight nibbles and multiplying is the cost, and the one exact restructuring tried (four chunks computed ahead)
changed nothing. (2) Batching is exact and large: a block of 8 tokens costs a third to a quarter of eight single steps (busy 327, 171,
156 ms against 1085, 450, 703; wall 359, 197, 175 against 1154, 498, 771), because the 2 to 15 token kernels dequantize each weight once
for eight tokens and the decode kernel is bit-equal to them. Decoding several evaluation questions in lockstep would use it; it needs a
trampolined walk (the voice's feed is the one model call) and a multi-sequence forward, and is not built. (3) Lookup drafting is not
worth building: `observe/spec-lookup-sim-run.bml` replays the 81 recorded streams (the voice's tokens apart from Form's injected
observations), proposes the continuation of the longest earlier match of the last one to three ids, and charges a verify block 1.0 plus
0.2 a token: 1.06 to 1.21 times at best (k 2, minimum match 2: 3,341 forwards and 3,888 single steps for 4,731 tokens; k 4: 1.55 tokens
a forward). An incident worth keeping: a first version of that door mapped over the integer in `questions` (a count) instead of
`questions_judged` and wrote its diagnostics until a 39 GB file stood in the scratch directory; it is bounded now (a list check, and a
run that is piped through `head -c`).

The frozen prefix, 2026-10-14. The train stage's device time is 86% one kernel, `nam_affine_mma` (23.8 of 27.7 s in a four-step
profile), and that kernel runs at 7.7 to 8.0 TFLOPs on every shape tried (3072 square, 8192 by 3072, 3072 by 8192, 165 tokens): the
matrix unit's fp32 rate. Two exact restructurings were measured in a lab and changed nothing: aligned 32-bit and 16-bit loads for the
packed word and the scale (402 against 405 us), and a 64-token by 32-row tile with four loads for four MACs (421 against 406). What
the profile did show is work that need not be done twice: the adapter is on the last 8 of 28 layers, the 20 below depend only on the
model and the token ids, and a whole row forwarded for training or validation meets the same hidden state at that boundary every
time. `nll-forward` now keeps it (`.hearth/prefix-cache/<hash>.f32`, the hash of the model, the numerics tag, the boundary layer and
the ids; the activation and a trailer of its own squared norm, maximum and a signature) and a later forward of the row maps the file as
the input of the first adapter layer, so the embed and the frozen layers are skipped and the tape holds the top layers only (the
backward already began at the first adapter layer). Real model, four training steps, three runs: the loss curve and the held-out result
are identical to the last digit with the cache off, cold and warm; cold costs nothing (25.6 s against 25.1) and saves 85 rows; warm hits
all 85 and the run takes 13.7 s, the after-training validation 8.1 s instead of 19.1; scoring 12 rows on the bench goes from 125 to 50 ms
a row. A zeroed file of the right size (a write-back that never came) fails its trailer and the row is recomputed
(`nll-prefix-cache-band` 63). A cycle's first pass fills the cache (about 1.5 GB for 728 rows) and every later pass on the same rows
skips the frozen forward: the train stage's 0.7 s forward a row falls toward 0.2 s. `observe/prefix-cache-run.bml` shows its size and
prunes by days unused or by size; a hit renews an entry's time. `FORM_NLL_PREFIX=0` turns it off.

The first full cycles after the kernel wave and the prefix cache (2026-10-14, 16:45 to 17:01, defaults, `shared` because the day-turn
sits on the machine; the candle at rest 27.5 ms). Cold, the cache empty: 358 s in all (refresh 1.4 s, train 245.8, evaluation 109.7,
usage 0.5); the train stage reads 1.63 ms a token forward and 0.90 backward, 3 cache hits and 462 misses (462 entries, 866 MB written),
the after-training validation 33.4 s. Warm, the same cycle again: 365 s (train 218.2, evaluation 145.1); 465 of 465 row forwards hit, the
after-training validation 10.4 s (3.2 times faster), a training row's forward 0.1 s against 0.2, the train stage 11% shorter. The
whole-cycle pair does not show the saving because the evaluation, which does not touch the cache, ran 32% longer in the second pass:
the same 81 questions kept the device busy 89.7 s over a 109.3 s wall the first time and 125.4 s over 144.8 s the second, and its own
candle rose from 27.3 to 39.6 ms in the first and from 27.5 to 51.1 ms in the second. The governor's state moved the same work by
40%, which is the size of the effect being looked for; the controlled evidence for the cache is the four-step trio above (25.1 s off,
25.6 cold, 13.7 warm, the loss curve identical to the last digit). What the pair also says, and this is the cycle-level check of exactness: the two cycles trained to the same adapter, since the 100
evaluation lines of each (the 81 questions, the family lines, the comparison) are identical, 67 of 81 exact, 4,735 local IDs, 142
controls, 139 observations, 12 refusals, none forged, with the first trained over an empty cache and the second over 462 hits.
What the pair says plainly: with the machine calm, a full cycle
(census, 100 steps on 400 rows, 81 questions answered in stream with 67 of 81 exact earlier today) takes about six minutes, against 16
on 2026-10-13 at a candle of 70 to 90, and no way yet to split that between the machine's state and the kernels without a back-to-back
pair of builds at one state (the decode pairs above give about 30% for the kernels). A backward that read 0.90 ms a token cold and
1.28 warm is not explained by the cache (the hit path hands the backward the same tape); the long rows read 1.3 to 2.6 ms warm, which
is the throttle again, and is left as read.

Decode, 2026-10-12: a token was 28.3 ms of which the device was busy 16.5; a token's 621 dispatches were encoded into one batch and
committed at the sync, so host (about 12 ms of encoding; buffer allocation is 2 of it) and device ran one after the other. The
decode forward now submits each layer's batch as it fills (`nll-flow`, `metal_submit`; the closing sync drains them in order):
20.7 ms a token (every 4th layer 21.6, every 2nd 20.9, every layer 20.7), device busy 16.9, the numbers unmoved. Eight held-out
questions asked of the bare voice with and without it gave the same 565 IDs, answers, controls and forged counts, 50.9 s to 47.3 s
(that arm spends most of a question on the teaching prompt's prefill, not on decode). A second find on the way: the epsilon head check
of 2026-10-11 left every correct head reading raising a false surprise (its expectation was the bound, its observation the
difference); `native-lora-resume-band.fk`, which no gate runs, had gone red on that and read 7 again after the reading's
expectation became "a difference within the bound" with the bound in its evidence.

Still open (named, not hidden): early stopping on a held-out check (the training loss wanders between 0.003 and 0.4 from
round 60 on while the held-out loss ends at 0.03 to 0.04, so some of the 100 steps are spent past the gain); the day-turn and the band sweeps do not take the GPU lease, so their time-slices still
inflate timings; the corpus digest is a 4 s Form SHA-256; the 17 MB tokenizer JSON parses in 3 s; forward and backward
are 25% non-device (a decode token is 621 dispatches, fusing q/k/v and gate/up would cut a fifth); `eval-compare`,
`eval-refusal` and `recipe-import` (10 refusals) are the model's own gaps, now visible without a harness ceiling over them.
