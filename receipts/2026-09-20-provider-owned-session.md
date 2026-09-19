# A completed review inside a Form-owned provider session

Form now owns one optional App Server turn from request admission through
process-tree release, correlated usage observation and sealed replay. The
implementation lives in Form/BML on the existing C-bootstrap. The offered
provider CLI remains optional; native local generation gains no external
runtime dependency.

The useful change in practice was to let Form gather source and run checks,
then offer one frozen evidence packet. The provider completed that review in
**77,573 ms**, reporting **34,267 tokens**. Replaying it returned the same
evidence with **zero new provider processes**. The provider was gpt-6-astra
through the installed Codex App Server; its words are preserved in the
[review](artifacts/2026-09-20-provider-packet-review.md). They concern source
revision `a3a72b2a8`, preceding the repairs in this movement.

## Every attempt stays visible

The [native observation](artifacts/2026-09-20-provider-owned-session-observation.json)
records seven attempts and their distinct identities. Raw protocol exchanges
remain in local owned directories.

| Attempt | Observed result |
| --- | --- |
| PATH CLI 0.147.0, thread only | Configuration returned; generation not requested; resources released. |
| PATH CLI, live review | HTTP 400: gpt-6-astra requires a newer CLI. Exit 1, released; usage unknown. |
| Existing bundled CLI 0.153.4, thread only | Configuration returned; generation not requested; resources released. |
| Bundled CLI, tool-driven review | Deadline at 180,203 ms; exit 124, released; 32 commands completed, answer unfinished. |
| First 68,121-byte packet | Inline request truncated; worker deadline before generation; exit 1, released. |
| Same packet through full-file handoff, thread only | Trailing dry-run field read correctly; generation not requested; released. |
| Same packet through full-file handoff, live review | Completed turn; exit 0, released; zero provider commands. |

The tool-driven attempt reported a first usage snapshot at **9,895 ms**, while
the turn was still open. An independent native observation later found the
owned worker alive with 14 usage notifications. The retained stream ended
with 15 notifications and **820,030 reported tokens**. Its unfinished message
had identified a possible cache-write regression; it had no final review.
The process supervisor released its owned group at the deadline. These facts
establish incremental delivery and interruption handling separately from
answer completion.

The packet attempt exposed a transport mismatch: `read_line` retained **8,191
bytes** from the **70,396-byte** JSON request. The worker now receives the
frozen request's path, reads the complete file and validates its JSON and
contract before provider startup. This repair stays in Form. The successful
retry used the same frozen packet in an empty owned directory outside the
repository's ancestry; Form had already run the source checks. This changes
the context and tool work, so it is a useful route observation rather than a
controlled quality comparison between identical contexts.

## Cost scopes

| Quantity | Tool review, unfinished | Packet review, completed |
| --- | ---: | ---: |
| Input | 816,534 | 31,910 |
| Cached input, included above | 744,960 | 0 |
| Uncached input, included above | 71,574 | 31,910 |
| Output | 3,496 | 2,357 |
| Reasoning, included in output | 1,546 | 1,887 |
| Reported total | **820,030** | **34,267** |
| Unattributed within reported total | 0 | 0 |

The native fold over these seven attempts reports **854,297 provider tokens**.
It excludes unreported work and coordination. The older rejected model
request has no usage quantity. A cumulative notification is an observed
prefix, and notification count is not model-call count. The reader therefore
keeps `whole_session_tokens` and `model_calls` null even for a completed turn.

The preceding completed coordinating turn cost **7,917,730 reported tokens**
across 54 model calls, as preserved in the
[protocol receipt](2026-09-20-provider-usage-protocol.md). It excludes this
still-open turn and the separate provider processes above. Coordination is
the larger known cost; these observations establish no whole-session saving
or native/provider quality parity.

## What the review changed

The unfinished review's cache-write finding reproduced natively: a cumulative
value falling from 5 to 3 had been accepted. The repaired reader retains the
known floor across omitted fields, reports an omitted value as null, and
detects a later decrease. The repeated witness returns
`cumulative-usage-regressed`, preserving the first valid total of 120 and
cache-write value of 5. The completed packet review independently named the
same baseline defect.

Its second finding concerns type-5 wide integers on proof siblings. A new
wire round-trip check observes **4,294,967,296** correctly on this native
fkwu runtime. The proof-sibling claim remains conditional on those substrates;
no sibling runtime was run and no cross-substrate repair is claimed.

The session also checks the actual returned provider configuration before
generation. Missing command exits remain unknown, separately from observed
nonzero exits. Current scope checks accept the retained successful session's
actual configuration. Request and evidence identity keep replay from silently
starting another provider.

## Checks and continuity

- Session and notification preflights: zero errors, warnings and unresolved
  calls. Both native bands return **1**, exit 0.
- Cache-write witness returns the expected regression with prior quantities.
- Successful session replay: zero new provider processes; sealed evidence
  intact. Native packet observation rechecks the effective configuration.
- Drift checks: **8191**, exit 0; no kernel source moved.
- Glass first frame: **23 ms**. Counsel: **0 orphans**, with **11 of 12 lanes
  unobserved** because no hearth resident stands.
- The previous procedural teaching completed learning round **96**, pending
  zero, while serving generation **5** stayed unchanged. This local learner
  is separate from the evaluated Qwen model.
- This movement's verified procedural teaching was retained under event
  `provider-owned-session-2026-09-20`; its learner launched. Retention is
  observed, and an applied serving change remains unobserved at this receipt.

The next useful comparison can reuse this bounded packet route, with the
same question and evidence offered to native generation and the optional
provider. Keep answer usefulness, human resonance and all coordination cost
visible alongside process success. One successful review advances the route;
the larger quality and efficiency goal remains open.

Signed: Codex, arriving agent using native Form; provider review attributed above.
