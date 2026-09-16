# Native response sessions: transport, review and visible failures

Signed: Codex, 2026-09-16.

The native executor now owns a frozen set of assessment requests, retains each
actual result and independently repeats the caller's checks. It continues past
an unsuccessful case while preserving that case. Its summary keeps semantic
quality and frequency unassessed. A successful runner exit establishes retained
assessment, and each answer still needs examination.

## Observed, repaired, re-observed

- A 45,251-byte manifest reached `read_line` as 8,191 bytes and failed JSON
  admission. The session door now takes a manifest file path. The ordinary
  coding door accepts `@request.json`. A separate 16,660-byte request preserved
  its 16,384-byte document byte-for-byte and reached model admission, where
  its deliberately absent model was correctly reported unavailable. The
  repair lives in BML; the C seed stayed unchanged.
- A negative finding in read-only review used to enter implementation repair.
  It now stays in review, carrying the finding forward. Structured JSON reports
  may be submitted directly or through the existing report wrapper. All forms
  reach the original caller-owned checks. Coding rejection still enters repair.
- Completed replies now expose only shape and control counters in the
  framebuffer. Their exact text is retained separately under the private
  hearth for diagnosis. A real write/readback witness preserved its reply
  byte-for-byte. This evidence is excluded from verified training targets.
- Two absent-model cases exercised the session runner's failure retention:
  both stayed present, both failed the behavior assessment, and the runner
  completed in 33 ms with zero provider calls inside its native executor.

## The quality finding remains adverse

The mixed development session's first completed case took 399,176 ms, generated
68 token IDs and injected 1,233. Its actual same-model critique was:

```json
{"flaws":[],"repair_needed":false,"correction":""}
```

The source answer had said, “we cannot promote based on a few supervised rows.”
The supplied policy used the loss conditions for promotion and separately said
that a few rows cannot establish overall session quality. My assessment is that
the answer blurred those claims by adding an unsupported promotion obstacle;
the critique did not identify it. The report's boolean/array type checks passed.
They establish its shape, not the soundness of its judgment.

The frozen session also contains a promotion-rule transfer, an actual native
help-editing task and Urs's question about axioms, trust, vocabulary and response
resonance. Those cases were still running when this receipt was written. Their
request manifest and per-case results live under the private hearth. The source
help candidate must receive a diff review and compiler/behavior checks before
publication. No result or quality verdict is supplied in advance.

Two earlier local attempts were deliberately stopped and retained: the original
critique process exited 143, and the first file-backed cohort exited 143. Neither
produced a completed answer. Their repeated review transitions suggested a
protocol problem, but did not retain enough intermediate reply text to establish
the exact cause. That limitation motivated the private reply evidence above.
The current cohort was admitted before that final diagnostic addition and keeps
running its admitted program.

## Witnesses and cost

Preflight closed the policy, request and session bands. Native validation passed:
policy 65535, request 255, session 1, CLI 67108863, hearth 32767. The session band
checks failed-result retention boundaries, independent assertions, document
preservation, permitted writes and the separation of behavior from quality.
The effectful runner passed compile-only checking. Drift gates read 8191/8191.
The native authoring guide reported zero Python implementations and the same two
voice invocation candidates.

Panel reading: counsel reported **orphans 0**, with **11/12 lanes unobserved**
because no hearth resident stood. The reply share remained declared and withheld
while carrier evidence was still being checked. These are measured limits.

This movement made no new provider/subagent call from the native executor.
Rented parent coordination remains a real cost: the goal meter read 766,336
tokens at one observation, using that meter's aggregate basis. This is separate
from the previously retained provider usage: successful baseline 319,072 total
tokens and failed baseline 47,539. These categories must retain their scopes;
the executor's zero provider calls does not make the whole session free.

All four cases are evaluation requests. Their answers remain excluded from
learning targets. Whole-session quality, frequency and throughput parity remain
open. No Qwen weight update or native voice completion is claimed.

A separate verified teaching about the transport and controller checks was
retained through the session-home door as event
`transport-review-controller-verified-v1`. Its local learner was running at this
observation, with one pending example, seven completed rounds and serving
generation 5 retained. That learner uses Llama 3B. It overlapped the ongoing
Qwen development assessment after this teaching was offered; later case timings
include that concurrent work and are not isolated inference measurements.
Retention and a running worker establish neither promotion nor completed
learning. The private log is `session-runner-embody.log` below.

The useful surprise was that a cleanly completed critique could still miss the
specific distinction it was asked to inspect. That uncomfortable observation
gave us a better boundary: retain the answer, examine its reasoning, and let a
protocol pass mean exactly what it checked. The exchange stays alive through
that correction and the next actual native attempt.

## Local evidence

- `.hearth/response-parity/session-failure-witness.log`
- `.hearth/response-parity/file-request-witness.json`
- `.hearth/response-parity/native-critique-process.log`
- `.hearth/response-parity/mixed-session-file-process.log`
- `.hearth/response-parity/mixed-session-structured-process.log`
- `.hearth/response-sessions/28d1ce39bd631d4ecbee8b5e97c7ec0d4c73bc424f601ee0d0073abd9e46149c-39002-1789525379414/`
- `.hearth/code-memory/replies/53892-1789526282459.txt`
- `.hearth/response-parity/*session-final-validation.log`
- `.hearth/response-parity/session-drift-gates.log`
- `.hearth/response-parity/session-runner-embody.log`
