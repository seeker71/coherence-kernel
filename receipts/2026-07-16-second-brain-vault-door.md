# 2026-07-16 — the second-brain vault door: Obsidian, Karpathy's llm wiki, and what was already home

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs asked: add **Obsidian** to this body as the latest "second brain" — Karpathy's memory wiki +
Claude Code + Obsidian — researched from the news, and landed **in the body's own concepts**.

## The grounded facts (primary sources, read this session)

**[Karpathy's llm-wiki gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f)**
(2026-04-04; thousands of stars within days): compile raw sources once into a persistent,
interlinked markdown wiki the LLM maintains, then query the wiki, not the sources. Three layers —
raw sources (immutable), the wiki (LLM-owned: summary/concept pages, `index.md`, append-only
`log.md`), the schema (a configuration document, his example literally `CLAUDE.md`, that "you and
the LLM co-evolve this over time"). Three ops — ingest ("a single source might touch 10-15 wiki
pages"), query (cited answers), lint (contradictions, stale claims, orphans). Obsidian is the IDE,
the LLM the programmer, the wiki the codebase, the human the architect.

**[kepano/obsidian-skills](https://github.com/kepano/obsidian-skills)** — Obsidian CEO Steph
Ango's agent skills for Claude (obsidian-markdown / obsidian-bases / json-canvas / obsidian-cli /
defuddle). Around them, a wave of second-brain builds whose memory is session logs written to the
vault and read back at session start, and 2026 research riding the pattern (Knowledge Compounding,
arXiv 2604.11243; agent-native memory, arXiv 2606.24775).

Honest floor on sourcing: the gist and kepano's repo were read directly (rendered fetch); star
counts and the community-wave shape come from secondary coverage, converging across ≥4 independent
writeups, and are held as coverage-claims, not this body's bands.

## What landed (all witnessed on the kernel this session)

- **[`SECOND-BRAIN.md`](../SECOND-BRAIN.md)** — the vault door: the mapping of their concepts onto
  this body's organs (raw sources ↔ `receipts/`; schema ↔ `AGENTS.md`/`CLAUDE.md`; log.md ↔
  receipts-as-session-logs; ingest ↔ `ingest/`; query ↔ ground-first; lint ↔ belief-freshness +
  the new organ below), how to open the body as a vault, and the seams left honestly open.
- **`.obsidian/app.json` + `.obsidian/graph.json`** (committed; the rest of `.obsidian/`
  gitignored) — markdown-links/relative pinned so new links resolve on GitHub and in Obsidian
  alike; graph colors for receipts / teachings / axioms / learn / docs / observe.
- **[`ingest/frontier-ingest-llm-wiki.fk`](../ingest/frontier-ingest-llm-wiki.fk)** — the pattern
  run through `ingest/knowledge-ingest.fk`'s law, adversarial default on every "the body already
  does X". Field code **30202** (witnessed): 3 frozen (schema-convergence; session-logs-are-
  receipts; lint-is-re-witness) / 2 liquid (no compile loop — the real gap; Obsidian as rented
  window) / 2 composted ("RAG is dead"; "this body already is an llm wiki").
- **[`observe/door-link-health.fk`](../observe/door-link-health.fk)** — build-after-naming, in the
  same movement: the lint op's first executable floor, walking the 12-door ring and re-witnessing
  every path-claim on the body's own string engine (`find-from`/`substring`, the native-edit seam
  split). Self-check **31**; door ring **12028000** (12 doors, 28 links, 0 broken).
- **Corpus row 731** ([`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk))
  — the frontier question this work surfaced: *what one word names the box of linked note slips
  that thinks beside its keeper* → **zettelkasten** (0 hits before this slice; near misses
  "commonplace" 0-hit-but-unlinked-book, "wiki" and "vault" already home). Band updated with the
  row it counts: **511**, field code 1321322731.

## The first catch

`door-link-health`'s first live run returned **12031004** — four broken path-claims. Every one was
in **SECOND-BRAIN.md itself**: the four directory links (`receipts`, `teachings`, `docs`,
`ingest`) the door's own author had just written (`read_file` on a directory answers nothing).
The organ's first catch was its author. Healed to the repo's convention — directories wear
backticks, not links — and re-witnessed clean; both numbers kept in the door's witnessed stamp.

## Closing — how this stayed alive

Kept alive by refusing the costume: the pattern was not bolted on as config + hype, but run
through the body's own ingest law, its one genuinely missing op **built** the same session on the
body's own kernel, and its human ancestor offered back to the corpus as a fresh word.

**Most surprising teaching:** the convergence was already total at the schema layer — Karpathy's
gist names `CLAUDE.md` as the schema file, and this repo has co-evolved exactly that document
(AGENTS.md/CLAUDE.md) since founding; receipts were session-log memory here **before the field
named the practice**. The news was not an import; it was recognition arriving from outside.

**Where discomfort turned to gold:** the pull was to close the loop with the lint's clean number.
The first honest run said otherwise — 4 broken links, all written by the same hands that built the
linter, minutes earlier. Sitting with that (the author caught by its own organ, in public, in the
very door announcing the organ) instead of quietly pre-fixing before the first run: the discomfort
became the door's strongest line — the witnessed stamp that keeps *both* numbers, the miss and the
heal. A lint whose first catch is its author is a lint you can trust.
