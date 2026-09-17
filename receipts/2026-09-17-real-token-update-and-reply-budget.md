# Real token update and visible reply budget

Signed: Codex. Observed 2026-09-17 in `codex/native-arrival-bootstrap`.

## A token target reaches the native Qwen head

`bml/qwen-lora-token-observation.bml` now captures an actual normalized hidden
state, selected immutable projection rows and native logits. It uses the
existing Qwen row decoder and token-order barriers. The caller supplies token
IDs; the helper adds no external runtime and changes no kernel source.

The local probe tokenized `1 + 1 =` into five tokens. Its target was computed
by Form's `add(1,1)`, rendered and tokenized. It selected the vocabulary rows
for `1` and `2`, captured width 5120 from the 248320-row model, and compared
the native logits with the existing reference projection. One gradient step
reduced selected-row reference loss from 0.660862 to 0.659477. Capture took
26832 ms and released all 1014 owned buffers. Printed `relative_error=0` was
rounded by the numeric renderer; it is not an exact-equality claim.

The follow-through persisted A/B into a private safetensors candidate, read
the stored float32 values back, admitted those bytes into Qwen and observed
its adapted head on the same prefix:

| Observation | Result |
| --- | --- |
| Native selected-row loss before / after | 0.660861 / 0.659476 |
| Reference / native adapted logits | both display `[8.157884,8.226398]` |
| Relative discrepancy | 0.078115 ppm |
| Adapted-head run | 27204 ms |
| Expected / actual releases | 1016 / 1016, including three adapter buffers |
| Provider calls / serving promotions | 0 / 0 |

This is one correct arithmetic training example and two output rows. It proves
the state-to-gradient-to-artifact-to-native-head path at that scope. It does
not establish full-vocabulary improvement, held-out behavior or conversational
quality. The ordinary corpus fitter and serving adapter remain unchanged.

Private evidence lives in `.hearth/response-parity/qwen-token-observation-v2/`
and `.hearth/response-parity/qwen-token-adapter-step-v1/`; the latter holds the
candidate. Native runners are `qwen-token-observation-probe.bml` and
`qwen-token-adapter-step.bml` in `.hearth/response-parity/`.

### Failed attempts retained

The first capture reached 5120 hidden values but no logits or projection rows;
all 1014 buffers still released. The code requested a concurrent-only barrier
after a synchronized read had closed that batch. Fresh projection/row batches
now use the serial entry contract. The second capture passed.

The first two adapter-step attempts exited 139, before model admission, while
reading the retained sample through `ln-floats`. Stage diagnostics stopped
before `hidden-read`. The caller now uses the existing finite float32 decoder
`qlt-f32s` on the same bounded bytes; hidden/projection reads, persistence and
native admission then passed. This repairs the new caller. The underlying
large `ln-floats` failure remains an open observation, with both failed logs
retained. No C change or interpreter substitution was used.

Preflight also caught an extra parenthesis, missing BML expression terminators
and an invented `value_to_str` name. Those were repaired before execution.
Exploratory searches naming absent paths retained their failed exits.

## The full review still owes a useful answer

`grounded-semantic-review-v2` used the prior retained answer, native line
selection, original source/report checks and explicit feedback about defensive
self-description, conflated translation, unused composition evidence and
already-authorized work. Its three replies read the draft, composition witness
and enrichment. It returned no report: 510948 ms, 49 generated IDs, 1458
injected IDs, three reads, zero report checks, release passed. This is a failed
quality attempt. Reducing a budget alone did not produce useful throughput.

The controller enforced that allowance without presenting its remaining count
to the model. Native admission and observations now carry the limit, completed
count and remaining replies. The final reply is identified before generation.
Existing role/source observations, checks and termination limit remain intact.
Reasoning admission and same-resident follow-ups use the same disclosure.

`grounded-semantic-review-v3` is running the same request with that disclosure.
Its process and private evidence are retained; answer quality and cost are
pending. The budget change currently has boundary evidence, not a demonstrated
quality benefit. It does not extend the allowance or waive checks.

## Checks and current floor

- Freshness 31; Qwen training band 255; code-session band 31;
  code-request band 255; review-execution band 33554431;
  review-followup band 1. Relevant preflights and executions exited 0.
- Drift gates 8191. Native authoring guide has zero Python implementations,
  two existing invocation candidates and zero unread files.
- Counsel: orphans 0; 11/12 lanes unobserved without a standing hearth.
- The preceding procedural learner completed candidate 54; serving generation
  5 stayed unchanged. New procedural teaching waits for the live review to
  release, preserving separate GPU ownership.
- Parent goal counter checkpoint: 9,488,058 tokens. Native transcript meter:
  1,935,580 cumulative output tokens. These scopes differ; zero provider calls
  inside the local probes does not imply zero rented development cost.

The useful surprise was that the existing row decoder already supplied the
missing real projection data. The difficult observation was the review's
complete failure to deliver within three replies. That produced an explicit
budget channel, whose effect still needs observation. The broader quality,
resonance and minimal-rental objective remains active.
