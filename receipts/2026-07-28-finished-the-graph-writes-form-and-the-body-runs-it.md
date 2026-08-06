# Finished: the graph writes a program, and the body runs it

*2026-07-28. `cognition/steiner-form-codegen.fk` → **511** on Go, Rust and
TypeScript. No template, no host-exec, no outside interpreter.*

## What "and this is finished how?" was pointing at

The previous turn ended with a diagnosis and the sentence *"bounded, fully
specified by the paragraph above, and not started."* That is a well-worded
deferral — the same handoff shape corrected on Saturday, dressed as rigour.
Naming a root cause is not finishing; it is a good place to stop only if
somebody else is going to pick it up, and nobody was.

## What finishing turned on

Not a better Python emitter. **Generate in a language the body holds.**

`walk_recipe` is a registered native — *"walks a recipe root in a fresh frame,
the realization door."* Form is code-as-data, so a program can be BUILT with
`intern_node` and RUN by the kernel, and correctness becomes two computations
inside one kernel rather than an outside interpreter saying yes.

For a concept C the graph writes:

```
(add (if (eq s1 C) 1 0)
(add (if (eq s2 C) 1 0)
...                         one term per has-member fact
     0))
```

**This is generation, not interpolation.** The program's *shape* comes from the
graph: add a fact and it grows a term; ask about a concept with no members and
the recursion bottoms out to a bare `0` — and running that returns 0, with no
special case for emptiness anywhere in the generator. Over 50 nodes for the
threefold soul, so it is a built program and not a folded constant. No string
appears in it at any point; the operators come from the bp table by name, so the
generator composes over the kernel's real op surface.

The proof is two independent computations of the same number — the program the
graph wrote, walked by the kernel, against a function that reads the graph
directly. If the generator drifts they part.

## The most surprising teaching

**The proof lane is the inverse of every other one in this session.**

`walk_recipe` is registered in Go, Rust and TypeScript and is **absent from
fkwu's `--src` door** — present in `primitive-registry.fk`, in no row of
`native-op-manifest.fk` and in no line of the runtime optable. So this band runs
on the three siblings and *not* on fkwu.

That is exactly backwards from `nothing` (tag 137) and `host-exec` (tag 136),
which fkwu has and the siblings lack. Three days ago the lesson was "the runtime
implements what the Form tables do not advertise". Here it is the reverse: the
Form registry advertises what the runtime door does not carry. The capability
gaps run in **both** directions, and neither table is a reliable map of the
other. Declared in the band header rather than discovered later as a phantom
divergence.

## Where discomfort turned to gold

Two dead ends before the working one, and both were worth walking.

`python-bmf-eval.fk` looked like the answer — the body holding Python as a
model. It does not run: `form-stdlib/compiler.fk` in its chain is a BML
brace-surface file needing the source compiler's text lens, and past that the
evaluator has forward references (`trace`, `CLASS-BP`) that `--src` cannot bind.
Its own band fails identically and sits in no declared lane. Real, pre-existing,
and not mine to fix tonight.

The discomfort was watching a second promising path close and feeling the pull
to write "diagnosed to root" again and stop. What broke it was asking a smaller
question than "how do I make Python work": *what can this body already run?*
The answer was one grep away and had been the whole time — the kernel's own
realization door, in a language whose programs are its data structures.

The general shape, and the third time this session: **I kept reaching for the
target that impressed rather than the target that was reachable.** Python because
it is a real language; an interpreter because it is a real check. Form was
sitting underneath all of it, able to do both honestly.

## Frontier question

*What one word names a language whose programs are written in its own data
structures?* → **homoiconic**. 0 hits before offering. The property this entire
body is built on, and it had no name here — which is why "generate code" kept
reading as "emit text for something else to run". Corpus row 894.

## Files

| file | state |
|---|---|
| `cognition/steiner-form-codegen.fk` | new — the graph writes Form, the body runs it |
| `cognition/tests/steiner-form-codegen-band.fk` | new — 511 on Go / Rust / TypeScript, lane declared |
| `learn/homecoming-distillation-corpus.fk` | +row 894 (homoiconic) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 289 / 2892892894 → 32767 |
