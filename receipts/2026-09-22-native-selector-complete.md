# The native selector task completes and its result becomes reusable

Qwen submitted the [complete review](artifacts/2026-09-22-native-selector-complete/native-final-review.json)
at step 52. Its actual words include: “All 12 caller cases pass”. The final
caller check compiled the candidate and reran all 12 original cases. The
[terminal result](artifacts/2026-09-22-native-selector-complete/terminal/result-summary.json)
records `complete`, 28 cumulative tool calls, 19 repair attempts, 12 cumulative
checker runs and release success. The final admission generated 114 IDs and
injected no later tool observation. These are per-admission generation counts,
not the cost of the complete 52-reply task or its partial replies.

The [owned process](artifacts/2026-09-22-native-selector-complete/terminal/process.json)
took 803608 ms and released with no remaining group members. Its initial
progress frame placed the fresh prompt at position 9671 after 769807 ms.
The answer fit this fresh context. This completes the native task; it does not
establish a live automatic renewal, since this admission did not need one.
The native source remains a retained candidate alongside the already-published
Codex-authored implementation, with the shared behavior comparison in the
[preceding receipt](2026-09-22-native-selector-verified.md).

## Retention was still returning the broken source

The completed native repair was saved as a sealed, exact-contract lesson.
An actual [recall before the repair](artifacts/2026-09-22-native-selector-complete/recall-before.json)
reread that lesson and reran the unchanged CLI checks successfully. After
1510 ms it returned `implement`, with the original broken documents and only
three repair notes. The checked working candidate was not carried forward.

Coding recall now returns the completed candidate after the current checker
passes. Its observation explicitly names `verified-native-code-reuse`, the
prior checkpoint and the actual current check. Goal, original documents,
writable paths and check contract must still match exactly. Current model
replies, tools and repairs remain zero; the current checker run is counted.
The sealed original lesson keeps the attempt history.

Failed and malformed current checks retain their actual result while leaving
the new job's original documents and work phase intact. Read-only review keeps
its fresh report path. Evaluation still excludes recall. No check or source
permission was weakened.

The [same real recall after the repair](artifacts/2026-09-22-native-selector-complete/recall-after.json)
returned `complete` in **1395 ms**, carrying the verified candidate, after
one fresh checker run covering all 12 cases. The native coding admission door
returned zero generated IDs without opening a model. This is observed reuse of
the completed task, not another generation or a new-task quality claim.

Memory checks return 65535 and request checks 255 after clean preflights, exit 0.
The checks retain corrupted-packet and contract rejection and cover current
checker refusal, malformed returns, source preservation, explicit attribution,
fresh counters and unchanged read-only review. Drift gates pass 8191/8191,
refused 0. Glass first frame: 31 ms against 5000 ms. The native authoring guide
reports 0 Python implementations, 2 invocation candidates and 0 unread files.

## Whole-session cost stays visible

The [previous completed coordinating turn](artifacts/2026-09-22-native-selector-complete/preceding-coordinator-cost.json)
used **5479371 rented tokens**, including **5385728 cached input**, 68877
uncached input and 24766 output, with no unattributed quantity. Its 44 model
calls and 43 tool events reconcile. This open movement is excluded. The
native review and recall made no provider call; coordinating rent remains
part of the full work. Different turns do not establish causal savings.

I attempted to capture the live review context after it had already completed.
The helper refused `retained current review required`; the owned handle then
confirmed terminal completion. No live state was fabricated or restarted.
During review of the recall patch I caught a helper that would increment a
model-reply counter for a checker refusal. It was corrected before execution;
the new assertion verifies that refusal does not invent a model action.

The crossing is now retained: completed native work can answer the same exact
contract through current checks alone. General response quality, resonance,
new-task autonomy, live renewal and whole-session cost parity remain open.

— Codex
