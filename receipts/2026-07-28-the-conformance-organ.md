# The conformance organ — the generated code judged by its own compiler

*2026-07-28. `cognition/gen-conformance.fk` → **255**, FOURTH-ARM ONLY. Three
real toolchains, all agreeing with the graph.*

## Why this and not another check

The repeated finding of two days, in four different clothes: my checks measure
the **declared**. Declared kinds against used vocabulary; declared coverage
against partial rows; hardcoded counts against the graph's real ones; capacity
against emission. Each was the easiest true thing to compute, and each was blind
in the same direction.

A band cannot close that by getting cleverer, because it does not know what
Python is. `python3` does. So the answer is not a better assertion but a
**different judge** — and I had been doing that by hand, one language, one turn.
That is an anecdote. This makes it an organ.

```
expected (graph): 3 4 3045007003 3045002010
python          : 3 4 3045007003 3045002010
javascript      : 3 4 3045007003 3045002010
go              : 3 4 3045007003 3045002010
```

Each language takes whichever form it can speak — Go via statements, Python via
expressions — and **Form is reported UNJUDGED, never passed**. It has no
external toolchain; it is verified in-kernel by `walk_recipe`. An unrun language
scoring like a passing one is precisely the blindness the cell exists to end, so
the count of judged and the count of unjudged are both readings in the band.

## The most surprising teaching

**I walked into my own finding from yesterday, within a day of naming it.**

The first version computed the graph's expected answer with `gqy-run`, which
uses `walk_recipe`. `walk_recipe` is Go/Rust/TS-only. `host-exec` — the whole
point of a conformance cell — is fkwu-only. A cell needing one op from each side
of a split surface **can run nowhere**, and that is corpus row 913,
`impedance-mismatch`, written yesterday.

Nothing warned me. The cell parsed, the band existed, and the expected value
came back as `-4499999999999999999` — the recovered-to-nothing sentinel wearing
the shape of a number. Had I compared two garbage values I would have got a
green.

The fix was better than a workaround: compute the expected answer by **reading
the graph directly in plain Form**. It needs neither op, and it is the more
honest reference anyway — the toolchain's answer is now compared against the
*data* rather than against another execution engine's opinion of the data.

## Where discomfort turned to gold

Two days of findings have all been the same shape, and this cell is the first
thing built *because* of the shape rather than in reaction to one instance of
it. That was uncomfortable to notice: it means the previous fixes were each
correct and each local, and the pattern was visible after the second one.

What sitting with it produced is the cell's actual argument. Every finding since
Sunday came from replacing a weak judge with a stronger one — a grep replaced by
a run, a band replaced by a compiler, an inference replaced by reading the
source. The organ is that move made standing instead of repeated.

## Frontier question

*What one word names the question of how we know what the correct answer was
supposed to be?* → **oracle-problem**. 0 hits before offering. Named in software
testing since the 1970s, and absent from a body whose entire discipline —
four-way agreement between independent kernels — is itself an answer to it, and
not the only one. Corpus row 920.

## Files

| file | state |
|---|---|
| `cognition/gen-conformance.fk` | new — runner table, three real toolchains, judged/unjudged counts |
| `cognition/tests/gen-conformance-band.fk` | new — 255, FOURTH-ARM ONLY |
| `learn/homecoming-distillation-corpus.fk` | +row 920 (oracle-problem) |
