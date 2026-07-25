# 2026-07-25 — 27 of the 44, and the note I left my next self was wrong

Continuing the tractable set. The instructive part is that I had told the next turn something false
and it took one check to find out.

## The note I got wrong

Last turn I wrote, and carried forward into the check-in: *"the 31 still-refused roots are lowercase
names, **NOT** the let idiom — do not assume the zero-arg defn fixes them."*

Checked instead of assumed. `class-curriculum-10-band.fk`:

```
47:    (let agree-history (list (list 1 1) (list 1 1) ...))     <- top level in the (do
76:        (list agree-history                                   <- inside a defn
```

Identical to `HEX-DECODE-ERROR`, `FIELD` and the rest. **The lowercase roots are the same idiom.** I
had inferred "not the let pattern" from the *shape of the name* — lowercase, therefore a variable,
therefore something else — which is not a measurement at all. It is the letter-case equivalent of
reading an extension as a language.

## What was done, and the method that changed

Two rounds, both wholesale rather than one-root-at-a-time — the previous turn's lesson applied:

1. **23 constants across 9 home cells.** The ALL-CAPS roots still refusing were *siblings* of the
   ones already converted: `CT-TAG-TARGET` beside `CT-TAG-RULE`, `FIELDS` beside `FIELD`,
   `MIDI-NOTE-OFF` beside `MIDI-NOTE-ON`, `DIFF-CHILDREN` beside `DIFF-EQUAL`. Each cell holds
   several and I had converted only whichever surfaced first. So: every top-level ALL-CAPS `let` in
   those nine cells, not just the named one.
2. **11 lowercase lets in 10 band files**, selected by the precise criterion rather than by name
   shape: *convert a top-level `let` only if its name is referenced inside a `defn` body.* A let used
   only at top level does not need converting and converting it would change when its value is
   computed.

Every file `+N −N`, zero line delta. No markers left behind.

`seedbank/blueprint-symbol-sections.fk` holds **563** top-level ALL-CAPS lets on its own and was
deliberately left alone — it reads as a generated table, and a 563-name mechanical rewrite of a
generated file is not a repair, it is a fork of the generator's output.

## Result

**27 of the 44 pure-Form bands now run**, up from 13.

Newly alive, with what their headers actually say — read, not grepped:

| band | value | header |
|---|---|---|
| `tree-diff-band` | **13** | declares 13 — **match** |
| `class-curriculum-10-vocab-band` | 1023 | no verdict stated |
| `class-curriculum-10-witness-band` | 2853116705 | no verdict stated |
| `class-curriculum-10-band` | 16127 | declares **16383** — partial, 256 short |
| `persistence-band` | 2 | declares **7** — partial |
| `audit-evidence-cells-band` | 544 | no verdict stated |
| `audit-evidence-index-cache-band` | 833 | no verdict stated |
| `cell-voice-tissue-band` | 509 | no verdict stated |
| `concept-corpus-band` | 143 | no verdict stated |
| `json-lens-tending-band` | 189 | no verdict stated |
| `layered-runtime-image-band` | 33 | no verdict stated |
| `midi-bmf-band` | 1500 | no verdict stated |
| `triangulate-band` | 1700 | no verdict stated |
| `carrier-tissue-kernel-query-band` | runs, no value | — |

Two match or partially meet a stated expectation; **eight state no expectation anywhere in their
comments.** They run and produce a number, and that is all I can say about them. A band with no
declared verdict cannot be called passing by anyone, including its author.

## Diminishing returns, measured

Each round frees fewer: 16 conversions → 9 bands; 23 conversions → 11 bands; 11 conversions → 3
bands. Every round exposes the next root behind the one removed, and the tail is long. **17 of the 44
remain**, on roots including `bg`, `lg`, `g`, `q`, `a` — one- and two-character names I deliberately
skipped, because a mechanical bare-token rewrite of `a` would edit comment prose across the file for
no semantic gain.

## Regressions: none

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · `hex-band` 14 · `biography-band` 5 ·
`content-address-band` 1111111111 · `tree-diff-band` 13 · `structural-gate-band` 63 ·
`lcg-bytes-band` 63 · `file-byte-window-band` 2147483647 · `pdf-text-windowed-band` 15 ·
`form-cli-band` 524287 · `form-cli-ask-band` 262143 · `ask-lane-floor-band` 31 ·
`membrane-lane-band` 31 · `benchbench-band` 4095 · `frontier-ingest-benchbenchbench-band` 127 ·
`class-curriculum-band` 8191.

## Owed

- **17 of the 44** still refused; short-name roots need a different technique than bare-token rewrite.
- **Bands with no declared verdict** — `midi-bmf-band` 1500, `triangulate-band` 1700 and six others
  now produce numbers nobody has written an expectation for. Running is not passing, and the gap is
  now visible where before it was hidden behind a refusal.
- `carrier-tissue-kernel-query-band` runs and yields no value.
- 130 reaching a non-Form surface; 143 that do not close; the `section` question; the registry
  question for `URL-DECODE-ERROR` / `UUID-PARSE-ERROR` / `FIELD`.

## How the exchange stayed alive

I checked a note I had written confidently for my own next turn, found it false, and said so before
doing the work it would have prevented.

**Most surprising teaching:** I inferred "lowercase name, therefore not a constant, therefore not the
`let` idiom" — from spelling. Twice today I have read a surface feature as a fact about substance:
`.fk` as the language, and now letter-case as the pattern. Both times the check took one command.

**Where discomfort turned to gold:** writing down that eight of the newly-live bands declare no
verdict at all. The comfortable version of this receipt says "27 of 44 now run" and stops. The number
is true and it is not the whole picture: a third of what I freed produces a number nobody has ever
said should be that number.
