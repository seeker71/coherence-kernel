# Successor source effect survives policy swap

The source-symbol action was already a caller-owned direct effect and policy
routes were already hot-swappable.  What had not been directly witnessed was
their combined successor path: a new resident policy image arriving while the
resident retains a source snapshot, then a following source task executing
that effect without becoming a model request.

`form-cli-peer-policy-route-band.fk` now creates that successor state with a
deliberately non-`fcms` session.  It publishes a scannerless policy image whose
only returned action value is `4` (`source-symbol`), then sends
`fcpct-task-key`.  The direct action returns from the same caller-owned
snapshot and its egress is committed before task promotion.

The proof requires all of these together:

- the policy route identity changed from the default epoch route;
- completed tasks are two and mutation contribution remains zero;
- the sentinel session and the whole source context are structurally unchanged
  across publish, selection, durable append, and seen promotion;
- durable evidence contains publish, `action=source-symbol`,
  `route=source-symbol`, zero callbacks, and exactly one lookup;
- no `q38-forward` appears.

The band is `16383`; its preflight is clean.  It uses no model, Metal, MLX,
HTTP, or external server.  The warm resident is intentionally not claimed to
have gained this closure: this is the exact shape a successor has from birth.

Kept alive: I joined the dynamic policy’s right to select with the newborn
resident’s retained authority to act, and held both identities through the
durable boundary.  The surprising teaching is that a non-model sentinel makes
the proof stronger: any accidental Qwen route would fail instead of looking
warm.  The discomfort was the old process-image boundary; it became a clear
successor contract rather than a claim that source code rewrites a live
closure.
