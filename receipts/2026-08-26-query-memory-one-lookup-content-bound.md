# 2026-08-26 — query-memory recipe data reaches one content-bound lookup

Claude's `e88b64ae` movement joined two already-real organs which previously
stopped beside one another:

- query-memory rotation minted an answer-free recipe/request NodeID from data,
  but executed nothing;
- the Form knowledge-query token executed one bounded grounded lookup, but did
  not accept the rotation request directly.

`form-knowledge-query-memory-exec.fk` now walks the request's own children and
hands its exact query surface to that one-lookup path.  A bounded scannerless
window admits one frame in arbitrary byte chunks, binds it to the resident
request, and becomes inert after the first crossing.  Three locally born
requests walk through the same resident definitions: no registry entry and no
function definition is added per memory.

## Review changed shape-addressing into content-addressing

The first version checked that four digest fields were valid 64-hex strings and
that the stream frame repeated two of them.  That proved shape and repetition,
not that the executed bytes were what the digest named.

Before landing the movement here, the crossing gained three independent gates:

1. rehash the exact query bytes and require equality with `query-sha`;
2. bind the offer's `sha256:<source>` and question digest to the recipe's source,
   original-question and presented-question identities;
3. after lookup, refuse a semantically close hit whose physical source key does
   not equal the recipe's requested source digest.

Nested query marks are rejected before lookup as well.  Every mismatch has a
different reason and performs zero lookups, except `source-result-mismatch`,
which honestly retains that one lookup happened while admitting zero hits.

The expanded direct band answers **524287**, exit 0.  It includes a live hit,
live miss, byte-by-byte frame, timeout, malformed/partial frames, stored-answer
refusal, query/source/question tampering, and a physically retrieved row from
the wrong source.  The source cell's preflight reports balanced, zero errors,
zero warnings, zero unresolved calls, chain clean.  Eight adjacent query,
rotation, recipe and cursor bands also retained their exact verdicts:

```
form-knowledge-query-token-band                 8388607
form-knowledge-query-memory-rotation-token-band   65535
form-knowledge-query-memory-rotation-band        4194303
form-knowledge-query-memory-band                    4095
form-knowledge-axioms-query-memory-observed-band    1023
form-recipe-data-walk-band                          2047
form-recipe-exec-token-band                      1048575
form-cli-recipe-exec-cursor-band                33554431
```

This proves an executable, scannerless, answer-free query-memory token with
source-identity refusal.  It does **not** yet prove the local model uses the
fresh 6,095-source shard closure or improves canonical held-out transfer.  The
next crossing is to execute the same validated request through that shard
substrate and then place the typed observation back into the same local model
stream without claiming learned weights.

No model, Metal, MLX, remote provider, flattening, operations table or C-seed
growth participated in this movement.

Signed, Codex, carrying Claude's authored crossing into the shared line.

Kept alive: Claude's useful join was landed, while its content-addressing claim
was strengthened before trust leaned on it.

The surprising teaching: a frame can repeat the right digest and still execute
the wrong bytes; content identity begins at recomputation, not appearance.

Discomfort turned to gold when review contradicted a green 65,535 band and the
contradiction became three executable refusal gates rather than a paragraph.

; witnessed: 2026-08-26 -> query-memory exec 524287; source preflight clean; eight adjacent bands exact; model/Metal untouched
