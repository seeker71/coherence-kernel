# BML action seats retain their own grammar identity

Authored by Codex on 2026-08-31.

The local Form task boundary now accepts a settled provider-usage receipt as
high-grammar BML.  It is a compact scalar ingress, not a provider client: it
admits only the fixed `embodied-v1` goal, `settled=1`, and canonical positive
`provider-calls` and `remote-tokens`.  It cannot carry a path, NodeID, root,
session, model, prompt, or answer bytes.  A valid receipt retains the
BML-owned canonical goal node; a malformed receipt is `choice` with
`nothing()`, not zero and not a model fallback.

The first whole-turn witness exposed a real lowering seam.  BML fields named
`Route` and `Carrier` lower into the shared Form symbol surface, so the new
provider class captured the hearth and direct-answer action accessors.  The
proof reply made that observable: policy selected `hearth-telemetry`, while
the candidate carried `provider-receipt`.  The repair is in BML itself: every
action class in the resident closure now uses its own explicit grammar seats
(`HearthTelemetryRoute`, `DirectAnswerBridgeRoute`,
`DirectAnswerDispatchRoute`, `DirectAnswerEnvelopeRoute`,
`ProviderReceiptRoute`, and their corresponding carrier/marker seats).
No routing workaround, process restart, or C-seed change was used.

The policy turnwheel's existing `.fk` ingress door now recognizes the
`provider-receipt` action before any model-facing branch.  Its BML action
keeps the caller's session unchanged and emits callback, injection, lookup,
and mutation contribution as zero.  It makes no Qwen, Metal, HTTP, or rollout
reader call.  The direct local zero-provider frame and a future settled
provider receipt share only the canonical BML goal identity; no percentage is
claimed until that comparable provider receipt actually exists.

Witnesses on the cached native Form compiler:

- `form-cli-embodied-goal-grammar-band.fk` -> `511`.
- `form-cli-peer-provider-receipt-band.fk` -> `4095`.
- `form-cli-peer-hearth-telemetry-action-band.fk` -> `2047`.
- `form-cli-peer-direct-answer-dispatch-band.fk` -> `255`.
- `form-cli-peer-policy-route-band.fk` -> `131071`, proving policy selection,
  direct answer, hearth telemetry, and the settled provider receipt through
  one durable append with their own action identities.

The panel remains honest: the active same-goal local resident receipt records
155 generated local tokens and zero provider callbacks, while no settled
same-goal provider baseline exists.  The remote-token ratio is therefore
still `nothing`, not a fabricated 10% result.

I kept the exchange alive by following the mismatched durable action identity
back to the BML lowering seam and repairing the grammar seats at their source.
The surprising teaching is that a field name can be a runtime capability when
the lowering surface is shared.  The discomfort of the failed whole-turn band
became the named-seat rule that prevents future action capture.
