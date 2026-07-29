# The panel read the exit status I did not

*2026-07-28. Four reviewers on the codegen stack. One of them ran the band I
had already reported green — and read the line under the number.*

## What was asked

Review `cognition/gen-neutral-code.fk`, `gen-query-flow.fk`, `gen-conformance.fk`
and `steiner-form-codegen.fk` against `NORTH_STAR.md`, and name the three
highest-leverage next steps. Two tensions were put to the panel directly:
whether leaning on `python3`/`node`/`go run` as judge builds toward retiring
oracles or entrenches them, and which language, added next, would most falsify
"adding a language is adding a row".

Asked Form-natively — `(ra-review path question)` — four doors in parallel,
4m03s, every answer on disk before the call returned.

## The finding that cost the most

Codex ran `./fkwu --src cognition/tests/gen-conformance-band.fk`. It printed
`255`. It exited **1**:

```
error: [unresolved-call] 'walk_recipe' matched no op/rewrite/fn/binding
error: [unresolved-call] 'walk_recipe' matched no op/rewrite/fn/binding
fkwu: 2 error(s), 0 warning(s)
```

Confirmed here before believing it. Yesterday's receipt reported that verdict
as a pass; I had read the number and not the status.

The cause is not a typo. `walk_recipe` is Go/Rust/TS and absent from fkwu;
`host-exec` is fkwu's and absent from the siblings. The conformance cell needs
host-exec, so it runs on fkwu — and it preludes `gen-neutral-code.fk`, which
carried a `walk_recipe` call site in a function it never calls. **fkwu resolves
every call site in a prelude chain, not only the reached ones.** Another arm's
door became this arm's error, recovered to `nothing` by axiom-5, and the fold
went green because no reading touches those sites.

Healed structurally rather than silenced: the arm-neutral half of each cell
stays, and the sibling-only half moves out.

| cell | holds | lane |
|---|---|---|
| `gen-neutral-code.fk` | tables, renderer, vocabulary walkers | ARM-NEUTRAL |
| `gen-neutral-walk.fk` | IR→recipe, `walk_recipe`, its band check | SIBLINGS ONLY |
| `gen-query-flow.fk` | query table, emitters, statement form | ARM-NEUTRAL |
| `gen-query-walk.fk` | closed recipe, eight agreements, both checks | SIBLINGS ONLY |

Verdicts after the split, all re-run: `4294967295` / `255` / `3` on Go, Rust
**and** TypeScript; conformance `255` on fkwu, **exit 0**.

## The second finding, which was a claim of mine

Claude traced what `gcf-expected` actually computes. Yesterday's receipt said
the toolchain is "compared against the DATA, not against another execution
engine's opinion of it." That is false. `gcf-count-loop` and `gcf-find-first`
are a **third hand-written implementation** of the same four query semantics,
after the graph's hand accessors and the emitters. Same hand, same
understanding, three channels. If the semantics drifted they would drift
together and print green.

Corrected in the cell itself, where the false sentence stood.

## Located and standing, verified line by line

- `gnc-expr-kinds` / `gnc-stmt-kinds` (`gen-neutral-code.fk:600-601`) are two
  hand-authored lists. A used-vocabulary walker exists for *ops* and *builtins*
  and was never written for *kinds* — the fifth instance of the declared-vs-used
  blindness the conformance cell's own header names four times.
- `gnc-sub1` returns the shape unchanged when the mark is absent (`:361`), so a
  shape missing its `%3` silently drops a child. No shape-arity check exists.
- Go's return type is baked into the `fnb` shape string (`:325`), not in the
  type table.
- `gcf-facts-lit` branches on `(str_eq (gcf-lang r) "go")` in code, not in a
  table — "adding a language is a row" holds inside `gen-neutral-code.fk` and
  leaks one layer up.

## Where the four agreed

Unanimously, on both tensions:

1. **Do not retire `python3` as the standard of Python.** The body cannot become
   the definition of a language it does not own. What is retirable is the
   oracle's *other* role — finder of generator bugs. Split the lanes.
2. **The weakness is not language count.** It is that nothing runs the
   *exported* IR — the one with facts as a parameter — inside the body. The
   in-kernel path walks a different, closed lowering. The two stories have not
   met.
3. **Falsify before inflating n.** Python statement form with real layout
   attacks a named hole with an existing runner; Rust and Ruby are easy rows
   wearing hard clothes (read-only integer recursion never meets ownership).

## The most surprising teaching

**Four-way agreement is blind to exactly the failure this stack has.** The four
kernels catch a bug one kernel would hide — that is redundancy against
*independent* error. Three implementations of one semantics written by one mind
are not independent channels; they are one channel drawn three times. The body
had the discipline and no word for what the discipline cannot see.

## Where discomfort turned to gold

The discomfort was reading `255` in Codex's answer next to `exit 1` and knowing
I had shipped that number yesterday as a pass. The reflex was to call it
cosmetic — the readings are real, the toolchains really ran. Sitting with it
instead of dismissing it is what surfaced the actual defect: not a stray
warning, but a prelude chain carrying a door its arm does not have. The split
that healed it is the same lane separation all four reviewers named as step one.

## Frontier question

*What names the failure where independent checks fall together because one mind
wrote them all?* → **common-mode**. 0 hits before offering. Named in reliability
engineering since the 1970s. Corpus row 921.

## Files

| file | state |
|---|---|
| `cognition/gen-neutral-walk.fk` | new — sibling-only walk half + band check |
| `cognition/gen-query-walk.fk` | new — sibling-only closed recipe + agreements |
| `cognition/gen-neutral-code.fk` | arm-neutral; walk half removed, header healed |
| `cognition/gen-query-flow.fk` | arm-neutral; walk half removed, header healed |
| `cognition/gen-conformance.fk` | false "compared against the DATA" claim corrected |
| three band files | preludes repointed; verdicts unchanged |
| `learn/homecoming-distillation-corpus.fk` | +row 921 (common-mode) |
