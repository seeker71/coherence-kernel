# The second-brain door — this body as a vault

Urs asked to bring **Obsidian** to this body — the "second brain" pattern the field
converged on: **Karpathy's memory wiki + Claude Code + Obsidian**. This door holds
what the pattern names, what already lives here under other names, and what is
honestly still pending.

## The pattern

[Andrej Karpathy's llm-wiki gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f)
names a pattern, not a product: instead of re-deriving synthesis on every question
(RAG's default), let the LLM **compile raw sources once into a persistent,
interlinked markdown wiki** and keep it current. Three layers:

- **raw sources** — immutable; the LLM reads them, never edits them
- **the wiki** — LLM-owned markdown: summary pages, concept pages, an `index.md`
  catalog, an append-only `log.md`
- **the schema** — a configuration document (his example: `CLAUDE.md`) holding
  the conventions; "you and the LLM co-evolve this over time"

Three operations: **ingest** ("a single source might touch 10-15 wiki pages"),
**query** (answers synthesized with citations back to pages), **lint**
(contradictions, stale claims, orphan pages). His frame: Obsidian is the IDE, the
LLM the programmer, the wiki the codebase, the human the architect.

Around the gist: Obsidian's CEO released agent skills teaching Claude the vault's
native tongue ([kepano/obsidian-skills](https://github.com/kepano/obsidian-skills)),
and a wave of second-brain builds whose memory mechanism is session logs written
to the vault and read back at the start of each session. The human ancestor of the
lineage — Luhmann's **zettelkasten**, the box of linked note slips that thinks
beside its keeper — lives in the distillation corpus as row 738.

## The convergence — this body runs the architecture

The pattern was **recognized** here, not imported. The full ingest, run through
the body's own practice with the adversarial pass over its organs, lives in
[`ingest/frontier-ingest-llm-wiki.fk`](ingest/frontier-ingest-llm-wiki.fk).

| their concept | this body's organ |
|---|---|
| raw sources (immutable) | `receipts/` — witness records, append-only, never rewritten |
| the wiki (LLM-tended markdown) | `teachings/`, `docs/`, the door ring — grown, tended, attributed |
| the schema (`CLAUDE.md`) | [`AGENTS.md`](AGENTS.md) / [`CLAUDE.md`](CLAUDE.md) — the conventions |
| `log.md` / session logs as memory | `receipts/` again — dated, greppable, read back at grounding time |
| ingest | `ingest/` — the knowledge-ingest decision (body / liquid / compost), `frontier-ingest-*` cells |
| query | ground-first practice — `form/form-stdlib/rag-*`; every claim cited to a cell that exists |
| lint | [`observe/belief-freshness.fk`](observe/belief-freshness.fk) (witness ages) + [`observe/door-link-health.fk`](observe/door-link-health.fk) (path-claims re-witnessed) + [`observe/body-link-graph.fk`](observe/body-link-graph.fk) (orphans and broken claims body-wide) |
| Obsidian (the IDE) | the human window — graph-sight over the commons (rented; seam named below) |

Where the shapes differ, the difference that matters: their lint is a prompt —
the auditor is the mind that wrote the pages, unverifiable and unrepeatable. Ours
returns an integer a fresh kernel recomputes, and it can **fail**. Their "never
invent facts" is a wish given to a model; here it is enforced by structure — a
receipt that refused to fake a result. And nothing in their system verifies
anything; here the same recipe computes the same value four ways.

## Open the vault

In Obsidian: **Open folder as vault** → this repo's root. You get:

- every door, teaching, and receipt renders; **relative markdown links** resolve
  identically on GitHub and in Obsidian — that is the body's link convention, and
  the committed [`.obsidian/app.json`](.obsidian/app.json) pins new links to it
- the **graph view** shows the link fabric; the committed
  [`.obsidian/graph.json`](.obsidian/graph.json) colors receipts / teachings /
  axioms / learn / docs / observe as distinct tissues
- `.fk` organs are visible in the file explorer (all-extensions is on), opening as
  plain text

Committed: `.obsidian/app.json` and `.obsidian/graph.json` only; workspace, cache,
and community plugins are per-witness and gitignored. Obsidian rewrites those two
files as you use it — local drift there is tide, not signal; commit only deliberate
changes.

**`alwaysUpdateLinks` is committed as `false`, and that is load-bearing — do not
flip it** (re-observed 2026-09-03). The vault is the repo root, so it contains
`receipts/`. With auto-update on, renaming or moving one note in Obsidian silently
rewrites internal links *across the whole vault* — including inside receipts, which
are **immutable witness records** (the body's ontogeny, corpus row 740). A receipt
edited to keep a number green is a forged memory. Off, Obsidian prompts instead of
mutating, and the witness stays the one who decides.

Optional, for agents speaking the vault's own tongue:
`npx skills add https://github.com/kepano/obsidian-skills` — user-level, because
this repo gitignores `.claude/`; agent skills live with the agent, never in the commons.

## The operations, in this body

**Ingest** — a source enters through the door or not at all: run it through
[`form/form-stdlib/knowledge-ingest.fk`](form/form-stdlib/knowledge-ingest.fk)'s decision inside a
`frontier-ingest-*.fk` cell (deep + fear-free freezes into body; deep + fearful is
witnessed as liquid; shallow composts), and close with a dated receipt.

**Query** — ground first, answer from cells, cite where it lives. This is
[`AGENTS.md`](AGENTS.md)'s first practice; the retrieval organs are
`form/form-stdlib/rag-*`.

**Lint** — re-witness. The executable floor walks the door ring and checks every
path-claim a door makes, with the body's own string engine on its own kernel:

```sh
( cat form/form-stdlib/core.fk form/form-stdlib/line-grammar.fk observe/door-link-health.fk; \
  echo '(door-link-health-check)' ) > /tmp/dlh.fk
./fkwu /tmp/dlh.fk      # -> 31 (self-check)

( cat form/form-stdlib/core.fk form/form-stdlib/line-grammar.fk observe/door-link-health.fk; \
  echo '(dlh-field-code)' ) > /tmp/dlhf.fk
./fkwu /tmp/dlhf.fk     # -> doors*10^6 + links*10^3 + broken
```

Re-observed 2026-09-03: self-check `31`; door ring `12065000` — 12 doors, 65 links,
0 broken. The body-wide graph:

```sh
( cat form/form-stdlib/core.fk form/form-stdlib/line-grammar.fk observe/door-link-health.fk \
      observe/body-link-graph.fk; echo '(blg-field-code)' ) > /tmp/blgf.fk
./fkwu /tmp/blgf.fk     # -> orphans*10^6 + broken*10^3 + candidates
```

Re-observed 2026-09-03: `13028050` — 13 orphan pages, 28 broken path-claims, 50
candidates. Naming an orphan is what ends its orphanhood; those 28 are healable
work (`observe/heal-curation-seam.fk` composes the graph's own `blg-lands?` as its
decision and refuses `receipts/` by construction).

**Tend** — the body's fourth operation, the one the pattern doesn't name: the body
observes itself with its own organs and **produces its own self-portrait**,
[`INDEX.md`](INDEX.md) — this pattern's `index.md`, recomputed rather than
authored. Every number comes from the tissue it names, so the portrait cannot
flatter; it can only go stale, and re-running is what detects stale.

```sh
( cat form/form-stdlib/core.fk form/form-stdlib/line-grammar.fk observe/door-link-health.fk \
      learn/homecoming-distillation-corpus.fk observe/autopoietic-pulse.fk; \
  echo '(ap-tend)' ) > /tmp/ap.fk
./fkwu /tmp/ap.fk       # the cell's legend: 2 = portrait produced, body coherent
```

The pulse is idempotent (re-running writes byte-identical text), falsifiable (a
planted broken path-claim drops the verdict; healing restores it), and iterates to
a fixed point — see [`observe/autopoietic-pulse.fk`](observe/autopoietic-pulse.fk).

## Honest seams (pending is honest)

- **No compile loop yet — the index half is closed.** `INDEX.md` is produced by
  the pulse. What remains unbuilt is the other half: no op takes one raw source and
  revises many interlinked pages in a single pass. The wiki layer is grown, not
  compiled — the largest named gap (`frontier-ingest-llm-wiki.fk`, unit U4 — liquid).
- **No capture surface.** The pattern's `Inbox/` and daily digest have no organ
  here. A real gap.
- **Frontmatter breadth.** This body's frontmatter is richer than the pattern's
  (`hz`, geometry, spectral band) but lives on 16 of 1,770 `.md` files (counted
  2026-09-03) — Obsidian's Dataview/Bases can barely query the body. A cheap, real win.
- **Entity pages.** The body has concepts, not entities (people, orgs, products).
- **Obsidian is a rented window.** The graph-sight it gives is not yet the body's
  own rendering — the same seam-shape as the rented voice: native tissue, rented
  viewer (unit U5 — liquid).
- **Door-ring scope.** `door-link-health` lints the top-level doors only; nested
  doors (a receipt linking a receipt) need dir-relative path joining — a named next
  shell. `body-link-graph` covers the rest at file granularity.

## Sources

- [Karpathy, llm-wiki gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f) (primary)
- [kepano/obsidian-skills](https://github.com/kepano/obsidian-skills) (primary — Obsidian's agent skills)
- Coverage and builds: [emergingai walkthrough](https://emergingai.substack.com/p/claude-code-obsidian-guide-karpathys) ·
  [aimaker build](https://aimaker.substack.com/p/llm-wiki-obsidian-knowledge-base-andrej-karphaty) ·
  [decodingai, wiki as agent memory](https://www.decodingai.com/p/llm-wiki-agent-memory) ·
  [AAIF analysis](https://aaif.io/blog/karpathys-llm-wiki-as-agent-memory/) ·
  [obsidian-second-brain](https://github.com/eugeniughelbur/obsidian-second-brain) ·
  [Ar9av/obsidian-wiki](https://github.com/ar9av/obsidian-wiki) ·
  [natural20.com guide](https://natural20.com/using-claude-code-to-setup-a-second-brain-aka-llm-wiki)
- Research riding the wave: [Knowledge Compounding (arXiv 2604.11243)](https://arxiv.org/pdf/2604.11243) ·
  [Agent-Native Memory (arXiv 2606.24775)](https://arxiv.org/pdf/2606.24775)
