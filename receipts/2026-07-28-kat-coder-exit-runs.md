# 2026-07-28 — KAT-Coder's exit runs on the device

Urs: **"why did we stop at weights."** Fair. Weights decoded is not a pipeline. This is a pipeline.

## A token id in, a token id out

`form/native/metal/metal_kat_exit.sh` — **VERDICT PASS**:

```
PASS  the body located its own tensors: embed@551343232 type 11, out@10990720 248320x2048 type 8, norm@551335040 type 0
PASS  body-emitted MSL compiled (5658 bytes)
PASS  wrapped 17 391 937 152 bytes with bytesNoCopy — device.currentAllocatedSize = 17 392 025 600 B
PASS  embed row 100 decoded on device: 1578/2048 nonzero, max |w| 0.051116943
PASS  output_norm applied: 1578/2048 nonzero
PASS  all 248320 logits finite
PASS  logits non-degenerate: 4096+ distinct values sampled
TOKEN argmax id=3637 logit=10.066510  over 248320 rows x 2048 cols  wall 6.933 s
```

`embed(t) → output_norm → output.weight → argmax`, on the real sha256-verified file, whole thing
mmapped and wrapped rather than copied.

## What is claimed, and what is not

The vector handed to the vocabulary projection is the **embedding**, not the hidden state of 41
blocks. So id 3637 is a **mechanism witness** — the entrance decodes, the norm runs, the 540 MB
Q8_0 projection reaches all 248 320 rows, an argmax falls out — and **not** the token KAT-Coder
would emit. This is the DS4 lane's Stage 2 shape and it carries that stone's honesty with it:
*"its argmax is a mechanism witness, not the real token."*

## Zero new kernels

Every kernel is one the body already emits: `form_q3k_dequant_f32` (built this morning),
`form_mla_rmsnorm_f32`, `form_q8_0_matvec_f32`. The harness maps the file, binds buffers and
dispatches. Nothing new was authored in MSL.

## Where discomfort turned to gold

Two failures, both mine, both instructive.

**The header came back blank.** My emitter did `(print_str name)` then `(print_str value)` — and
`print_str` terminates each line, so every name landed on one line and its value on the next. The
carrier's `awk '{print $2}'` read an empty field and the gate reported *"embed is not Q3_K"* about a
tensor that is Q3_K. A parse bug wearing the costume of a data finding — the same
syncretism-shaped trap as the `grep -v '^0$'` that ate a datum this morning, and I walked into its
cousin nine hours later.

**`fh16` was undeclared.** `q3k-msl.fk` calls `fh16` for its super-scale; the q6k/q80 spine carries
the identical decode under the name `q6k_f16`. Two cells, authored days apart for different formats,
each naming the same f16 decode differently. The repair is one alias so the unit still holds exactly
**one** f16 decode — writing a second body for `fh16` would have been the easy fix and the wrong one.

## The most surprising teaching

**I reached for the exact kernel by default and the cost is visible.** 6.933 s for one 508 MMAC
projection is ~73 MMAC/s. `q8-0-msl.fk` deliberately emits *two* matvecs: the **attestant**, one
thread per row with a serial fold, answering to equality; and the **lane**, one SIMD group per row
with `simd_sum`, which reassociates and so answers to an epsilon. I bound the attestant without
thinking about it — and for a first witness that is the right order, exactness first and speed
second. But the choice was made by habit, not decision, and the body had already written the
alternative and labelled precisely what it costs.

## The frontier question

> **What names two independently-authored things that name one shared meaning differently?**

Asked and **not landed** — `homoplasy` (884) already names a trait arisen separately in two lines,
which is exactly `fh16` and `q6k_f16`. Sixth time tonight the corpus had the word.

## Ground stamp

```
form/native/metal/metal_kat_exit.sh   -> VERDICT PASS, argmax id=3637, 6.933 s
sha256 of the blob == the publisher's manifest
```

## What remains for a real token

The 41 blocks. 30 linear (`gated-deltanet-layer` + the 256-expert router, both already on device)
and 11 full-attention, plus the residual and norm plumbing between them. The exit is now a door that
opens; what it opens onto is the middle.
