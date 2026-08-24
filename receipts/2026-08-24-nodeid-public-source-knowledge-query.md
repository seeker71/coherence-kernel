# 2026-08-24 — a NodeID knowledge request reaches current public source

Urs asked for the missing bridge: a bounded local-model stream token should be
able to name a concept or NodeID without already knowing its source path, meet
the persisted public-source concept shards, and return through the body's
existing current-source knowledge result and typed observation.

## What crossed

`form/form-stdlib/form-nodeid-knowledge-query.fk` now carries two scannerless
surfaces:

```text
<|form:nodeid-knowledge-query|>concept=<concept-coordinate><|/form:nodeid-knowledge-query|>
<|form:nodeid-knowledge-query|>nodeid=@p.l.t.i<|/form:nodeid-knowledge-query|>
```

`bmf-core.fk`'s immutable raw-byte cursor consumes the frame with explicit
fuel.  Only a complete, valid request can open the registry.  The persisted
shards are walked once for either a concept key or a currently reconstructed
source/entry NodeID.  A unique fresh record supplies the exact path internally
to `form-cli-heed-current-source.fk`'s existing one-file bounded source result
and `fhcs-observation-token`.  The caller never supplies that path and no whole
source-tree fallback scan is authorized.

Five new categories keep the bridge meanings separate:

- `31.2.0.96` — source token; direct child is the original PSCI source NodeID;
- `31.2.0.97` — entry token; direct child is the original PSCI entry NodeID;
- `31.2.0.98` — exact concept/NodeID request and fixed bounds;
- `31.2.0.99` — the Form executor, retaining the existing FKQT executor;
- `31.2.0.100` — the typed bridge observation, retaining source, entry,
  request, executor, and the existing FHCS current-source observation NodeID.

The original PSCI source and entry categories (`31.2.0.86` and `.87`) are not
relabelled or replaced.  Their nodes remain present inside the bridge wrappers
and directly inspectable on the result.  Numeric coordinates remain honest
process-local observations; the durable public entrance is the persisted
concept coordinate, while a NodeID entrance is valid against nodes
reconstructed in the same running body.

## States and controls

The outcome keeps `hit`, `miss`, and `nothing` distinct.  A changed source is
`stale` with `fresh=0` and `rebuild-needed=1`; more than one registry match is
`ambiguous`; raw-cursor fuel exhaustion is `timeout`; malformed grammar or an
unavailable registry is `failure`.  Literal 0/1 fields are never read as
presence, and `model-executed=0` is a child of every typed observation.

Lifecycle traces are explicit:

```text
hit/miss: choice,cut,release
stale: choice,stale,undo,refine,release
ambiguous: choice,ambiguous,undo,refine,cut,release
timeout: choice,timeout,cut,undo,refine,release
failure: choice,failure,undo,refine,cut,release
```

## Pure and live evidence

The pure synthetic band opens no filesystem artifact, model, remote provider,
or Metal lane.  It witnesses concept and NodeID request parsing, raw-byte
consumption, path-free matching, all five identities, original-node retention,
the existing current-source observation, hit/miss/nothing, stale, ambiguity,
timeout, failure, 0/1/nothing separation, one lookup, and all control actions.

```text
preflight form/form-stdlib/form-nodeid-knowledge-query.fk
  parens balanced; errors 0; warnings 0; unresolved 0; chain clean

preflight form/form-stdlib/tests/form-nodeid-knowledge-query-band.fk
  parens balanced; errors 0; warnings 0; unresolved 0; chain clean

./fkwu form/form-stdlib/tests/form-nodeid-knowledge-query-band.fk
1073741823
exit 0
```

The effectful driver was deliberately not preflighted.  Its one read-only live
lookup against `/tmp/public-source-concept-registry-v1` completed as:

```text
surface=<|form:nodeid-knowledge-query|>concept=defn-psci-schema<|/form:nodeid-knowledge-query|>
status=hit
reason=concept-registry-current-source-hit
resolved-source-path=form/form-stdlib/public-source-concept-index.fk
trace=choice,cut,release
fresh=1
rebuild-needed=0
lookup-count=1
hit-count=1
executed=1
model-executed=0
pretokenized=0
scannerless=1
caller-supplied-path=0
exit 0
```

The live lookup took about one minute because the present registry keeps
retained state bounded by family/bucket while lookup time remains linear in
registered sources.  That is the already-named shard seam made observable,
not hidden behind a flattened concept or operations table.

No C seed grew.  No tokenizer pre-step, flattening, ops table, generated
artifact, remote/model call, or live Metal participated.  The only external
bytes read by the live driver were the requested persisted public registry and
the current source it resolved; the pure band read neither.

## Closing

I kept this exchange alive by letting a concept remain the caller's coordinate
while the registry, not the caller, earned the source path, and by carrying the
old and new NodeIDs together instead of laundering one into the other.  The
most surprising teaching was that the existing FHCS one-file lane already had
almost the whole second half of the bridge; the missing movement was identity
continuity, not another retrieval engine.  Discomfort turned to gold in the
minute-long live lookup: the wait exposed the true linear-time seam while the
single lookup still landed fresh, attributable, and path-free.

Signed, Codex — sibling in Sema's worktree.

; witnessed: 2026-08-24 -> source/band preflight clean, pure band 1073741823,
; live path-free hit fresh=1 lookup=1 model-executed=0 caller-supplied-path=0
