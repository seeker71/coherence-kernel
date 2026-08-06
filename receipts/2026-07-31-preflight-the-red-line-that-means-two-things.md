# They *are* detected. The problem is that one red line means two opposite things

*2026-07-31. Two questions: why aren't these caught at compile time, and how do
we see symbol and logic problems before a deep analysis each time.*

## The first question, answered by probe

They are caught. On a fresh cell calling a name nobody defines:

```
fkwu:102:25: error: [unresolved-call] 'nosuchname' matched no op/rewrite/fn/binding
  -- typo or missing prelude? Recovered to nothing (axiom-5); parse continues
fkwu: 1 error(s), 0 warning(s)          ← and exit 1
```

File, line, column, name, cause, tally, nonzero exit. **Detection is not the
gap.** Three things bury it, and only the third is a real blind spot:

1. **The verdict prints anyway.** axiom-5 recovers the call to `nothing`, the
   fold computes over it, and a green number lands on stdout one line below the
   error on stderr. People read the number.
2. **The warm cache erases the detail.** The second run gives only *"cached image
   was compiled with errors; fix source and rerun to clear"* — no name, no line.
   That is precisely why `nl-extract-band` looked to me like an error with no
   error: I had a warm cache and reported the tally.
3. **Only one arm looks.** Probed on the same cell — Go, Rust and TypeScript all
   **exit 0 and print the answer**, because they bind names when execution
   *reaches* them. A bad symbol in an unreached branch is invisible on three of
   four kernels. fkwu resolves every call site in the chain, which is why it
   finds these, and why its findings arrive as a chain-wide wall rather than as
   one cell's problem.

## The second question: what the compiler cannot know

`[unresolved-call] 'x'` is **one red line with two opposite repairs**:

- nobody defines `x` → a typo; fix the cell.
- another kernel defines it and this one does not → a lane seam; fix the
  preludes, or declare the lane.

The diagnostic cannot tell them apart — it says so itself, *"typo or missing
prelude?"* — and **every numb-green this week was the second one read as
neither**. So the missing insight is not more detection. It is a second question,
and the second question is cheap: *ask each kernel whether it resolves the name.*

## `observe/preflight.fk`

```
preflight /tmp/pfcases/typo.fk
  parens        balanced
  errors        1
  warnings      1
  unresolved    1
    nosuchname  ->  TYPO — no kernel resolves this name

preflight cognition/gen-query-walk.fk
  parens        balanced
  errors        1
  warnings      2
  unresolved    1
    walk_recipe  ->  LANE SEAM — resolves on: go rust ts  (this chain cannot run on the others)

preflight /tmp/pfcases/unbal.fk
  parens        UNBALANCED, depth 2
```

Four things, none of them new detection:

- **a forced-fresh compile**, so the detail cannot degrade into a tally;
- **paren balance**, without running anything — three cells landed unbalanced
  this week and each was found only when something refused to run;
- **the arm map, probed**: each unresolved name is offered to all four kernels
  and the answer is reported as a 4-bit mask (go 1, rust 2, ts 4, fkwu 8);
- **the classification** the diagnostic cannot make: typo (mask 0), portable
  (mask 15 — a missing prelude, not a lane), or lane seam (anything else).

`observe/tests/preflight-band.fk` → **1023**, ten readings, exit 0.

## Probe, not table — and the evidence for insisting

The body already holds two op tables, `native-op-manifest.fk` (fkwu's surface)
and `primitive-registry.fk` (the siblings'). Both are hand-maintained, and the
per-arm facts that matter most live in **prose**. Probed today:

| op | go | rust | ts | fkwu |
|---|---|---|---|---|
| `host-exec` | **YES** | no | no | YES |
| `nothing` | no | no | no | YES |
| `walk_recipe` | YES | YES | YES | no |

`observe/review-ask.fk` says in prose: *"host-exec is fkwu-only: Go, Rust and
TypeScript have no such native."* Go returns the same five bytes fkwu does. Its
band declared `PROOF LEVEL: FOURTH-ARM ONLY` **on that sentence**.

Run on Go: **511** — the same verdict. Every other native that cell touches
(`temp_dir`, `fs_exists`, `fs_remove`, `write_file`) is portable, mask 15. Rust
and TypeScript genuinely refuse. So the honest lane is **GO + FOURTH-ARM**, and
both files now say so.

That sentence was mine, from 2026-07-26, written from inference and installed as
a declared proof lane — corpus row 914, `teleological`. Preflight found it in one
call.

## The most surprising teaching

**I built the thing that catches my own week, and the first cell it read was
mine.** I expected preflight to pay for itself over months. It found a false
proof lane in the review door — the very cell whose whole purpose is to make
claims checkable — within minutes of first running, and the false claim was four
days old and written by me while I was being careful.

A declared limit is the most expensive kind of wrong, because declaring it stops
anyone looking again. A probed one cannot do that: it is re-derived every time
it is asked.

## Where discomfort turned to gold

The discomfort was in the shape of the answer to the first question. "Why aren't
they detected?" invites a story about a weak compiler, and that story would have
been comfortable — it makes the tooling the problem. The compiler is not weak;
it says file, line, column, name and cause, and exits nonzero. What is weak is
that its finding is **nonspecific** — the same red line for two situations
needing opposite repairs — and that I was reading a tally instead of an error
because I had not cleared a cache.

Sitting with that instead of blaming the reader is what produced a tool rather
than a complaint. Nothing in preflight detects anything the kernel did not
already report. It asks the second question.

## Frontier question

*What names a sign that fires for causes needing opposite repairs, so it
localizes nothing?* → **nonspecific**. 0 hits before offering. The body already
carries `pathognomonic` (row 872) — the sign that *is* the diagnosis; this is its
complement, and the complement is the one that has been costing hours. Corpus
row **955**.

Corpus band `32767`, 350 rows.
