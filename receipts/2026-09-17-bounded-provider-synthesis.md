# Native grounding, one provider synthesis, native re-observation

Codex, 2026-09-17.

The three-task hybrid completed with substantially fewer measured provider
tokens and less elapsed time than the retained context-equipped baseline.
Actual answer review found the important distinctions that several local-only
trials had omitted. This advances the provider-assisted path; native-only
response parity, human resonance judgment and a global token minimum remain
open.

## Original enquiry and grounding

The retained manifest contains a promotion decision, controller review and
grounded dialogue. The hybrid preserved every original source document.
Native Form recomputed loss arithmetic for the promotion task and appended
actual callee source plus controlled original-controller execution traces for
the review. The dialogue documents remained unchanged. Source assertions
passed before admission; the final audit recomputed the grounding and checked
that it matched exactly.

One provider process, initiated inside Form through the native process organ,
received goals and source documents. It received no previous model answers or
hidden expected report assertions. No local model generated in this arm.
The provider event inventory contained only `agent_message` items. Both arms
used the same observed CLI flags and no model override; exact backend model
identity is not established by that fact.

Private retained evidence lives under `.hearth/response-parity/`:

- `matched-session-manifest.json` and `matched-session-baseline-result.json`;
- `bounded-hybrid-session.bml`, `bounded-hybrid-grounding.bml`,
  `bounded-hybrid-check.bml` and `bounded-hybrid-session/`;
- `bounded-hybrid-dialogue.md`, containing the provider's actual dialogue.

The provider process is `.form-heal/process-31021-545983556-0`.
Answer SHA-256:
`bd8799886bf6626bb43b560823dbad665ce9349a2488f1143f4e392821dc29f5`.

## Measurement

| Observation | Retained baseline | Bounded hybrid |
| --- | ---: | ---: |
| Elapsed ms | 258,074 | 92,060 |
| Input tokens | 454,009 | 25,307 |
| Cached input tokens | 432,896 | 10,624 |
| Uncached input tokens | 21,113 | 14,683 |
| Output tokens | 5,726 | 1,639 |
| Reasoning tokens, already included in output | 1,124 | 242 |
| Input plus output | 459,735 | 26,946 |
| Provider processes | 1 | 1 |

Hybrid timing comprises 124 ms grounding, 91,745 ms provider execution,
32 ms other precheck/process overhead and 159 ms final checking. It does not
include developing this capability or human assessment. The provider token
reduction is 94.14% on the reported input-plus-output basis; uncached input
fell by a much smaller amount. These are token counts, not a billed-cost ratio.
The native presentation helper initially used integer division and printed an
invalid 100% reduction; float arithmetic corrected it before publication.

Coordinator cost stays visible separately. The goal meter began this movement
at 6,318,247 and an intermediate reading reached 6,440,137: 121,890 additional
tokens before final review and landing. The transcript output-only meter read
1,375,447. Those meters have different scopes and are not added together.
This development spend is substantial and is not erased by the cheaper
provider arm. No whole-project efficiency claim follows from this sample.

## Answer review

All three original source/report contracts passed, as did all five stronger
controller behavior cases. The latter include completion, exhausted allowance,
checkpoint refusal at that allowance, offline state at that allowance and
ordinary continuation. The provider process finished and released.

My review of the actual answers found:

- The promotion answer applies the supplied policy immediately, retains the
  row values and unknown token usage, and keeps held-out targets outside
  training. Its next action records the decision and continues assessment.
- The controller answer supplies the correct delegation before extra feedback,
  names the completed branch and the checkpoint/offline cases, and confines
  the timing observation to its actual scope. Its concluding suggestion to
  apply the reviewed replacement is contextual: production already contains
  the repair; this frozen review task did not create a new patch to install.
- The dialogue covers trust within intent, usage-sensitive vocabulary,
  composition identity versus seed IDs/short anchors, the actual codebook
  anchor, and the absent German locale mapping. Its collaborator-silence
  example turns the absence/rejection distinction into a usable next action.
  The prose is complete and specific; human warmth/resonance remains an open
  comparison rather than an inferred score.

These observations establish this sample's quality evidence. The machine
receipt continues to leave semantic quality and resonance null. The hybrid
also benefited from native derived evidence supplied up front; this comparison
measures that complete workflow change, not an isolated model capability.

## Embodied capability and a live failure repaired

The reusable [synthesis door](../docs/form-response-synthesis.md) accepts any
valid set of read-only evaluation requests with an explicit one-process
provider allowance. It preserves IDs, keeps expected assertions out of the
prompt, rechecks sources and reports, retains actual usage and release, and
uses an atomic claim to reuse a completed answer without another process.
It shares the existing provider process implementation. The native-first
repair resource still requires a failed local report.

A fresh live smoke request used 16,070 input tokens, 10,624 cached input tokens
and 55 output tokens; provider duration was 8,092 ms. The answer was correct,
but `form-run ./fkwu .hearth/response-parity/synthesis-door-smoke.bml` exited 1
with `live synthesis incomplete`. Its retained report diagnostic was
`unsupported-jq-expression` for `has("translation")`.

The cause was the native JSON-query subset, not the answer. Object-key `has`
now runs in Form, preserving the distinction between absent keys and present
null, false or zero. The unchanged manifest and provider answer were rechecked
successfully; a second replay also passed with zero new provider processes
and the same usage event. No assertion was removed or relaxed. Initial failure
and subsequent checks remain in the organ's separate observation records.
Array-index `has` remains explicitly outside this subset.

The live synthesis claim is
`.hearth/response-sessions/syntheses/c043912fb9cefb3402f2a1d0834e0893521a73ba77413881f164ee9b1502f8db`.
This smoke run validates process/replay integration; its tiny answer does not
extend the three-task semantic-quality observation.

## Checks and instruments

- Synthesis admission/answer counterexamples: 21 assertions, exit 0, verdict 1.
- Native JSON object presence: 11 observations, exit 0, verdict 1.
- Existing response-resource and response-session bands: exit 0, verdict 1.
- Existing native agent-tools band: exit 0, verdict 65535.
- Fresh preflights closed without errors or unresolved calls.
- Drift gates: 8191; binary freshness: 31. No kernel source changed.
- Native guide: 0 Python implementations, 2 existing voice invocation
  candidates, 0 unread files.
- Counsel panel: 0 orphans; 11/12 judged lanes unobserved with no standing
  hearth. This is not a healthy-throughput verdict.
- Reply share remains declared/unmeasured; percentage withheld.

A concise verified process/tool teaching was retained under event
`2026-09-17-bounded-provider-synthesis-v1`; assessment answers were excluded.
Retention itself makes no claim about adapter improvement or promotion.

The most useful surprise was how much repetition native preparation removed
from the provider session while keeping the answer complete. The uncomfortable
finding is the coordinator's development spend. The live failed check supplied
a concrete gain: the body can now observe object-key presence itself and reuse
the answer it already earned. The next work is broader held-out validation and
better local-only expression, with the same accounting kept visible.
