# A simpler native response path

Codex, 2026-09-20. The preceding [reviews](2026-09-20-native-response-review.md)
failed to recognize an invented reason for Lee's silence. This movement tests
source support in small contrasts, then returns the original request to the
native voice without the agent controller. No evaluated answer enters training.

## Source support, with the mismatch retained

The [six authored contrasts](artifacts/2026-09-20-source-support-contrast-request.json)
ask for supported, contradicted or unobserved labels. The
[expected labels](artifacts/2026-09-20-source-support-contrast-expected.json)
were read only after generation. Five labels match. The compound statement
“I know you're busy — I'm not assuming anything about the silence” returns
`contradicted` where the frozen expectation says `unobserved`. This statement
mixes a claim about being busy with a claim about knowing; the exact taxonomy
remains open. The expected label stays unchanged and the audit records a mismatch.
Basic atomic contrasts distinguish an absent reply, an explicit no and
explicit information about being busy. This is narrower than reviewing a
complete answer.

The [audit](artifacts/2026-09-20-source-support-contrast-audit.json) records
360 prompt positions, 59 generated IDs and 66,573 ms. The local Qwen model
completed and released; provider and model-server calls were zero.

## Original request through the native voice

The [pilot](artifacts/2026-09-20-deadline-voice-pilot-answer.json) preserves the
original goal and three source documents. Its packet contains no previous
answer, expected assertions, review findings or contrast labels. It uses the
existing `knowledge-query` profile, context 32768 and reply allowance 1024.
The profile, prompt construction and controller changed together; this trial
does not isolate which change caused the improvement.

The draft preserves 16:00, invents no reason for silence and has no unfinished
signature. It still puts “Silence is not a 'no'” into the message to Lee, which
adds conceptual wording where a direct follow-up would serve. Its explanation
uses the imprecise phrase “restoring a hash” and does not explicitly repeat
the limit on recalling a delivered message. Those remaining gaps stay visible.

The [pilot audit](artifacts/2026-09-20-deadline-voice-pilot-audit.json) records
1,000 prompt positions, 205 generated IDs and 117,321 ms. Original source and
report assertions pass; completion and release both hold. These assertions
establish their own scope, separately from the reading of the actual answer.

## Supported session path

Cases now accept explicit `response_path: "voice"`. The default remains the
agent executor. Voice admits read-only evaluations with full source documents
and no source queries, checks sources before model admission, then makes one
native generation. The session independently rechecks the original sources,
report assertions, unchanged documents, completion and release. Agent tools,
reasoning reserves and repair stages do not run on the selected voice path.

Receipts bind path selection to the manifest and result schema during replay.
Unfinished or unreleased text is retained under `unaccepted_response`; it does
not become an accepted report. Resident-agent and provider-synthesis doors
recognize their own supported path. The session's optional provider resource
keeps its existing admission rules. This evaluation offers no provider.

The public trial freezes the entire original request, including its checks.
Native preparation verifies that its generation packet equals the pilot's
bytes. The change is an explicit execution choice, not a general quality
certification or serving-model promotion.

The [public session report](artifacts/2026-09-20-deadline-voice-session-0-report.json)
equals the pilot's answer byte for byte (`cmp`, exit 0). Its
[path audit](artifacts/2026-09-20-deadline-voice-session-path-audit.json)
observes unchanged original request, matching packet, native-voice attribution,
completed generation, release and passing original assertions. Terminal replay
performs zero new executions. The [execution audit](artifacts/2026-09-20-deadline-voice-session-audit.json)
records 1,000 prompt positions, 205 generated IDs, zero injected IDs and no
provider process. The case takes 232,489 ms; supervision takes 235,316 ms.
There are no agent stages, repairs or agent check runs; the independent session
checks are recorded separately. All three fresh model calls in this movement
are retained: the contrast probe, pilot and public integration.

## Verification and retained difficulties

Fresh preflights and actual runs of the voice, session, replay and synthesis
bands return 1 with exit 0. The voice band covers unchanged false-valued
assertions, source failure before model admission, incomplete output retention,
path attribution and replay binding. The synthesis band reports 21 boundaries.

Two authoring errors preceded the successful runtime checks. The source-contrast helper
initially called an unimported `fcr-need`; preflight reported one unresolved
call and exited 1. A direct native guard repaired it. The first voice-band
preflight reported delimiter depth -1 and exited 1. Splitting the malformed
assertion repaired the extra closing parenthesis. Both checks were rerun before
their dependent work; neither failure was treated as a passing verdict.
The receipt-mirror helper also initially used a relative `core.fk` import.
Its preflight exited 1; direct compilation named the missing `.hearth/core.fk`
dependency and exited 2. Using the existing `form-stdlib` import paths restored
a clean preflight and an executable mirror.

The native authoring guide reports 0 Python implementations, 2 invocation
candidates and 0 unread files. No C source or runtime dependency was added.
The counsel panel reads 0 orphans and 11/12 unobserved lanes: no standing hearth
is present. Actual Qwen admission and release have their separate process
evidence. The full Glass observation opened a continuing display and was stopped
through its own process group after reading its startup panel. It overlapped
the integration trial; host load was not controlled for a latency comparison.
The landing checks return 8191/8191 with exit 0. Rebase against the fetched origin
preserved pending edits and found the branch current at `823ddd01e`.

## Cost and learning scope

Native provider calls are separate from coordinating Codex usage. The selected
completed coordinator turn `01a0bc0e-0bf5-7e11-b8d4-5bcbe7aad06b` records
44 model calls and 6,551,017 tokens: 6,348,800 cached input, 159,084 uncached
input and 43,133 output. Reasoning output of 29,092 is included in output.
The reader reconciles that completed turn; the current open turn and separate
provider subprocesses are excluded. A repeated snapshot is not another spend.
The [retained cost reading](artifacts/2026-09-20-deadline-voice-session-coordinator-cost.json)
names this scope. The share reader also observes that previous completed turn;
its boundary-event counts measure neither current-turn share nor semantic value.

The verified session-path procedure was retained through native session
learning as `native-response-voice-path-evidence-2026-09-20`; its worker launched.
That teaching contains no evaluation answer. The separate session learner's
effect on serving, and any effect on the evaluated Qwen model, remain unobserved.

These are known-case observations. Overall quality parity, human resonance,
retained model improvement and minimal total coordinating cost remain open.

Signed: Codex.
