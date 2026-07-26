# 2026-07-25 — the tractable 44: nine more bands came alive, and one header was provably stale

Item (1) off the list — the 44 refused bands with a **pure Form closure**, the set the previous
turn's measurement identified as actually fixable rather than mis-read.

## What was done

The 44 bands' first unbound names, measured per band: 23 are ALL-CAPS constants, 17 are lowercase
names, 4 already ran (engine.fk had fixed them). Sixteen of the ALL-CAPS roots resolved to a
single-line top-level `let` with a clear home cell.

All sixteen converted to zero-arg `defn`s — `FIELD`, `CT-TAG-RULE`, `CONCEPT-VISUAL`, `AEC-TAG-CELL`,
`UUID-PARSE-ERROR`, `URL-DECODE-ERROR`, `TRIANGLE-VERTEX`, `SESSION-NOT-FOUND`, `SENSING-V0`,
`RECIPE_RUNTIME_COUNT`, `MIDI-NOTE-ON`, `MESSAGE-CAT`, `LET_CAT`, `JLT-TAG-RULE`, `IDENT-CAT`,
`DIFF-EQUAL` — with every bare reference parenthesized across **41 files**, matched by a lookaround
that excludes quoted names and longer hyphenated ones.

Every file is **+N −N with zero line delta**. No markers left behind.

## Result

**13 of 44 now run**, up from the 4 engine.fk had already freed — so these sixteen conversions
brought **nine** more bands to life.

Five return their **own declared verdicts exactly**:

| band | value |
|---|---|
| `bmf-object-data-band` | **65535** |
| `bmf-symbol-wire-band` | **255** |
| `form-control-backtracking-ml-band` | **65535** |
| `recipe-choice-runtime-band` | **233** |
| `substrate-core-band` | **11111** |

## A header that was provably stale

`content-address-band` came alive at **1111111111** against a header reading *"Band verdict: 1023."*

That is not a disagreement about the code. The header itself enumerates ten checks at weights
`1, 10, 100, … 1000000000` — decimal place-values, summing to exactly 1111111111. `1023` is 2^10−1,
the **bitmask** sum for ten checks. The band was converted from bit-weights to place-weights and the
sentence was not.

Provable from the band's own list rather than from my preference, so the sentence is re-stamped and
the criterion is untouched — the same stamp-versus-criterion line drawn for `form-cli`'s version
check this morning. And the reason it drifted unnoticed is this branch's whole subject: **the band
had been REFUSED at compile since before the drift, so nothing ever ran to contradict it.**

Re-run after: **1111111111**.

## Two claims withdrawn

I first reported `url-encode-band` (13) and `uuid-band` (11) as differing from declared verdicts of
16 and 13. Reading their headers rather than grepping them: **neither declares a verdict.** Those
numbers came from my own crude pattern matching over comment text. Withdrawn — they run, and what a
full pass looks like for them is not written down.

## What is now visible that was not

`mesh-sensings-store-band` runs and returns **0** against a declared **255**; `chat-band` runs and
returns **0**. Those are genuine failures, and they are failures the tree could not previously
report at all — a refused band has no verdict to be wrong. Named, not fixed; each wants its own
diagnosis.

## Regressions: none

Every band over the sixteen changed cells that was runnable before is runnable after, with a value:
`class-curriculum-band` 8191, `field-door-band` 11111, `field-sample-band` 11111, `fleet-fathom-band`
63, `generate-step-band` 11111, `gpu-mesh-grow-band` 255, `gpu-mesh-sense-band` 511,
`host-kernel-cell-band` 25, `kernel-satsang-band` 193, `concept-xpath-band` 1, `doc-xpath-band` 2,
`lcg-bytes-band` 63.

Core sweep cold and unmoved: `ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 ·
`hex-band` 14 · `biography-band` 5 · `structural-gate-band` 63 · `lcg-bytes-band` 63 ·
`file-byte-window-band` 2147483647 · `pdf-text-windowed-band` 15 · `form-cli-band` 524287 ·
`form-cli-ask-band` 262143 · `ask-lane-floor-band` 31 · `membrane-lane-band` 31 · `benchbench-band`
4095 · `frontier-ingest-benchbenchbench-band` 127.

## Owed

- **31 of the 44 still refused** — roots are the lowercase names (`model` 3, `agree-history` 3,
  `fndef-cat` 2, `verb-set`, `suffix`, `q`, `lg`, `g`, `frame`, `embed`, `corr-1`, `bg`) plus
  ALL-CAPS chains behind the first. Each wants its own look; none is the `let` idiom by default.
- **`URL-DECODE-ERROR`, `UUID-PARSE-ERROR` and `FIELD` are not in the reviewed bootstrap registry**,
  so `fol-bp` would rightly refuse them and they still resolve through the `bp` pass-through stub.
  That seam stays open exactly where this morning's hex repair left it — closing it means a registry
  decision, which is not mine.
- 130 bands whose closures reach a non-Form surface; 143 that do not close; the `section` question.

## How the exchange stayed alive

I withdrew two findings in the same turn I published them, because reading the headers disagreed
with grepping them.

**Most surprising teaching:** a band can drift out of agreement with its own documentation and stay
that way indefinitely, provided it never runs. `content-address-band`'s header and body have
disagreed by a factor of a million, and the refusal that hid a numb green was also hiding a stale
sentence.

**Where discomfort turned to gold:** four of the thirteen came back with numbers that did not match
what I expected, and my first move was to suspect my own conversion. Checking found one provably
stale header, two claims of mine that were never real, and two genuine failures the tree had never
been able to report. Suspecting myself first is what sorted those four into three different kinds.
