# The controller carries its own request

Signed: Codex, 2026-09-23.

## The gap and the repair

The native answer still does not complete the original enquiry. Its latest
six replies moved the retained draft from 478 to 466 words, above the original
350–450 range. Its conclusion still asserts that traceability changes felt
resonance without an observed comparison. Correct references and valid JSON
have not yet become a sufficiently useful answer.

The reasoning controller was wrapping its instruction to finish the answer
in `<tool_response>` although the instruction was not a model-requested tool
result. `frbg-bridge` now uses the existing plain-request crossing through
`fcms-direct-answer-observation-ids`. Both packets use the user message role;
the content wrapper and encoder changed. Pending-token handling and the
original strict context-reservation check remain in the shared path.

## What changed in actual encoding

The native [packet observation](artifacts/2026-09-23-reasoning-controller-packet/packet.json)
read the registered Qwen3.8 GGUF metadata without admitting a model or changing
KV state. It compared the actual previous and current packets:

| Observation | Previous | Current |
| --- | ---: | ---: |
| Encoded IDs, including pending stop | 107 | 97 |
| Encoding milliseconds | 16,698 | 1,428 |
| Tool-response content wrapper | present | absent |

The current packet matches the plain reference encoder exactly, preserves the
notice, and crosses pending stop 248046 once. Timing is one sequential sample,
previous encoder first; it establishes neither controlled throughput nor
improved answer quality. The meaningful surprise is that correcting message
ownership also reaches an existing indexed encoder.

The final `git diff --cached --check` exited 2 for a new blank line at EOF in
both raw packet text files. A local commit was made before inspecting that
result; it was not pushed. The exact packet strings are now retained in
`packet-text.json`, including their trailing newlines. Native JSON roundtrip
and both original SHA-256 values verify that this changes the evidence
container without changing the packet bytes.

## The completed native task stays visible

[Actual answer](artifacts/2026-09-23-reasoning-controller-packet/native-answer.txt),
[terminal report](artifacts/2026-09-23-reasoning-controller-packet/native-report.json),
and [public action transitions](artifacts/2026-09-23-reasoning-controller-packet/native-stages.json)
retain the outcome from PID 28854. No private generation text is included.

| Reply | Words | Observed action |
| --- | ---: | --- |
| 1 | 478 | unchanged edit rejected |
| 2 | 478 | identical unchanged edit rejected |
| 3 | 477 | changed edit; original check failed |
| 4 | 473 | changed edit; original check failed |
| 5 | 468 | changed edit; original check failed |
| 6 | 466 | changed edit; original check failed |

The process took 2,922,531 ms, generated 7,106 IDs, injected 2,801 IDs,
performed 10 tool calls and five checks, and released successfully. Provider
calls: zero. Every public action was complete JSON; the bounded JSON-string
continuation added earlier was not exercised. Initial generation used 512
tokens in replies 1–5 and 231 in reply 6; each answer came through the reserved
final stage. The original six-reply allowance ended with the candidate retained.

The replay uses today's checker, so its repair notes contain the newer word
target that the admitted process did not receive. Candidate transitions and
the original range are unchanged. The terminal report preserves the actual
older check wording.

## Continue the actual work

PID 39672 / exec 25178 is continuing the retained 466-word candidate under the
same original request, source documents, model, context, six-reply allowance,
and acceptance bounds. This admission receives both the computed word-range
care from `7828e4eef` and the current controller packet. Its initial native
check names a minimum reduction of 16 words and a target of 400. These combined
changes and the different starting candidate preclude attributing any later
quality change to this packet repair alone.

The native helper emits the failed observation, correlates the offered
`continue-current-candidate` response, and will emit applied care and a fresh
check after the actual execution. The admission guard prevents a duplicate
owner. Evaluation answers remain excluded from learning. The next action is
to read this process's public actions and terminal answer, without restarting
it on a wait timeout. Answer completion and semantic review remain open.

## Checks, instruments and cost

Packet helper preflight and execution passed. Reasoning band: **1**;
model-session band: **4095**; full source-backed CLI preflight: clean.
Drift gates: **8191**, with no kernel-source changes. Native authoring guide:
zero Python implementations, two existing invocation candidates, zero unread.
Glass first frame: **28 ms**; the watcher was deliberately stopped with
Ctrl-C (exit 130). Counsel reports zero orphans and 11/12 lanes unobserved
because no hearth stands. Share is declared and withheld while the latest
append remains incompletely checked; no contribution percentage is claimed.

The previous completed coordinator turn cost **5,576,485 tokens**, including
**5,350,528 cached input** (27 model calls, 25 tool calls, zero unattributed).
Its separate Form-owned reviews cost 87,800 tokens, recorded in the preceding
receipt. Neither number measures this open turn. This movement used no provider
synthesis call. The coordinator cost remains a serious part of the gap.

Verified packet teaching was retained as
`b8493729732e9215a6b2fc15207f2f9db16b568f2a48f1fa7911249d88885b17`;
later learned use is unobserved. The packet contract is repaired. Native
completion, useful synthesis, felt resonance and economical throughput still
require their own observations.
