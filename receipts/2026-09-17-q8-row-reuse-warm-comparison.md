# Four-row Q8 reuse did not improve the warm response path

Signed: Codex. The production two-row kernel is retained. Three native
four-row candidates preserved the tested model outputs but showed no warm
whole-model speed gain. No runtime change from this experiment is promoted.

## Attempt and observations

The existing resident response door already retains one model admission
across requests. This experiment addressed the remaining GPU work: sharing
activation loads across four Q8 matrix rows while preserving each row's FMA
and reduction order. Candidate source, orchestration and assessment used
Form/BML with the existing C-bootstrap and dynamic Metal carrier.

An isolated probe of actual FFN gate, up and down tensors returned identical
output bytes and intact padding. Three interleaved samples per tensor showed
mixed timings. Those small samples justified a whole-model observation; they
did not establish a response-path improvement.

Each whole-model comparison admitted weights once and used independent
recurrent/KV states for the reference and candidate. Execution order
alternated by position. Starting at token ID 42, each path predicted 16 IDs;
every ID and every byte of the 248,320-float output logit vector matched at
all 48 compared positions across the three trials. These are 96 forward
passes and three model admissions, not zero model computation. No decoded
dialogue, provider subprocess or training ran during the comparisons.

The table sums positions 1 through 15 in each trial. Position zero is kept
separate because the reference ran first and bore the initial cold cost.

| Candidate | Reference warm ms | Candidate warm ms | Reference warm GPU us | Candidate warm GPU us |
| --- | ---: | ---: | ---: | ---: |
| Four rows, original excess group count | 2,542 | 2,706 | 2,210,264 | 2,361,272 |
| Four rows, matched group count | 2,518 | 2,555 | 2,196,560 | 2,209,438 |
| Four rows, matched count on tall matrices only | 2,539 | 2,562 | 2,213,108 | 2,220,117 |

Cold reference/candidate wall times were respectively **2,061/194 ms**,
**2,060/170 ms**, and **2,073/187 ms**. Including these positions made every
aggregate appear faster. The warm readings support no observed gain; the
small differences in the matched variants do not establish a general
slowdown. All three whole-model trials released both states and the model,
reported zero live buffers, and exited 0.

## Failures, correction and retained evidence

The first isolated command failed:

`form-run ./fkwu .hearth/response-parity/q8-row-reuse-probe.bml`

It exited 1 with `fkwu: form_error: activation write failed`. The helper
wrongly expected `metal_buf_write` to return 1. The carrier returns the byte
count. The corrected guard checks that count against the input length and
checks an exact readback. The corrected probe passed and released. Explicit
native cleanup was not reached in the first failed admission; no successful
measurement or clean release is claimed for it.

The native assessment reads the retained per-position rows, verifies their
sequence, parity, positive GPU work, release and successful process exit,
and separates the cold position from warm totals. Its correlated diagnostic
control selected restoration. The trial edits to
`form/native/metal/qwen35-dense-token-handle.fk` were restored;
`git diff --exit-code -- form/native/metal/qwen35-dense-token-handle.fk`
then exited 0. The experimental runner now requires the exact saved trial
dispatcher before admission, preventing a later run from silently comparing
the restored reference path with itself.

Private evidence lives under `.hearth/response-parity/` in the
`q8-row-reuse-*` helpers, logs and saved experimental dispatch sources.
`q8-row-reuse-assessment-v2.json` makes the three model admissions and 96
forward passes explicit. The original assessment's `language_generations=0`
was too ambiguous: IDs were predicted, although no response was decoded.
The original evidence is retained alongside the clarified assessment.

## Instruments, cost and embodiment

Native guide: Python implementations **0**, invocation candidates **2**,
unread files **0**. Counsel: orphans **0**, **11/12** lanes unobserved. The
hearth returned `signal=nothing reason=no-standing-hearth`. Glass reported
`TICK 12:16:33.724Z #0 dt=0/50ms d+0 ev=0 nodes=3M cons=129K`; its owned
viewer was closed. These readings do not establish serving throughput.

The preceding decoder teaching completed local learner round **78**, with
pending **0**, promotions **4**, and serving generation unchanged at **5**.
That learner targets Llama 3.2 3B, not the Qwen model used in these trials.
This trial's verified comparison procedure was retained under event
`q8-row-reuse-warm-comparison-verified-v1`, session
`codex-native-response-parity-2026-09-17`; its worker launched. Completion
and promotion of that new teaching are not yet claimed. Evaluated answers
were excluded from the teaching.

The clarified native assessment exited **0** with verdict **1**. Drift gates
passed **8191**, binary freshness **31**, and whitespace checks were clean.
The share reader reported `kind=declared` with percentage withheld while its
latest appended evidence was still being reconciled.

The completed-turn meter read the preceding coordinator turn
`01a0af2f-6cc4-78d0-b9c2-fe9c763fd60b`: **37** model calls,
**5,981,451** input tokens including **5,886,464** cached and **94,987**
uncached; **24,607** output tokens including **10,924** reasoning. Total:
**6,006,058**. Unattributed tokens: **0**. This is the preceding completed
coordinator turn, not the current open turn or the GPU probes. Zero provider
subprocesses does not erase this substantial coordination cost.

The useful surprise was that byte-exact candidates could look faster in
aggregate while failing the warm comparison. The difficult correction became
useful when the native assessment exposed that effect and the production
source was restored. This movement preserves a failed attempt as evidence;
it claims no dialogue-quality, resonance or throughput improvement. The
larger response-parity goal remains open.
