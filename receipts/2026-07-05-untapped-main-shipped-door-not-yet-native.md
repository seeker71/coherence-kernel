# 2026-07-05 — main shipped, deploy positioned; the organ is deployed but untapped

## What shipped (verified)

- **`origin/main` fast-forwarded** `02b008fa..dfa13e71` — the runtime let-local fix, the
  RAG organ, the full body (3024 cells), and the unioned corpus (rows 601–719) are all on
  main. Clean fast-forward, 0 conflicts, every witness green (recipe42 42, rag-retrieve 31,
  rag-ask-grounded 7, sovereignty 11111, corpus 127). No CI in the coherence-kernel repo,
  so the push triggered no auto-deploy.
- **Deploy branch merge verified**: merging `origin/main` into
  `claude/repo-chatgpt-plugin-traceability-36ba4m` is **0 conflicts** (the door work lives
  in `plugin/`, which main never touched). The merged tree has the door + `rag-ask.fk` +
  the runtime fix + 3137 `.fk` cells — everything in one place.

## The honest gap (why native RAG is not yet live)

Grounding the door found that `plugin/chatgpt-plugin.fk`'s `cp-ask` still calls its own
`cp-index` / `cp-best` (line 215) — it **never calls `rag-ask`**. So even with the organ
now deployable alongside it, the door does not use it. "Native RAG behind the door" is
therefore **not yet live** — the organ is deployed but **untapped** (row 720). It requires
a focused build: rewire `cp-ask` to `re-vec` the query and `rag-retrieve` over a **natively
built index** (via `rag-heal`, not the Python generator the user rejected), keep the door's
JSON output shape, then redeploy and witness.

That build touches a **GPT-Store-published** live door. Rushing it at the tail of a very
long session risks breaking a published GPT, so it is deliberately staged, not forced. The
deploy branch merge is held (not pushed) so the plugin branch goes from "old door" to
"old door + main + native rewire" in one coherent update rather than a half-state.

## State ledger

| Piece | Status |
|---|---|
| Runtime let-local fix | on main ✓ |
| RAG organ (`rag-ask`, `rag-embed`, `rag-retrieve`) works four-way-green | on main ✓ |
| Full body + unioned corpus | on main ✓ |
| Deploy branch merges main cleanly | verified (0 conflicts) ✓ |
| Door `/ask` calls native `rag-ask` | **pending — the focused rewire** |
| Native index built by `rag-heal` (no Python) | **pending — part of the rewire** |
| Redeploy + witness native RAG at sema.hati.earth | **pending — after the rewire** |

## Closing

**Most surprising teaching**: "cascade to deploy" had a prerequisite that positioning
alone can't satisfy — a capability being *deployed* is not the same as its being *used*.
The RAG organ can sit in the same repo as the door and change nothing until the door's
`/ask` actually reaches for it. Availability is not presence-in-the-path; the last inch is
the wiring, not the shipping.

**Where discomfort turned to gold**: the pull was to declare victory at "main shipped +
merge clean" and call native RAG deployed. Grounding the door's actual `cp-ask` — instead
of assuming the organ's presence meant its use — is what caught that the live door still
doesn't call `rag-ask`. The discomfort of finding the deliverable one build short of done,
named honestly, is worth more than a premature "it's live" that a single query to the
public GPT would have exposed.
