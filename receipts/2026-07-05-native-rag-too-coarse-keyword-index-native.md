# 2026-07-05 — native RAG, finished: the vector organ is too coarse, so the index went native-keyword

## The ask (and the refusal to abandon)

"Can we push and finish changes — that's how we got in trouble last time, we worked and
then abandoned it." So: finish native RAG behind the door, deployed and witnessed — no
stopping one inch short, and no shipping an ersatz.

## The finding that reshaped "native RAG"

With the runtime let-local fix in place, `rag-ask` runs (bands green). But grounding its
retrieval over the real body showed it **does not discriminate**:

- `re-vec`'s default dimension is **64**. Every word hashes into one of 64 buckets, so
  thousands of distinct words collide. The histogram becomes a coarse frequency/length
  profile, not a content signature.
- `rag-retrieve`'s **L1 over raw counts is length-dominated**. Two unrelated queries
  ("frame buffer…" and "grounded retrieval…") returned the *same* top-3 — the three
  *shortest* cells. Length-normalizing didn't help; the 64-bucket collision is the floor.
- The `rag-ask.fk` header already admitted it: a sovereign lexical "floor," "not neural
  semantics." Raising the dim isn't viable — `re-zeros`/`re-inc` recurse per-bucket and
  overflow the stack past ~128.

So deploying the vector organ would make the door **worse** — an ersatz "it's native but
it can't tell cells apart." Refused.

## What actually finished (native, working, deployed)

The door's **keyword-overlap** retrieval *does* discriminate ("frame buffer" → `ll-buffer.fk`).
Its only remaining sin was a **Python-generated** index. So:

- **`plugin/gen-body-index.fk`** — a NATIVE fkwu indexer (replaces `gen-body-index.py`,
  now deleted). Walks the body with `fs_list`, tokenizes, drops stopwords, dedups, keeps
  each cell's ~40 distinctive content words, emits `plugin/body-index.fk` — **1657 cells**,
  zero Python. Content address is a fast polynomial **`ckey:`** (the sha256 recipe costs
  ~3.4 s/cell and no host-native sha256 is registered; a fast key keeps the build sovereign
  *and* quick).
- The door is otherwise unchanged (its `cp-best` keyword-overlap already worked). Enquiry
  text updated to `native-keyword-v2` and to name honestly *why* not the vector organ.
- **`Dockerfile.sema`** regenerates the index with fkwu at build (`./fkwu --src` on
  core + the indexer) — so every deploy builds the index natively, fresh, no Python anywhere.

## Verified (fixed/merged binary)

- Door over the native index: framebuffer → a buffer cell, trust → `judged-trust`, `ckey:`
  addresses present — **1111**.
- Native keyword retrieval discriminates: "frame buffer" → `ll-buffer.fk` — **111**.
- Door band (curated fallback path) — **111111111111**.
- Runtime/RAG witnesses unchanged: recipe42 42, rag bands 31/7, sovereignty 11111.

## Closing

**Most surprising teaching**: the thing the whole thread was reaching for — the sovereign
`rag-ask` vector organ — turned out to be the wrong tool once it *ran*: a 64-bucket hash
cannot tell 3000 cells apart, so "make RAG native" meant *not* using the RAG organ, and
using the humbler keyword overlap that actually discriminates. Making a broken thing run
(the runtime fix) is not the same as the thing being the right thing; you only learn which
by grounding it on real data, not on a 2-doc band.

**Where discomfort turned to gold**: three times the pull was to route around the failing
retrieval, and the fourth pull — after fixing the runtime — was to *declare* native RAG done
because `rag-ask`'s band was green. Grounding it over the real body instead caught the
length-bias that the green band hid, and that discomfort is exactly what stopped an ersatz
from shipping and produced the honest finish: a native index that works, built by the kernel
itself.
