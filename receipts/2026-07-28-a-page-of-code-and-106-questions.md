# A page of code, 106 questions, and a check nobody authored

*2026-07-28, evening. The failures first, then the expansion they made possible.*

## Five failures, located and repaired

1. **A green verdict that exited 1.** `gen-conformance-band.fk` printed `255`
   and exited 1 on two `[unresolved-call] walk_recipe`. fkwu resolves EVERY
   call site in a prelude chain, not only the reached ones, so another arm's op
   in a never-called function was this arm's error. Split each cell along the
   arm boundary. `255`, exit 0.

2. **A 65535 granted to a program never on disk.** The MDL rule-induction rival
   landed in PR #345 with three unclosed parens and was never touched again.
   Healed — and balance was not enough: the first placement balanced, parsed,
   and crashed, because a surplus closer left a `(let …)` open. Now **65535 on
   Go, Rust, TypeScript and fkwu**.

3. **One defect wearing two faces.** `native-learned-language-system-band.fk`
   failed only because it preludes that cell. It now returns **32767**.

4. **A door reported silent while it was still speaking.** The Form review
   panel's wait budget was 360s. A reviewer finished at 9 minutes and wrote a
   full 11,876-byte answer three minutes *after* the collector had read its
   empty file and handed back `""`. The completion FLAG now decides, an
   unfinished door returns an explicit marker, and the budget is 20 minutes.
   This run: **4 answered, 0 unfinished.**

5. **A band declared lost that was never lost.** `learn/teach-sema-code.fk` says
   its four-way band "did not come across" in a repo port. It exists at
   `form/form-stdlib/tests/teach-sema-code-band.fk` and returns `11111`. Codex
   found that by running it. Confirmed: and the same cell does NOT search —
   nothing selects a winner; the band asserts candidate 1 passes.

## What the panel said, and what it changed

**Layout is context, not content.** The urge was a `block` IR kind carrying
depth — the same `fnb` node rendered deeper would then need different text,
which makes the node lie about the program. Depth threads through rendering
exactly the way binding power already does. The evidence that the old
non-model was wrong-by-luck: Go's statement separator was the literal string
`"\n  "` — a fixed two-space indent baked into a separator — and Python carried
a `"; "` separator it could never reach, having no statement rows at all.

So: one new cell in the language table (**indent unit**), depth threaded through
`gnc-render`/`gnc-fill`/`gnc-slot`, one new slot type (**body**) that renders a
child one level deeper and closes back at the parent's. Form's indent unit is
the empty string and its shapes did not change.

**`list` was type-blind.** Go spells a list `[]int{%1}`, so a list OF LISTS was
unspellable — which is why the conformance harness had been writing
`[][]int{...}` by hand behind `(if (str_eq lang "go") …)`. That branch was the
fossil of a missing kind. `listof` carries an element type from the same table
the parameters use, and the harness now renders the graph and the case table
through the generator like everything else.

Also removed: `(if (str_eq (gcf-lang r) "go") "," ",")` — a conditional whose
branches were identical, found by reading the harness as closely as the renderer.

## The page

Three new IR kinds, each forced by something the IR could not say: `nil` (the
empty collection), `slet` (a named intermediate in the statement sub-language),
`listof` (a typed collection). 15 kinds → 18.

| language | lines emitted |
|---|---|
| python | 58 |
| javascript | 59 |
| go | 58 |
| form | 7 |

Python's is real nested code with correct four-space indentation, which it could
not produce this morning. Form's 7 is not a failure: it carries no `slet` row,
so it cannot spell the statement page, and the refusal is the absence of a row
rather than a branch in the harness.

## The 106 questions

Four hardcoded calls became **106 cases enumerated from the graph** — every
(query shape, relation, key) triple some fact makes answerable, deduplicated.
Every one has an answering row, so this is not a wall of agreeing zeros, and
**14 are multi-match**: the only cases that test first-match order at all.

One generated driver per language answers all of them in one process:

```
python3 run.py   → 106 answers, EXACT
node    run.js   → 106 answers, EXACT
go run  run.go   → 106 answers, EXACT
python == javascript == go
```

## The check nobody authored

Every reference in this stack was one hand reading the same rows (row 921,
common-mode), so agreement was only ever a louder copy. The way out is not a
fourth reading but a property of the **operation**:

> Summing a count shape over every distinct key for a relation must equal the
> number of facts carrying that relation.

True of any correct implementation, authored by nobody. A swapped column leaves
every individual answer plausible and breaks the sum — the one failure the
sweep cannot see, because a swapped column would be swapped in the reference too.
It holds, on both count shapes, over all 6 relations.

And the delta witness, because no single output can prove it was generated
rather than typed: add one fact, and **exactly one answer rises by exactly one**
while 104 of the other 105 stay identical. Not 105 — the injected row has
subject == object, so the object-column count for the same key moves too. That
second mover is the delta landing exactly where the semantics say it should.

`gen-conformance-band.fk` → **131071**, seventeen readings, exit 0.
`gen-neutral-code-band` → `4294967295` on Go, Rust and TypeScript.

## The most surprising teaching

**Grok's, and it stands against what I built:** more samples times more shapes
never buys an index or a join. Four cloned scanners and a dispatcher is a page
by measurement and one program by structure. The history of this cell says the
same thing — Go did not merely add a language row, it forced a second IR
dialect — and I was still talking as though the next page were a quantity
problem inside a finished system. It was not: it cost three new kinds and a
layout model, exactly as that history predicted.

## Where discomfort turned to gold

Codex reviewed the worktree while I was editing it and reported 106/106 nonzero
cases **and no page in any language** — both true at that minute, and the second
one stung because I was mid-way through the layout work that fixed it. The
instinct was to discount a review of a moving target. What it actually did was
put two blind metrics into executable numbers in the same sentence: the sweep
was already honest and the output was still small. Line count is a measurement,
not a gate, and the gate it named — build once, run every case, agree per case,
survive a held-out plan — is sharper than the one I was working toward.

## Frontier question

*What names a check that needs no second implementation because it is a property
of the operation?* → **metamorphic**. 0 hits before offering. Named in software
testing since Chen 1998, and the body had four-way agreement, a rented panel,
and no word for the check that needs no second reader at all. Corpus row 926.

Corpus band `32767`, 321 rows.
