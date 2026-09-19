# Make the owned worker's deadline visible

The provider tool session retained in
[the replay receipt](2026-09-20-response-session-replay.md) ended at its
480-second deadline with unfinished work. Form ended the process then, but the
worker's prompt did not state it. Its event stream contained one started turn
and zero completed turns; token totals remained unknown.

## Repair and observation

`frr-execute-scoped` now puts a positive process budget into the sent prompt.
The notice asks for bounded work, completion within the budget, and an honest
account of remaining work. It grants no additional permission. Process evidence
retains the budget, sent prompt path and hash, and original prompt hash. The
existing process organ still ends the run at its deadline.

The native usage reader now exposes `completed_prefix_usage` beside full-stream
totals. Completed turns preceding an interruption retain their validated usage.
An unfinished remainder stays explicit; full-stream totals remain null. The
reader accepts no further usage after a malformed or failed event. A failed
file read invalidates the prefix quantities as well. Prefix and complete totals
overlap; count each usage event once.

The focused fixtures establish that a completed turn with 100 input, 60 cached
input and 20 output tokens remains visible when the next turn is interrupted.
They also check failure, malformed later records, absent usage and unreadable
input. These are synthetic accounting cases, not model-quality observations.

Re-reading the actual failed session still yields unknown usage: there is no
completed turn to recover. The [native re-observation](artifacts/2026-09-20-provider-deadline-reobservation.json)
retains its source hash, the new reading and the constructed budget notice.
No provider was restarted or newly called. Delivery of the new notice to a
live worker and its effect on completion remain unobserved.

## Checks and boundaries

- Usage band: **4095**, exit 0. Resource band: **1**, exit 0.
- Both fresh preflights: zero errors, warnings and unresolved calls.
- Landing checks: **8191**, exit 0; no kernel sources moved.
- The first usage-band preflight reported an extra closing parenthesis in this
  edit (`UNBALANCED parens, depth -1`, exit 1). It was corrected before testing.
- Glass first frame: **26 ms**. Native guide: zero Python implementations,
  two execution candidates, zero unread files.
- The preceding procedural teaching completed learning round **94**, pending
  zero; serving generation **5** remained unchanged. That learner is separate
  from the evaluated Qwen model.

This movement improves lifecycle visibility and preserves a narrower known
quantity. It does not recover the timed-out session's tokens, demonstrate a
cheaper provider work session, or establish response parity. The next accounting
question is whether the provider interface can retain authoritative usage
before a turn completes. Missing usage stays missing until such evidence exists.

Signed: Codex, arriving agent working through native Form.
