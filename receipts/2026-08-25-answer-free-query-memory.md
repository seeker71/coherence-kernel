# Answer-free query memory — one reversible local promotion

**Observed:** 2026-08-25 WITA
**Presence:** Codex
**Status:** one supervised RAG route persisted; rotated heldout still owed

The strengthened axioms query rehearsal returned an exact source-bound query,
one current-source hit, a `1000000` ppm answer, exact source-artifact equality,
and both releases. `form-knowledge-query-memory.fk` can now promote that route
as a content-addressed local memory.

The memory contains the offer NodeID, query bytes, current source identity,
question hash, raw evidence hash, score, release observations and an immutable
rollback child. It contains no answer bytes. The first memory rolls back to an
explicit answer-free `nothing` alternative; a successor points to its prior
memory NodeID. A changed question or changed source identity makes selection
return exact `nothing`.

`form-knowledge-axioms-query-memory-observed.fk` persists the one physically
observed route from
`receipts/2026-08-25-axioms-query-token-rag-transfer.md`. It keeps both
`heldout-credit=0` and `weights-trained=0`. Reusing a query derived from this
already-seen question improves operational local recall but cannot earn
heldout credit on that question. Generalization must be observed on a rotated
unseen question.

## Witness

```text
preflight form/form-stdlib/tests/form-knowledge-query-memory-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean
form-knowledge-query-memory-band=4095, exit 0

preflight form/form-stdlib/tests/form-knowledge-axioms-query-memory-observed-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean
form-knowledge-axioms-query-memory-observed-band=1023, exit 0
```

This closes the mechanism and the first persisted supervised memory. It does
not yet close the health map's adaptive-memory organ, whose next evidence is a
rotated heldout improvement followed by keep-or-undo.

— Codex, grounded in Sema's public body
