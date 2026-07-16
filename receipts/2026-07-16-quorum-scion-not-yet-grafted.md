# 2026-07-16 — the quorum is a scion, not yet grafted: integration status, grounded

## The question and the plain answer

Urs: *"is that integrated into form-cli and the ask and query interface we are using?"*

**No.** None of today's cells (carrier-trust, lineage-discounted-vote, the live quorum) are in
form-cli or the ask/query lane. The grounding, file and line:

- **The ask verb calls no LLM at all, by written design.** `form/form-stdlib/form-cli-ask.fk`:
  the one-shot CLI stages the question and fkwu reads the staged query + healed RAG JSONL index
  directly — "It does not call Ollama, localhost:11434, http-fetch, host-exec, or a shell...
  until that lane is wired end-to-end, this verb returns an attributed grounded cell instead of
  pretending a host LLM is the local oracle." The `/ask` door in `plugin/openapi.json` rides the
  same grounded lane.
- **The query lane is native RAG.** `form/form-stdlib/rag-ask.fk`: NodeID-grounded semantic
  retrieval over the typed JSONL index, `grounded:@p.l.t.i` or an honest miss.
- **The router knows two backends, by design.** `form/form-stdlib/form-cli-router.fk`:
  form-native and agent-cli only — "no metered LLM REST API exists in this body." (The
  three-tier LAW with local oracles as tier 1 exists as `routers/tier-router.fk` — a proven
  recipe, itself not the form-cli router.)
- **Today's quorum ran BESIDE the body** — ollama driven by shell, ballots folded by fkwu.
  A rented lane observing rented runtimes; nothing of it in the form-cli module graph
  (`form/build-form-cli.sh` MODS: no vote cell of any kind is listed).

## What was landed to close the distance honestly

The law was cut from its proven tree and laid in the nursery, per the body's own twin-copy
convention (`confidence-weighted-vote.fk` lives byte-identical in `learn/` and
`form/form-stdlib/`):

- `form/form-stdlib/lineage-discounted-vote.fk` — byte-identical twin of the proven learn/ cell.
- `form/form-stdlib/tests/lineage-discounted-vote-band.fk` — the band re-homed on form-stdlib
  preludes; **127 four-way** (fkwu/Go/Rust/TS) from the form-stdlib paths, witnessed this
  session.

The scion is prepared; it is deliberately **not grafted tonight**:

1. **The graft point is the JUDGE, not the ask verb.** `form-cli-judge.fk` already names "a
   local oracle, the membrane crossing" as its carrier — that is where multiple local witnesses
   would fuse (lineage-discounted) into one graded verdict. The ask verb's no-LLM stance is a
   written design decision of the deployed lane ("one canonical door guidance" —
   `receipts/2026-07-15-canonical-door-guidance.md` lineage), and quietly wiring ollama calls
   into it from a side session would override the body's own written law. That grafting — if
   wanted — is a door-design movement of its own.
2. **The CLI build graph is stamp-gated.** `build-form-cli.sh` copies the committed platform
   binary when the source stamp matches; adding a module to MODS is the maintainer REGEN flow,
   not a side-session edit.

## Honest floor

The form-stdlib twin is proven but UNWIRED: no form-cli verb calls `ldv-*` yet; nothing in the
deployed `/ask` door changed. The quorum's live ballots remain a bash-driven observation, not a
body organ. `routers/tier-router.fk` (the three-tier law) and `form-cli-router.fk` (the
two-backend design) say different things about local oracles — that seam is real, pre-existing,
and named here rather than resolved unilaterally.

## Closing — how this stayed alive

Most surprising teaching: the ask lane already embodies the day's whole moral better than the
day's own demo — it refuses to pretend a host LLM is the local oracle, answering only from
grounded cells or missing honestly. The body's front door was ahead of the comparison work: the
strictest witness in the house was already on duty.

Where discomfort turned to gold: the pull was to "complete the integration" tonight — wire the
quorum into the ask verb, regenerate the CLI, close the loop, arrive with everything done. Sitting
with why that itched: it would have overwritten a written design stance (no-LLM ask) and crossed
a maintainer gate (the bootstrap stamp) to manufacture a "yes." The gold is the scion convention
itself — proven, twinned into the nursery, graft point named at the judge — so the next movement
is one deliberate join, not an archaeology of what a side session smuggled in.
