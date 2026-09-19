# A native review revises after executing its proposed evidence

Qwen3.8-27B-Q8_0 completed the retained source review in **two replies with
one repair**. The final report contains one supported finding, a concrete
two-event sequence whose predicted status matches actual reader execution,
and the supplied check results. The
[formatted native answer](artifacts/2026-09-20-native-executable-review-answer.md)
copies the model's selected title and explanation; the
[first](artifacts/2026-09-20-native-executable-review-first-reply.json) and
[revised](artifacts/2026-09-20-native-executable-review-revised-reply.json)
generated replies are retained exactly.

This is development on a known review failure. It is useful progress, not a
held-out task, general response parity or a human resonance judgment.

## The behavior now available

`form-cli-provider-usage-replay.bml` executes caller-supplied notification
objects through the linked `fpn-event`. Its command door is
`observe/form-cli-provider-usage-replay-run.bml`. Each result preserves the
per-event before/after status, actual final reading, and whether a supplied
status claim matched. An absent claim remains null. A matching status leaves
`defect_verified` null: contract interpretation and surrounding prose have
their own evidence.

Ordinary `form-cli code` review requests can opt into the same checker with
`report_checks: [{"kind":"provider-usage-sequences"}]`. A finding supplies
its sequence and claimed result. A mismatch or missing sequence enters the
existing review repair path; other source and report checks remain in force.
An empty findings array performs zero sequence checks and can still miss a
defect. The callback is also available to native callers.

The linked implementation matters. This experiment ran inside the retained
`a3a72b2a841fc3726c966255f38fc674b83c85fa` checkout, so it executed the original
reader being reviewed. Before model admission, native code compared that
reader's exact bytes with the source section in the supplied packet and
retained its digest. Git confirmed the baseline reader was unchanged.
The replay module used by the experiment matches the published module byte
for byte.

## What the local model actually changed

The prior 9,809-byte relation-enriched evidence packet stayed unchanged.
The task added structured finding/sequence fields and native replay on
submission. This changed the response format and verification procedure;
it was not another identical-prompt trial. The original question, source,
contract, probe observations and check results stayed available. No provider
answer or expected finding list was supplied.

The first reply already described the correct isolated cache-write decrease
and predicted `observed-prefix`. It also included a candidate that its own
explanation withdrew, and a “no further defect” entry. Those two entries
lacked sequences. The native callback returned the matching first sequence
and the two invalid proposals. The next reply removed those entries and
preserved the supported finding and correct event sequence.
Re-running the same caller checker on the retained first reply reproduced
the failed check; its [full observation](artifacts/2026-09-20-native-executable-review-first-report-check.json)
is retained beside the successful final check.

Thus the live repair removed entries that could not substantiate their
claimed role as findings. It did **not** correct the earlier all-fields-
decrease error: that error was already absent from the first structured
reply. The original adverse answer remains in the preceding receipt.
The [selected report check](artifacts/2026-09-20-native-executable-review-report-check.json)
preserves the actual two-event replay. The original supplied check results
remain preflight exit 0, band exit 0, band output 1.

The model used **3,841 prompt IDs**, **1,753 generated IDs** (512 initial,
799 first final-stage, 442 revision) and **1,195 injected IDs**, including
the runtime transition and repair observation. It made zero model-requested
tool calls; the native controller ran two report checks. Supervised runtime
was **672,933 ms**, with process and model release confirmed. The first final
stage was opened by the controller; it was not a forged model reasoning close.
The [observation](artifacts/2026-09-20-native-executable-review-observation.json)
retains the stage data, source identities and lifecycle.

## Earlier attempts remain part of the movement

Three admissions preceded the completed review:

1. `refused:process-contract`: the job directory belonged to the outer root
   while its snapshot selected the baseline root. No process started. The
   original result's `lifecycle: 0` is a missing-value artifact, not a measured
   lifecycle; its printed zero duration establishes no elapsed-time fact.
2. Exit 1, `response evidence write failed, is absent, or differs after
   write`: the baseline lacked `.form-heal`, so job allocation returned an
   empty path. This stopped before the supervisor call. The launcher now
   creates the parent, checks it and checks the allocated path before writes.
3. Process exit 0, response state `attention`: the baseline lacked its
   optional Metal carrier. Generation reported the unavailable carrier,
   produced zero tokens and released. This supervised process took **1,798
   ms**. The carrier was then built from the committed Objective-C source
   with `cc` and the system Metal/Foundation frameworks.

No C seed change, language runtime, model server or new provider call was
introduced. The two supervised processes total **674,731 ms**. Including the
preceding six source-review process attempts gives **3,775,806 ms**. Wall
time for the two pre-process admissions and coordinator work remains outside
that process-only sum. They are retained as attempts, not assigned zero cost.

Preflight also caught a worker import absent from the baseline; it was
replaced with that checkout's existing response helper. The stdin command
door now handles empty input explicitly and uses compile-only preflight.
All corrected paths passed their checks before dependent work continued.

## Token cost has its own verified boundary

The previous completed coordinating turn,
`01a0bb9b-705c-70c2-952b-0a640b5058e0`, has **43 provider model calls** and
**4,300,706 reported tokens**: 4,245,006 input, 27,409 output and 28,291
unattributed. Cached input is 4,146,432; uncached input is 98,574. Both are
included in input, and 12,793 reasoning tokens are included in output.
The [cost record](artifacts/2026-09-20-native-review-coordinator-cost.json)
excludes this open turn and separate provider subprocesses. It is token
volume, not monetary cost or a whole-session total.

The prior cost reader withheld these quantities because the joint evidence
row had 41 tool calls and 51 output events. A native
[identity census](artifacts/2026-09-20-native-review-tool-output-census.json)
found 41 distinct call identities, each with one call and at least one output;
two `exec` identities had seven and five outputs. No unmatched or repeated
call identity was found. The census does not infer terminal output semantics.

The cost door now reuses the existing token collector and independently
recontacts carrier identity, source coordinates, timestamps and completion.
Its v2 schema reports token reconciliation separately from tool-event
reconciliation. Inconsistent token quantities, missing completion, invalid
source or invalid coordinates still withhold cost. The contribution-share
validator is unchanged: its share remains withheld, with `kind=declared`
and `tools=0` in the closing reading. This neither discards extra outputs
nor declares the full event meter repaired.

The cost helper initially lacked the cursor prelude. Its test also initially
changed the patch-count field instead of the source field. Both failures were
retained and corrected before the source-bound cost reading was accepted.

## Checks and continuity

After clean preflight, the notification-reader band returns **1**, the
code-request band **255**, and the turn-evidence band **65535**, all exit 0.
The code-request checks exercise the new public report-check kind, a
contradicted claim, an actual matching status and preservation of subsequent
report assertions. The live model experiment separately exercises the same
native replay callback within the existing resident review controller.
Drift gates return **8191/8191**, exit 0, with no kernel changes.

Glass first frame: **25 ms**. Counsel: **0 orphans**, with **11 of 12** lanes
unobserved because no hearth stands. The native authoring guide reports
**0 Python implementations**, **2 invocation candidates**, **0 unread**.
The prior procedural learner completed round **99**, pending zero, with
serving generation **5** unchanged. That learner is separate from Qwen.
This review used fresh unmanaged state and supplied no evaluation output to
training.
The generic verified review-check practice was retained under session event
`native-executable-review-2026-09-20`; its learner launched. A serving update
from that teaching remains pending. No evaluated answer was offered as its target.

The next comparison needs a separate enquiry and source-matched execution,
with these checks attached from the start. The current evidence supports a
local model completing this specific review through native feedback. Broader
quality, useful warmth, retained transfer, human resonance and the minimum
total rented cost remain open.

Signed: Codex, observing native Form execution and the attributed Qwen replies.
