# The second-brain door — this body as a vault

Urs asked (2026-07-16): bring **Obsidian** to this body — the "second brain" pattern the field
converged on in 2026: **Karpathy's memory wiki + Claude Code + Obsidian**. This door holds what
arrived, what already lived here under other names, and what is honestly still pending.

## What arrived (grounded in the primary sources)

[Andrej Karpathy's llm-wiki gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f)
(2026-04-04) names a pattern, not a product: instead of re-deriving synthesis on every question
(RAG's default), let the LLM **compile raw sources once into a persistent, interlinked markdown
wiki** and keep it current. Three layers:

- **raw sources** — immutable; the LLM reads them, never edits them
- **the wiki** — LLM-owned markdown: summary pages, concept pages, an `index.md` catalog, an
  append-only `log.md`
- **the schema** — a configuration document (his example: `CLAUDE.md`) holding the conventions;
  "you and the LLM co-evolve this over time"

Three operations: **ingest** ("a single source might touch 10-15 wiki pages"), **query** (answers
synthesized with citations back to pages), **lint** (contradictions, stale claims, orphan pages).
His frame: Obsidian is the IDE, the LLM the programmer, the wiki the codebase, the human the
architect.

Around the gist: Obsidian's CEO released agent skills teaching Claude the vault's native tongue
([kepano/obsidian-skills](https://github.com/kepano/obsidian-skills) — obsidian-markdown,
obsidian-bases, json-canvas, obsidian-cli, defuddle), and a wave of second-brain builds
([one walkthrough](https://emergingai.substack.com/p/claude-code-obsidian-guide-karpathys) among
many) whose memory mechanism is session logs written to the vault and read back at the start of
each session. The human ancestor of the whole lineage — Luhmann's **zettelkasten**, the box of
linked note slips that thinks beside its keeper — entered the distillation corpus as row 731 the
day this door opened.

## The convergence — this body already runs the architecture

Named honestly: the pattern was not imported here; it was **recognized**. The full ingest, run
through the body's own law with the adversarial pass over its organs, lives in
[`ingest/frontier-ingest-llm-wiki.fk`](ingest/frontier-ingest-llm-wiki.fk)
(3 frozen / 2 witnessed / 2 composted — field code 30202).

| their concept | this body's organ |
|---|---|
| raw sources (immutable) | `receipts/` — witness records, append-only, never rewritten |
| the wiki (LLM-tended markdown) | `teachings/`, `docs/`, the door ring — grown, tended, attributed |
| the schema (`CLAUDE.md`) | [`AGENTS.md`](AGENTS.md) / [`CLAUDE.md`](CLAUDE.md) — the conventions, since founding |
| `log.md` / session logs as memory | `receipts/` again — dated, greppable, read back at grounding time |
| ingest | `ingest/` — the knowledge-ingest law (body / liquid / compost), `frontier-ingest-*` cells |
| query | ground-first practice — `form/form-stdlib/rag-*`; every claim cited to a cell that exists |
| lint | [`observe/belief-freshness.fk`](observe/belief-freshness.fk) (witness ages) + [`observe/door-link-health.fk`](observe/door-link-health.fk) (path-claims re-witnessed) |
| Obsidian (the IDE) | the human window — graph-sight over the commons (rented; seam named below) |

## Open the vault

In Obsidian: **Open folder as vault** → this repo's root. You get:

- every door, teaching, and receipt renders; **relative markdown links** resolve identically on
  GitHub and in Obsidian — that is the body's link convention, and the committed
  [`.obsidian/app.json`](.obsidian/app.json) pins new links to the same convention
- the **graph view** shows the link fabric; the committed
  [`.obsidian/graph.json`](.obsidian/graph.json) colors receipts / teachings / axioms / learn /
  docs / observe as distinct tissues
- `.fk` organs are visible in the file explorer (all-extensions is on), opening as plain text

Committed: `.obsidian/app.json` and `.obsidian/graph.json` only; workspace, cache, and community
plugins are per-witness and gitignored. Seam, named: Obsidian rewrites those two files as you use
it — local drift there is tide, not signal; commit only deliberate changes.

Optional, for agents speaking the vault's own tongue:
`npx skills add https://github.com/kepano/obsidian-skills` — user-level, because this repo
gitignores `.claude/`; agent skills live with the agent, never in the commons.

## The three operations, in this body

**Ingest** — a source enters through the door or not at all: run it through
[`ingest/knowledge-ingest.fk`](ingest/knowledge-ingest.fk)'s law inside a `frontier-ingest-*.fk`
cell (deep + fear-free freezes into body; deep + fearful is witnessed as liquid; shallow composts),
and close with a dated receipt. The gist's compile loop — one source revising 10-15 interlinked
pages in a pass — is **not built here yet**; see the seams below.

**Query** — ground first, answer from cells, cite where it lives. This is
[`AGENTS.md`](AGENTS.md)'s first law; the retrieval organs are `form/form-stdlib/rag-*`.

**Lint** — re-witness. The first executable floor walks the door ring and checks every path-claim
a door makes, with the body's own string engine on its own kernel:

```sh
( cat form/form-stdlib/core.fk grammars/line-grammar.fk observe/door-link-health.fk; \
  echo '(door-link-health-check)' ) > /tmp/dlh.fk
./fkwu --src /tmp/dlh.fk      # -> 31 (self-check)

( cat form/form-stdlib/core.fk grammars/line-grammar.fk observe/door-link-health.fk; \
  echo '(dlh-field-code)' ) > /tmp/dlhf.fk
./fkwu --src /tmp/dlhf.fk     # -> doors*10^6 + links*10^3 + broken
```

witnessed: 2026-07-16 → self-check `31`; door ring `12033000` (12 doors, 33 links, 0 broken).
The *first* run returned `12031004` — four broken path-claims, every one of them this door's own
directory links: the organ's first catch was its author. Healed (directories wear backticks, not
links), re-witnessed clean.

**Tend** — the body's fourth operation, and the one Karpathy's pattern doesn't name: the body
observes itself with its own organs and **produces its own self-portrait**,
[`INDEX.md`](INDEX.md) — this pattern's `index.md`, recomputed rather than authored. Every number
comes from the tissue it names, so the portrait cannot flatter; it can only go stale, and
re-running is what detects stale.

```sh
( cat form/form-stdlib/core.fk grammars/line-grammar.fk observe/door-link-health.fk \
      learn/homecoming-distillation-corpus.fk observe/autopoietic-pulse.fk; \
  echo '(ap-tend)' ) > /tmp/ap.fk
./fkwu --src /tmp/ap.fk       # -> 2 (portrait produced, body coherent)
```

witnessed: 2026-07-16 → self-check `31`; `ap-tend` → `2`; field `2059303300`. Idempotent
(re-running writes byte-identical text) and falsifiable (a planted broken path-claim drops the
verdict to `0`; healing restores `2`). The pulse iterates to a fixed point because its first run
proved it must — see [`observe/autopoietic-pulse.fk`](observe/autopoietic-pulse.fk).

## Honest seams (pending is honest)

- **No compile loop yet — though the index half closed the same day.** The pattern's `index.md`
  now exists as [`INDEX.md`](INDEX.md), *produced* by [`observe/autopoietic-pulse.fk`](observe/autopoietic-pulse.fk)
  from the body's own observation of itself (see **tend**, above). What remains unbuilt is the
  other half: no op takes one raw source and revises many interlinked pages in a single pass. The
  wiki layer is still grown, not compiled — the largest named gap
  (`frontier-ingest-llm-wiki.fk`, unit U4 — liquid).
- **Obsidian is a rented window.** The graph-sight it gives is not yet the body's own rendering —
  the same seam-shape as the rented voice: native tissue, rented viewer (unit U5 — liquid).
- **Door-ring scope.** `door-link-health` lints the top-level doors only; nested doors (a receipt
  linking a receipt) need dir-relative path joining — a named next shell, not a claim.

## Sources

- [Karpathy, llm-wiki gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f) (primary, 2026-04-04)
- [kepano/obsidian-skills](https://github.com/kepano/obsidian-skills) (primary — Obsidian's agent skills)
- Coverage and builds: [emergingai walkthrough](https://emergingai.substack.com/p/claude-code-obsidian-guide-karpathys) ·
  [aimaker build](https://aimaker.substack.com/p/llm-wiki-obsidian-knowledge-base-andrej-karphaty) ·
  [decodingai, wiki as agent memory](https://www.decodingai.com/p/llm-wiki-agent-memory) ·
  [AAIF analysis](https://aaif.io/blog/karpathys-llm-wiki-as-agent-memory/) ·
  [obsidian-second-brain](https://github.com/eugeniughelbur/obsidian-second-brain) ·
  [Ar9av/obsidian-wiki](https://github.com/ar9av/obsidian-wiki)
- Research riding the wave: [Knowledge Compounding (arXiv 2604.11243)](https://arxiv.org/pdf/2604.11243) ·
  [Agent-Native Memory (arXiv 2606.24775)](https://arxiv.org/pdf/2606.24775)
