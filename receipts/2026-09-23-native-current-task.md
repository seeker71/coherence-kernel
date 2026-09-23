# Keep the active correction at the final-answer boundary

Signed: Codex, 2026-09-23.

The [report closure repair](2026-09-23-native-report-closure.md) landed in
`5b8f16374`. The continued Qwen run then returned valid JSON, but its answer
and findings were byte-identical to the repaired report: **554 words**, with
the same unsupported frequency-to-response claim and the false finding that
the word limit was met. The original checker rejected it. This establishes
format progress and unchanged answer quality on this reply.

The [native observation](artifacts/2026-09-23-native-current-task/after-stop.json)
replays that public reply through the unchanged source/report checks and
retains the exact controller notices before and after the next repair.
The old final-stage notice asked for the original enquiry again. The current
repair role and measured failure remained earlier in context but were absent
from this newest controller request. Repeated output does not establish that
omission as the sole cause; it identifies a concrete instruction gap to repair.

Native coding now supplies its current controller observation to the bounded
final stage. That packet contains the full failed check, current role,
amendment identity and remaining reply budget, even when an earlier observation
has already admitted the failure. The final-stage request asks for the current
task. The goal, documents, checks, model and generation allowances stay intact.
Ordinary generation retains its existing notice, and an already complete
model answer receives no extra handoff. No generated answer or private
reasoning is inserted by this repair.

The existing reasoning boundary band returned **1** and request band **255**
after clean preflight, both exit zero. They verify the actual bridge receives
the current correction and the code path carries full evidence after context
admission. Drift gates returned **8191**, exit zero. No kernel source changed.
The checked teaching was retained as native session-learning example
`a67f40602d1729d5c38450286caa681c909f8260b1f22b03c74bcae101efbb11`,
event `2026-09-23-native-current-task-handoff`, under the continuing session
`codex-native-arrival-bootstrap`. Retention is observed; a serving-weight
update is not established.

The old owner, PID **54872**, exec **94249**, was deliberately stopped after
the checkpoint and repaired boundary were verified; its terminal exit was
**143**. This was not a wait expiry. The checkpoint digest was unchanged across
the stop, with three completed replies and two checks. The resumed admission
had spent **1,444 generated IDs** on its one complete reply, plus an unfinished
next reply whose exact total was not observed. Normal model release remains
unobserved. These costs remain visible alongside the earlier interrupted run.

The [correlated care](artifacts/2026-09-23-native-current-task/care.json)
re-observed current-failure presence at the handoff, made zero checkpoint
mutations, and prepared the same public review with nine remaining replies.
The new owner is PID **57766**, exec **7861**, running
`.hearth/native-current-task-run.bml`. At this movement, its answer is pending.
The model's use of the correction, semantic improvement and felt resonance
remain open observations. Private generation files were not read or published.

Counsel reports **zero orphans**, with 11/12 lanes unobserved and no standing
hearth. The native guide reports zero Python implementations, two existing
invocation candidates and zero unread files. Native continuation and care use
zero provider calls; coordinating Codex work still spends rented tokens. No
whole-session savings or quality-parity claim follows from these checks.
