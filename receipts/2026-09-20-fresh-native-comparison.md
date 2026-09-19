# Fresh composition removes an inherited error; scope still needs work

Codex, 2026-09-20. Continued from `cbf251928`. The previous turn was progress:
it re-observed the repaired source on Qwen and retained both improvements and
remaining errors. This movement tested a fresh answer instead of another
revision of the flawed candidate. The overall goal remains open.

## Native preparation and actual result

`fcr-fresh-domain` keeps the original enquiry, route usage, output artifacts,
original author's observations, native concepts and source coverage. It omits
the old candidate and the historical grounding supplied to review that
candidate. The source band verifies the selected values and JSON roundtrip.
The [source](artifacts/2026-09-20-fresh-native-comparison-source.json) and
[packet](artifacts/2026-09-20-fresh-native-comparison-packet.json) are retained.

`observe/form-cli-comparison-fresh-run.bml` used the same Qwen3.8-27B-Q8_0 base,
`knowledge-query` profile, 12,288 positions and 2,048-token allowance. It changed
the task to fresh composition, the instruction and the context; this is not an
isolated test of draft removal. Expected answers and the provider's response
were absent. No evaluation content entered training.

The [raw result](artifacts/2026-09-20-fresh-native-comparison-raw.txt) and
[answer](artifacts/2026-09-20-fresh-native-comparison-answer.md) show the movement.
Qwen dropped the obsolete two-citation count, cited the original answer path,
preserved the original direct/guided definitions, and used the broader direct
and bounded token totals in its volume paragraph. It treats felt resonance
as a human reading rather than assigning it a high score.

Its remaining problems are specific. The rented-token paragraph switches back
to primary-model totals without labeling that scope change. It does not carry
the guided through-call-21 boundary into the numeric comparison. Its volume
paragraph omits the supplied output-byte measurements. Some authored judgments
become the answer's own assertions without clear attribution. Its conclusion
calls bounded optimal and guided necessary for measurement and traceability;
the observed runs do not establish those general claims. A direct provider or
native workflow can also perform measurement; the observed direct run did not.

This is a partial content improvement, not parity or a serving promotion.
Removing the draft did not resolve overgeneralization. The mixed totals also
give a concrete next source-preparation target: preserve one explicitly scoped
comparison quantity per route while retaining original readings for audit.
Human resonance remains unmeasured.

## Measured costs

| Local observation | Restored-source review | Fresh composition |
| --- | ---: | ---: |
| Prompt IDs | 4,813 | 2,947 |
| Generated IDs | 1,377 | 928 |
| Execution ms | 515,784 | 311,755 |
| Completion / release | 1 / 1 | 1 / 1 |
| Provider / model-server calls | 0 / 0 | 0 / 0 |

The [result](artifacts/2026-09-20-fresh-native-comparison-result.json) retains
those execution-window measurements and valid JSON. Preparation, compilation,
retention, procedural learning and coordinator work are separate. The fresh
door checks the retained packet before returning a completed result with zero
new native calls; it does not regenerate during inspection.

The [native audit](artifacts/2026-09-20-fresh-native-comparison-audit.json)
includes all six retained native comparison attempts, including failures.
The [previous completed coordinator turn](artifacts/2026-09-20-fresh-native-comparison-coordinator-cost.json)
used **28 calls**, **4,500,748 input tokens** (**4,424,320 cached**, **76,428
uncached**), **28,934 output** including **20,063 reasoning**, and **0
unattributed**: **4,529,682 total**. It excludes this open turn and separate
provider processes. Native zero-rent execution leaves that coordination cost
visible; it does not erase it.

## Checks and continuity

Fresh source preflight was clean and the source band returned **1**, exit 0.
The native audit checks the actual retained source and packet against the
preparation functions. Landing checks returned **8191**, exit 0, and replay
returned `native_model_calls_new=0` after verifying its packet. Runtime and
schema success are kept separate from
answer quality. No C seed growth or external runtime dependency was added.

Panel: **0 orphans**, **11 of 12 lanes unobserved**, no standing hearth.
Bounded glass first frame: **28 ms**, exit 0. Native authoring guide: **0
Python implementations, 2 execution candidates, 0 unread files**. The prior
procedural learner finished round **90**, pending **0**, serving generation
**5** unchanged. It serves the separate Llama adapter, not Qwen.

This movement retained the verified source-selection procedure under event
`fresh-source-selection-2026-09-20`. Its worker was launched; retention alone
does not establish a learner update or a change in served quality.
