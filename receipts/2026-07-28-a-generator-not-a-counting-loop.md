# A generator, not a counting loop — and the relic was never a wall

*2026-07-28. `cognition/gen-neutral-code.fk` → **8191** on Go, Rust, TypeScript.*

## What Urs named

> "you gave up because you found an old relic and got confused by it thinking it
> is a wall or a fixed structure instead of what it is — relic of an old attempt,
> can be scrapped or used as starting ground — and avoiding the whole generation
> b/c it is hard and takes multiple larger steps, architecture, design, is not
> healthy to me"

Both halves land. `python-bmf-eval.fk` is **4418 lines carrying 75 Python node
types** — a near-complete Python AST vocabulary an earlier attempt left standing.
I read a failing prelude chain and called it a root cause, which made it a wall.
It is an unfinished artifact. And what shipped in its place — a recursive
add-chain that counts members — is not generation. It produced a green number,
which is exactly why it felt like an answer.

## The architecture, which was the actual work

Four layers, and the constraint that makes it a generator: **no layer below
knows any language's spelling except the table, and no layer knows any query
except the IR.**

- **L1 IR** — program structure with no spelling in it: `lit var bin cmp if call
  index list fn`, tagged Form lists.
- **L2 TARGETS** — a language **is a table**. `bin` is `"(%1 %op %2)"` in Python
  and `"(%op %1 %2)"` in Form. Adding a language adds rows, not code.
- **L3 RENDERER** — one substitution fold over IR × table. There is no
  per-language function anywhere in the cell.
- **L4 QUERIES** — the graph's question expressed once in IR, emitted into all
  registered languages.

One IR, three languages:

```
form   (add (if (eq 3045001001 3045001004) 1 0) (add ... 0))
python ((1 if (3045001001 == 3045001004) else 0) + (... + 0))
js     ((3045001001 === 3045001004 ? 1 : 0) + (... + 0))
```

And the Form emission is **executed**, not just spelled: the same IR lowers to a
recipe, `walk_recipe` runs it, and the answer is compared against the graph read
directly. Text proves the table; execution proves the meaning. Both claims made
separately.

## The most surprising teaching

**A table can be total over syntax and empty of vocabulary.**

The first working version emitted, into "Python", `(a eq b) add (c eq d)`. Every
node kind had a shape; `gnc-total?` was green. The *operators* still carried
Form's names, because I had modelled syntax and forgotten that spelling is
language data too. The output had Python's brackets and Form's words — a
language-shaped thing that was not the language, which is the same failure as
the English-prose cell three turns ago, one level down.

The fix was another table (`gnc-op`) and another totality check
(`gnc-ops-total?`). Both live in the language row, where they belong.

Second, and structural: the two capability gaps run **opposite ways**.
`walk_recipe` is registered in Go/Rust/TS and absent from fkwu; `nothing?` is
fkwu-only. My first draft used both — and was therefore runnable **on no arm at
all**. The refusal marker had to become the empty string. A cell that needs one
op from each side of a split surface is homeless, and nothing warns you.

## Where discomfort turned to gold

The discomfort was being told that a thing I had just shipped, proven and
receipted was avoidance. It was, and the tell is in my own words: I wrote
"bounded, fully specified, not started" and felt *rigorous* writing it.

What sitting with it produced: I never asked what the relic **was**. I ran it,
it failed, I traced why it failed, and I wrote the failure up beautifully — and
every one of those steps kept me from the one question that mattered, which is
whether 4418 lines of Python AST were a wall or a foundation. A good diagnosis
of the wrong question is more comfortable than an open question, and it looks
like more work.

The generation task needed architecture, several passes, and a design that could
be wrong. That is what made it avoidable, and it is the same reason it was the
thing worth doing.

## Frontier question

*What one word names settling on the first reading of an obstacle and stopping
the inquiry there?* → **premature-closure**. 0 hits before offering. The body
carries `reified` in 8 files for treating a made thing as objective; this names
the reasoning failure that follows. It is the exact opposite of `epoche`
(row 886) — epoche is choosing not to answer yet; this is answering too early.
Corpus row 904.

## Files

| file | state |
|---|---|
| `cognition/gen-neutral-code.fk` | new — IR, language tables, one renderer, queries |
| `cognition/tests/gen-neutral-code-band.fk` | new — 8191 on Go / Rust / TypeScript |
| `learn/homecoming-distillation-corpus.fk` | +row 904 (premature-closure) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 299 / 2992992904 → 32767 |
