# Merge, continue, expand, witness, learn

*2026-07-28. `gen-neutral-code.fk` **4294967295**, `gen-query-flow.fk` **255**,
both on Go / Rust / TypeScript. The generated Go compiles and answers correctly.*

## Merge

Rebased onto main, two commits behind. And the merge immediately taught
something: `gen-query-flow` came back **223 instead of 255**.

## Continue — and the band was red for the wrong reason

The failing reading was `gqy-emitted-n`, pinned at 12. It now read 16. But the
number was `queries × languages` — a **product of two counts**, which cannot
observe an empty cell of the grid it multiplies. Go had just been registered, so
the product said 16 while Go rendered **none** of the four queries: it has no
expression-form shapes.

So the metric was reporting *capacity* and being read as *output*, and it went
red on a stale pin rather than on the real gap. That is luck, not detection.
Replaced by asking each cell whether it actually emitted.

## Expand

The real gap the honest count exposed: Go had four queries it could not write.
Given the statement sub-language from the last turn, the fix was a statement
form of the same four query rows — no new query rows, no new IR kinds. Every
language is now covered **by one form or the other**, and the band checks that
rather than checking a product.

## Witness

All four generated queries, compiled and run by the **real Go toolchain** against
the real 35-fact graph:

```
generated Go : 3 4 3045007003 3045002010
the graph    : 3 4 3045007003 3045002010
```

Including both node ids above 2³¹ — the values the TypeScript kernel destroyed
until this morning.

## Learn — the most surprising teaching

**A product of counts is structurally blind to a hole.**

`queries × languages` can only ever describe a full grid. The moment one cell is
empty the number is wrong in a way no amount of care in *reading* it can catch,
because the emptiness never reaches the arithmetic. The metric did not degrade
when Go arrived — it had always been describing a shape rather than a fact, and
Go was simply the first case where shape and fact came apart.

This is the fourth time in two days that a measurement counted the declared
rather than the actual: declared kinds vs used vocabulary, declared coverage vs
partial rows, hardcoded 6-and-10 vs the graph's real counts, and now capacity vs
emission. The pattern is not carelessness — each measure was the easiest true
thing to compute at the time.

## Where discomfort turned to gold

The band caught this by going red, and I nearly "fixed" it by updating the pin
from 12 to 16 — which would have been correct arithmetic, a green band, and the
permanent loss of the finding. The stale pin was the only reason I looked at the
metric at all.

What stopped me was the standing rule about witnessed facts: before changing a
pin, probe what it should be. Probing gave 16, and asking *what are those 16*
gave the answer that four of them do not exist. **The habit that saved it was
not scepticism about the metric — it was refusing to write a number I had not
measured.**

## Frontier question

*What one word names counting what could happen as though it had?* →
**energeia** (Aristotle's actuality, against *dunamis*, potentiality). 0 hits
before offering. The body carries `affordance` in 5 files for what a thing makes
possible, and had no word for the error of counting that as though it had
occurred. Corpus row 919 — landing beside row 918 `pseudo-oracle`, which arrived
the same day from another line and names a test grounded in the world rather
than in another implementation. This one names a measurement grounded in
neither.

## Files

| file | state |
|---|---|
| `cognition/gen-query-flow.fk` | real emission count; statement form of all four query rows |
| `cognition/tests/gen-query-flow-band.fk` | the 32-bit reading now checks emission and per-language coverage |
| `learn/homecoming-distillation-corpus.fk` | +row 919 (energeia) |
