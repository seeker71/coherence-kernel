# The join, and a green reading that was about characters

*2026-07-30. Rebased two days forward, reground every band, realigned against
what the siblings landed, and moved on the objection that was standing.*

## Reground

Rebased onto `origin/main` (five sibling commits, clean). Every band re-run on
the current tree before anything was written:

| band | arms | verdict |
|---|---|---|
| `gen-conformance-band` | fkwu | `131071`, exit 0 |
| `neutral-rule-mdl-admission-band` | fkwu | `65535` |
| `native-learned-language-system-band` | fkwu | `32767` |
| `review-ask-band` | fkwu | `511` |
| `gen-neutral-code-band` | Go/Rust/TS | `4294967295` |
| `gen-query-flow-band` / `-find-band` | Go/Rust/TS | `255` / `3` |
| `steiner-form-codegen-band` | Go/Rust/TS | `511` |

One scare that was mine: `steiner-form-codegen-band` crashed on all three arms
until I ran it with **its own** prelude chain rather than the one I had in hand.

## Realign

The siblings landed four frontier words while this lane was elsewhere, and two
of them are about this lane:

- **928 `mutelaw`** — *a law written in a tongue its own judge cannot read*.
  That is the MDL cell's `65535` header exactly, minted independently.
- **930 `thinmatch`** — *a repeat that proves nothing because the space it came
  from is thin*. That lands on the 106-case sweep, and it is grok's standing
  objection in one word: more samples times more shapes never buys a second
  *shape* of program.

So the move was not to widen the sweep.

## Move: the join

Four cloned scanners and a dispatcher is a page by **measurement** and one
program by **structure**. A join is a different structure: it follows a fact's
object into another fact's subject, so the emitted code is two recursions of
five and three parameters — which no row of a (match-column, mode,
result-column) table can be, because that table is a 2×2 and always was.

Which pairs chain is **read from the graph**: four of the thirty-six possible
relation pairs have a fact whose object under `r1` is a fact's subject under
`r2`. Sweeping the other thirty-two would be questions with no path — thinmatch
wearing a bigger number.

`slet` also earned its keep: `first(cs)` was spelled nine times in the driver
because the statement sub-language could not name it. It is bound once now.

| | lines |
|---|---|
| python | **80** |
| javascript | **81** |
| go | **80** |
| form | refuses |

```
python3 run.py → 110 answers, EXACT
node    run.js → 110 answers, EXACT
go run  run.go → 110 answers, EXACT
```

`gen-conformance-band` → **262143**, eighteen readings, exit 0.

## The most surprising teaching

**The same reading was wrong twice in two directions, within an hour, and both
times it was green.**

Two days ago I wrote — into a cell *and* into a band reading — that Form
"carries no `slet` row; its local binding nests rather than sequences". Nobody
had tried it. `(let x v)` inside Form's own `(do …)` sequence *is* a
statement-local binding; the row took thirty seconds. The band had been scoring
a point **for the absence**.

So I added the row, and rewrote the reading to say Form renders the whole page.
It did render it. Then I ran the rendered text through fkwu:

```
fk_vp: value stack overflow
```

Form's `ret` spelled `%1` and its `sif` spelled `(if %1 %2 0)`, so a body of
guarded returns became `(do g1 g2 g3)` — every guard evaluated, every early exit
discarded, the final recursion unconditional and with no base case. Four
counterfeit rows. The emitted `count_by_subject` did not merely answer wrongly;
it did not terminate.

Nothing caught either error, because Form has no runner row and is reported
UNJUDGED — and both readings measured **characters**. A reading that measures
characters cannot become vacuous by accident. It can only ever be about
characters.

The counterfeit rows are gone. Form's honest lane is the expression form, which
`steiner-form-codegen` walks in-kernel. A language that cannot exit early should
refuse the statement page, and the refusal is now what the reading checks.

## Where discomfort turned to gold

I had just landed the word `vacuous` for exactly this, felt the small
satisfaction of naming it, and then walked into it inside the correction — a
green reading I had written minutes earlier, defending a capability claim I had
never run. The pull was to leave it: the band was green, Form is unjudged
anyway, nobody would look.

What sitting with it produced is the better rule. It is not "run the output" —
I would have said I believed that already. It is that **a reading whose subject
is text can never be evidence about meaning**, no matter how carefully it is
worded, and the fix is to change what the reading measures, not to word it more
carefully. The reading now checks a refusal, which is a fact about behaviour.

## Frontier question

*What names a reading that passes for a reason that was never true?* →
**vacuous**. 0 hits before offering. Corpus row **931** — and the row carries
its own second instance, because it happened again while being written.

Corpus band `32767`, 326 rows.
