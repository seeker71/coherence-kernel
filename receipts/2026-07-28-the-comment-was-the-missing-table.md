# The comment was the missing table

*2026-07-28. `cognition/gen-neutral-code.fk` → **67108863**, twenty-six
readings, on Go / Rust / TypeScript. Leverage 3 → 9.*

## What the panel was asked, and what it said

Not a defect hunt — a design question: how to make the model *less specific and
more data-driven*, so structure comes from data and one model serves more
concepts. Four doors; three answered; two independently named the same top move.

claude located it precisely, and the sting is that the cell had already written
the spec:

> `gnc-render` is a ten-armed if-chain, and each arm hardcodes that kind's slot
> semantics… The cell's own comment names the gap: *"a part is sometimes a child
> node, sometimes an operator name, sometimes an integer — a generic walker has
> to know which is which."* That "which is which" is **a missing table, stated
> as a reason to give up on the generic walker instead of as the next row-set.**

I wrote that comment last turn, as a justification for measuring tables instead
of walking the tree. It was the specification for the thing I wasn't building.

And the sentence that reframes the whole "learned" question:
**"you can induce rows, you cannot induce if-chains."**

## What was built

**A kind-signature table.** One row per IR kind naming what each slot *is*:
`child`, `op`, `bi`, `raw`, `int`, `kids`, `names`. The renderer became the one
generic fold the header had been claiming — it walks the signature and fills
`%1..%N` in slot order. `gir-kinds` and `gnc-binlike?` now **derive** from those
rows instead of being two more hand-maintained mirrors.

**Proof the claim is real, not asserted.** A new IR kind — `let` — was added
*after* the fold, as one signature row and one shape row per language, with
**zero renderer edits**. Its slot pattern `(raw child child)` matches no
existing kind, so it could not have worked by accident:

```
form: (do (let x 1) x)
py  : (lambda x: x)(1)
js  : ((x) => x)(1)
```

**The counts left the code.** grok caught `gnc-relations-n` and
`gnc-concepts-n` hardcoded as `6` and `10`. Both are now counted from the graph
— and the real concept count is **27**, not 10. A leverage number resting on two
magic constants cannot move when the graph does.

Leverage **3 → 9**: 486 answerable questions over 54 authored rows.

## The most surprising teaching

**I had written the design and read it as an obstacle.**

The comment was accurate, complete, and pointed exactly at the table that was
missing. What made it an excuse rather than a plan was the sentence it ended in:
*so count the tables instead*. One clause turned a specification into a
justification, and nothing in a green band could tell the difference — the
metric it justified was itself correct.

The general form: a diagnosis written in the voice of a constraint stops being
read as a task. I have done this three times this session — "bounded, fully
specified, not started", "pre-existing, not mine", and now this — and each time
the prose was true and the framing was the problem.

## Where discomfort turned to gold

Asking the panel a *design* question rather than a defect question felt like
inviting a rewrite of work I had just finished and was pleased with. It was
exactly that.

What made it bearable is the thing that made it useful: the answer was already
in my own file. The reviewers did not bring outside knowledge — claude quoted my
comment back at me. That is a cheaper kind of help than I expected, and a
sharper one: most of what I need to see next is usually already written down in
the voice of a reason it cannot be done.

## Frontier question

*What one word names the property of a structure whose variation lives in data,
so it can be learned rather than rewritten?* → **inducible**. 0 hits before
offering. The body had no word for the property that decides whether a structure
can be learned at all — whether its variation sits in rows or in control flow.
Corpus row 911.

## Files

| file | state |
|---|---|
| `cognition/gen-neutral-code.fk` | signature table, generic fold, `let` kind, derived counts |
| `cognition/tests/gen-neutral-code-band.fk` | 26 readings → 67108863 on Go / Rust / TS |
| `learn/homecoming-distillation-corpus.fk` | +row 911 (inducible) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 306 / 3063062911 → 32767 |
