# The local voice returns to the fkwu process

Codex, 2026-09-19. Rebased `codex/native-arrival-bootstrap` onto
`origin/main` at `7f64081e7`, preserving the two pending reserve documentation
edits. The new upstream voice route contacted a llama.cpp server on loopback.
This movement replaces that dependency with the existing native model session.

## Resolve at the executing boundary

`form-cli-native-voice.bml` selects registry weights, uses the compact
`knowledge-query` profile in `fkwu`, retains the actual generated IDs and text,
and observes model release. Stopped generation with a valid stream and successful
release can supply an answer. Incomplete text remains available as raw evidence.
Health readings carry availability, completion and release at this boundary.

`native-voice-run.bml` and grounded synthesis use this shared path. The legacy
`oracle` option selects local generation. New rows say `fkwu-model-session`;
old external-server rows keep their original source. The movement caller now
waits for the result row after model progress, keeping the complete child output
instead of ending the pipe after its first line.

An omitted provider offer now leaves grounded synthesis native-only. The
existing optional provider path remains separate. Its short-answer observation
now reports the measured length without inventing a tool-use cause.

No C seed change or additional runtime was needed. The current Qwen path uses
the host's in-process Metal carrier and the registered model artifact.

## Re-observe the real voice

The same pinned comparison enquiry and native composition went through
`observe/native-voice-run.bml` with a 1,536-ID allowance. The complete returned
words are preserved unchanged in
[the native answer](artifacts/2026-09-19-fkwu-native-voice-answer.md).

| Observation | Result |
| --- | ---: |
| Model | Qwen3.8-27B-Q8_0, registry `qwen38-q8` |
| Prompt IDs | 3,502 |
| Generated IDs | 1,050 |
| Completed / released | 1 / 1 |
| Native generation and retention | 471,950 ms |
| Ground preparation | 25 ms |
| Provider calls / model-server calls | 0 / 0 |
| Rented tokens within this native call | 0 |
| Census local-process / network dispatches | 0 / 0 |
| Structural axes / receipt citations / named limits | 9 / 0 / 1 |

This establishes a complete local generation through the C-bootstrap runtime.
It does not establish parity in quality or throughput. Coordinating provider
work is outside the native call's zero-rent boundary.

The answer retained the pending quality and resonance judgments, but omitted
receipt citations. It broadened exact dictionary misses into claims that the
resources lack definitions, and repeated a zero minimum from its supplied
composition. Those are actual remaining quality defects.

The minimum had a source-level cause: the composition mixed provider and
deterministic native contest rows and used zero for both absence and a value.
The calculation now selects provider rows with an observed nonnegative integer
rent and the declared structural floor. Absence remains `nothing`; an observed
provider zero stays zero. Native controls retain a sound provider minimum when
native zero rows and failing-floor rows are added, and distinguish missing usage.
The retained ledger now reads **10,875** for that provider-only structural subset.
Two control runs exited 1 with `order: only numbers have an order`. The first
interpretation blamed boolean evaluation; a focused native probe instead showed
that bare `nothing` was a closure. The callers now invoke `nothing()` to obtain
absence, and an explicit branch selects the initial value before numeric order.
The intermediate band preflight also exited 1; landing stayed paused.
The original model answer stays unchanged. A later model answer from the corrected
composition still needs observation; this repair establishes the source reading.

The surprise was useful: a native model can faithfully repeat an incorrect
native calculation. Removing the server dependency and correcting the supplied
calculation are observed progress. Citation use and interpretation of lookup
coverage remain the next response-quality work.

## Evidence and closing

Private live evidence:
`.form-heal/native-voice-64746-798887170-0/` holds the packet, raw generation,
generated IDs, metadata and health event. The row is retained in
`receipts/rent-ledger.jsonl` under `c-bootstrap-native-voice-2026-09-19`.

A public grounded-synthesis request naming an absent model, with provider
permission omitted, returned `registry-model-artifact-absent`, source `none`,
zero generated IDs and zero provider attempts. Its evidence is
`.form-heal/grounded-66024-799156629-0/`. Availability was reported without
adding a runtime or provider fallback.

Fresh preflight found zero errors and unresolved calls for the affected doors.
The grounded synthesis band returned **1023**, the existing code-request band
**255**, each with exit 0. The native guide reported Python implementations
**0**, execution candidates **2**, unread files **0**. Counsel reported
**orphans 0** and **11/12 lanes unobserved** without a standing hearth.
The share reader remained declared and withheld percentages while validating
the appended transcript. Its 85,324 ms refresh is retained as coordination
overhead, separate from native generation.

The verified procedural teaching was retained as session-learning event
`c-bootstrap-native-voice-and-source-cost-boundary-2026-09-19`; its worker was
launched. Retention alone does not establish a changed serving model. None of
the evaluated model answers was submitted as a learning target.

The earlier matched provider comparison and its full correction cost are in
[the companion receipt](2026-09-19-fresh-matched-native-synthesis.md).
