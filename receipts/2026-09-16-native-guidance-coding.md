# Native coding of review guidance

Signed: Codex, 2026-09-16.

## Work owned by the native model

The previous review selected the correct policy action, then deferred its next
action to a future window and invented a numerical evidence requirement. This
movement gives the local model a real source-edit task: improve the reusable
review guidance while preserving its JSON, checking, read-only and expression
constraints. It does not receive an assessment answer to copy.

The original wording was mechanically extracted into
`form/form-stdlib/bml/form-cli-review-guidance.bml`. The policy calls the same
function for initial entry and later observations. A frozen document task was
then supplied separately to native Qwen and a context-equipped provider session
with native Form tools. Provider dispatch and usage measurement occur inside
Form. The provider's source edit was not supplied to Qwen.

The completed native coding session took **464,958 ms**, with **nine replies**,
**three tool calls**, **two completed tasks**, **two check runs**, **zero
repairs**, **1,121 generated IDs**, **1,059 injected IDs**, and successful model
release. It used **zero provider calls**. The result contains an actual changed
document and passes independent document-contract and structural rechecks.
The source also passes native BML preflight before publication.

Its new wording directly connects the next action to the supported current
decision, makes broader evaluation optional unless the source requires it, and
excludes invented thresholds or evidence requirements. The coordinator read
the actual source against the frozen task before publishing it. Its hash is
`9d0a3648260c5ba3e87a787ddc201c9077c5000841183d0d587e06c054f36468`.

The native result is retained under
`.hearth/response-sessions/5d7f7dda01e2f8e1c9f101e6dd433daa48188216c795a2607ba2c8d7cd4474b4-18528-1789545571885`.
The frozen request, rubric, candidate source and publication checks live under
`.hearth/response-parity/action-guidance-*`. Publication checks the current
source against the original bytes before replacing it with the observed
candidate. Evaluation candidates remain excluded from weight training.

## Re-observed answer

The unchanged promotion request was then run through the installed native
candidate. Its request bytes match the prior run exactly, with digest
`2e399951c0dabc361a6303b317b7dd941610b4a926c433461cd0e0b74fedc3af`.
The result completes in **411,051 ms**, with **1,433 generated IDs**, **234
injected IDs**, **two replies**, **one native tool call**, **two check runs**,
zero repairs, successful release, and zero provider calls. The initial reply
uses native verification; the next supplies the report.

The actual answer retains the correct decision and evidence. Its next action
now records the supported promotion, preserves unknown usage as unknown, keeps
held-out targets outside training, and logs the usage gap for later evidence.
It no longer introduces an unsupported number of required rows or postpones
the supported decision. This is the coordinator's reading of the actual
answer; a human resonance preference has not been observed.

The previous answer used **2,094 generated IDs** and **499,462 ms**. This pair
therefore observes **88,411 ms less elapsed time** and **661 fewer generated
IDs**, alongside the additional native verification call. That is one pair,
not a general timing guarantee. It closes the specific action-language gap
without establishing whole-session parity.

Evidence root:
`.hearth/response-sessions/93c3c327d7e7a053f5ff4726ab24f9818fe9c0a4dafa73f9734ac08e9452d0c7-19839-1789546117703`.
The native comparison proof is
`.hearth/response-parity/action-guidance-response-proof.json`; the actual
before/after final answers are in the adjacent
`action-guidance-response-comparison.md`. Private reasoning stays excluded.

## Comparison and development costs

The retained provider candidate makes a similar targeted wording change. Its
successful source-edit process took **50,746 ms**, using **107,730 input
tokens**: **97,792 cached**, **9,938 uncached**, plus **1,281 output tokens**.
It reread the source and invoked native verification. The original verifier
had the formatting defect described below; the unchanged candidate passes the
corrected structural check and native BML preflight. Its original final reply
accurately reports that verification had not passed in that session. The
later recheck is separate evidence, not a rewritten provider outcome.

An earlier provider attempt took **92,353 ms**, with **11,623 uncached input**
and **2,834 output tokens**, but its sandbox could not retain the edit or
refresh the private tool cache. The adapter also returned its in-memory edit
without checking persistence. The retained document remained unchanged. The
adapter now verifies written bytes, and the provider receives write access to
the private evidence directory. The failed attempt and its costs remain.

Across both provider attempts: **143,099 ms**, **302,521 input tokens**,
including **280,960 cached** and **21,561 uncached**, plus **4,115 output**.
These are actual completed provider event totals. Coordinator usage is
separate. They do not establish a global minimum in rented tokens.

The original structural check expected `1` plus newline from native `rg -c`.
That tool returns `path:1` plus newline. This was a coordinator harness defect,
not a model failure. The first native attempt was deliberately terminated
after an observed **7 min 44 sec** because no valid candidate could satisfy
that formatting expectation. It has no completed result or native release
receipt. Its process exits **143** and its log is preserved as
`action-guidance-native-invalid-check-process.log`.

The corrected request changes only that expected output format. A native
observation proves the rest of the task and source identical, verifies one
definition, and rejects zero and two definitions. A correlated framebuffer
control selects revision, followed by those actual rechecks. The corrected
native run starts again from the original source. Both attempts count toward
development cost; the completed run is not the entire experiment.

## Evidence retention repair

The initial reasoning record and its final-channel reply could share one
millisecond timestamp. `fcac-retain-reply` then returned an empty evidence path
for the second write. A live trace exposed this as `reply_evidence:null`.
The owner now searches for an unused numbered suffix before writing. A forced
collision with synthetic markers verifies two distinct retained paths and
preserves both original byte strings. The proof is
`.hearth/response-parity/reply-collision-proof.json`; it contains no prompt or
answer content. This change does not alter model generation or actions.
The live follow-up also observes the repaired boundary: its first final reply
has a `-1.txt` evidence path alongside the initial generation at the same
millisecond, and its later report has its own retained path.

## Checks and continuity

Policy and request preflight and native validation pass. Both candidate source
files compile cleanly; the installed source is the native candidate. The
corrected harness observation, forced collision, and actual response comparison
all return 1 with exit 0. Diff check is clean and drift gates return **8191**.
The native authoring guide completes.

The previous learning worker completed round **16**, with **four promotions**
and serving generation **5** unchanged. This is the Llama learning lane; it is
not a Qwen weight update. The verified implementation teaching for this
movement concerns persistence, count-output format and retained execution
boundaries, with assessment answers and candidate wording excluded.
It is retained through the native session home as session
`native-guidance-coding-v1`, event
`verified-persistence-and-check-boundaries-v1`, with its worker launched.
Retention does not establish a later promotion.

Panel: **0 orphans; 11/12 counsel lanes unobserved**, no standing hearth.

The useful surprise is that the local model can author the targeted guidance
change through the complete native coding loop. The difficult part of this
movement was separating harness defects from model evidence; retaining the
failed attempts made that distinction testable. Overall quality, resonance and
throughput parity remain open.
