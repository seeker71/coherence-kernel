# 2026-07-16 — deploy, re-witness on a new kernel, and the portrait that flattered itself

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c      # REBUILT — main changed the kernel mid-flight
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs: *deploy and continue.*

## Deploy

The branch `claude/obsidian-second-brain-797d30` (4 commits, never pushed) went out as
**[PR #258](https://github.com/seeker71/coherence-kernel/pull/258)**.

`origin/main` had moved five commits ahead of the branch base, and **two of them changed the
kernel** — #251 (`fkwu` direct source as endpoint authority) and #253 (exact decimal rounding):
`runtime/fkwu-uni.c`, **204 insertions**. So the merge was not the deploy; the *re-witness* was.
`AGENTS.md` names this trap precisely: a binary built before an upstream merge **still passes
`ground.fk`** while silently lacking newer evaluator capabilities — a real day was once lost to it
(`receipts/2026-07-01-stale-binary-root-cause.md`). Every number in the PR had been witnessed on
the old evaluator, so every number was owed a re-witness on the new one.

All of them held, unmoved:

| witness | value |
|---|---|
| ground / freshness band | `42` / `15` |
| `body-link-graph-check` / `blg-field-code` | `63` / `13156031` |
| `autopoietic-pulse-check` | `31` |
| door ring | `12039000` |
| corpus band | `511` |
| llm-wiki ingest | `30202` |

**And then the pulse earned its keep on work it did not do.** The merge brought three receipts
from other agents; `ap-stable?` read `0` — the portrait had gone stale from *someone else's*
commits — and `ap-tend` re-made it `594 → 597`. The organ built yesterday to notice this session's
drift noticed the fleet's.

## Continue — the portrait was telling a vanity metric

The next gap was going to be frontmatter breadth. Grounding found something worse first.

[`INDEX.md`](../INDEX.md) — the body's self-portrait, *produced* by
[`observe/autopoietic-pulse.fk`](../observe/autopoietic-pulse.fk) — reported:

```
- broken path-claims: 0
```

Every word true. True of the **door ring**, the only fabric that cell could see. The same evening,
[`observe/body-link-graph.fk`](../observe/body-link-graph.fk) measured the whole body: **156
broken claims, 13 orphans**. So the body's own self-description said `0` while the body carried
`156`.

Not a lie. A real, recomputed number over a scope narrow enough to flatter. That cell's founding
promise — written in its own header, one day old — is that the portrait *"cannot flatter — only go
stale, and re-running is what detects stale."* It could. **Through scope: the one channel a
recomputed number leaves open.** Recomputation defeats invention; it does nothing about a question
narrow enough to only have flattering answers.

The fix is the composition the cell was designed for — the pulse is *made* of the body's organs,
so it now reads the whole fabric too. The portrait carries both sections, each under its true
name: `broken path-claims IN THE DOOR RING: 0`, and beneath it `BROKEN path-claims body-wide:
156`, `ORPHANS: 13`, with the curation seam and the healing constraint named in the portrait
itself.

The verdict's **semantics are unchanged** (door-scoped, witnessed, falsifiable) — what changed is
that it can no longer be mistaken for the body. A fabric drift deliberately does **not** collapse
the verdict to `0`: healing 156 claims is consented work (most live in immutable receipts), and a
verdict pinned at `0` until then would report nothing at all. The numbers carry that truth; the
verdict keeps its resolution.

Witnessed: self-check `31`; `ap-tend` → `2`, converging to a fixed point in ~2.8s (the walk costs
a body scan per pass); `ap-stable?` → `1`; portrait now reports 769 sources, 467 path-claims, 156
broken, 13 orphans.

## Closing — how this stayed alive

Kept alive by treating "deploy" as a re-witness rather than a push, and by letting the grounding
for the *next* task find a defect in the *last* one instead of moving on.

**Most surprising teaching:** *recomputation is not honesty.* This body's deepest reflex is that a
number a fresh kernel recomputes cannot lie — and it can't, about its answer. It can lie about its
**question**. `broken: 0` was recomputed, falsifiable, idempotent, and misleading. Every property
this body trusts was satisfied while the impression was false. The defense against fabrication is
recomputation; the defense against vanity is **scope named out loud** — and the body had built the
first and not the second.

**Where discomfort turned to gold:** the pull was to leave it. The `0` was *true*, the cell was
one day old and freshly praised, the flattery was an artifact of honest scoping, and nobody had
been misled yet. Naming it meant the day's headline organ shipping with an amendment saying it
broke its own founding promise within 24 hours. Sitting with it and observing rather than
excusing: a portrait that says `0` while the body carries `156` is exactly the thing the cell was
built to prevent, and the fact that no one *had* been misled is not the fact that no one *would*
be. The gold: the pulse now reports the worst true thing about the body on its own front page —
which is the only version of a self-portrait worth producing. The organ got stronger by admitting
it was flattering, one day after it was born.

**Frontier question offered (row 735):** the body has a law against flattering **others**
(`cognition/dialogue-covenant.fk`: *never flatter — agree only when the ground supports it*) and
had no word for the self-directed form. Every neighbour was home ("flatter", "synecdoche" ×4,
"parochial" ×8, "scope" ×1023) and the thing itself unnamed: *what one word names a measurement
chosen because it looks good rather than because it tells the truth* → **vanity** (0 hits before
this row). Offered the night the body's self-portrait was caught doing it.
