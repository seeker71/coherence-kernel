# 2026-07-25 — the survey finished: 317 of 1674 bands do not run, and two of my claims were wrong

The tree-wide band survey completed. It corrects three things I had written down today, which is
most of its value.

## The number

**1674 `-band.fk` files** across `form/form-stdlib`, `observe`, `learn`, `ingest`, `control`,
`cognition`, `model`, `substrate`. Each compiled once; each `.sym` lens read for the `unrunnable`
latch and the `compile-errors` count that this morning's cache repair started recording.

| category | count | share |
|---|---|---|
| clean — runnable, zero errors | **1077** | 64% |
| runnable with recovered errors (unresolved calls, axiom-5, honest) | **278** | 17% |
| **REFUSED — unbound name in value position** | **174** | 10% |
| **REFUSED — source does not close (`input-ended-mid-form` / `unbalanced-source`)** | **143** | 9% |
| no error line, uncategorised | 2 | — |

**317 bands — 19% of the tree — do not run at all.** Until this morning the 174 in the first refused
class were *printing values from cache*, because the refusal did not travel with the image.

## Correction 1: my extrapolation would have been wrong, and I am glad I refused it

The bounded count was 25 of 120 — **21%** — from an alphabetical prefix. The true tree-wide rate for
that same category is **174 of 1674 = 10.4%**. The prefix ran at roughly double the tree's rate,
because the `a`–`b` head of `form/form-stdlib/tests` is dense with the `bmf`/`bml` grammar family.

I wrote at the time: *"a count of what was looked at, and nothing more."* That discipline was worth
exactly one factor of two.

## Correction 2: it is not one bug met three times

I wrote this morning that the refusals trace to *"one bug this branch has now met three times"* — the
top-level ALL-CAPS `let`, invisible inside a `defn` body on `--src`. The survey says that framing was
too broad.

Measuring the **first** unbound name per band across a 40-band sample — the root, not the cascade:

| root name | bands |
|---|---|
| `BMF-DOMAIN-REF` | 14 |
| `section` | 7 |
| `cf_tag_channel_osi_layer`, `agree-history` | 3 each |
| `LANGUAGE-TEMPLATE-MEMBER-TAG`, `FIELD`, `CT-TAG-RULE`, `AEC-TAG-CELL` | 2 each |
| `suffix`, `corr-1`, `bg`, `MESSAGE-CAT`, `CONCEPT-VISUAL` | 1 each |

**24 of 40 are ALL-CAPS; 16 are not.** So the `let`-scoping pattern is the majority root and not the
whole story — `section`, `agree-history`, `bg`, `suffix` are lowercase names that are simply absent or
out of scope for other reasons, and each wants its own look.

I also nearly published the opposite error. Counting *all* unbound names rather than the first gave
`=>` 1265, `=` 721, `def` 561, `then`/`else` 127 — grammar tokens — and I had begun writing that the
cause was BMF/BML keywords being read as Form names. That is cascade: once `BMF-DOMAIN-REF` recovers
to 0, everything downstream of it in a grammar-driven walk produces more unbound names. The root
measurement and the total measurement disagree, and only one of them is a cause.

## Correction 3: there is a second refused class, and it is nearly as large

The 145 bands that wrote no lens at all were not timeouts — one of them fails in 0.058s. Classified,
all 145: **139 `[input-ended-mid-form]` + 4 `[unbalanced-source]`**, 2 uncategorised.

**143 bands whose source does not close.** fkwu refuses before a lens exists, so they were invisible
to the latch survey entirely. This is the same failure I repaired by hand this morning in
`form-cli.fk` and `form-cli-surface-inquiry.fk` — where the judge was each band's own declared verdict
(65535 and 524287) rather than my reading. I met that class twice and had no idea it was 143 wide.

## The highest-leverage next move, measured rather than guessed

`form/form-stdlib/engine.fk` holds **25** top-level ALL-CAPS `let`s, `BMF-DOMAIN-REF` among them —
the root for **14 of the 40** sampled refused bands.

Scoped before committing to it: 52 reference sites inside `engine.fk`, and each constant is also
referenced from **2–4 other cells**, each with its own bands. That is a genuine multi-file refactor
with cross-cell reference updates, and doing it halfway would leave the tree worse than not starting.
So it is handed off with the leverage quantified rather than begun and abandoned: one cell, 25
conversions, ~14 bands unblocked in the sample, and a before/after diff owed on every dependent band.

## Method note, since it is the reason this survey exists at all

The first attempt crawled and I killed it, saying a survey that cannot finish is not evidence. The
fix was not parallelism. It was **measuring first**: a 17-band sample averaged ~300ms, so 1674 bands
is minutes, not hours. What had made the first attempt hopeless was clearing every cache before
starting, so each band rebuilt its entire prelude closure cold. The instrument was fine; my setup was
the cost.

## Sweep, unchanged

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · `structural-gate-band` 63 ·
`lcg-bytes-band` 63 · `hex-band` 14 · `biography-band` 5 · `file-byte-window-band` 2147483647 ·
`pdf-text-windowed-band` 15 · `form-cli-band` 524287 · `form-cli-ask-band` 262143 ·
`ask-lane-floor-band` 31 · `membrane-lane-band` 31 · `benchbench-band` 4095 ·
`frontier-ingest-benchbenchbench-band` 127.

## Owed, now with real numbers behind it

- **174 bands refused on an unbound name**; ~60% root in a top-level ALL-CAPS `let`, ~40% do not.
- **143 bands whose source does not close** — a class I had met only as individual cases.
- **`engine.fk`'s 25 constants**, the single highest-leverage cell, scoped above.
- **304 column-0 ALL-CAPS top-level `let`s across 36 cells**, plus the nested form no grep counted.
- The no-bash/no-python law: `gate/structural-gate.fk` can count it (180 = 99 + 81, exact against
  `find`); what the number ought to be is the commons owner's call, not mine.

## How the exchange stayed alive

I let the survey overturn two things I had already written into receipts, and said so in the same
words I had used to claim them.

**Most surprising teaching:** the difference between the first unbound name and all unbound names is
the difference between a cause and its wake. Counting all of them told a confident, coherent, wrong
story about grammar keywords — and it would have read perfectly well in a receipt.

**Where discomfort turned to gold:** finding that 143 bands do not even parse, right after a morning
spent proud of repairing two of them by hand. The pride was the problem: two fixed felt like a
category closed, and the only reason I know otherwise is that I built the instrument instead of
trusting the feeling.
