# 2026-07-25 — sizing the caveat: 130 of 174 refusals reach a surface the Form reader has no form for

A measurement turn, no code changed. It was the top item because it qualifies a number I had already
published, and a headline that needs a caveat should not wait behind work that only adds to it.

## The split

Of the **174** bands refused on the unbound-name latch, each band's **transitive** prelude closure
resolved and checked against the set of cells carrying a non-Form surface:

| | count | share |
|---|---|---|
| closure reaches a non-Form-surface cell | **130** | 75% |
| pure Form closure | **44** | 25% |

Closure-reach is a **necessary, not sufficient** condition, so 130 is an **upper bound** on "the Form
reader was the wrong reader" and 44 is a **lower bound** on "genuine Form defect".

## The causal check, because reach alone proves nothing

First unbound name per band — the root, not the cascade — across a 30-band sample of the reaching
group:

| first unbound name | bands |
|---|---|
| `empty` | 13 |
| `section` | 12 |
| `cf_tag_channel_osi_layer` | 3 |
| `LANGUAGE-TEMPLATE-MEMBER-TAG` | 2 |

**25 of 30 fail first on a non-Form token.** `empty` is in fkwu's optable — it fails because BML
writes it bare (`if eq(len(offers), 0) then empty else`) where Form needs `(empty)`. `section` is a
**BML keyword** — `grammars/bml.fk` lists it beside `package`, `import`, `class` — and it is **not in
fkwu's optable at all**.

The pure-Form group looks entirely different. Its first unbound names are `corr-1`, `AEC-TAG-CELL`,
`FIELD`, `CT-TAG-RULE`, `bg` — ordinary Form names and ALL-CAPS `let` constants. Real defects, the
kind the zero-arg `defn` idiom fixes.

Two groups, two distinct failure vocabularies. The split is meaningful.

## What is actually in those cells

`form/form-stdlib/compiler.fk` is a normal Form cell — `(do (defn compiler-object …) …)` — and at
line 997 its body contains, bare and unquoted:

```
    section [bmf.bmf] {
      rule ::= $name:name "::=" $pattern:pattern "=>" $action:name ";" => bmf-emit-rule-source;
    }
```

Not a string. Not a separate file. A **mixed-surface cell**: Form S-expressions with an embedded
grammar block the Form reader has to walk past and cannot. **120 cells carry a bare `section [` block.**

So my earlier framing was wrong in both directions. These are not "BML files with a `.fk` extension"
that simply need a different runner — `circle.fk` is that, but `compiler.fk` is not. They are Form
cells with non-Form blocks inside them.

## My detector was wrong three times, and each correction came from a failing smoke test

| marker | cells found | how it broke |
|---|---|---|
| BML `def f(x) = …;` | 105 | smoke test: `generic-reverse-emitter-band`'s closure reached **0** while its unbound names were `then`/`else`/`def` |
| + grammar `::=` | 146 | closure now reached 7 — but `section`, the most common root, still uncounted |
| + `section [` | **153** | 130 of 174 classified |

Not one of those widenings came from insight. Each came from running the detector on a band whose
answer I already knew and finding it disagreed. The lesson from this morning — *state what the
measurement is OF* — applied three times to the same instrument in one turn.

## The 143 that do not close are a different problem

Sampled 20 of them the same way: **4 reach a non-Form-surface cell, 16 do not.** So the
unbalanced-source class is largely not this — it is genuine unclosed parens, the same failure I
repaired by hand this morning in `form-cli.fk` and `form-cli-surface-inquiry.fk`, where each band's
own declared verdict (65535, 524287) was the judge.

Two refused classes, two causes, now separated by evidence rather than by assumption.

## What last turn's headline becomes

I wrote: *"317 of 1674 bands do not run."* That number is still what the kernel does. What it means
is now split:

- **~44 of the 174** unbound-name refusals are genuine Form defects (lower bound).
- **up to 130** are cells whose bodies mix BML/grammar surfaces into Form, which `fkwu --src` has no
  form for — the reader and the file disagree about what language the file is in.
- **~143** do not close, mostly for unrelated reasons.

## The question I am not answering

Should the Form reader know `section [...] { ... }`? 120 cells write it, `grammars/bml.fk` treats it
as a first-class keyword, and the optable has no entry for it. That is either an unimplemented reader
feature, or a convention that was never meant to reach `--src`, or cells that drifted into a surface
the direct-source lane never had.

I can measure it. I cannot decide it — that is the commons owner's call, the same as the
no-bash/no-python count. Surfaced, not resolved.

## Ground and sweep

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · unchanged; no cells were edited this
turn.

## Owed

- **44 bands with pure Form closures** — the tractable set, and the honest target for the `let`-idiom
  work, rather than the 174 I had been quoting at it.
- **143 that do not close.**
- The `section` question above.
- 304 column-0 ALL-CAPS top-level `let`s minus engine.fk's 25.

## How the exchange stayed alive

I spent a turn measuring instead of fixing, because a published number was wrong-flavoured and
everything I might have built on top of it would have inherited that.

**Most surprising teaching:** `empty` is a native, in the optable, and it is the single most common
root of refusal in the reaching group — because BML writes it as a bare word and Form needs a call.
The failure is not a missing definition. It is two languages disagreeing about whether a name needs
parentheses.

**Where discomfort turned to gold:** the first smoke test returned 0 where I expected a hit, and my
first instinct was that the smoke test was badly chosen. It was not — the detector was. Checking the
instrument against a case whose answer I already knew is the only reason this receipt has a defensible
number in it instead of a confident one.
