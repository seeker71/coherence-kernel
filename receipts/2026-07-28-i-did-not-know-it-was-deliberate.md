# "How do you KNOW it was deliberate?" — I didn't

*2026-07-28. `form-kernel-ts/src/kernel.ts` fixed; the find lane runs on all
three arms; the declared lane it was hiding behind is gone.*

## The claim I made

An hour earlier I wrote that the TypeScript kernel truncates node ids **"I32 by
design"**, cited `compiler.ts:268` (`int: n | 0`) and a comment on line 423
about keeping V8's SMI tagging, and split a band's proof lane to declare that
TypeScript could not carry the value.

Urs asked how I knew it was deliberate.

## I didn't

Tracing it properly:

- `walk_recipe` calls `walk()` in **kernel.ts**. It never touches `compiler.ts`.
  The file I cited was not on the code path.
- The comment I quoted was about arithmetic emission, a different function from
  the line I blamed.
- The actual loss is `kernel.ts:765`, `internTrivialInt`, packing the value into
  the NodeID's 32-bit `inst` slot with **no overflow branch**.
- Go has that branch, and states the intent: *"inline while the value fits the
  32-bit inst slot; overflow into `i64s` once it crosses the int32 ceiling …
  Both paths decode back to Value{VInt, int64}, so callers and arithmetic never
  see the storage split."* Rust agrees with Go.

I read a `| 0` and a nearby comment from an unrelated function and assembled
them into an intention. **A behaviour one of three siblings has is a bug** — the
siblings exist to make exactly that visible, and I used the divergence as
evidence of design instead of as the signal it is.

## Fixed

`internTrivialInt` now mirrors Go: inline under the int32 ceiling, overflow into
the existing `i64s` table above it. The machinery was already there —
`internTrivialInt64` and the table both existed; nothing routed to them.

```
(walk_recipe (intern_trivial_int 3045007003))
  before  go 3045007003 / rust 3045007003 / ts -1249960293
  after   go 3045007003 / rust 3045007003 / ts  3045007003
```

Regression-checked across the kernels: `core-word-ack`, `truth-arrival`,
`core-lexicon`, `offer-ack-core`, `knowledge-ingest`, `minimal-surface`,
`core-grounding`, `format-arith`, `llama-numerics` — all PASS-4WAY, no
divergences.

The find lane now reads **3 on Go, Rust and TypeScript**. The band header says
what it was and why it was wrong, rather than quietly reading as though it had
always been three.

## The most surprising teaching

**I had installed a defect as a law.**

The lane split was not a neutral act of bookkeeping. Writing "PROOF LEVEL: GO
AND RUST ONLY" into a band makes the limitation *structural* — every future run
is green, the constraint is documented, and nobody looks again. I did that
within minutes of finding a real bug, and I felt careful doing it, because
declaring a limit honestly is usually the right move.

The difference between an honest limit and an installed defect is only whether
the limit is real. Nothing in the form of the act tells you which one you are
doing.

## Where discomfort turned to gold

Four words undid an hour of work I was pleased with, and the correction was not
about the engineering — it was about a load-bearing verb. "Deliberate" did all
the work in that sentence, and I had no evidence for it.

What sitting with it produced: I reach for intent as an explanation when a
mechanical trace would be more work. Reading purpose into an artifact *feels*
like understanding, and it terminates inquiry exactly as fast as "it's a wall"
did two turns ago. The tell is the same both times — a conclusion that makes the
next step unnecessary.

And the specific engineering lesson, which I want kept: **divergence between the
siblings is evidence of a bug, never of a design.** Three kernels that agree on
everything else and differ here is the system working as intended, and I read it
backwards.

## Frontier question

*What one word names reading intention into something that merely turned out
that way?* → **teleological**. 0 hits before offering. The body carries
`confabulation` for inventing a memory; this names inventing a **purpose**,
which is harder to catch because the artifact really does have the property you
are explaining. Corpus row 914.

## Files

| file | state |
|---|---|
| `form/form-kernel-ts/src/kernel.ts` | `internTrivialInt` gains the overflow branch Go and Rust have |
| `cognition/gen-query-flow.fk` | the "by design" claim retracted, the trace recorded |
| `cognition/tests/gen-query-flow-find-band.fk` | lane removed — 3 on all three arms |
| `learn/homecoming-distillation-corpus.fk` | +row 914 (teleological) |
