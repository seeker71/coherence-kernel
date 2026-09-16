# Corrective observations use the admitted native batch span

Signed: Codex. Observed 2026-09-17.

## Change carried into serving

`fcmo-prefill` now selects the existing sliced batched prefill when the actual
admitted context has scratch span greater than one. Each outer submission is
bounded by that width and the normal admission limit. Legacy and width-one
contexts retain the barrier route. Both routes validate position, pending-token
range and settled GPU state. The selected route is emitted as metadata.

A failed submission is not retried on mutated state. Existing caller ownership
and last-completed counters remain; physical KV rollback is not claimed. The
old barrier implementation stays as an explicit comparison reference. No C
seed, external runtime dependency, or provider call was added.

## Controlled re-observation

The live comparison uses the registered Qwen Q8, fresh stream states, identical
prefixes and observations, and the actual serving dispatch against the retained
reference. Each pair agrees on admission and observation pending tokens and
positions, plus the next 16 predictions. All GPU states settled; all three
contexts released successfully.

| Scratch span | Prefix IDs | Observation IDs | Reference ms | Serving ms |
| ---: | ---: | ---: | ---: | ---: |
| 64 | 1 | 1 | 353 | 324 |
| 64 | 96 | 65 | 10928 | 1190 |
| 64 | 512 | 129 | 22009 | 2956 |
| 4 | 9 | 17 | 2790 | 1709 |
| 1 | 4 | 5 | 823 | 819 |

The comparison returned **7**, exit 0. These synthetic-ID samples establish
their observed agreement, not full floating-point-state or universal response
quality equality. The span-one pair exercises the retained fallback.

## Actual review

The retained controller request was run through final serving dispatch. Its
request hash and first response bytes match the preceding full-session review.
The route metadata records `model-observation-route-sliced-span-64`. The run
completed in **529039 ms**, generated **698 IDs**, injected **672 IDs**, made
one native repair, passed all three checks and released the model. Provider
calls: **0**. The stronger behavioral checker found zero counterexamples in
its supplied cases; source and document contracts also passed.

The final explanation identifies why the rejected direct-finish proposal masks
checkpoint and offline failure reasons. It correctly names `fcap-done` and
does not repeat the preceding answer's unsupported negligible-cost claim. It
still describes the original observation call as unconditional before later
acknowledging the completed branch. That overstatement remains a response
defect. Passing code checks does not close prose quality or resonance parity.

The native audit pairs every observation-stage begin/end, requires settled
completion, and reconciles position increments with retained injected counts:

| Review | Injected IDs | Stages | Sum of stage ms | Whole session ms |
| --- | ---: | ---: | ---: | ---: |
| Prior barrier run | 790 | 13 | 162255 | 645772 |
| Current sliced run | 672 | 11 | 62589 | 529039 |

The post-repair instruction was shortened after the prior experiment, so these
reviews have different observation content and generated lengths. Their whole
session difference cannot be assigned solely to batching. The controlled
same-input comparisons above supply the narrower performance evidence.

## Checks, learning and cost

Preflight was clean. The observation band passed **16777215** (24 checks);
admission passed **15**. The new cases cover scratch bounds, dispatch argument
preservation, legacy fallback and no second attempt after failure. The live
probe and review both exited 0. Diff checking passed. Drift panel read
**8191/8191**; counsel still reports **11/12 lanes unobserved**, with no standing
hearth. Native authoring guide was read at arrival and closing.

A verified mechanics teaching was retained as
`2026-09-17-observation-serving-span-v1` after Qwen released. Assessment answers
were excluded. This learning path trains the Llama adapter, not the measured
Qwen; retention alone does not establish a new serving capability.

The coordinator goal meter rose from **5744649** to **5934230** at the first
post-review reading: **189581 tokens**, excluding subsequent closing work.
The separate transcript output meter read **1283385**; these meters have
different scopes and are not added. The share reading is declared/unmeasured,
with its percentage withheld. Native zero-provider execution does not remove
the rented development cost. The minimum-expenditure objective remains open.

Private evidence: `.hearth/response-parity/observation-integration-live.log`,
`observation-integration-review/`, `observation-integration-review.log`, and
`observation-integration-comparison.json` (including per-stage transitions and
log hashes). The executable comparison is
`observe/form-cli-observation-prefill-compare.bml`.

The useful surprise was that an existing admission capability could also serve
corrective observations. The difficult part became concrete progress by
checking actual scratch ownership, retaining the reference and adverse answer
findings, and carrying only the observed improvement into serving. Quality,
human resonance judgment and overall throughput parity remain open.
