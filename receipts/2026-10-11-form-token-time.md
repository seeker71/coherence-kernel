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
0.5 s and 53.7 s. The door now says them apart. The after-training check is the largest part of a short train stage and costs
about 0.66 s a row where a training row's forward costs 0.2 s: the next thing to read (does validation take the fast path?).

Still open (named, not hidden): early stopping on a held-out check (the training loss wanders between 0.003 and 0.4 from
round 60 on while the held-out loss ends at 0.03 to 0.04, so some of the 100 steps are spent past the gain); the day-turn and the band sweeps do not take the GPU lease, so their time-slices still
inflate timings; the corpus digest is a 4 s Form SHA-256; the 17 MB tokenizer JSON parses in 3 s; forward and backward
are 25% non-device (a decode token is 621 dispatches, fusing q/k/v and gate/up would cut a fifth); `eval-compare`,
`eval-refusal` and `recipe-import` (10 refusals) are the model's own gaps, now visible without a harness ceiling over them.
