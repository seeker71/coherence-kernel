# The panel verified the generator, and each answer opened the next layer

*2026-07-28. `cognition/gen-neutral-code.fk` → **262143**, eighteen readings, on
Go / Rust / TypeScript. The emitted Python and JavaScript run.*

## Using the door on our own work

The Form-native panel (`ra-review`) was pointed at the generator, four doors, 282
seconds. All four answered; three found the same class of defect independently.

**claude** — the renderer *never refuses*. A child with a missing shape returned
`""` and the parent spliced it, yielding `(x + )`. The cobol check passed only
because cobol has *no* rows, so the **root** collapsed; a language with a
**partial** row set produced holes silently. And the final else treated *any*
unknown kind as bin/cmp, so an IR constructor forgotten from `gir-kinds` stayed
green in `gnc-total?` and mis-rendered.

**claude / codex** — the query **compiled the database into the program text**,
one `if` per fact, because the IR had no way to name data at runtime. Data
wearing a program.

**grok / codex** — no precedence model, parens frozen into shapes; totality
measured on the wrong product.

## What was built from that

- **Refusal propagates.** Any empty part makes the whole empty; a partial
  language now yields nothing instead of a hole.
- **Unknown kinds refuse** rather than falling through to bin/cmp.
- **The data left the program.** The query is now a recursive function over a
  parameter, and `gnc-data-leaked?` checks that the coordinate prefix `3045`
  appears nowhere in the emitted text:

```
FORM: (defn cnt (xs c) (if (eq (len xs) 0) 0 (add (if (eq (head xs) c) 1 0) (cnt (tail xs) c))))
PY  : def cnt(xs, c): return (0 if (len(xs) == 0) else ((1 if (pyfirst(xs) == c) else 0) + cnt(pyrest(xs), c)))
JS  : function cnt(xs, c) { return ((jslen(xs) === 0) ? 0 : (((jsfirst(xs) === c) ? 1 : 0) + cnt(jsrest(xs), c))); }
```

- **A runtime prelude ships with the code**, because Python and JavaScript have
  no `first`/`rest` and the generator had been emitting calls into thin air.
- **Conformance**: `python3` and `node` were run on the emitted modules. Both
  answer `3` for `cnt([1,2,2,3,2], 2)`. The Form output is verified the other
  way — `walk_recipe`, inside the kernel.

## The most surprising teaching

**Three rounds, and each fix exposed the same-shaped gap one level deeper.**

The syntax table was total over node kinds and **empty of operator spelling** —
so "Python" came out carrying Form's words: `(a eq b) add (c eq d)`. Fixed with
an operator table. That table was total and **empty of builtins**. Fixed with a
builtin table. That table was total and **emitted calls to functions that did
not exist**. Fixed with a runtime prelude.

Every layer was complete by its own measure and incomplete by the next one's,
and each looked finished from inside — which is exactly why a four-way green
band saw none of them. `gnc-total?` was green through all three.

## Where discomfort turned to gold

The discomfort was watching a green 8191 turn out to be three defects deep, on a
cell I had just written *in response to being told I was avoiding the hard work*.
The reflex was to feel that the correction had not landed.

What sitting with it produced is the more useful reading: the correction landed
exactly, and this is what the hard work looks like from inside. Multi-step
architecture does not arrive finished; it arrives with the next layer's gap
already in it, invisible until something outside your frame looks. That is not a
failure of the design — it is the reason a review door is worth having, and the
reason "I proved it four ways" was never going to be enough on its own.

Left standing and named, not smoothed: **precedence is still frozen into the
shape strings**, so a language needing real operator precedence would need a
printer, not a table. codex is right that this is a multi-target expression
printer with a compiler's ambitions. It says so now.

## Frontier question

*What one word names a check complete by its own measure and blind to the next
layer?* → **local-completeness**. 0 hits before offering. The body carries
`unfalsifiable` and `barnum` for claims that cannot fail; this names a check
that *can* fail and still cannot see. Corpus row 906.

## Files

| file | state |
|---|---|
| `cognition/gen-neutral-code.fk` | expanded — refusal, builtins, runtime prelude, data-out-of-program |
| `cognition/tests/gen-neutral-code-band.fk` | 18 readings → 262143 on Go / Rust / TS |
| `learn/homecoming-distillation-corpus.fk` | +row 906 (local-completeness) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 301 / 3013012906 → 32767 |
