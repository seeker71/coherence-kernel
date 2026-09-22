# Native recovery of a retained coding reply

Signed: Codex, 2026-09-22.

The preceding goal movement was progress: it landed actual process-status
checking and an explicit missing-source signal. This movement repairs another
observed cause from the same native run, without another model admission.

## The actual before and after

Qwen's retained 856-byte edit reply had an extra `}` at byte 842, where the
arguments array expected a comma or closing bracket. The owned checkpoint's
feedback was only “Malformed response. Return exactly one JSON object; no
markdown or commentary.” The model repeated the malformed edit and eventually
exhausted its context.

The existing validator now retains the precise failing byte and expectation.
For typed tool commands, the coding organ can remove one mismatched closer
immediately before the expected closer, preserving every other byte and
requiring strict admission of the whole candidate. It records the correction
and runs the command through the ordinary tool restrictions and caller checks.

The **unchanged retained reply**, against its original resident documents,
now produced **one guarded edit in one coding step, with zero new model calls**.
The native receipt identifies the removed byte and the before/after hashes.
The model's original bytes remain unchanged in the earlier receipt. This is
native grammar recovery implemented by Codex, not a newly generated model answer.

I reused the original native caller-checker definitions from the retained job
and ran them on that resulting candidate. Compilation and the seven predicate
cases passed, but the original checker still returned **passed=0**: the old
output-only landing decision was still present. The recovery exposed the next
real implementation gap without granting task completion. The production
landing repair from the preceding movement remains separate.

## The signal belongs to the coding organ

At the live reply boundary, `organ-health.bml` carries an unhealthy syntax
reading asking for `valid-json-response`, the applied delimiter repair, and a
fresh correlated reading. Re-observation validates the candidate again and
matches both source hashes. Its health is about JSON syntax only.

Other syntax failures retain their byte-level diagnosis. An unrecoverable
version of the same reply remained unchanged through two steps: two counted
failures, zero tool calls and zero checks. A subsequent valid reply clears the
syntax need. Ordinary valid replies emit no syntax-health rows. No source or
answer text enters those health rows.

The repair record survives checkpoint serialization and appears in the caller's
result. Read-only mode continues to reject edits. Non-tool reports and malformed
string escapes are not rewritten. Original caller-check failures still prevent
completion. All new runtime meaning is Form/BML; the C seed is unchanged.

## Verification and cost

Native syntax witness: **1**; coding policy: **65535**; coding request: **255**;
resident tool edges: **131071**; generation JSON boundary: **1**; JSON presence:
**1**; JSON codec: **8191**. These checks cover syntax positions, existing JSON
admission, the retained native reply, authority, checkpoint retention and
completion refusal. Each exited zero. Drift gates passed **8191/8191**, exit 0.

The previous completed coordinating turn cost **6,395,579 rented tokens**:
6,317,604 input including 6,138,368 cached input, 52,037 output, and 25,938
unattributed tokens. Its 42 tool calls and outputs reconciled. This is completed
turn `01a0c84a-1032-7fa2-8b40-c30b1b9a506d`, not this open movement. Different
work was done across the turns; the lower total alone establishes no efficiency
or quality improvement.

Glass startup panel: **32 ms**. Counsel: **0 orphans**, with **11/12 lanes
unobserved** because no hearth stands. The output-only session meter read
**4,036,112**. Native authoring guide: zero Python implementations, two existing
invocation candidates in `voice-say.bml`, zero unread sources. The share reader
reported `kind=declared` while checking the append range; no percentage is claimed.

The recovered step required zero model calls. Full native task completion,
response resonance, context supply during generation and session-wide parity
remain unproven. The current repair carries a specific recurring format failure
into a native action whose effect and limits are visible.

Evidence: [same-reply observation](artifacts/2026-09-22-native-json-attention/observation.json),
[syntax health](artifacts/2026-09-22-native-json-attention/syntax-health.json),
[unchanged original checker](artifacts/2026-09-22-native-json-attention/original-checker.json),
[completed-turn cost](artifacts/2026-09-22-native-json-attention/preceding-coordinator-cost.json).
