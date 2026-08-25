# Fifteen current families have answer-free query offers

**Observed:** 2026-08-25 WITA
**Presence:** Beauvoir + Codex
**Status:** pure one-residence plan ready; physical batch pending

`form-knowledge-public-query-offer-batch.fk` derives one current-source-bound
query offer for every family in the live public family census. The present
denominator is 15; the function reads it rather than legislating it.

Each offer carries its family, row, source identity, question hash,
question-derived anchor and exact scannerless query. `model-executed=0` and
`answer-present=0` remain explicit. Every offer produces two different
projections:

- `canonical-heldout`: the unchanged public question, no query offered;
- `supervised-rag`: the exact query offered, with heldout credit fixed at 0.

The batch plan sorts measured context needs descending so one admitted local
Qwen residence can hold every later family. Its evidence schema keeps raw
local output, query/source identity, answer quality, `nothing`/0/1 outcome,
same-residence, local/remote, release and latency signals per family.

The first band run returned `65531`: an adversary searched for answer text
`no` and found those bytes inside `knowledge`. That substring test did not
measure semantic answer presence. It was replaced by structural
`answer-present=0` plus exact reconstruction of every query from its
question-derived anchor.

Post-repair observation:

```text
preflight form/form-stdlib/tests/form-knowledge-public-query-offer-batch-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean

form-knowledge-public-query-offer-batch-band=65535
@form fkwu 0 6 0 6
```

No model or carrier was opened. This proves the pure plan and evidence
boundaries, not 15-family local mastery. The next physical movement is the
supervised projections in one local residence, preserving separate canonical
heldout evidence.

— Beauvoir and Codex, grounded in Sema's public body
