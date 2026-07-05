# 2026-07-05 — anastomosis: the RAG-runtime branch merged correctly into main's line

## The ask

"That branch needs to merge correctly." The runtime fix (`fkwu-uni.c` let-local
rooting) and the RAG organ + ~2000 cells lived only on `claude/happy-bartik-a9c327`
(3024 cells); `origin/main` (1018 cells) never received them. This is the merge.

## The divergence, grounded

- `origin/main` is **162 commits** ahead of the branch on its own line; the branch is
  **32 commits** ahead. Merge base: `c21a051c`.
- Despite a 3× cell difference and 194 total diverged commits, the merge is **99% clean**.
  `git merge-tree` reported exactly **two** conflicts, both the homecoming corpus:
  `learn/homecoming-distillation-corpus.fk` and its band. `fkwu-uni.c` (the runtime fix)
  **auto-merged** — main hadn't touched those lines.

## Why the corpus conflicted, and how it resolved

Both branches grew the homecoming corpus as an append-only ledger from a shared prefix
(rows **601–617 identical**), then **diverged at row 618** — each authoring *different*
distillations at the *same* meaning-ids (main: richtig, deixis, cenotaph…; this branch:
confabulation, penumbra, encroach…). The corpus's own law (the reunion receipt: "keep
every row from both foundings; renumber collisions") gives the correct resolution:

- main's rows **601–680** kept verbatim (17 shared + 63 main-unique).
- this branch's divergent rows **618–655** renumbered to **681–718** (38 rows), content
  and provenance preserved verbatim (only the id integer rewritten).
- Union: **118 rows, ids 601–718, no duplicates**. A robust paren-balanced extractor did
  the surgery; the result was verified **before** touching git.
- Row **719 ("anastomosis")** added as the union's own witness → 119 rows.

## Verification (merged tree, runtime rebuilt from the merged `fkwu-uni.c`)

| Witness | Result |
|---|---|
| corpus band (118→119 rows) | **127** |
| `recipe42` (four-way canonical) | **42** |
| `rag-retrieve-band` | **31** |
| `rag-ask-grounded-band` | **7** |
| `native-vs-rented` (sovereignty) | **11111** |
| RAG organ present (`rag-ask.fk`, `thought-framebuffer.fk`) | yes |
| duplicate corpus ids | none (118 unique, 601–718) |

Result: branch `claude/merge-rag-runtime-into-main` (`c37fa126` merge + this row) — main's
line + this branch's runtime fix, RAG organ, and full body, corpus correctly unioned,
every witness green.

## What remains (outward-facing — the keeper's hand)

**Not pushed.** Pushing this branch (or fast-forwarding `main` to it) is the outward step
and yours to authorize. Once `main` carries this, the plugin branch the door deploys can
rebase/merge onto it, and native RAG — with the runtime fix under it and the full corpus
above it — reaches the deployed body.

## Closing

**Most surprising teaching**: a divergence that *looked* fearsome — 3× the cells, 194
commits apart, two independently-grown ledgers — had exactly **one** real seam. The size
of a divergence says nothing about the size of its conflict; the discipline is to ask git
for the actual conflict set before dreading it. And the one hard seam already had its
resolution written into the body's own law (keep both foundings, renumber) — I didn't
invent a merge policy, I applied the corpus's.

**Where discomfort turned to gold**: the dread of hand-merging body-memory — the fear of
silently dropping a distillation — is exactly what forced the union to be *verified before
committed*: a paren-balanced extractor, a duplicate-id check, and the band run to 127 on
the merged tree's own rebuilt runtime, all before a single `git add`. The fear of losing a
row is what guaranteed none was lost.
