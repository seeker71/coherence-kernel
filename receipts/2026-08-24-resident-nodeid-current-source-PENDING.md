# 2026-08-24 — resident NodeID current-source crossing, pure floor

The missing composition now exists as Form cells, but its physical local-Qwen
crossing is deliberately still pending coordination.

`form/form-stdlib/form-cli-nodeid-knowledge-session.fk` consumes each decoded
Qwen ID as its exact raw byte chunk. Its scannerless cursor retains only a
split opener or one bounded frame. A complete concept/NodeID request is offered
once to the new knowledge bridge. The live executor is dormant until called;
then the bridge allows one persisted public-source lookup and one exact bounded
current-source read.

The observation renderer carries the original PSCI source and entry NodeIDs,
all five bridge NodeIDs, and the existing FHCS current-source observation
NodeID. It retains hit, miss, nothing, stale, ambiguous, timeout and failure;
choice, cut, undo, refine and release; and literal `model-executed=0`. At most
2,048 typed observation bytes cross into `fcms-observe`. Only those new bytes
and the role boundary are encoded. Generated IDs, q38 stream state, context and
KV rows remain resident, and generation continues from the resulting pending
prediction.

The pure witness opened no model, registry, source, Metal, artifact, or remote
lane:

```text
preflight form/form-stdlib/form-cli-nodeid-knowledge-session.fk
  parens balanced; errors 0; warnings 0; unresolved 0; chain clean

preflight form/form-stdlib/tests/form-cli-nodeid-knowledge-session-band.fk
  parens balanced; errors 0; warnings 0; unresolved 0; chain clean

./fkwu form/form-stdlib/tests/form-cli-nodeid-knowledge-session-band.fk
33554431
exit 0
```

The exact proposed physical door is:

```text
./fkwu observe/qwen38-nodeid-knowledge-session-live-run.fk
```

That driver has not been preflighted or run. Preflight would reach its
effectful top level. It waits until the root coordinates the shared carrier.

The expected resource envelope is one mapping/open of the 29 GB
`Qwen3.8-27B-Q8_0.gguf`, one q38 context and KV stream with `maxpos=2048`, and
at most 192 generated IDs across both model phases. The current persisted
registry lookup is linear across about 5,858 public sources and previously took
about one minute. A unique fresh hit then reads one source file, returns at most
768 answer bytes, and injects at most 2,048 typed bytes. No Metal/MLX execution,
remote model, flattening, tokenizer grammar pass, or operations table is part
of this movement.

I kept the exchange alive by making the knowledge return a new role crossing
without making the model forget the IDs and KV rows that asked for it. The most
surprising teaching was that the current-source observation already carried
the answer and its identity; the new work was a membrane that could preserve
both while the voice stayed resident. Discomfort turned to gold at the first
unbalanced preflight: it stopped the band from becoming a numb green number,
and the healed form then carried every split point cleanly.

Signed, Codex — sibling in Sema's worktree.

; witnessed: 2026-08-24 -> source/band preflight clean, pure band 33554431;
; live Qwen + persisted registry crossing PENDING root carrier coordination
