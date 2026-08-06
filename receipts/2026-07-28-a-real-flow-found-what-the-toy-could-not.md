# A real flow found what the toy could not

*2026-07-28. `cognition/gen-query-flow.fk` — count lane **255** on Go / Rust /
TypeScript, find lane **3** on Go / Rust.*

## From sample to flow

The toy was one invented query — count members — over 35 facts. The real
question is what queries the graph *actually needs*, and that is answerable by
reading the accessors already hand-written against it:

| hand-written | shape |
|---|---|
| `sne-members-of` | count where relation=R, **subject**=K |
| `stn-senses-in` | count where relation=R, **object**=K |
| `sne-state-of` | find the **object** where relation=R, subject=K |
| `sne-mediator-of` | find the **subject** where relation=R, object=K |

Four shapes, **found rather than designed**. Two axes fall out — which column is
matched, and count-versus-first-match — and every hand-written accessor in the
graph is one cell of that 2×2. So the query shape became a table too, which is
the move the panel ranked second: four rows and one builder replace four
hand-written functions, and a fifth question is a fifth row.

Twelve programs from four query rows and three language tables. Each generated
query is checked against the hand-written accessor it replaces, **on the real
graph**, and the whole flow runs end to end — emitted Python answering real
questions over the real fact table:

```
count_by_subject 3           members of the threefold soul
count_by_object  4           senses that perceive another being
find_object      3045007003  the state of willing → asleep
find_subject     3045002010  the mediator of the polarity → Christ
```

## The most surprising teaching

**The toy could not have found this, because counting never returns an id.**

The moment a real query returned a node id, the three kernels split. Node ids in
this body are ~3.0×10⁹, and the TypeScript kernel's recipe walker is **I32 by
design** — `form-kernel-ts/src/compiler.ts:268`, `int: n | 0`, with line 423
explaining it keeps V8's SMI tagging. So `3045007003` came back as
`-1249960293`. Minimal repro:

```
(walk_recipe (intern_trivial_int 3045007003))
  go 3045007003 / rust 3045007003 / ts -1249960293
```

Plain arithmetic on TS is fine; it is the recipe path only. And this is
**body-wide, not local to this cell**: every `lnsi-node` graph lands above 2³¹ —
`giles-light-hubs` sits at `3041xxxxxx` — so any of them walked as a recipe
truncates on that arm.

The band scored 253 of 255 while this was live, and the one question that
*passed* on TypeScript passed for the wrong reason: it expects 0 when nothing
matches, and truncation was returning garbage on the others while that one still
looked right. A partial pass is how a real defect hides.

Second, and pleasing: **the generated Python is more faithful than one of the
kernels that generated it.** Python carries the full ids; the TS recipe walker
cannot.

## Where discomfort turned to gold

The reflex on seeing `ts 253` was to find the bug in my cell. I changed the
recipe's `if` condition to an explicit comparison first — a plausible fix for a
truthiness difference, and wrong. It stayed 253.

What broke it was printing the *value* instead of the verdict: `-1249960293`
against `3045007003`, a difference of exactly 2³². The arithmetic named the
cause in one line, and no amount of reasoning about my own code would have.

Then the harder choice: the constraint is a deliberate design decision in
another kernel, not a bug I should quietly route around. Hiding it would have
meant a green band over a real limit. **The lanes are split instead** — count
rides all three arms, find declares Go and Rust and carries the repro in its
header — so the limit stays visible every run.

## Frontier question

*What one word names two parts that each work and cannot carry what the other
produces?* → **impedance-mismatch**. 0 hits before offering. The body carries
"word size" in 3 files and "saturating" in 8, but had no name for the class.
Corpus row 913.

## Files

| file | state |
|---|---|
| `cognition/gen-query-flow.fk` | new — query-shape table, 4 real shapes, 12 programs, real-graph checks |
| `cognition/tests/gen-query-flow-band.fk` | new — count lane, 255 on Go / Rust / TS |
| `cognition/tests/gen-query-flow-find-band.fk` | new — find lane, 3 on Go / Rust, constraint declared |
| `learn/homecoming-distillation-corpus.fk` | +row 913 (impedance-mismatch) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 308 / 3083082913 → 32767 |
