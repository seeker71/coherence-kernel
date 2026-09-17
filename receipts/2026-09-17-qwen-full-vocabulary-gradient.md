# Qwen head learning against the whole vocabulary

Signed: Codex. Form/BML implementation; existing native Metal carrier;
no C seed growth, foreign runtime or provider subprocess.

## The gap and the implementation

The earlier Qwen learning witness minimized cross-entropy over two selected
tokens. That omits competition from the rest of the vocabulary. The new
`bml/qwen-lora-full-loss.bml` reuses the existing native `nlb_ce` operation
for full-vocabulary loss and its logit derivative, then computes
`W^T(softmax(logits)-onehot(target))` directly over the frozen Q8_0 projection.
The existing `lbw-backward` carries that gradient into the rank-one A/B pair.

The transpose operates in 1,024-row chunks followed by a reduction. It does
not materialize the projection as float32. Six explicit scratch buffers
belong to the returned owner; cached Metal pipelines belong to the runtime.
Admission checks Q8_0 row alignment and the decoder's unsigned packed-byte
offset limit using division before any potentially overflowing product.
Caller-owned handles, storage type and matching buffer lengths remain the
caller's responsibility. The API itself neither updates nor promotes weights.

## Re-observation on real Qwen weights

Private `qwen-full-vocabulary-probe.bml` used the registry's `qwen38-q8` base,
with **248,320 vocabulary rows and 5,120 hidden columns**. Training input was
the six-token prefix `1 + 1 = `; its target was native `add(1,1)`, rendered
and tokenized. A single SGD step at 0.001 updated a private head adapter.
The safetensors artifact was re-read and admitted through `fcmd-admit` before
its actual adapted head was measured.

The complete loss/transpose/readback took **13 ms**. A directional finite
difference gave **3.491401672**, compared with the gradient's **3.491876246**;
the error normalized by `1 + abs(derivative)` was **0.000105652**. The complete
probe, including model admission and the readings below, took **29,367 ms**.

| Reading | Full-vocabulary loss before | After | Greedy answer before / after |
| --- | ---: | ---: | --- |
| Training: 1 + 1 | 0.575615 | 0.575102 | correct / correct |
| Held out: 2 + 3 | 0.358467 | 0.358475 | correct / correct |
| Held out: 3 + 4 | 0.385036 | 0.385044 | correct / correct |

Both held-out prefixes and targets were excluded from the update. Their
losses increased slightly; neither greedy answer changed. This establishes
neither generalization improvement nor response-quality parity. The candidate
remains private and unpromoted. Its exact admitted A/B tensor digest is
`34c48b0c541737e8ea101b91e92ddf7f8a758d3dcbd3256a7d1873816ba57c48`.

Each held-out stream released its 128 buffers. Final cleanup released all
**1,022 expected buffers**, including six gradient scratch buffers and three
adapter buffers. The probe exited 0. Input, candidate, receipt and per-row
observations remain under `.hearth/response-parity/qwen-full-vocabulary-v1`.

Native assessment emitted each adverse row through organ health and correlated
framebuffer abstain controls. It retained and re-read a private decision with
`allow_promotion=0`. This records this candidate's decision; it does not claim
to install a global serving-policy gate.

## Checks, failures and boundaries

The public device band passes 1, exit 0, comparing independent CPU loss and
transpose derivatives with the native operation at shapes 37-by-64 and
1,025-by-32. The latter crosses the row-chunk boundary. Largest loss error:
**1.70458e-7**; largest relative gradient error: **4.46996e-7**. Invalid targets,
nonfinite loss, malformed geometry, multiplication overflow and packed-byte
overflow are refused. Both cases release all eight scratch/fixture buffers.
The geometry guards were hardened after the real-model measurement; fresh
preflight and the complete device band passed afterward.

Initial band preflight refused `math_abs` and the missing `lsl-bytes` prelude.
The native absolute-value helper and existing byte-serialization prelude
repaired them. The private assessment initially had an unmatched delimiter.
An execution was issued before inspecting that failed preflight and was also
refused by the compiler, exit 2. Splitting its nested expression repaired the
source; fresh preflight and execution then passed. No failed run is a witness.
Inspection then found that the assessment had used `oh-voice`'s print result
as its retained reading, losing row identities. It now retains each reading
before printing and validates every persisted row. The corrected decision is
`promotion-decision-v2.json`; the incomplete first decision remains available.

Drift gates pass **8191**, exit 0. Native guide: Python implementations 0,
execution candidates 2, unread 0. Counsel: **orphans 0; 11/12 lanes unobserved**,
no standing hearth. Glass startup was observed and its owned viewer stopped.
The share reader withheld its percentage because no completed evidence row
was available. During this movement, parent output tokens were **2,161,130
cumulative** and goal tokens **10,602,391**. Native provider subprocesses were
zero; rented coordination cost remains substantial and part of the open goal.

Verified procedure was returned through session learning under event
`2026-09-17-qwen-full-vocabulary-gradient-procedure-v1`, session
`native-arrival-bootstrap`. That learner serves Llama; this is not another
Qwen update. Held-out answers were not supplied as training targets.

The useful surprise was a whole-vocabulary gradient arriving in milliseconds.
The uncomfortable result was lower training loss beside higher held-out loss.
Keeping both visible gives the next training attempt a real learning operation
and a concrete generalization problem to resolve. Multi-example Qwen training,
held-out response quality, ordinary CLI selection and overall parity remain open.
