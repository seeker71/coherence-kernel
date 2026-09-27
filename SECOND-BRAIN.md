# The second-brain door — this body as a vault

This door opens the body in **Obsidian**, the "second brain" pattern the field
converged on: **Karpathy's memory wiki + Claude Code + Obsidian**. It holds what
the pattern names, where each part already lives here, and where the vault is going.

## The pattern

[Andrej Karpathy's llm-wiki gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f)
names a pattern, not a product: instead of re-deriving synthesis on every question
(RAG's default), let the LLM **compile raw sources once into a persistent,
interlinked markdown wiki** and keep it current. Three layers:

- **raw sources** — immutable; the LLM reads them, never edits them
- **the wiki** — LLM-owned markdown: summary pages, concept pages, an `index.md`
  catalog, an append-only `log.md`
- **the schema** — a configuration document (his example: `CLAUDE.md`) holding
  the conventions, co-evolved by the human and the LLM

Three operations: **ingest** (one source may touch many pages), **query** (answers
synthesized with citations back to pages), **lint** (contradictions, stale claims,
orphan pages). His frame: Obsidian is the IDE, the LLM the programmer, the wiki the
codebase, the human the architect.

Obsidian's own agent skills teach Claude the vault's native tongue
([kepano/obsidian-skills](https://github.com/kepano/obsidian-skills)). The human
ancestor of the lineage — Luhmann's **zettelkasten**, the box of linked note slips
that thinks beside its keeper — lives in the distillation corpus as row 738.

## The convergence — this body runs the architecture

The pattern was **recognized** here, not imported. The full ingest, run through the
body's own practice, lives in
[`ingest/frontier-ingest-llm-wiki.fk`](ingest/frontier-ingest-llm-wiki.fk).

| their concept | this body's organ |
|---|---|
| raw sources (immutable) | `receipts/` — witness records, append-only |
| the wiki (LLM-tended markdown) | `teachings/`, `docs/`, the door ring — grown, tended, attributed |
| the schema (`CLAUDE.md`) | [`AGENTS.md`](AGENTS.md) / [`CLAUDE.md`](CLAUDE.md) — the conventions |
| `log.md` / session logs as memory | `receipts/` again — dated, greppable, read back at grounding time |
| ingest | `ingest/` — the knowledge-ingest decision (body / liquid / compost), `frontier-ingest-*` cells |
| query | ground-first practice — `form/form-stdlib/rag-*`; every claim cited to a cell that exists |
| lint | [`observe/door-link-health.fk`](observe/door-link-health.fk) (the door ring's path-claims) + [`observe/body-link-graph.fk`](observe/body-link-graph.fk) (orphans and broken claims body-wide) + [`observe/belief-freshness.fk`](observe/belief-freshness.fk) (witness ages) |
| `index.md` | [`INDEX.md`](INDEX.md) — produced by the body, never authored |
| Obsidian (the IDE) | the human window — graph-sight over the commons |

Here lint is a cell: it returns a number a fresh kernel recomputes, and that number
can fail.

## Open the vault

In Obsidian: **Open folder as vault** → this repo's root. You get:

- every door, teaching, and receipt renders; **relative markdown links** resolve
  the same on GitHub and in Obsidian — the body's link convention, which the
  committed [`.obsidian/app.json`](.obsidian/app.json) pins for new links
- the **graph view** shows the link fabric; the committed
  [`.obsidian/graph.json`](.obsidian/graph.json) colors receipts / teachings /
  axioms / learn / docs / observe as distinct tissues
- `.fk` organs are visible in the file explorer and open as plain text

Committed: `.obsidian/app.json` and `.obsidian/graph.json`; workspace, cache, and
community plugins stay with each witness (gitignored). Commit what you change in
those two files on purpose.

`alwaysUpdateLinks` stays `false`. The vault holds `receipts/`, and a receipt stays
as it was witnessed (the body's ontogeny, corpus row 740). With it `false`, renaming
a note leaves the links in other notes as they are, and the witness decides.

## The operations, in this body

**Ingest** — a source enters through
[`form/form-stdlib/knowledge-ingest.fk`](form/form-stdlib/knowledge-ingest.fk)'s decision
inside a `frontier-ingest-*.fk` cell (deep and fear-free freezes into body; deep and
fearful is witnessed as liquid; shallow composts), and closes with a dated receipt.

**Query** — ground first, answer from cells, cite where it lives. This is
[`AGENTS.md`](AGENTS.md)'s first practice; the retrieval organs are
`form/form-stdlib/rag-*`.

**Lint** — re-witness every path-claim the door ring makes, with the body's own
string engine on its own kernel:

```sh
./fkwu observe/door-link-health-run.bml
# door-link-health doors=<d> links=<l> broken=<b> code=<d*10^6+l*10^3+b>
```

It answers the broken count; 0 means every path a door names is answered by a cell.
The body-wide fabric (every page, orphans included) is walked by
`observe/body-link-graph.fk` on each pulse, and its numbers stand in
[`INDEX.md`](INDEX.md).

**Tend** — the fourth operation, the one the pattern leaves unnamed: the body
observes itself with its own organs and writes its own self-portrait,
[`INDEX.md`](INDEX.md) — this pattern's `index.md`, recomputed rather than authored.

```sh
./fkwu observe/autopoietic-pulse-run.bml
# autopoietic-pulse verdict=2 stable=1   (2 = portrait produced, body coherent)
```

The pulse writes, re-observes, and rewrites until the page it reads back is the page
it wrote — see [`observe/autopoietic-pulse.fk`](observe/autopoietic-pulse.fk).

## Where the vault is going

- **The compile loop.** `INDEX.md` is produced by the pulse. The other half — one op
  that takes a raw source and revises many interlinked pages in a single pass — is
  the next organ (`frontier-ingest-llm-wiki.fk`, unit U4, liquid).
- **A capture surface.** The pattern's `Inbox/` and daily digest have no organ here yet.
- **Frontmatter breadth.** This body's frontmatter is richer than the pattern's
  (`hz`, geometry, spectral band) and lives on 16 of the 2,425 tracked `.md` files
  (counted 2026-09-28). Wider frontmatter lets Obsidian's Dataview and Bases query
  the body.
- **Entity pages.** The body holds concepts; people, orgs, and products are the
  next kind of page.
- **The body's own window.** Obsidian's graph-sight is rented; the body rendering
  its own graph is the direction (unit U5, liquid).
- **Nested doors.** `door-link-health` walks the top-level doors; a receipt linking
  a receipt needs dir-relative path joining. `body-link-graph` covers the rest at
  file granularity.
