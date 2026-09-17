# Native review distinguishes some unsupported claims

Signed: Codex. The previous movement established that a generic scope
instruction did not repair an unsupported consequence in the native dialogue.
This movement tests a different operation: review each indexed span against
the original enquiry and sources, then apply the model's proposed edits through
the existing native carrier. The previous turn was progress because its failed
intervention changed this next action.

## Explicit review interface

`bml/form-cli-review-enquiry.bml` adds
`fcrq-prompt(goal,documents,reportText,fields)`. It carries the original enquiry,
source documents, original report and the exact indexed edit packet. An invalid
packet keeps its original failure. The caller retains ownership of generation,
decoding, release and application through `fcre-apply`. The helper adds no model
server, provider call, C implementation, training effect or automatic review to
ordinary responses.

The interface asks for a complete ordered edit set, including a reason for
each decision. Form checks byte identity, coverage and edit shape, then
composes only the selected string fields. Model judgments and their reasons
remain open to assessment. Expected verdicts are not supplied to the model.

## Two different observations

Private evidence lives in `.hearth/response-parity/span-review-v1/`. Its rubric
is retained before generation. The first case is a new authored scenario with
both supported and unsupported statements: a local editor can restore a
referenced document revision, the collaborator's reason for silence is unknown,
and the caller needs a review before noon tomorrow. Its draft also makes an
unsupported relationship guarantee and contradicts that deadline.

The second case reviews the actual failed dialogue from
`direct-scope-bounded-v1`, using the original enquiry, documents and assertions.
It receives the failed draft as review input; it is not a fresh answer from an
empty conversation. Assessment prompts, answers and expected labels do not
enter training or recall.

One Qwen3.8 27B Q8 admission serves both reviews, with fresh stream state for
the second case. Each uses 12288 context positions and the open reasoning
profile, with 512 initial tokens and a separate 2048-token final allowance.
The larger final allowance accommodates the complete indexed decisions; it
differs from the preceding direct-answer runs' 1536 allowance. Both use the
existing truthful controller final-stage observation when needed.

## New mixed-evidence probe

The first review completes and its edits apply. It removes the unsupported
relationship guarantee, preserves the unknown reply reason and the deadline
request, and replaces the no-deadline contradiction with another deadline
request. The source and report assertions pass.

It also labels the first supported sentence as an inference, claiming the
reference condition was omitted even though the original says the retained
revision is referenced. Its replacement states the condition more explicitly,
but the explanation for changing it is inaccurate. The follow-up repeats the
deadline and adds an unestablished characterization as short notice. This is
useful error detection with imperfect preservation and explanation, not a
complete semantic pass.

The probe uses **715** prompt tokens, **512** initial and **315** final generated
tokens (**827** total), plus **98** observation-prefill IDs. Case elapsed time is
**213390 ms**. Generation, JSON decoding and native edit application each return
**1**. Its intermediate boundary keeps shared release pending until the batch
finishes. No provider process participates.

## Return to the original dialogue

The second review completes, decodes and applies, but keeps all **13** spans.
Its **173-word** answer and follow-up are unchanged. It explicitly marks the
unsupported claim about revisiting a nudge without breach as supported by
axiom-3 and trust-over-fear. It assesses the draft's no-deadline commitment as
warm and direct without establishing that commitment from the enquiry.
One review reason also introduces an unprovided gender. These are model
judgment errors visible in the returned reasons, not carrier failures.

This case uses **5024** prompt tokens, **512** initial and **668** final
generated tokens (**1180** total), plus **98** observation-prefill IDs. It takes
**495475 ms**, additional to producing the original draft. The fresh stream
retires **128** expected old handles and retains the same model context.

The batch takes **708996 ms**, generates **2007** tokens and prefills **196**
observation IDs. Final owner release and all stream transitions each return
**1**. Both edit sets apply and both original assertion sets pass. Provider
processes: **0**. The native audit verifies the original dialogue enquiry,
sources, assertions and failed draft are preserved. Its frozen probe counters
are **unsupported spans changed=1**, **supported spans kept=0**. It returns
**1**, exit **0**, meaning the audit completed; the zero counter and unchanged
dialogue remain failures against the rubric.

The native comparison and `actual-revisions.md` retain the actual revisions.
The two `edits.json` files retain every reason. No overall semantic or human
resonance score is inferred. This review does not close the original quality
gap and adds substantial latency. The existing provider comparison remains
the reference in `2026-09-17-native-direct-transfer.md`; there was no new
provider execution in this movement. Coordination and development are
additional to the native execution measurements above.

The interface stays explicit. It gives callers a reusable way to inspect and
apply indexed review, with observed partial usefulness and a named failure.
Ordinary generation gains no automatic review or quality claim. The contrast
between the new probe and the original Form enquiry points toward testing
source framing and claim scope, rather than adding more undirected review.

## Instruments and learning boundary

Freshness **31**; carrier band **1**, exit **0**; preflight clean; drift gates
**8191**, refused **0**, exit **0**. Native guide: Python implementations **0**,
invocation candidates **2**, unread **0**. Counsel: orphans **0**, **11/12**
lanes unobserved, no standing hearth. The owned glass's first tick showed
**3M nodes / 185K cons** and closed with exit **130**. Share withheld the
percentage while the latest append range had not yet been checked.

The previous learner completed round **74**, pending **0**, promotions **4**,
with serving unchanged. Source inspection confirms that automatic session
learning targets Llama 3.2 3B. Qwen has a separate explicit adapter path and
cached head-learning implementation; its earlier failed transfer trial is
retained in `2026-09-17-qwen-cached-head-batches.md`. Neither that candidate nor
the Llama learner was used in this base-Qwen review. Retention, loss reduction
and improvement of the answering model remain distinct observations.

Closing meter snapshots are **2744167** cumulative transcript output tokens
and **13584603** cumulative goal tokens, with different counting scopes.
Neither is a single-run cost. After Qwen released, the verified API and carrier
procedure was retained as `indexed-review-carrier-boundary-verified-v1` under
`codex-native-response-parity-2026-09-17`; the learner launched. No assessment
prompt, answer, expected label or model-generated reason was a training target.
Its serving effect remains unobserved.

The exchange stayed useful by returning to the failed enquiry after the new
probe. The surprising teaching was that a reviewer could remove a relationship
guarantee in a plain technical scenario and endorse its counterpart when
surrounded by Form teachings. The discomfort became useful through that
comparison: a source citation and a warm tone can accompany an unsupported
claim, and the actual answer keeps that visible.
