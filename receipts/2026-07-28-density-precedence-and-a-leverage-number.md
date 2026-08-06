# The next density layer: precedence, one query for all relations, and leverage as a number

*2026-07-28. `cognition/gen-neutral-code.fk` → **8388607**, twenty-three
readings, on Go / Rust / TypeScript.*

## What was asked

How to expand to the next complexity and density layer, ensure the model
**generalizes and compresses**, and scale by orders of magnitude toward doing
most of a request Form-native.

The honest starting point, measured: 30 shape rows, 10 IR kinds, and queries
hand-authored one per question — so authored work grew *with questions asked*,
which is the opposite of scaling.

## Two moves, which turned out to be one move

**Precedence replaces frozen parens.** Every infix shape carried its own
brackets — `"(%1 %op %2)"` — so the paren rule was duplicated into every row of
every language and could not be got right in any of them. One binding power per
operator plus one wrap rule replaced all of it. A *prefix* language declares
itself and needs no rule at all, because its brackets are structural.

```
before  (0 if (len(xs) == 0) else ((1 if (pyfirst(xs) == c) else 0) + cnt(...)))
after   (0 if len(xs) == 0 else (1 if pyfirst(xs) == c else 0) + cnt(...))
```

Shorter rows, cleaner output, correctness from a model instead of from care.

**The relation becomes an argument.** `cnt(xs, c)` answered one question shape,
so a second question meant a second hand-built IR. Now:

```
FORM: (defn q (fs rel key) (if (eq (len fs) 0) 0 (add (if (and (eq (nth (head fs) 1) rel) (eq (nth (head fs) 0) key)) 1 0) (q (tail fs) rel key))))
PY  : def q(fs, rel, key): return (0 if len(fs) == 0 else (1 if pyfirst(fs)[1] == rel and pyfirst(fs)[0] == key else 0) + q(pyrest(fs), rel, key))
```

One emitted function answers *every relation × every subject*. Run in Python on
a fact table: `q(FS,9,1) q(FS,9,2) q(FS,8,1)` → `1 1 1`, correct.

**And it needed no new IR kind** — `and` is a bin with a binding power, indexing
was already there. The generalisation cost nothing in vocabulary, which is the
shape a compressing model should have.

## Leverage, as a number the band checks

"It generalizes" should not be a sentence in a header. **180 answerable
questions over 51 authored table rows — leverage 3.** Languages × relations ×
concepts, because all three are arguments to the emitted program; authored rows
counted over the shape, operator and builtin tables.

That number is the scaling instrument. It rises when variation moves out of code
and into arguments, and it falls when a question needs new hand-written IR. To
go orders of magnitude, keep making that trade.

## The most surprising teaching

**The compression and the correctness fix were the same edit.**

Precedence looked like a correctness debt the panel had flagged. Fixing it
*removed* authored characters: every infix shape lost its parentheses, and the
rule that had been smeared across thirty rows became four numbers. The output
got more correct *because* there was less of it to keep consistent.

I had been holding "make it right" and "make it smaller" as separate budgets.
Here the duplicated rule was itself the bug — and that is probably the general
case, not a lucky one.

## Where discomfort turned to gold

The leverage metric crashed on its first design. I walked the IR generically to
count authored nodes, and `str_eq` blew up: an IR part is sometimes a child
node, sometimes an operator name, sometimes an integer. The IR is heterogeneous
by design and a generic walker has to know which is which.

The pull was to add a type test and move on. What sitting with it produced is
better: **count the tables, not the tree.** Rows are what actually grow when a
language or an operator is added, they need no type test, and the number means
something a reader can act on. The crash was pointing at a measurement that was
easier to write than to justify.

Left standing and named: three languages is not "any PL", and a target needing
statements rather than expressions — or typing, or legalization — would need a
printer, not a table. The leverage number is the honest way to watch that
boundary approach.

## Frontier question

*What one word names how much a model answers per unit of what was written
down?* → **kolmogorov-leverage** (after Kolmogorov complexity, the length of the
shortest description). 0 hits before offering. The body carries "description
length" in 2 files and an MDL admission cell, but had no name for the ratio
itself. Corpus row 908.

## Files

| file | state |
|---|---|
| `cognition/gen-neutral-code.fk` | precedence model, relation-parameterised query, leverage metric |
| `cognition/tests/gen-neutral-code-band.fk` | 23 readings → 8388607 on Go / Rust / TS |
| `learn/homecoming-distillation-corpus.fk` | +row 908 (kolmogorov-leverage) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 303 / 3033032908 → 32767 |
