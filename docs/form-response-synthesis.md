# Native grounding with one provider synthesis

`observe/form-cli-response-synthesis-run.bml` accepts a set of read-only
assessment tasks, checks their sources in Form, asks an explicitly offered
provider for one complete response, and rechecks each returned report. This
is a provider-assisted response path with zero local-model generation. It
advances alongside the native-only response work.

Pass one JSON manifest on stdin:

```sh
form-run ./fkwu observe/form-cli-response-synthesis-run.bml < manifest.json
```

The manifest uses the existing response-session shape:

```json
{
  "session": "my-grounded-review-v1",
  "cases": [{"id": "my-review", "request": {}}],
  "provider": {
    "allowed": true,
    "interface": "codex-exec",
    "max_processes": 1,
    "seconds": 180
  }
}
```

Replace the empty `request` with a valid native review request: `mode=review`,
`evaluation=1`, the complete `goal`, read-only `documents`, source `checks`,
and `report_checks`. Every case needs a unique nonempty ID. The request
contract and examples live in [native coding](form-native-coding.md).

Do the useful native calculation, retrieval or execution before constructing
the manifest. Preserve original sources and label any derived observations
with their scope. The synthesis organ verifies the supplied source assertions;
it does not infer which additional grounding a question needs. Its prompt
contains case IDs, request hashes, goals and documents. Expected report-check
values stay local. Caller-supplied documents can contain drafts to review;
the synthesis prompt identifies that possibility explicitly.

The provider returns one JSON object:

```json
{"results":[{"id":"my-review","report":{}}]}
```

Each `report` contains the complete answer requested by that case. Form
requires exactly one object report per supplied ID, reruns the original
source/report assertions, verifies successful process completion and release,
and reads actual usage from the completed provider transcript. Tool execution
events leave the result needing attention. The instruction asks for synthesis
without tools; this event check observes compliance after execution and is not
a provider-side tool-disabling mechanism.

The native process organ owns the optional CLI process in an empty directory
outside the repository. A single atomic claim keyed by the complete manifest
prevents duplicate admission. Repeating the same manifest checks the retained
answer and usage hashes, returns zero new processes and preserves the same
`usage_event`. Count that event once. An interrupted claim stays unresolved;
inspect its owned evidence before further action. The deadline limits elapsed
process time, not tokens. Missing provider usage stays unknown.

The output contains metadata and a private `answer_path`. It leaves semantic
quality and frequency/resonance unscored. Source/report assertions establish
their declared checks; inspect the actual answer and add relevant behavioral
checks to assess its usefulness. This assessment path sends no answers to
learning. Provider availability is optional for Form itself and required only
for this explicitly selected path.

The [native-first repair resource](form-response-resource.md) continues to
require a failed retained report. Synthesis has its own explicit entry point;
it does not fabricate a failure to obtain assistance.

## Observed comparison

On one retained three-task comparison, native arithmetic/source preparation,
one provider synthesis and final checks took 92,060 ms, with 26,946 reported
input-plus-output tokens. The context-equipped baseline took 258,074 ms and
459,735 reported tokens. Their uncached input counts were 14,683 and 21,113.
This is a sample observation, not proof of a global token minimum or native
voice parity. The [receipt](../receipts/2026-09-17-bounded-provider-synthesis.md)
retains the accounting, answer review and open quality boundaries.

A fresh three-task comparison retained the same original documents and checks
for both routes. Its first synthesis took 29,609 ms and 24,082 input-plus-output
tokens; the baseline took 53,482 ms and 76,135 tokens. Both answers blurred
generation completion with JSON validity. One further synthesis received
executed native counterexamples. Including that refinement, the synthesis path
took 54,203 ms and 48,520 tokens, with 25,370 uncached input tokens versus the
baseline's 15,613. Its initial speed advantage therefore did not survive the
correction. These execution windows exclude coordinating work, whose separate
cost is retained in the [fresh comparison receipt](../receipts/2026-09-19-fresh-matched-native-synthesis.md).
Actual answer review still found a causal error; passing the original checks
does not establish semantic parity or human resonance.

## Preserve source coverage while shrinking the packet

The retained comparison has native preparation and evaluation doors:

```sh
form-run ./fkwu observe/form-cli-comparison-review-prepare.bml
form-run ./fkwu observe/form-cli-comparison-review-run.bml
```

Preparation reads the committed comparison artifacts, keeps the original
question, candidate and measurements, restores the antecedent of the identity
seam, and supplies selected historical grounding that the candidate had seen.
It separates the review instructions from the JSON document instead of nesting
an encoded packet inside another encoded packet. Preparation calls no model.
The second door explicitly uses the offered provider for this retained
evaluation; its manifest claim prevents a second admission on replay.

The source checks establish preserved content and schema. They do not establish
that every relevant source was selected. Historical grounding describes what
the candidate saw; it does not independently verify that grounding's claims.
Keep an unsupported claim distinct from a contradiction, and retain the scope
of partial excerpts. Expected report assertions stay outside the provider
prompt.

The [source restoration receipt](../receipts/2026-09-20-comparison-source-coverage.md)
records both the improved answer and its cost: 21,069 provider tokens, 716 more
than the prior review. Rewrapping saved 1,106 prompt bytes before adding the
missing context. Those byte counts are not token measurements. Both retained
answers replayed with zero new provider processes; count each usage event once.

The matched local door is
`form-run ./fkwu observe/form-cli-comparison-native-review-run.bml`.
It reads the retained restored source directly, uses the same review instruction,
and runs registry `qwen38-q8` in the current `fkwu` process with the
`knowledge-query` profile, 12,288 positions and 2,048 generated-token allowance.
It retains raw output even when completion or JSON parsing fails. Its separate
claim directory prevents a duplicate generation; a repeated completed request
returns retained metadata with `native_model_calls_new=0` and the same
`native_usage_event`. This is an evaluation with no provider call or
training. Carrier envelopes differ from the provider run; the source and task
are compared separately from answer quality.

`observe/form-cli-comparison-fresh-run.bml` tests fresh local composition. It
selects the original question and current evidence fields, omitting the old
candidate and the grounding supplied only to review that candidate. The source
band checks those omissions and preserves the selected values byte-equivalently
through JSON encoding. Its retained packet must match before replay. This is a
different task from review, so an observed improvement does not isolate which
change caused it. The [fresh-answer receipt](../receipts/2026-09-20-fresh-native-comparison.md)
records a smaller, faster answer that still mixes token scopes and overstates
which workflow is necessary. It establishes no quality parity.
