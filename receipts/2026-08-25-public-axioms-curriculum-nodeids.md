# 2026-08-25 — public axioms became resident teaching packets

The axioms lane now has a real public-source teaching organ, not only a
mastery finalizer.  `form-knowledge-axioms-public-curriculum.fk` derives
fifteen concepts from current public body sources and offers five presentations
of each: direct, paraphrase, corner case, source-bound scannerless query, and
explicit-path scannerless query.  That is 75 addressable lessons in the current
organ; it is a living public-source projection, not a fixed target.

Each lesson is interned under category `31.2.0.120` from its exact concept,
variation, prompt, response, source path, current streamed source SHA-256, and
search anchor.  The resident packet is category `31.2.0.121`.  Its correlated
nothing alternative is category `31.2.0.122`.  Invalid concepts, variants,
unreadable sources, stale reconstruction, or tampered outer records return
exact `nothing`; the alternative node exists only inside a valid packet so a
controller can retain the refused route's identity without replacing nothing.

The callable adapter is:

```form
(fkap-source-bound-hook concept variant)
```

It returns the content-addressed lesson and packet NodeIDs, the current source
SHA, both scannerless query surfaces, exact public teaching prompt/response,
and literal `model-executed=0`, `weights-trained=0`.  It performs no model,
Metal, MLX, network, or weight-update work.

The direct-source `states` observation in this checkout was:

```text
lesson-nodeid=0.2.0.10
lesson-category=31.2.0.120
packet-nodeid=0.2.0.20
packet-category=31.2.0.121
nothing-alternative-nodeid=0.2.0.15
nothing-alternative-category=31.2.0.122
source-id=sha256:893d1a85f5a9475705e59f1bac50b9b19076e2fafb55751b52897af3a6d5bcad
source-bound-query=<|form:knowledge-query|>axiom-1-states<|/form:knowledge-query|>
path-query=<|form:knowledge-query|>axiom-1-states ; axioms/core-axioms.form<|/form:knowledge-query|>
model-executed=0
weights-trained=0
packet-valid=1
```

The `0.2.0.*` identities above are the observed interned NodeIDs in that direct
process.  The fixed category plus exact content children are the reconstructible
identity; a source or lesson-byte change intentionally births a new NodeID.

## Proof

- curriculum source fresh preflight: balanced, errors `0`, warnings `0`,
  unresolved `0`, exit `0`;
- adversarial band fresh preflight: balanced, errors `0`, warnings `0`,
  unresolved `0`, exit `0`;
- direct adversarial band: expected `2097151`, observed `2097151`, exit `0`;
- direct adapter renderer: `status=hit`, all three category identities rendered,
  `packet-valid=1`, `model-executed=0`, `weights-trained=0`, exit `0`;
- canonical v3 redacted manifest: rows `30`, families `15`, coverage/current/
  unique `1`, all leakage aggregates `1..20 = 0`, current dataset SHA equals
  sealed dataset SHA, `dataset-valid=1`, exit `0`.

The adversarial band reconstructs all 75 lesson nodes and all 75 packets from
live source hashes.  It also rejects unknown concepts, invalid variants,
tampered response/path/source fields, checks the three category boundaries,
keeps source-bound and explicit-path query grammars distinct, and pins the
corner cases where choice returns present `0`, cut commits even to nothing,
undo restores exact state, timeout differs from exhaustion, missing thought
evidence abstains, a JIT plan is not carrier execution, and a current census is
not a fixed mastery target.

## Leakage and integration boundary

The v3 manifest exposes only redacted counts and identities.  Its zero counts
prove that the already registered teach/v1/recipe surfaces remain disjoint from
the sealed evaluator.  This new hook was deliberately not inserted into the
concurrently active standard publisher or teach-layer arrays in this movement,
so the old aggregate does **not** pre-clear these 75 new public lessons.  When a
resident source-bound caller registers the hook, the redacted audit must be run
again; a nonzero aggregate means the heldout must be renewed from another public
corner rather than withholding knowledge from the local mind.

The private consent artifact was never opened.  One early broad API-name search
accidentally emitted one evaluator-row line before the search was narrowed to
definition names only.  That content was not copied, used, or rendered in any
artifact here.  The subsequent audit used only the canonical redacted manifest.

No source list, flattening path, operations table, model carrier, or private
evaluation artifact was edited.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-25 -> public curriculum 75; band 2097151; direct packet valid 1; v3 redacted dataset-valid 1
