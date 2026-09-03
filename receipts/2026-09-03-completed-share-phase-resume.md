# Completed share resumes its selected phase

**Movement:** the completed-turn meter now resumes a physically valid
`locating-start` or `collecting` cursor before searching the append tail for a
terminal again.  An interrupted old state can no longer leave terminal
discovery and start discovery competing for the same published health slot.

The stone was visible in the live instrument: refresh said `discovering` at a
newer cursor while health said `locating-start` for an older selected turn.
The selected start row already carried the terminal coordinate, task id,
carrier identity, provider, and frozen size; throwing those away on every
invocation made the body repeat work and let two phases coexist.

The repair keeps each invocation bounded to one 2 MiB evidence movement:

- progress resumes only when its retained start row still validates against
  the bound carrier and the start/terminal task coordinates are physically
  present;
- unresolved or resolved start discovery resumes directly from its durable
  cursor;
- invalid/superseded phase state is cleared before fresh discovery, while a
  deliberately bound historical target remains explicit;
- live health ignores a stale phase or candidate when a newer physically valid
  continuation exists;
- explicit target rebinding clears every incompatible continuation row.

## Witness

- preflight, cursor band: balanced, 0 errors, 0 warnings, 0 unresolved;
- `form-cli-turn-evidence-cursor-band.fk` -> **33554431**, exit 0;
- `form-cli-share-health-band.fk` -> **16383**, exit 0;
- `form-cli-turn-evidence-live-band.fk` -> **16778238**, exit 0;
- evidence ABI **65535**, remote-call evidence **268435455**, Grok executable
  BML **2047**, and Glass publication **65535**, all exit 0;
- bounded framebuffer exchange: 4 correlated events, split absent after the
  actuator/re-observation window, and no rollout content emitted;
- live phase sequence stayed aligned from `locating-start` through
  `collecting` and `validating-latest`.

Final review also closed two attribution boundaries: provider usage now enters
only through a structurally parsed top-level `event_msg` carrying an admitted
`token_count` or `provider_usage` payload, and completion requires its typed
terminal payload. The Form JSON walk consumes the complete row, ignores key
order, and distinguishes direct payload fields from nested objects and escaped
string content. Current `payload.info.last_token_usage` and the historical
direct payload position are the only admitted usage locations; there is no
recursive descendant search. Nested tool output shaped like usage/completion
is rejected. Each invocation
reads at most one complete-line 2 MiB slice; an oversized physical JSONL row,
pending-call overflow, oversized call identity, or oversized persisted progress
withholds reconciliation instead of growing memory without a bound.  Completed
call identities retire as soon as their output is reconciled.

The final live row became `kind=observed` only after all §8 boundaries
reconciled: 1,372 events, native/local/remote **135/576/661**, normalized event
share **10/42/48**, source-bound identity and timestamps, start byte
183331350, complete byte 213490789, call-local provider usage, tool outputs,
Form receipt bytes, lanes, and latest-complete validation.  Semantic
contribution remains explicitly outside this meter.

The self-watch counsel read `hopper=0`, `fails=0`, `errs=0`, `kvpct=34`; its
worst current lane was `p95=50505`, not evidence reconciliation.  This change
therefore removed repeated evidence walking without claiming a wider latency
repair.

The surprising teaching was that the durable cursor was not missing; the body
was faithfully saving it and then declining to trust its own physically bound
state on the next breath.  The discomfort was seeing a freshly reconciled row
expire when a newer sibling turn completed during validation.  That became
gold because the meter refused the tempting percentage, selected the newer
completion, traversed it, and only then spoke an observed share.

— Codex (Sol), 2026-09-03
