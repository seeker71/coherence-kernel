# Landing status and missing source attention

Signed: Codex, 2026-09-22.

## The actual movement

`ld-land` discarded the drift-gate process status and accepted a `refused=0`
substring. It now runs the existing native process organ, retains stdout,
stderr and the process evidence directory, and requires both the exact status
`"0"` and the existing success marker. The report includes that status and
evidence path. Seven boundary cases passed; the actual gates returned
**8191/8191, exit 0**, with the returned status matching the process record.
The gate program and the other landing operations remain unchanged.

The repair is Form/BML on the existing C bootstrap. It adds no runtime or model
server dependency. Codex completed the source repair after the native coding
run stopped incomplete; this is an arriving-agent contribution.

## What the native run established

One Qwen3.8-27B-Q8_0 admission used the native coding core, an owned checkpoint,
catalog source context and a native caller checker. The initial admission was
2,547 positions. Tool observations added 8,381 IDs and generation produced
1,360 IDs. At position 12,287 it retained an unfinished reply and returned
`attention`. There were 14 completed replies, 11 tool calls and **zero checks**.
The candidate remained byte-identical to the original. Process duration was
1,384,671 ms; the process exited zero and released its resources. Process success
does not establish task completion.

The native diagnosis included:

> The requested prelude path is not a resident document in this context; only the catalog documents are available.

That diagnosis was correct. I had supplied the landing file, gate program and
process door, while omitting helper definitions the model sought. The native
draft contained the exact-zero predicate direction, but its edit was malformed
JSON and returned a timing report instead of the required gate output. None of
that draft was accepted as an edit. The returned bytes, including the unfinished
reply, remain in the [native reply evidence](artifacts/2026-09-22-landing-process-status/native-reply-bytes.json).

## Failure becomes actionable attention

Replaying the exact retained search against the original three documents
initially returned exit 1 with empty stdout and stderr. That hid the difference
between an unavailable source and an available source with no matching text.

Resident search now reports exit 2 with the unavailable path and explains the
catalog boundary. The same native coding step enters `repair`, retains the
original documents and counts one tool call. Available-source no-match remains
exit 1; directory selection, glob exclusions and explicit empty stdin retain
their behavior. The existing document-context witness covers these boundaries.

The existing source-care organ then supplied four selected helper sections
through `oc-hear` and re-read them. Supplying the missing report document made
the original search return exit 0 with **7,363 output bytes**. This observed
source recovery used zero model calls. It establishes delivery and search
recovery; no second native generation was run, so improved completion remains
unobserved. The initial failed run remains part of the cost and quality record.

The teaching is concrete: reducing admitted text helps only when the tools can
still reach the definitions needed for the work. `AGENTS.md` now names that
caller responsibility and the missing-source repair signal.

## Instruments and remaining boundaries

The cost reader was repeatedly scanning because its turn selector contained a
whole JSON cost report. I preserved that value locally and reset it through
the existing native target-binding door. The next reading completed with
**8,659,701 total rented tokens** for completed turn
`01a0c824-92c7-7061-b460-3010a86efade`: 8,614,960 input, including 8,414,848
cached input, plus 44,741 output. All 53 tool calls and outputs reconciled.
This is the preceding completed turn, not this open movement or a native-run
token total. The selector still accepts arbitrary text; admission validation
remains a repair to carry forward.

Glass first-frame panel: **30 ms**. The output meter read **3,999,746 session
output tokens**, a narrower measure than the full-turn cost. The share reader
reported `kind=declared`, with percentage withheld while checking the appended
range. No current-turn contribution percentage is claimed.

I launched Glass without a terminal and stopped my owned live process after
the reading to end its continuous output. Consequently
`form-run ./fkwu observe/form-glass-run.fk` exited 1 with
`fkwu: form_error: Glass live runner failed; owned sensors released`, followed
by `fkwu: form_error: Glass supervisor ended with an error`.
The supervisor observed live exit 143 from that intentional termination.
This is retained as an instrumentation outcome, not a passed check.

The code checks passed: document context **1**, resident tool edges **131071**,
coding policy **65535**, seven landing boundary cases and actual drift gates
**8191/8191**, all exit 0. Native authoring guide: zero Python implementations,
two existing invocation candidates in `voice-say.bml`, zero unread sources.

Automatic context supply during a running coding admission, specific JSON
repair guidance, and successful native completion of this task remain open.
Native parity and lower total rented-token cost have not been established.

Evidence: [native outcome](artifacts/2026-09-22-landing-process-status/native-outcome.json),
[search before](artifacts/2026-09-22-landing-process-status/search-before.json),
[search after](artifacts/2026-09-22-landing-process-status/search-after.json),
[care and re-observation](artifacts/2026-09-22-landing-process-status/search-care.json),
[real gates](artifacts/2026-09-22-landing-process-status/verified-gates.json),
[completed-turn cost](artifacts/2026-09-22-landing-process-status/preceding-coordinator-cost.json).
