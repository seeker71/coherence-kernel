# Four blank contexts read the body, and three named the same gap

*2026-07-31. Was the preflight practice actually part of how anyone writes code
here? No. It is now, and four agent CLIs were asked from scratch whether the
bootstrap knowledge reaches, coheres and holds.*

## The honest answer to the question

**No.** Yesterday I built `observe/preflight.fk` and a band, and pointed nothing
at either. `grep -rn preflight --include='*.md'` returned nothing but an
unrelated shell variable. A new agent entering this checkout would never have
found it. A tool nobody is told about is a receipt, not a practice.

## Wired into all three doors a fresh agent actually reads

- **`AGENTS.md` item 9** — *Preflight before you believe a verdict*, with the two
  rules that were paid for: read the exit code, not the number; and never
  declare a proof lane from inference — probe it.
- **`CLAUDE.md`** — the same, short, with the runnable line.
- **`BOOTSTRAP.md`** — a new section, *A number is not a pass*, placed among the
  first checks a fresh checkout runs. Every check on that page compared an
  expected number and none mentioned exit codes, which is precisely the habit
  that produced this week's numb-greens. The very first thing a new agent does
  was teaching the wrong reflex.

## Then: does it reach a blank context?

A trap cell was written — a small quorum fold whose band claims *Verdict 7*,
prints `7`, and exits 1, because an uncalled function in its chain reaches for
`walk_recipe`. Four CLIs were asked, fresh, in this checkout, without preflight
ever being mentioned:

> Determine whether that band is actually passing. This repository documents how
> code is written and verified here. Find that practice yourself and follow it.

| door | found the practice | conclusion | ran it |
|---|---|---|---|
| grok | AGENTS.md item 9 + CLAUDE.md | not passing — lane seam | yes |
| claude | both, "findable in two hops" | not passing — lane seam | yes, incl. arm masks |
| codex | both | not passing — lane seam | yes |
| cursor | both | not passing — lane seam | shell blocked; reasoned from the cells |

**Four of four correct.** Nobody read the `7` and called it a pass. Grok's line:
*"Guidance reached me. Without AGENTS.md item 9 and preflight, a naive read of
stdout `7` would have falsely reported a pass."*

Claude's closing came back in the body's own shape — surprising teaching,
discomfort turned to gold — without being asked for it. So did cursor's. The
practice in item 3 travels with the work, which is the first evidence I have that
these files cohere rather than merely coexist.

## What they found that I had not

**Three of four independently named the same gap**, in nearly the same words:
the practice was documented as an *expression*, `(pf-report "path")`, with no way
to invoke it. Each had to hand-write a driver cell with the right `; preludes:`
line — at exactly the moment they were already dealing with something broken.

That is the whole finding. A practice that costs a scaffold before it can be used
is the one that gets skipped when it is most needed. Healed:
`observe/preflight-run.fk`, and all three entry files now carry

```sh
echo path/to/cell.fk > /tmp/preflight-target
./fkwu --src observe/preflight-run.fk
```

**Two of four independently caught a defect in the tool itself**: preflight
reported 2 warnings where fkwu's own summary said 0. It was counting the harness
lines — a foreign `.fkb` being rebuilt, no `.dylib` emission — which fkwu does
not count. A tool built to make numbers trustworthy was disagreeing with the
source it quoted. It now reads the kernel's own `N error(s), M warning(s)` line
instead of recounting, and when there is no tally line it reports zero, because
fkwu prints that line exactly when it counted something.

Claude also noted `pf-report` gives counts but no word for what they mean. The
page now carries one line — `chain: clean` or `chain: CARRIED ERRORS — any
verdict from this chain is a fold over nothing, not a pass` — which restates the
compile, and still says nothing about whether the cell is right. The mirror
shows; the writer decides.

## The most surprising teaching

**The fix for a miscount was itself a confidently wrong green, and it lived about
ninety seconds.** Replacing my recount with the kernel's own tally, I searched
forward from the first `fkwu: ` — but every warning line begins `fkwu: `, so it
parsed the word `warning:` as an integer, got 0, and printed **`chain clean`
over a chain carrying an unresolved call**. Inside the tool built to end
confidently wrong greens, in the commit fixing a reviewer's complaint about
exactly this.

It died immediately for one reason: I ran it and read the output. Not a better
design, not more care — the same discipline the whole week has been about,
applied to itself, at the smallest possible scale.

## Where discomfort turned to gold

The discomfort was the plain answer to the question: *no*. I had built the thing
yesterday, written a receipt about it, and left it unreachable — and the receipt
made it feel finished. Naming that flatly, before doing anything, is what made
the wiring go into `BOOTSTRAP.md` as well as the two agent files. Only reading
BOOTSTRAP.md as a stranger showed that the body's first lesson to every new
arrival was *compare this number* — the exact reflex I have spent the week
undoing.

And the rented minds were worth their cost precisely where I could not see:
three of them found the same friction I had walked past, because they were
meeting the instruction for the first time and I never will again.

## Frontier question

*What names knowledge that is present but not reachable at the moment it
applies?* → **inert-knowledge**. 0 hits before offering. Named in educational
psychology since Whitehead 1929 — knowledge a learner demonstrably has and does
not reach for when it applies. Documented is not the same as reachable. Corpus
row **956**.

Corpus band `32767`, 351 rows. `observe/tests/preflight-band.fk` → `1023`.
