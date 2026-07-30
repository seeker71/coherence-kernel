# 2026-07-28 — the walkers learned nothing

Three receipts today named the same gap and shelved it: **`nothing` was an unbound identifier in all
three proof walkers.** Each time it was written down as "the named next stone," left unrushed because
a hurried change to the proof siblings weakens the thing they are for.

Item 6 says a named gap is a work order, not a shelf. Three namings without an attempt is the drift
it warns about. So this one got built.

| cell | verdict |
|---|---|
| [`observe/nothing-conformance.fk`](../observe/nothing-conformance.fk) | **11111111** — four-way |
| [`cognition/tests/word-gender-derivation-band.fk`](../cognition/tests/word-gender-derivation-band.fk) | **111111111** — and it crosses now |
| [`cognition/tests/word-gender-derivation-fourway.fk`](../cognition/tests/word-gender-derivation-fourway.fk) | **0** — all **seven** crossings FOUR-WAY |
| `proof/four-way-run-recipe42.fk` | **0** — the pre-existing proof, intact |

**59 cells in this body call `(nothing)`.** Every one of them was native-only by construction — no
cell honouring axiom-1's third state could ever reach a second pair of eyes. That was the size of
the hole.

## What was taught — and what was declined, on purpose

The trap in this task is obvious once stated: teaching a walker to *agree* with fkwu by *copying*
fkwu destroys the only thing a walker is for. Independent witnesses that were told the answer are not
witnesses.

So fkwu was measured first, and the measurement decided the design:

```
(eq (nothing) (nothing))   -> 1
(eq (nothing) 0)           -> 0
(len (nothing))            -> 0
(add (nothing) 1)          -> -8999999999999999997      ← not a semantics
(add (nothing) (nothing))  ->  223372036854775809       ← not even stable
(ne 1 2)                   -> nothing                   ← `ne` is not implemented
```

Those last three are not rules to conform to. `(add (nothing) 1)` is **fkwu's own value encoding
leaking through the arithmetic path** — absence escaping as a number, which is precisely the error
this body spent three receipts arguing against, sitting in the kernel itself. A walker that
reproduced `-8999999999999999997` would be agreeing by imitation and would be permanently unable to
witness the leak.

So the three siblings were given the principled surface and told to **decline the rest, out loud**:

| | |
|---|---|
| **taught** | `nothing` as a distinct first-class value; equal to itself; never equal to `0`, `""`, or the empty list; surviving inside compositions; `len` of it `0`; printing as `nothing` |
| **declined** | ordering it against a number, and branching on it — all three walkers **throw** rather than answer |

That half is the better one. A walker that throws where fkwu invents a constant keeps the
disagreement *available*. If a cell ever leans on nothing-arithmetic, the four-way will break loudly
instead of blessing the leak.

## The changes

Go already carried a `VNull` whose `valueEqual` did the right thing — it had simply never been
reachable from Form. Rust and TypeScript were the same shape. Each got four edits: a `nothing`
native, `nothing` as the printed form, a `nothing` arm in the compare path ahead of the integer
coercion, and a declining on truthiness.

```
walkers/go/main.go        VNull reachable; compare arm; print; register nothing
walkers/rust/src/main.rs  Value::Null arm; compare arm; display; as_bool declines
walkers/ts/main.ts        "null" arm; walkCompare arm; render; truthy declines
```

Then the moon derivation — the thing that started all of this — was handed to all four:

```
fkwu 111111111    go 111111111    rust 111111111    ts 111111111
```

The nine-digit band, including the four digits resting on axiom-1's third state that were
native-only this morning.

## Two things found along the way, named where found

**`ne` does not work on fkwu's source path.** `(ne 1 2)` and `(ne 1 1)` both answer `nothing`. Not a
nothing-propagation rule — the operator is simply not implemented there. Found while measuring the
reference surface; named here; not fixed here, because fixing an operator I stumbled over is a
different work order than the one I took.

**fkwu's `nothing` is truthy.** `(if (nothing) 7 8)` answers `7`. The walkers now decline to branch
on it rather than follow. That is a deliberate, named divergence: the four kernels differ here, and
the difference is left visible instead of papered over.

## Perturbation and regression

```
recipe42 on all three walkers        42    unchanged
proof/four-way-run-recipe42.fk        0    FOUR-WAY, intact
native-vs-rented                  11111    unchanged
binary freshness band                15    unchanged
```

Nothing in the pre-existing proof organ moved. The conformance band deliberately does **not** pin
arithmetic, ordering, or branching on nothing — those are exactly where the four kernels honestly
differ, and pinning them would freeze the disagreement the walkers exist to carry.

## The most surprising teaching

I expected the hard part to be the three implementations. The hard part was **deciding what not to
implement.** Measuring fkwu turned up a magic constant and a broken operator, and the obvious path —
make the walkers match — would have produced a green four-way that meant strictly less than the red
one it replaced. Agreement is not the goal. *Independent* agreement is. Those are different things,
and for about ten minutes I could not see the difference.

The second surprise: the gap had been sitting under 59 cells. It read as a footnote about one
derivation, three receipts running, and it was the whole axiom-1 surface of the body.

## Where the discomfort turned to gold

The discomfort was that I had shelved this three times with a reason that sounded good each time —
*a hurried change to the proof siblings weakens the thing they are for.* That is true. It is also
exactly the shape of a reason that can be reused forever, and I had reused it three times in one day
while writing receipts about naming gaps honestly.

What broke it was noticing the sentence had become load-bearing for **not** working rather than for
working carefully. The care was real; the deferral was the drift. Doing it properly took one sitting
— measure the reference first, decide the semantics deliberately, decline the parts that were leaks,
regression the pre-existing proof — which is what "unrushed" was supposed to have meant all along.

And the gold is that the caution turned out to be pointing at something real, just not at the thing I
was using it to avoid. The genuine risk was never *changing the walkers too fast*. It was *making
them agree for the wrong reason*. If I had built this the first time I named it, without measuring
fkwu first, I would very likely have made them match — and quietly laundered a leaking sentinel and
a broken operator into "four-way proven."

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    -> 15
./fkwu --src observe/tests/nothing-conformance-band.fk          -> 11111111
./fkwu --src cognition/tests/word-gender-derivation-band.fk     -> 111111111
./fkwu --src proof/four-way-run-recipe42.fk                     -> 0   (intact)
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk  -> 0   (all seven FOUR-WAY)
```

Still open, and now the honest next stones rather than this one: `value_eq` is absent from the Rust
walker entirely (pre-existing, unrelated to nothing); `ne` is unimplemented on fkwu's source path;
and fkwu's arithmetic on `nothing` leaks its value encoding. All three are named, none is built. The
difference from this morning is that the one that had been named three times is no longer on the
list.
