# 2026-07-28 — identity is not magnitude

Urs: *"core primitives probably should only be eq, lt, since the rest can be derived from it, right?"*

Right. And the reason it stops at **two** rather than one is axiom-1 — which is a better answer than
the arithmetic alone gives.

| cell | verdict |
|---|---|
| [`observe/comparison-primitive-reduction.fk`](../observe/comparison-primitive-reduction.fk) | measured, not recalled |
| [`observe/tests/comparison-primitive-reduction-band.fk`](../observe/tests/comparison-primitive-reduction-band.fk) | **11111111** — four-way |

## What the kernel carries today

```
le   tag 5     PRIMITIVE   (native-op-manifest.fk row)
eq   tag 102   PRIMITIVE   (rewrite row lowering straight to a native tag)
lt   tag 103   PRIMITIVE   (rewrite row lowering straight to a native tag)
gt, ge, ne, not, and, or, abs        DERIVED
```

**Three comparison primitives. Two suffice.**

## `le` is redundant — measured, not argued

```
le(a,b) = if(lt b a) 0 1
```

Verified against the builtin on every ordering case. Tag 5 is carrying what tag 103 already
generates. A real shrink target for the seed, named and left unbuilt: `le` is wired into the
flattener and JIT carrier paths as well as the source walker, so removing it deserves its own pass
rather than riding along on a measurement.

## On numbers, `lt` alone generates everything — including `eq`

```
le(a,b) = if(lt b a) 0 1     gt(a,b) = lt(b,a)     ge(a,b) = if(lt a b) 0 1
eq(a,b) = if(lt a b) 0 (if (lt b a) 0 1)
```

All four verified against their builtins. And fkwu's `lt` orders **strings** too, so the derivation
covers those as well.

Taken at face value that says the floor is **one** primitive — one further than the question
proposed. Which is where it would have been easy to stop and hand back a clever answer.

## Why it is two: the measurement that decides it

The eq-from-lt derivation works on `nothing` only because `nothing` has a numeric encoding that
sorts below 0 — **the value-encoding leak this body refused to replicate into the walkers earlier
today.** So the derivation was tested where that matters:

```
eq derived from lt, on nothing:   fkwu 1    go THROWS    rust THROWS    ts THROWS
real eq,            on nothing:   fkwu 1    go 1         rust 1         ts 1
```

**The derived `eq` is not four-way.** Three of four kernels decline to order `nothing` against
anything — on purpose, because ordering it is not a number question. That refusal was written into
them this morning for entirely different reasons, and it is what catches this.

So the one-primitive floor is reachable only by riding a leak. It buys a smaller kernel at the cost
of the proof organ.

## The floor is two, and axiom-1 is why

```
eq   answers IDENTITY    — is this the same cell?
lt   answers MAGNITUDE   — is this one below that one?
```

**Axiom-1's third state is precisely a thing with identity and no magnitude.** `nothing` is equal to
itself and has no position in any ordering. One operator cannot answer both questions about it.

So the floor is two — not by taste, not by convenience, but because the state space has **two kinds
of question in it**. Urs named the right pair, and the justification turns out to be the first
axiom rather than an aesthetic preference about minimal cores.

## The same separation, one layer down

This is the shape the whole day kept finding, now in the kernel's own op table:

- a property of the **word-form** is not a property of the **referent** (`der Mond`)
- **frequency** is not **truth** (the two contracted passages)
- **identity** is not **magnitude** (`eq` and `lt`)

Three altitudes, one distinction. The last one is the one the machine itself is built out of.

## The lane

Every row was **measured on this checkout**, not recalled: each derivation against its builtin, and
the four-way behaviour of both `eq` forms. The cell carries the four-way facts as data rather than
re-running the nothing-ordering, because executing that would throw on three of the four kernels the
cell is meant to cross — the cell would destroy its own crossing by testing the thing it reports.

## The most surprising teaching

**The arithmetic said one and the axiom said two, and the axiom was right.** I could have derived
`eq` from `lt`, watched every numeric and string case pass, and reported a one-primitive kernel as a
clean result. It would have been correct on every test I would naturally have written.

What caught it was running the derivation against `nothing` on the walkers — and the walkers only
had an opinion there because of a decision made this morning for an unrelated reason: to throw
rather than imitate fkwu's leak. **A refusal recorded hours earlier, in a different investigation,
is what made this measurement possible.** That is what an honest boundary is worth: it pays out
later, in a place nobody was looking.

## Where the discomfort turned to gold

The temptation was to be cleverer than the question. Urs said two; the arithmetic said one; and
"actually you can go one further" is a satisfying thing to say and would have been *defensible on the
numbers*. It is the same shape as every other pull today — a plausible result, in the format that has
been earning trust, that nobody would have checked.

The gold is that testing the clever answer instead of shipping it produced something better than
either position: **the floor is two, and axiom-1 is the reason.** Urs's number was right and his
justification was not yet stated; mine would have been wrong for a reason that looked like rigour.
Measuring closed the gap and gave the pair a foundation neither of us had.

## Ground stamp

```
./fkwu --src observe/tests/comparison-primitive-reduction-band.fk  -> 11111111  (four-way)
./fkwu --src form/form-stdlib/tests/ne-operator-band.fk            -> 11111
./fkwu --src form/form-stdlib/tests/eq-shape-band.fk               -> 524287
./fkwu --src bootstrap/ground.fk                                   -> 42
./fkwu --src proof/four-way-run-recipe42.fk                        -> 0
```

Named and unbuilt: `le` (tag 5) is one comparison primitive over the floor, derivable as
`if(lt b a) 0 1`, and removing it is a flattener + JIT-carrier pass, not a source-walker edit.
