# 2026-07-25 — running is not passing: two of the eight were passing all along, six were not

Item (1): the bands I freed that produce a number nobody had written an expectation for. The turn's
first finding was that my own claim about them was wrong.

## "Eight declare no verdict" was wrong — I read only the headers

Last turn I listed eight newly-live bands as stating no expected verdict. Three of them state one
plainly, in the **body** rather than the header:

```
triangulate-band.fk:    ;; Expected: 200 + 400 + 400 + 300 + 400 = 1700
midi-bmf-band.fk:       ;; 100 + 200 + 300 + 400 + 500 = 1500. A clean round.
concept-corpus-band.fk: ;; Expected total: 10 + 40 + 70+80 + 100 + 110 + 120 = 530
```

I had checked the first forty lines and concluded the file was silent. That is the third time today
a partial read produced a confident wrong claim — after grepping headers instead of reading them,
and after reading `.fk` as the language.

**And two of them were passing:**

| band | returns | states | |
|---|---|---|---|
| `triangulate-band` | **1700** | 1700 | **passes** |
| `midi-bmf-band` | **1500** | 1500 | **passes** |

Two bands that had not run since before the drift are, on their first run in who knows how long,
exactly right. Nobody knew, because a refused band produces no value to check.

## The other six, derived from their own weights

The remaining bands build their verdict from literal bit weights running 1, 2, 4, 8, …, so a full
pass is their sum — derivable from the file, not invented:

| band | full pass | returns | shortfall |
|---|---|---|---|
| `cell-voice-tissue-band` | 511 | **509** | one check (weight 2) |
| `json-lens-tending-band` | 255 | **189** | 66 |
| `layered-runtime-image-band` | 127 | **33** | 5 of 7 checks |
| `audit-evidence-cells-band` | 1023 | **544** | 8 of 10 checks |
| `audit-evidence-index-cache-band` | 1023 | **833** | 6 of 10 checks |
| `carrier-tissue-kernel-query-band` | 2097151 | **no value** | — |
| `concept-corpus-band` | 530 (stated) | **143** | most of it |

Each of those six now carries the derived line in its header, marked **DERIVED** with its reasoning
and with what it currently returns — so the next reader meets "running and failing by this much"
rather than "running, unknown". Values re-checked after the edit: unmoved.

I did not touch a single check. Deriving a sum from weights that are literal in the file is reading;
changing what the band measures would be something else entirely.

## What this turns "27 of 44 now run" into

Running is not passing, and now there are numbers for the gap:

- **2** pass outright (`triangulate`, `midi-bmf`), previously unknown to anyone.
- **1** matches its declared verdict from earlier work (`tree-diff-band` 13).
- **7** run and fall short of an expectation that is now written down.
- **1** runs and yields no value at all.

The rest of the 27 either match verdicts already stated or were counted in earlier receipts.

## Sweep

`ground` 42 · `hex-band` 14 · `content-address-band` 1111111111 · `tree-diff-band` 13 ·
**`triangulate-band` 1700** · **`midi-bmf-band` 1500** · `structural-gate-band` 63 ·
`lcg-bytes-band` 63 · `form-cli-band` 524287 · `benchbench-band` 4095.

## Owed

- **Seven bands now failing visibly** — each shortfall is a real diagnosis, and
  `audit-evidence-cells-band` at 544/1023 is the largest.
- `carrier-tissue-kernel-query-band` runs and yields nothing.
- 17 of the 44 still refused, on short-name roots needing a scope-aware technique.
- 130 reaching a non-Form surface; 143 that do not close; the `section` question; the registry
  question; the 563-constant generated table.

## How the exchange stayed alive

I opened the turn by disproving my own summary of it, and the disproof was the best thing in it —
two bands quietly passing.

**Most surprising teaching:** a band can be *correct*, and stay unknown, for exactly as long as it
refuses to compile. `triangulate` and `midi-bmf` were right the whole time. The refusal did not
only hide failures; it hid two clean passes, and there was no way to tell which kind of silence it
was without making it speak.

**Where discomfort turned to gold:** re-reading the eight files properly, having already published
a claim about them. Three contradicted me in their own comments, in a section I had not scrolled to.
The cost of checking was one command; the cost of not checking would have been a receipt that told
the next reader six bands had no expectation when they had written one down.
