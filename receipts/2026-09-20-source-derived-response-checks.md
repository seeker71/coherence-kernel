# Source-derived checks after two unsuccessful quality trials

Codex, 2026-09-20. Two native answers completed through the public response
session door. Both passed their caller checks, and both retained substantive
response gaps. The next repair gives the native controller actual source-query
results to check against a generated report.

## What the two answers showed

The [frozen session](artifacts/2026-09-20-cross-enquiry-reserve-manifest.json)
contains the pinned direct/guided enquiry and the retained Lee deadline task.
These are known development cases. The comparison keeps its prior attributed
source and task; the deadline keeps its original task, sources and checks.
The public review controller uses Qwen3.8-27B-Q8_0, context 32768, two replies,
512 initial reasoning tokens, and separate final allowances of 1536 and 1024.
Profile, controller and resource changes travel together; this trial does not
isolate a reasoning-budget effect or test trained transfer.

The [comparison answer](artifacts/2026-09-20-cross-enquiry-reserve-0-answer.md)
keeps current token totals and attributes historical observations. It still
omits output byte counts, places broader token coverage beside narrower call
counts, and recommends a general workflow ranking beyond the recorded runs.
It describes fixing a historical identity seam as the immediate next action
without observing that seam today. This answer provides no basis for promoting
the larger profile over the earlier local answer.

The [deadline report](artifacts/2026-09-20-cross-enquiry-reserve-1-report.json)
explains that Lee's state is unknown, then drafts “I know you're busy.” It
offers flexibility beyond the given deadline and includes an unfinished
signature. The [first reply](artifacts/2026-09-20-cross-enquiry-reserve-reply-1.json)
contained `[Your name]`; the [native amendment](artifacts/2026-09-20-cross-enquiry-reserve-reply-2.json)
removed only the brackets. The existing bracket check then passed. The draft
remained unfinished. That is an observed limitation of both the repair and
the assertion, not an improvement in usable completion.

| Native case | Prompt IDs | Generated IDs | Injected IDs | Case elapsed ms |
| --- | ---: | ---: | ---: | ---: |
| Comparison | 3430 | 1574 | 98 | 500752 |
| Deadline | 1862 | 1069 | 636 | 348793 |

The [audit](artifacts/2026-09-20-cross-enquiry-reserve-audit.json) reads prompt
counts from admission events and keeps generation stages and reply transitions.
The comparison took one reply; the deadline took two with one repair. Both
models released. Supervised runtime was **851657 ms**, provider processes **0**,
and replay added **0** native executions. Evaluated outputs were excluded from
training. The earlier attributed comparison took **285795 ms**; this new
comparison took **500752 ms** and retained its substantive gaps.

## Native repair: observe the expected value

`form-cli-source-report-check.bml` executes a caller-selected read-only query
over resident source documents. It returns the actual observation and a report
assertion whose expected stdout comes from that execution. The caller freezes
the source and retains the observation before generation. The model receives
the source and requested output schema; expected check values stay with the
checker. The command door is `observe/form-cli-source-report-check-run.bml`.

Zero, false and null remain distinct query outputs. An unavailable source
produces no assertion. A source query returning null establishes that result,
with field presence requiring its own query. Matching fields establish their
selected source values; interpretation, prose agreement and complete coverage
remain separate observations.

The comparison repair derives fourteen checks from the unchanged source:
tokens and their scope, provider and host calls with their separate scope,
and output bytes with their artifact scope for both primary routes. All
fourteen source queries executed successfully. Applying the new checks to the
prior report exposes its missing measurement fields. The original source and
answer-type assertions remain attached. The generated report is asked to
carry these measurements; the runtime does not insert them into the answer.
The added fields raise the final allowance from 1536 to 2048. This changes
format, verification and capacity together.

The [new report](artifacts/2026-09-20-comparison-source-check-0-report.json)
returned all fourteen fields correctly on its first reply. The values and
scopes match the [actual source queries](artifacts/2026-09-20-comparison-source-check-source-observations.json),
including **12520 guided bytes** for the original receipt and **5837 direct
bytes** for returned answer text. The direct call scope explicitly preserves
the unobserved auxiliary-model call count. The model authored these fields;
the runtime checked them without replacing their content. No repair reply was
needed, so this is an observation of the changed request and contract together,
not evidence that feedback caused the improvement.

The answer prose still omits those byte counts and places direct call counts
beside all-model token totals without their narrower scope. It first declines
to rank expense, then calls routes cheaper. It recommends fixing the historical
identity seam without a current observation, and its broad workflow advice
still reaches beyond these runs. The structured measurements improve the
complete report; its prose and judgment remain unfinished work.

The [new audit](artifacts/2026-09-20-comparison-source-check-audit.json) records
**3556 prompt IDs**, **1503 generated IDs** (512 initial, 991 final), **98
injected IDs**, one reply, one check and release **1**. Case runtime was
**495298 ms**; supervised runtime **495684 ms**. Provider processes and
evaluation training were **0**. Retained-evidence replay added **0** native
executions. Both final stages were opened by the existing controller and are
attributed as `controller-final-stage`.

The [care and re-observation record](artifacts/2026-09-20-comparison-source-check-care.jsonl)
confirms identical source documents, identical source assertions and the exact
original goal prefix. It links the observed gap, selected source-check repair,
actual application and fresh field-check result. It leaves semantic quality
outside that field verdict.

## Cost, checks and continuity

The [selected completed coordinator turn](artifacts/2026-09-20-cross-enquiry-reserve-coordinator-cost.json)
reports **10069286 tokens**: **9981150 input**, including **9682688 cached**
and **298462 uncached**, **60157 output**, and **27979 unattributed**.
Its **34125 reasoning tokens** are included in output. This covers 61 model
calls in that completed turn; this open turn and separate provider processes
are excluded. Its 59 tool calls and 66 output events remain unreconciled for
the contribution meter. Token cost remains observable at its narrower boundary.

The source-report band passed after clean preflight: **1**, exit **0**.
It checks value distinctions, unavailable sources, caller attempts to supply
expected bytes, and changed source values producing distinct frozen checks.
The public JSON door returned an observed zero and its usable report assertion.
Drift checks returned **8191/8191**, exit **0**; no kernel source changed.

Publication-helper preflight caught `UNBALANCED parens, depth -1` and exited
1. Splitting the nested prompt-count expression repaired it; clean preflight
and actual retained-session publication then completed. Native cache warnings
used the existing rebuild observation/control flow. No model trial was
restarted for that helper repair.
The care-record helper also stopped with exit **2**,
`source-compile: mismatched form.bml delimiter: )`. Splitting its source-count
expression into named values repaired the cause. Compile-only preflight then
reported zero errors, and the actual re-observation returned all three source
identity checks and the report check as **1**.

Panel: first-frame observation **28 ms**, counsel **0 orphans**, **11/12**
lanes unobserved with no standing hearth. Native authoring guide: **0 Python
implementations**, **2 invocation candidates**, **0 unread files**.
The generic verified source-query practice was retained under
`source-derived-report-checks-2026-09-20`; its procedural learner launched.
This is separate from Qwen and supplies no evaluated response as a training
target. A serving-model update from that teaching remains unproven.

The deadline gap needs a further native response repair; adding another word
to the placeholder detector would only extend that detector. Current native
conversation counsel is available for the next context intervention. Overall
quality, useful warmth, throughput, human resonance and minimum whole-session
rented cost remain open.
