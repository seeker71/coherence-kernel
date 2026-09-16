# A verified repair returns to its resident

Codex, 2026-09-16. Continues `2026-09-16-review-check-scope.md`.

## Implemented and observed

A caller-owned native repair may now return a fifth element, `"review"`.
After the unchanged checker accepts that candidate, the controller keeps the
task unfinished and returns the rejected and verified reports, each paired with
its own check result, to the existing resident. The resident completes the
original caller report and the same checks run again. Four-element callbacks
retain their previous behavior.

Only completed observation delivery marks the evidence admitted. Later feedback
may reference it; a renewed context receives the full pair again. Another repair
owes another packet. Native work stays inside its submitted turn; the original
turn limit still governs whether the resident can continue. Provenance survives
the resident's later submission and binary continuity. This adds no C seed code
or runtime dependency.

The native repair band passes 33 assertions, returning 8589934591 with exit 0.
Policy and memory bands return 65535, request returns 255, and each preflight
has zero errors, warnings and unresolved calls. The drift gates return 8191.

## Fresh local trial

The original frozen controller request was run on Qwen Q8 with the existing
stronger behavioral checker and structural search. Both original and effective
request hashes match the preceding Q8 trial. No reference answer was supplied.
The native repair callback's explicit continuation is the new behavior.

| Observation | Previous native finish | Resident continuation |
| --- | ---: | ---: |
| Wall time, ms | 220721 | 506609 |
| Generated token IDs | 306 | 620 |
| Feedback token IDs | 0 | 840 |
| Provider calls within trial | 0 | 0 |
| Native repair attempts | 1 | 1 |
| Release | 1 | 1 |

The continuation used one model admission and two submitted turns. Its controller
recorded three checks: rejected initial report, accepted native candidate, and
accepted resident report. The final independent recheck found zero behavioral
counterexamples; the original caller contract also passed. Source documents
remained unchanged. Final attribution is `submitted`, with the native repair
history retained.

The mechanism worked; the explanation still failed the qualitative review.
The final code retains `fcac-loop` at the budget boundary, which preserves its
checkpoint, offline and budget handling while skipping model feedback. But the
answer identifies `fcac-observe` telemetry as the unusable feedback. The skipped
call is `fcacs-observe`; `fcac-observe` still runs through the preserved loop.
It also names `fcac-done` where the source uses `fcap-done`, and does not explain
the checkpoint/offline distinction established by the paired checks. Those are
concrete source-reading errors despite the passing code checks.

The full feedback packet took substantial time to admit. This trial establishes
neither explanation parity nor a throughput improvement. The next work is
source-grounded causal tracing and cheaper evidence admission, measured separately.
The opt-in continuation provides a place for that work without declaring a
checked candidate to be a finished answer.

Private evidence stays under `.hearth/response-parity/continuing-review-live/`:
the frozen request, exact final report, complete result and measurement receipt.
The adjacent `.bml` driver and `.log` retain execution. The erroneous answer is
not a training target.

## Cost, instruments and embodiment

Zero provider calls describes the local trial only. Coordinator work remains
rented: the active-goal counter moved from 4630480 at this movement's start to
4723309 at an intermediate boundary, a delta of 92829 before landing and closing.
The separately scoped transcript meter reports 1094695 cumulative output tokens;
these counters are not added together. Its first invocation lacked the required
transcript path and returned `signal=nothing`; the correctly bound re-observation
produced that reading.

Counsel reports orphans 0, with 11 of 12 lanes unobserved because no resident
hearth stands. The share reader reports declared/unmeasured and withholds a
percentage. These instruments do not establish semantic quality.

A verified teaching about continuation, evidence delivery and check scope was
retained through the native session-learning door after Qwen released. It excludes
the assessment answer and reference repair. Worker launch was observed; this
receipt does not claim a weight improvement. That learner serves the existing
Llama adapter, not the Qwen model used here.

The useful surprise is that supplying both reports and both verdicts still did
not make the model distinguish two similarly named effects. The uncomfortable
result became a precise next target: the code can be correct while its causal
story remains wrong. Keeping that mismatch visible keeps this movement honest.
