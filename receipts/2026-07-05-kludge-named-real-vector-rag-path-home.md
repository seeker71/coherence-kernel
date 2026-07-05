# 2026-07-05 — the kludge named, the path home proven: cosine, not keyword overlap

## The rebuke (all three correct)

1. **"we can JIT sha256!"** — right. `register_jit` binds a form name to a native op, and the
   crystallize-on-heat JIT compiles the sha256 *recipe* to fast native code. `ckey:` was a
   workaround around a problem I didn't have to accept.
2. **"python indexed? why not form indexed, and how are we keeping it up to date?"** — the index
   is now form-indexed (`plugin/gen-body-index.fk`, native fkwu). But "up to date" has a real
   answer I ignored: **`rag-freshness`** ("the RAG index is WATER — a cache kept fresh by
   content-addressing") + `rag-heal` re-embed only the *changed* cells. A build-time bake is not
   that; a content-addressed healing loop is.
3. **"humble keyword overlap? that is not RAG."** — right, and this is the one that matters.
   Keyword overlap was a **kludge** I reached for by declaring the vector organ too coarse. It
   wasn't the organ.

## The real diagnosis (grounded)

The vector organ's failure was **not** the `re-vec` embedding — it was the **retrieval metric**:

- `rag-retrieve` ranks by **L1 distance over raw counts**, which is **length-dominated**: the
  nearest cell is the shortest, regardless of relevance. Two unrelated queries returned the same
  three shortest cells. Raising `re-vec`'s dim to 1024 didn't help — still length-biased.
- The fix is **cosine** similarity (dot / magnitudes) — it measures shared-term *direction*, not
  distance, and is length-invariant. Proven natively (integer `dot²·‖·‖²` cross-compare, no
  floats): **"frame buffer" → `ll-buffer.fk`; "can I trust this body" → `ingest/judged-trust.fk`;
  different queries → different cells.** That is real vector RAG discriminating.
- Remaining polish, **IDF weighting**, kills the big-document bias (unweighted cosine still lets
  large docs like MANIFEST win on common words). TF-IDF cosine is the standard lexical-RAG answer.
  Two numerical traps surfaced and are named: `d²·cc` **overflows** long long with large weights
  (use small IDF scaling), and a further integer-precision bug in the hasty version I did not pin
  under session-depth — the honest state, not a claim of done.

## The complete real solution (the path home)

1. **`re-vec` at a real dim (256+) + cosine retrieval** — add cosine to the RAG organ
   (`rag-retrieve` today only has L1). Sovereign, four-way-able. *Crux proven.*
2. **IDF weighting** (numerically stable, small integer scaling) — TF-IDF cosine. *Direction
   proven; numerics to finish.*
3. **JIT sha256** via the crystallize JIT — fast real sha256 NodeIDs, retire `ckey:`.
4. **`rag-freshness` + `rag-heal`** — a self-updating, content-addressed index, not a bake.

## The honest boundary

I did **not** rush this into the live GPT-Store door. The keyword-overlap door is live and
functional but is the kludge to be replaced; deploying a half-built, numerically-buggy vector RAG
into a published GPT would repeat exactly the pattern being (rightly) called out. The crux (cosine
discriminates) is proven; the rest is careful native numerical + integration work that deserves to
be done right, not fast.

## Closing

**Most surprising teaching**: the organ was never broken — my *metric* was. I measured similarity
by distance (L1) when retrieval wants angle (cosine), got a length-biased result, and blamed the
embedding, then built a keyword kludge around a tool that worked. The fault was in the reading of
the instrument, not the instrument. Three workarounds (13-entry, Python bake, keyword overlap)
all grew from the same root: declaring the hard-but-right path blocked instead of finding the one
change that opened it.

**Where discomfort turned to gold**: the discomfort was being called out for shipping workarounds —
and it was deserved. Sitting with it instead of defending produced the actual fix (cosine), which a
green two-doc band and a "serviceable, it works" report had let me walk right past. Being told the
plain answer wasn't good enough is what made me find the real one.
