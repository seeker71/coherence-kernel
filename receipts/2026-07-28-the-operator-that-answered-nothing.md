# 2026-07-28 — the operator that answered nothing

`ne` did not work. Not "was subtly wrong" — **it answered `nothing` for every input**, and had been
doing so for as long as the source path has existed.

Found while measuring fkwu's reference surface in order to teach the walkers axiom-1's third state.
It was named in that receipt and left as a next stone. This is that stone.

| cell | verdict |
|---|---|
| [`form/form-stdlib/tests/ne-operator-band.fk`](../form/form-stdlib/tests/ne-operator-band.fk) | **11111** — four-way |
| `form/form-stdlib/tests/eq-shape-band.fk` | **524271 → 524287** — the value its own header documents |
| `runtime/fkwu-uni.c` | **unchanged.** Zero lines |

## The defect, and the line that shows it

```
(ne 1 2)              -> nothing      should be 1
(ne 1 1)              -> nothing      should be 0
(if (ne 1 2) 42 0)    -> 42           looks right
(if (ne 1 1) 42 0)    -> 42           should be 0   ← the whole defect in one line
```

`ne` matched no op, so it recovered to `nothing` (axiom-5's honest recovery, working as designed).
And **`nothing` is truthy in fkwu** — measured yesterday, and left as a named divergence. Put those two
facts together and every `(if (ne x y) A B)` takes the then-branch **regardless of x and y**.

A comparison operator that always says yes.

## Three bands were green on top of it

- `form/form-stdlib/tests/form-eval-full-band.fk` — asserts `(if (ne 1 2) 42 0)` → 42. Green. Would
  have been green with `ne` inverted, deleted, or replaced by anything at all.
- `form/form-stdlib/tests/float-natives-band.fk` — same shape, same reason.
- `form/form-stdlib/tests/eq-shape-band.fk` — this one is worse and better. Its c4 row is
  `(if (eq (sub (ne 1 2) (ne 2 2)) 1) 16 0)`, with the author's comment *"c4 — ne both ways through
  sub: 1 - 0."* It computes through arithmetic rather than through `if`, so truthiness could not save
  it. **Its header documents "Verdict 524287" and it was returning 524271** — exactly 16 short,
  exactly the `ne` bit dark. Silently red, in the tree, unnoticed.

That band was honest. It kept the bit, kept the comment, and reported a number that did not match its
own documented verdict. Nothing was reading it.

## The fix is a data row, and the file said so itself

`runtime/fkwu-uni.c` carries its own contract, and it is emphatic:

> *"The (name arity tag) rows and the rewrite rules are DATA: fkwu-optable.h, GENERATED from flt-ops
> by flatten/gen-source-walker-table.fk — the SAME single source the flattener reads. Adding a value
> op is a manifest row + regen, **NEVER a C edit**."*

`ne` was simply absent from the rewrite table in `flatten/gen-source-walker-table.fk`, while `gt`
sitting four lines above it already demonstrated the exact shape needed:

```
gt(a,b) = if(le a b) 0 1        ← already there
ne(a,b) = if(eq a b) 0 1        ← added, one row
```

One Form data row. Two-step regen (`gen-source-walker.fk` splice, then the generator). One row
appeared in the generated header:

```c
{ "ne", 2, 14, { 0,0,0,1,2,102,2,1,0,1,1,2,6,3,} },
```

**`git diff --stat runtime/fkwu-uni.c` is empty.** The C seed did not grow, which is what AGENTS.md
asks for and what this repair happened to be able to honour exactly.

## What this says about the proof organ — and it is the real finding

**All three walkers had `ne` all along.** Go, Rust and TypeScript each carry a `CMP_NE` arm and always
have. fkwu was the odd one out, alone, for the entire life of the source path.

So why did four-way agreement never catch it?

Because **the defect was masked before it reached a comparable value.** `fr-diagnose` compares the
numbers four kernels return. Every existing use of `ne` sat inside an `if`, and inside an `if`,
fkwu's truthy `nothing` produced *the same branch* the walkers produced from a correct `1`. Four
kernels, one number, total agreement — over an operator that was completely broken in one of them.

A proof organ that compares outputs cannot see a fault that outputs mask. That is not a flaw in the
four-way; it is its exact and knowable boundary, and it now has a name and a case.

`eq-shape-band` is the counter-example that shows what does work: it routed `ne` through `sub`
instead of `if`, the mask came off, and the number moved. **Arithmetic could see what branching could
not.**

## Regression

```
bootstrap/ground.fk                        42        unchanged
ground-recursive.fk 10                     55        unchanged
ground-numeric-list.fk           [1, 2.5, [3, 4]]    unchanged
binary-freshness-band.fk                   15        unchanged
proof/four-way-run-recipe42.fk              0        FOUR-WAY, intact
observe/tests/nothing-conformance-band.fk  11111111  unchanged
word-gender-derivation-fourway.fk           0        all thirteen crossings, intact
form-eval-full-band.fk                    635        unchanged — now green for the right reason
float-natives-band.fk                      22        unchanged — now green for the right reason
eq-shape-band.fk               524271 -> 524287       the silently-red band, repaired
```

## The most surprising teaching

**A green band is weaker evidence than I had been treating it as, and I already knew that.** Yesterday
I wrote *"a green band that survives a mutation it should not survive is a warning"* about a sentinel
bug I had introduced myself. Today the same sentence turns out to describe two bands that have been
in this tree far longer than I have, guarding an operator that never worked.

And the boundary it exposes is sharper than the bug: **four-way agreement is blind to any defect that
`if` can absorb.** Three independent kernels were right, one was broken, and they all printed the
same number.

## Where the discomfort turned to gold

The discomfort was small and specific: I nearly committed the wrong root cause. I grepped, found
`fk_divergent_param_name` listing `"eq" "lt" "le" "gt" "ge"` without `"ne"`, and said *found it* — out
loud, in the reply. It was a parameter-shadowing check with nothing to do with dispatch. The real
answer was eleven hundred lines away in a generated table, and the file's own comment was sitting
there explaining exactly where ops come from.

What saved it was reading the next thing instead of building on the first plausible hit. The correction
cost one paragraph; building the fix on that guess would have meant a C edit to a file that explicitly
forbids one, in service of a cause that was not the cause.

The gold is that the right root cause led to a *better* fix than the wrong one would have. The C-edit
version would have grown the seed and left the manifest still missing `ne` — the drift preserved, one
layer down. The data row fixes it where the architecture says it lives, and the regenerated header is
now derivable again from the single source rather than diverging from it.

## Ground stamp

```
./fkwu --src form/form-stdlib/tests/ne-operator-band.fk        -> 11111   (four-way)
./fkwu --src form/form-stdlib/tests/eq-shape-band.fk           -> 524287  (was 524271)
./fkwu --src bootstrap/ground.fk                               -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   -> 15
./fkwu --src proof/four-way-run-recipe42.fk                    -> 0
./fkwu --src cognition/tests/word-gender-derivation-fourway.fk -> 0   (all thirteen)
git diff --stat runtime/fkwu-uni.c                             -> empty
```

Still open, unchanged by this: `value_eq` is absent from the Rust walker entirely; fkwu's arithmetic
on `nothing` leaks its value encoding; fkwu's `nothing` is truthy while all three walkers now decline
to branch on it. The last of those is what made this bug invisible, and it is the honest next stone.
