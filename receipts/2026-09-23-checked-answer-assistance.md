# Finish the answer through Form, and keep the native gap visible

Codex, 2026-09-23. The preceding movement repaired direct native edits. This
movement serves the same unfinished answer through the existing offered provider
resource, while the original native Qwen task continues.

## Actual work and cost

The [preparation](artifacts/2026-09-23-checked-answer-assistance/answer-edit-provider-prepare.bml)
preserves the native task's original goal, draft and source documents. The
provider receives no Codex answer or native revision. Its output transport is a
read-only report; Form reconstructs `answer.md` and reruns the original check.

| Form-owned provider stage | Words | Provider time | Tokens, including cached input | Cached input |
|---|---:|---:|---:|---:|
| Initial synthesis | 460 | 31,888 ms | 20,041 | 10,624 |
| Repair from actual failed count | 427 | 27,542 ms | 20,536 | 10,624 |
| Both calls | — | 59,430 ms | 40,577 | 21,248 |

The [first observation](artifacts/2026-09-23-checked-answer-assistance/answer-edit-provider.observation.json)
keeps the failure. The [repair request](artifacts/2026-09-23-checked-answer-assistance/answer-edit-repair.request.json)
formalizes the already requested 350–450-word range in `report_checks`; all prior
source and shape assertions remain. Its prior report came from the first
Form-owned provider call, retained locally, not from the local model.
The [final reading](artifacts/2026-09-23-checked-answer-assistance/answer-edit-assisted.observation.json)
counts distinct usage events once and verifies the same answer against both
the document and report contracts. Both processes completed and released.

This is provider execution time, excluding coordination and preparation. The
[previous completed coordinating turn](artifacts/2026-09-23-checked-answer-assistance/direct-repair-completed-turn-cost.json)
cost **6,229,077 tokens**, including **5,988,352 cached input tokens** and
**25,844 unattributed tokens** already included in the total. This open turn
has no completed total. The bounded calls are not the whole movement's cost.

## What the answer establishes

The [427-word answer](artifacts/2026-09-23-checked-answer-assistance/answer-edit-assisted.answer.txt)
is attributed to **codex-exec through native Form**. Reading it against the
unchanged source packet: all six axes are present; referenced and unreferenced
cells remain distinct; supplied annotations remain distinct from felt resonance;
translation surfaces, codebook anchors and model decoding retain their scopes.
The native word counter passes, and other documents remain unchanged.

Expression remains less direct than the retained 407-word Codex revision: the
opening spends a sentence qualifying what Form does, and much of the answer
lists definitions instead of showing how the interaction changes. This is my
reading of the text, not a numeric quality score or a report of Urs's resonance.
Whole-session quality, cost parity and local-only completion remain open.

The live Qwen task applied its 492-word edit on reply 8. Its action is byte-identical
to the action previously refused in repair (`cmp` returned 0). At that boundary
it had generated 5,501 IDs, injected 5,025, run four tools and two checks in
5,619,623 ms. The runtime then renewed its context. These are an unfinished
stage's counters, not a completed-session comparison. The same process continues;
it was neither restarted nor supplied the assisted answer.

## Repairs made at the observed boundaries

1. **Complete packets through the standalone synthesis door.** The actual
   36,376-byte manifest reached `read_line` as 8,191 bytes and invalid JSON.
   The first call returned `invalid-or-oversized-synthesis-manifest` with zero
   provider processes. The door now uses the CLI's file-aware command and
   accepts `@path`. The unchanged packet completed through that path. Replaying
   it returned zero new provider processes and the same usage event.
2. **Check the answer inside a report.** `word-range` accepts `field` in
   `report_checks`, selecting one exact top-level JSON string. It reuses the
   existing decoded-text counter. Missing, null and non-string fields fail;
   all subsequent checks remain. The original 460-word report, which passed
   shape checks, is now refused by the explicit length assertion. The existing
   repair resource consumed that actual observation and returned 427 words.

The first preparation preflight caught an extra closing parenthesis. The
request band then caught a missing-field lookup masquerading as an empty
string: `an empty report field differs from absent null or non-string content`.
An explicit field-presence check repaired it; the assertions remain intact.

Clean preflights precede request band **255**, synthesis boundary band **1**
(26 observations), resource band **1** (34 observations), and the full CLI.
Drift gates return **8191**, exit 0; no kernel or foreign runtime change.
The guide reports 0 Python implementations, 2 invocation candidates, 0 unread.
Glass first frame **54 ms**; intentional Ctrl-C returned 130. Counsel reports
0 orphans and 11/12 unobserved lanes. Share remains declared with percentage
withheld during carrier append validation.

The verified implementation teaching is retained as
`b2bd90641051e2ae9fba5213ef041c4ea0c52e9158ebcf878b97b7758e491c78`,
event `2026-09-23-report-word-range-and-file-synthesis`. Its later training and
use remain to be observed. Evaluated answers are excluded from training.
