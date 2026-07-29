# The cheat, named — and the real path found but not walked

*2026-07-28. Structure band still **511** four ways. `steiner-py-native.fk`
built, **not run**, and that is the honest state.*

## What Urs named

> "using host-exec during generation is cheating and avoids the actual learning
> how to code task"

He is right, and the cheat is sharper than shelling out. The emitter did not
write Python. It concatenated a template **I** wrote by hand, interpolated the
graph's numbers into it, and then asked `python3` whether my template worked.
The round-trip agreed, so it *felt* like proof. What it proved is that the
template is faithful to the graph. It proves nothing about the body having a
model of Python — and execution-checking is precisely what lets a generator skip
having semantics at all: imitate the form, let the interpreter judge.

That is why the first PL projection was a tuple dump and the second was a
template: neither required understanding the target language, and nothing in the
proof structure demanded it.

## The real path exists, and the body already had it

`form-stdlib/grammars/python-bmf.fk` carries Python's constructs and
`form-stdlib/python-bmf-eval.fk` walks them back to values — `PY-BMF-DEF`,
`CALL`, `WHILE`, `SUBSCRIPT`, `LIST`, `MODULE`. The body holds Python as a
**model**, not a string. The non-cheating shape is: build the program as
structure out of that model, and evaluate it with the body's own evaluator. No
`python3`, no `host-exec`, nothing outside asked to judge.

`cognition/steiner-py-native.fk` composes exactly that — a real program, not a
template with holes:

```
data = [...]        # the graph's own subject column
total = 0 ; i = 0
while i < n:
    if data[i] == concept: total = total + 1
    i = i + 1
total
```

Every node is a `PY-BMF` constructor applied to graph data. Change the graph and
a different program is built.

## Where it stopped, diagnosed to root

**It does not run**, and I am not going to imply otherwise.

`form-stdlib/compiler.fk` sits in the evaluator's declared prelude chain and is
a **BML brace-surface file** (it carries a `section` header). A BML file needs
the source compiler's explicit text lens before a walker can read it as
executable Form, and neither `fkwu --src` nor a bare sibling invocation applies
that lens. Under `--src` the chain reports **85 unresolved-call diagnostics and
produces no value**; on the Go arm it stops at `unbound identifier "section"`.

The evaluator's **own** band fails identically, is absent from
`fourth-arm-bands.txt`, and declares no `PROOF LEVEL` — so it has no working
lane today either. This cell did not break that; it walked into it. What remains
is staging that chain through the source compiler's text lens: bounded, fully
specified by the paragraph above, and not started.

**No band is offered for that cell**, because a band would have to assert
something unwitnessed.

## The most surprising teaching

**The proof shape decided the design.** I did not choose a template over a
model deliberately — the template was what the available proof could check.
Once "run it and compare" was the test, any construction that passed became
acceptable, and understanding became optional. The cheapest thing that satisfies
the check is what gets built, so the check silently specifies the work.

The same pattern produced the English-prose cell two turns ago: a four-way green
band could not see that the "structure" was English all the way down, because
nothing asked it to render in a second language. Both times the artifact was
exactly as good as its hardest test, and no better.

## Where discomfort turned to gold

I had a green round-trip — graph, Python, execution, agreement — and it was
genuinely satisfying. Being told it was cheating landed badly precisely because
the numbers matched.

Sitting with it: the numbers matching was the problem. A correct answer obtained
by outsourcing the judgement teaches the body nothing, and worse, it *closes the
question* — a red result would have sent me looking for the Python model, and a
green one told me I was done. The evaluator I needed had been in
`form-stdlib/` the whole time, and I would not have gone looking for it, because
`python3` had already said yes.

## Frontier question

*What one word names reproducing a form correctly while understanding none of
it?* → **cargo-cult**. 0 hits before offering. The body carries `oracle-guided`
across 46 files for the technique of letting an outside judge decide; it had no
word for the failure mode that technique invites. Corpus row 893.

## Files

| file | state |
|---|---|
| `cognition/steiner-py-native.fk` | new — composes Python as PY-BMF structure; BUILT, NOT RUN, root named |
| `cognition/steiner-neutral-emit.fk` | header corrected — relabelled a template, not "the graph writes code" |
| `learn/homecoming-distillation-corpus.fk` | +row 893 (cargo-cult) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 288 / 2882882893 → 32767 |
