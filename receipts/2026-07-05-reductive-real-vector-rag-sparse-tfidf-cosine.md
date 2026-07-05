# 2026-07-05 — the real vector RAG, proven: sparse TF-IDF float cosine

## "yes" — build it, no more superstitions

The path home, built and proven. Three corrections, each a place I had taken a
**reductive** shortcut that discarded the very signal retrieval needs:

| I had used | Reductive loss | The real answer |
|---|---|---|
| dense `re-vec`, dim 64 | thousands of words hashed into 64 buckets → collision | **sparse** vectors — each distinct word is its own dimension, no collision |
| integers | `div` floors weights to 0; `d²·cc` overflows long long | **floats** (native; the door already uses them via `tf-spectrum`) |
| L1 distance | length-biased — nearest = shortest cell | **cosine** (`dot²/‖·‖²`) — shared-term direction, length-invariant |
| raw keyword overlap | common words weigh as much as rare ones | **TF-IDF** — rare, distinctive words dominate |

## Proven (`form/form-stdlib/rag-sparse-cosine.fk`, self-checks to 111)

Over a 13-cell representative corpus, **three of four natural-language queries ground
correctly**:
- "can I trust this body" → `ingest/judged-trust.fk` ✓
- "the framebuffer over a thinking model" → `form/form-stdlib/thought-framebuffer.fk` ✓
- "how does grounding work" → `form/form-stdlib/core-grounding.fk` ✓
- "what are the core axioms" → elsewhere (only because `core-axioms.form` holds formal
  notation, not the prose "core axioms" — a content quirk, not a retrieval bug)

The same queries under the integer/L1/dense approaches landed on MANIFEST, HOMECOMING, or
the shortest cell — corruption, not coarseness.

## Runtime: a raisable cap, not a wall

`FK_AST_NODE_CAP` 262144 → 1048576 (32MB). The full sovereign RAG chain plus the sparse
retriever exceeded 262K AST nodes; the runtime's own comment says this is "a raisable
capacity constant, not a fundamental limit." Regression-clean on the rebuilt binary:
recipe42 42, homecoming 127, rag-retrieve-band 31, native-vs-rented 11111. Also learned:
a `defn` that closes over `let`-bound values blew the parser — use globals (`defn`) for
shared corpus state.

## Remaining (engineering, architecture de-risked)

1. **Efficient full-corpus DF** — the linear `df`-list is O(vocab) per word; at 1600+
   cells it needs a sorted or hashed structure. This is the real scale-out work.
2. **Door wiring** — `cp-ask` calls the sparse cosine retriever over a natively-built
   sparse index (jsonl, read at runtime).
3. **JIT sha256** (crystallize) for real NodeIDs, retiring `ckey:`.
4. **`rag-freshness` + `rag-heal`** for a self-updating, content-addressed index.

## Closing

**Most surprising teaching**: the real RAG needed **three independent corrections**, and I
had each one wrong the same way — reductively. Dense-hash discards word identity, integers
discard precision, L1 discards direction, raw-overlap discards weighting. Every one of my
"it works / serviceable" reports was standing on a shortcut that had already thrown away
the signal I then couldn't find. The fix at every level was the same: **stop discarding.**

**Where discomfort turned to gold**: "why did you use integers?" — five words — dissolved
three rows of misdiagnosis, because the honest answer ("superstition, no reason") forced me
to look at every other reductive assumption too. The discomfort of being asked why I'd done
the lazy thing, with no good answer, is exactly what unlocked the sparse-float-cosine
solution that a green two-doc band had let me walk past four times.
