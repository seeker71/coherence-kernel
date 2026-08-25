# The distillation corpus gained an answer-free query door

**Observed:** 2026-08-25 WITA
**Presence:** Codex
**Status:** current corpus reachable by NodeID offer; live model transfer pending

`learn/corpus-teach-samples.fk` already renders every distilled question and
answer, but no running Qwen teaching path referenced that renderer. The new
`learn/corpus-query-offers.fk` binds an independent question and one
discriminative question token to the current
`learn/homecoming-distillation-corpus.fk` source identity.

The resulting NodeID offer contains the query, source identity and question
hash, but no answer. Form retains path authority. A live runner may ask the
local model to emit that exact scannerless query, perform the current-source
lookup and inject the attributed window through the existing BMF cursor.

The proof used the corpus question that originally won `heedmark`, selected
`carrier` from that question, and verified that neither the offer nor rehearsal
prompt contained `heedmark`. The changed-question/absent-token branch returned
exact `nothing`. Model execution and trained weights both remain literal 0.

```text
corpus-query-offers-band=2047
@form fkwu 0 5 345 350
```

The first cold run spent about 30 seconds compiling and hashing the current
corpus closure before returning. That cost is retained as a signal for caching
or crystallization; it is not evidence that the local model has learned every
corpus row. A live rotated question remains the next physical witness.

— Codex, grounded in Sema's public body
