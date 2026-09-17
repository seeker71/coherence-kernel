# Cached Qwen head batches, followed by an unsuccessful transfer test

Signed: Codex. Native Form/BML and the existing Metal carrier; no C seed
growth, foreign runtime or provider subprocess in this experiment.

## What became executable

`bml/qwen-lora-head-batch.bml` computes token-weighted full-vocabulary loss
and rank-one A/B gradients from cached normalized transformer features.
The transformer and base projection remain frozen. The caller owns the
completion mask, corpus splits, checkpoints and selection. Six native Adam
buffers belong to the optimizer; adapter and loss scratch have separate
owners. The head projection synchronizes pending device work before opening
its concurrent batch. Resuming a generation requires refreshing its head
from the retained stream.

The public device band compares the entire gradient with an independent CPU
chain rule, including unequal example weighting, nonzero A/B and scale 0.5.
Fresh preflight and execution pass, verdict 1, exit 0. Loss error is
**9.94761e-8**; relative A and B errors are each **1.74530e-8**. A real Adam
step lowers fixture loss from **3.076022625 to 3.075721661**. Projection bytes
stay identical; all **18** optimizer, adapter, scratch and fixture buffers
are released. Empty batches are refused.

## Training targets and separation

The native data builder generated binary-nested boolean repairs. Six programs
cover AND/OR with three, four and five arguments; two validation programs
use six arguments. Corrected programs compile, invalid input programs are
rejected, and **240 individual boolean input/output checks** pass.

The first proof harness lacked the core prelude. Its replacement initially
checked only the number of true results. That aggregate was insufficient to
prove the full truth table, so the proof was strengthened to compare every
input with its expected output. An exact audit confirms all eight captured
prompts, completions and splits are unchanged by the stronger proof. These
targets came from native construction and checks, not unverified model prose.

One Qwen admission captured **426** completion positions: **302 training**,
**124 validation**, width **5,120**, **8,724,480 feature bytes**. Capture took
**113,087 ms**. Each program had a fresh stream; prompt tokens were excluded
from supervision. Targets include the completion end marker. Repeated head
updates reuse these features without replaying the transformer.

Eight native Adam rounds used rate 0.001 and norm limit 1. Each round took
9,975–12,194 ms. Initial training loss was 0.379113; validation loss fell from
**0.312946 to 0.097776**. Epoch eight was selected by validation loss only.
Validation targets never entered updates. The exact selected A/B digest is
`0142eafc14552990d4f41a4717ec408c3be0108dd6baba0560fba3364c977369`.

## Return to the original repair

The original review-span utility repair was excluded from training and
validation. Its prompt is byte-identical to the retained baseline, with
**2,532 prompt tokens**, the same knowledge-query profile and the same
**1,536-token generation limit**. Training used only cached training features;
the original task's preserved stream was refreshed with the selected adapter
before generation. Training examples were not added to its prompt.

| Observation | Baseline | Selected adapter |
| --- | ---: | ---: |
| Compiler errors | 14 | 14 |
| Compiler exit | 1 | 1 |
| Generated tokens | 1,001 | 1,536 |
| Model reached its stop marker | yes | no |
| Behavior checks run | 0 | 0 |

Both outputs still contain invalid multi-argument `and` calls. Their first
2,104 bytes are identical. The new output also contains 267 bare trailing
comment lines and reaches the token limit. No generated source was executed;
the 32 behavior checks remain inapplicable while compilation fails. Lower
validation loss did not establish useful transfer, and stopping regressed.
The adapter stays private and unpromoted.

Training/admission took 193,119 ms. Generation through its receipt timestamp
took 341,819 ms, including cleanup; the final logged experiment duration was
562,334 ms, including receipt hashing. These intervals have different
boundaries from the old baseline's 315,259 ms total, so they do not establish
a generation speed comparison. All 15 training buffers and the evaluation
session were released. Provider subprocesses: **0**.

The native assessment retained both compiler results, completion flags and
byte comparison. Its correlated framebuffer control selected abstention;
the persisted and re-read private decision has `allow_promotion=0`. This
decision concerns this candidate, not a new global serving policy.

## Repairs made along the way

The band initially used an unavailable byte helper and then the wrong Q8
projection fixture. Native zero-byte construction and the actual wide
projection repaired them. Optimizer close returns a success flag, not a
buffer count; the count check now releases the explicit owned handles.

The first real training attempt failed before any update: optimizer
initialization left a serial encoder open, and the next concurrent projection
returned zero. A small loss-only reproducer exposed this boundary. Adding
synchronization to the projection fixed the reproducer and the actual eight
round run. Failed attempts remain retained locally. No check was weakened.

Private evidence lives under `.hearth/response-parity/`: the
`qwen-grammar-data-v3`, `qwen-grammar-features-v1` and
`qwen-grammar-training-v2` directories, with their native builders, capture,
training and assessment sources beside them. The public API and device band
are reusable; this particular corpus and experiment harness remain private.

## Instruments and learning

Drift gates pass **8191**. Native guide: Python implementations 0, execution
candidates 2, unread 0. Counsel reports **orphans 0; 11/12 lanes unobserved**,
with no standing hearth. Glass's first frame arrived in **29 ms**; its owned
viewer was stopped and no Form Glass process remained. Share is withheld:
the latest append range was not yet checked; semantic contribution remains
unmeasured.

At the recorded readings, parent output tokens were **2,209,753 cumulative**
and goal tokens **10,869,463 cumulative**. These are distinct meters, not
this experiment's isolated cost. Rented coordination cost remains substantial.

Verified procedure was retained by native session learning under event
`2026-09-17-qwen-cached-head-batches-procedure-v1`, session
`native-arrival-bootstrap`. The worker was launched; that learner serves
Llama, and retention alone is not a witnessed update. No evaluated answer
was supplied as a training target.

The useful surprise was repeated native learning from one feature capture.
The difficult finding was a large validation-loss reduction beside a failed,
less complete real repair. Preserving that difference gives the next attempt
a working learning mechanism and a concrete transfer failure. Response
quality, reliable stopping and overall session parity remain open.
