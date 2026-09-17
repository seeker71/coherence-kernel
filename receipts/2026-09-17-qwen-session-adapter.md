# Qwen learning reaches the session that generates

Signed: Codex. Native Form/BML, with no C seed growth, external language
runtime, model server or provider subprocess added.

## Observed gap and repair

The earlier Qwen token-loss work produced and admitted a learned rank-one
head adapter. Ordinary Form model sessions still called the base forward
function. The separate automatic session learner trains Llama 3B; its retained
teachings do not update the Qwen model used in our response comparisons.

`bml/form-cli-model-adapter.bml` now carries an explicitly admitted Qwen
adapter in its owning model session. The public embedding doors are
`fcms-attach-adapter` and `fcms-open-adapted`. Generation, coding, recipe,
NodeID and BML-prefix continuations use the session's selected forward path.
Tool observations and independent-stream renewal refresh the adapted final
head after prefill without replaying an input token. Refusals retain ownership
so the final release includes adapter buffers. Renewal transfers that owner
while retiring only the old stream.

Admission validates the current 5,120-wide boundary and finite float32 A/B
tensor bytes. The carrier hashes exactly the two byte strings admitted to the
device. This is tensor identity, not artifact metadata or training-base
provenance. Callers remain responsible for choosing the correct base model.
No candidate was promoted. JSON `code` and the resident turnwheel do not yet
select this adapter; callers must explicitly use the Form API.

## Re-observation on the local device

Private runner `qwen-adapter-session-probe.bml` used the previously trained
`qwen-token-adapter-step-v1/candidate.safetensors` with its original Qwen3.8
27B Q8 base and five-token arithmetic prefix. The candidate's admitted A/B
SHA-256 was
`598932f82b794d8d77c367b1cc544dbf1d31b1d8b12ed46aa77d94acbce21dc2`.
The runner exercised attachment to a freshly prefilled ordinary session;
it did not exercise the ChatML `fcms-open-adapted` convenience wrapper.

| Stage | Stream position | Relative error against adapted reference | Selected effect present |
| --- | ---: | ---: | --- |
| Attached | 5 | 0 | yes |
| Coding continuation | 6 | 0 | yes |
| Ordinary decode | 7 | 0 | yes |
| Tool observation | 28 | 0 | yes |
| Independent stream | 5 | 0 | yes |

The reference check compared actual selected logits and the pending token
against a fresh adapter-aware head. Each stage then also observed the base
head and restored the adapted head. Renewal reproduced the original selected
logits exactly, retired all 128 expected old-stream handles and admitted no
new weights. A context-capacity refusal preserved the carrier. Final release
succeeded for all **1,016 expected handles**, including three adapter buffers.

The run completed in **32,913 ms**, exit 0, provider processes 0. Selected
training loss was **0.660861 before, 0.659476 after**. This repeats the effect
of one two-token training objective through the ordinary session path. It
does not establish full-vocabulary loss, held-out learning, answer quality,
resonance, or throughput parity. No new training was performed in this probe.
Stage receipts and the original candidate remain private under
`.hearth/response-parity/qwen-adapter-session-v1` and its sibling probe/log.

## Checks and retained failure

Fresh preflight and execution passed for model-session **4095**, code-session
**31**, recipe-exec-session **4095**, NodeID-knowledge-session **33554431**,
and BML-prefix-session **8388607**, all exit 0. Added checks exercise ownership
through updates/refusals/renewal, duplicate and late admission, partial handle
accounting, finite tensors, malformed byte types and ordered tensor identity.

The first model-session band failed because the new test fixture passed
`hex-decode`'s byte list to a string operation: `str_len: only a string has a
length -- ask value_kind first`. The fixture now constructs native byte
strings, and admission explicitly rejects wrong types. The corrected checks
passed before the device witness. This was a fixture defect, not an inference
failure or evidence against the adapter.

Drift gates pass **8191**, exit 0. Native guide: Python implementations 0,
execution candidates 2, unread files 0. Counsel panel: **orphans 0; 11/12
lanes unobserved**, with no standing hearth. Glass startup was observed and
the owned viewer was interrupted after reading; that interruption is not a
test failure. The share reader withheld a percentage while validating the
latest append range. Parent output-token meter read **2,134,776 cumulative**;
the goal meter read **10,361,118 total tokens** during this movement. Local
provider count 0 does not erase the rented coordination cost.

Verified ownership procedure was offered to the separate session-learning
door as event `2026-09-17-qwen-session-adapter-procedure-v1`, session
`native-arrival-bootstrap`. Evaluation answers were not supplied as targets.
This retention is not a Qwen training or promotion claim.

The useful surprise was that a valid learned change could be absent from
ordinary generation. Following that uncomfortable mismatch produced a
verified route from candidate admission to later session use. The next open
boundary is whether an appropriate Qwen candidate improves held-out answers
when this route is used, with its admission and throughput costs included.
