# 2026-08-24 — knowledge integration is five witnesses, not one score

Yes asked us to keep moving until at least 95% of Form knowledge is integrated
into the local LLM, including Form-native knowledge lookups inside the decoded
stream.  The first honest movement was to make “95%” capable of refusing us.

## The live census

`form/form-stdlib/form-knowledge-integration-census.fk` recursively reads fifteen
source families and admits authored `.fk`, `.bml`, `.bmf`, `.form`, and `.md`.
Generated `-xtal.fk` mirrors are excluded so crystallization does not count one
meaning twice.

The final live run found **5,742 current readable sources**, including this
receipt and concurrently arriving sibling cells.  The cross-family seed registry maps **15** of them.  No completed local
model heldout observations were offered.  Its answer was therefore:

| axis | percent |
|---|---:|
| current-source accessibility | 100 |
| grounded NodeID RAG | 0 |
| source-artifact provenance | 0 |
| teaching-prefix exposure | 0 |
| learned weights | 0 |
| repository ready at 95% | **0** |

The source-access mark is honestly bounded: byte count plus first and last byte
proves that the file is readable now and catches ordinary stale/truncated rows.
It is not called a cryptographic signature.  Same-size interior changes belong
to the query organ's current CTOR NodeID and 64-hex source/answer provenance
keys; a later independent source-hash reconciliation can deepen that seam.

## What one integrated heldout concept must carry

An observation counts only when all of these agree:

1. The source is in the current live census and still readable.
2. A concept registry row maps the concept and source family to that path.
3. Train and holdout lineages are distinct; exact overlap, paraphrase-cluster
   overlap, and common-generator overlap are all zero; the holdout is a
   counterexample, cross-operation, novel program, or adversarial boundary.
4. The completed `form-knowledge-query-token` ABI parses the emitted envelope,
   returns a grounded hit, carries valid REF and current CTOR NodeIDs, performs
   one fkwu lookup and one hit, and says `model-executed=0`.
5. The same result carries the source path plus 64-hex source and answer keys.
6. A separate native execution exits 0 and equals the heldout expected value.
7. The reasoning verdict equals its semantic key and cites the same source.
8. Local-model evidence says local, remote calls 0, and carries an output key.
9. The current teaching/query prefix was actually exposed.  Its NodeID and
   byte count are evidence distinct from retrieval.
10. Learned weights are recorded separately.  RAG uses `weights-learned=0`;
    a hybrid tensor claim needs a nonempty adapter/checkpoint key.

The repository verdict additionally requires every census source to have at
least one registered concept, overall source and concept coverage at least 95%,
and every nonempty family independently at least 95%.  A 95%-overall fixture
with one untouched small family is refused.  Fixture reports can exercise the
arithmetic but can never set `repository-ready95=1`; only the internally
recomputed live census has that scope.

## Direct native evidence

No flatten/ops-table validation was used; that runtime gate is being tended by
another sibling.

```text
preflight census cell       balanced, errors 0, unresolved 0
preflight live run          balanced, errors 0, unresolved 0
preflight census band       balanced, errors 0, unresolved 0
preflight fkqt cell         balanced, errors 0, unresolved 0
preflight fkqt band         balanced, errors 0, unresolved 0
fkqt band                   4194303, exit 0
census band                 1048575, exit 0
live census                 5742 sources; 15 registered; repository-ready95=0
```

The census band admits a real-source 19/20 fixture at exactly 95% and rejects
18/20.  It also rejects exact leakage, paraphrase leakage, a common generator,
an invalid query envelope, missing provenance, retrieval-path mismatch, native
execution failure, wrong reasoning, absent prefix exposure, registry gaps,
duplicate-output inflation, and the family blind spot.  A hybrid fixture proves
learned-weight count is separate (1/20, 5%) while the RAG fixture remains 0%.

## The honest floor

This is the ruler and the executable query membrane, not the 95% integration.
The next owed work is to generate the full concept/source registry, create
independently authored executable holdouts per concept family, expose the query
protocol in the actual local-model prefix, collect real fkqt results during
decode, and only then decide where RAG/overlay is sufficient and where a local
adapter earns weight learning.

I kept the crossing alive by joining the sibling's completed typed query result
instead of minting a competing token language.  The surprising teaching was
that 100% source accessibility can coexist honestly with 0% model integration;
the files being local is not the model knowing them.  The discomfort turned to
gold when the first “95%” fixture returned only 90%: one invented grammar path
did not exist, and the current-source gate refused the toy row until the path
was real.

## Continuation — the current source is a retrieval lane, not a learned mind

A sibling brought `form-knowledge-source-search.fk`: bounded byte-window search
over the live source tree with a Form-streamed SHA-256, returning schema
`source-artifact-stream-v1`.  The census now recognizes that result without
calling it NodeID RAG.  Admission recomputes the selected file's full SHA-256
through bounded windows, verifies the current path, requires all eligible files
in the search root to be accessible, and re-reads only the expected answer span
(at most 768 bytes) to reject answer substitution.  It never materializes the
whole source and depends on no flattened operations table.

The new source-retrieval observation carries no local-model, teaching-prefix,
reasoning-verdict, REF/CTOR, or weight fields.  Consequently a valid source hit
increments only current-source retrieval/hash coverage.  It cannot increment
model reasoning or learned weights, and fixture scope still cannot set the
repository-ready bit.

Focused evidence:

```text
source-search band           65535, exit 0
extended census band         1048575, exit 0
live current source probe    bootstrap/ground.fk
Form SHA-256                 af2630a7b4b22c081165393bfe082f8836a19db82c1a48351c95f49427d6ca07
fresh path/hash/answer       1
heldout source retrieval     0 / 5742
repository-ready95           0
```

The extended band rejects a forged SHA-256 and a substituted bounded answer,
then proves that one admitted direct-source retrieval yields 1/20 retrieval and
hash coverage while full reasoning and learned-weight counts remain zero.

One scope seam remains visible: the census includes four `.bmf` authorities,
while the current source-search organ admits `.grammar` but not `.bmf` (and this
tree currently has zero `.grammar` files in census roots).  Those four sources
therefore cannot earn this fallback's retrieval bit today; the denominator does
not hide them.  NodeID RAG or a future source-search extension must carry them.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-24 -> fkqt 4194303; source-search 65535; census 1048575; live repository-ready95 0
