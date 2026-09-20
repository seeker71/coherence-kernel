# Native review of a native draft

Codex, 2026-09-20. The preceding
[conversation-counsel trial](2026-09-20-native-conversation-counsel.md)
left a concrete contradiction: the explanation said Lee's reason for silence
was unknown, while the draft said “I know you're busy.” The draft also kept
`Your name`. This movement tests recognition and correction through fresh
native review, with no supplied defect list or arriving answer.

## A general second review missed the gap

The [review request](artifacts/2026-09-20-deadline-native-review-0-request.json)
contains the exact candidate, exact original task and all of its source
documents. It asks for supported findings with candidate/source quotations
and a complete revised response. It explicitly allows an empty findings
array when the candidate is correct.

Qwen returned [an empty findings array](artifacts/2026-09-20-deadline-native-review-0-report.json)
and copied the candidate into `revised_response`. Native semantic equality
[confirmed the copy](artifacts/2026-09-20-deadline-native-review-reobserve.json).
The unsupported reason and unfinished signature remained. This is a failed
recognition observation on this known case. It does not support installing a
general second review as a quality safeguard.

The controller completed on its first reply, passed the supplied assertions
and released the model. Those assertions checked an array type plus the
original report checks projected into `revised_response`. They did not check
the findings' completeness or truth. The original candidate already passed
the projected checks before admission; their shape did not seed a defect.

The [audit](artifacts/2026-09-20-deadline-native-review-audit.json) records
2,796 prompt positions, 849 generated IDs (512 initial and 337 final), 98
injected IDs, one reply, no repair and one check run. The case took
322,857 ms and its complete supervised process took 323,166 ms. Replay
performed zero new native executions. No provider process or evaluation
training was used. The [care flow](artifacts/2026-09-20-deadline-native-review-care.jsonl)
retains the selected action and fresh observation.

## A requirement-by-requirement diagnostic

The next request keeps the same candidate, original task, sources, Qwen model,
context 32768, two-reply limit, initial allowance 512 and final allowance 1536.
It asks for every explicit requirement, with candidate and source evidence,
a status and an explanation. It also explicitly applies each requirement to
every relevant field of the response. No expected findings or statuses are
provided. These task changes travel together; this is a diagnostic of
recognition under a more specific review task, not an isolated wording effect.

The [unfinished final output](artifacts/2026-09-20-deadline-native-requirements-unfinished-final.txt)
contains requirement rows and begins a revised response. Its visible rows
still mark the problematic draft as compliant:

- The row quoting “Hi Lee, I know you're busy” says the draft addresses Lee
  “without inventing a reason for silence.”
- The row quoting the full draft, including `Your name`, calls it complete
  and usable.
- The row about inventing a reason quotes “I'm not assuming anything about
  the silence” as evidence of compliance. That disclaimer leaves the earlier
  assertion of being busy unexamined.
- The source-only row treats the absence of external citations as sufficient.
  That observation does not establish support for the draft's factual claims.

The initial stage used 512 IDs; the separate final stage used its entire
1,536-ID allowance. The generation observation reported
`final-generation-incomplete`. The controller accepted zero replies and
produced no complete report. The session result is
`unresolved`; the model released normally. The source checks passed and
the subsequent report check observed the absent report. The exact
final-stage bytes remain unfinished; no closing braces or missing response
were supplied afterward. Initial reasoning remains private and unread.

The [audit](artifacts/2026-09-20-deadline-native-requirements-audit.json)
records 2,837 prompt positions, 2,048 generated IDs and 98 injected IDs.
The case took 555,835 ms and its complete supervised process took
556,186 ms. Terminal replay performed zero new native executions. There was
no provider process or evaluation training.

The [care flow](artifacts/2026-09-20-deadline-native-requirements-care.jsonl)
records incomplete generation, selects retention of the separate final
output, applies that action and verifies the retained bytes. Availability
of the unfinished output does not change the completion failure.

The visible assessment already missed the substantive gaps before its
output ended. More output space would address completion; these observations
do not establish that it would correct the assessment. Neither review
approach is promoted into serving. On this case, the work remains in
recognizing whether a practical claim is supported and applying that
recognition to a complete response. A quotation and a passing requirement
label establish no such recognition by themselves.

## What the runtime presently establishes

`fcap-verified` in `form-cli-code-policy.bml` completes when its caller's
checker returns a passing result. A native semantic review is not performed
after that result. The two tasks here use the existing native review
executor in separate fresh sessions. They are diagnostic work coordinated by
Codex, not a new autonomous serving pipeline. No runtime profile or model
weight changed for these evaluations. The C-bootstrap-only boundary holds.

The experiment helper now accepts its paths, goal and rubric as input,
reusing preparation, process ownership, publication and re-observation.
Each attempt keeps an exclusive evidence directory and its frozen manifest;
the launcher checks preparation and prior ownership before admitting work.
The publication helper now retains a separate unfinished final stage when
no report exists, with completion and evidence availability kept distinct.

## Cost and continuity

The [coordinator snapshot](artifacts/2026-09-20-deadline-native-review-coordinator-cost.json)
names completed turn `01a0bbfc-aa03-74f0-9213-f503c24fab05`: 43 model calls,
4,616,187 tokens, including 4,465,536 cached input, 97,527 uncached input,
25,380 output and 27,744 unattributed tokens. Reasoning output of 12,466 is
already included in output. This snapshot excludes the current open turn
and separate provider processes. Do not add it again if that turn has
already entered a cumulative ledger. Native zero-provider execution is not
zero-cost coordination.
The closing share reader reconciled that previous completed turn as
`kind=observed`; its event-count share does not measure semantic contribution
or the current open turn.

The previous procedural learning worker completed round 102 with an empty
queue. Its candidate remains different from the serving generation. That
observation concerns the separate session learner; it supplies no quality
evidence for these Qwen answers. No evaluated answer was used as a teaching.
The verified procedure for retaining an unfinished final stage was returned
through the native session-learning door as event
`native-review-incomplete-retention-2026-09-20`. Its example was retained and
a worker launched. That procedural teaching contains no evaluated answer;
an effect on serving remains unobserved.

## Verification

The native helper preflight had zero errors and unresolved calls. Drift
checks returned 8191/8191, exit 0, with no kernel source changes. The native
authoring guide reported 0 Python implementations, 2 invocation candidates
and 0 unread files. The counsel panel read 0 orphans and 11/12 unobserved
lanes, with no standing hearth. Actual native model execution and release
are retained in each supervised process record, separately from that panel.

Signed: Codex.
