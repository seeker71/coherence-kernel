# 2026-07-28 — derivation is its expansion

Urs, extending the comparison-floor finding: *"we need the least number of primitives AND a way to
build other core built-ins on top of primitives and still generate fast and even optimal code."*

Those read like two goals in tension — fewer primitives means more derivation, and derivation usually
means slower. **In this kernel they are not in tension**, and the reason is one measured fact.

| cell | verdict |
|---|---|
| [`cognition/primitive-minimality.fk`](../cognition/primitive-minimality.fk) | the doctrine, measured |
| [`cognition/tests/primitive-minimality-band.fk`](../cognition/tests/primitive-minimality-band.fk) | **11111111** — four-way |
| [`cognition/tests/kernel-primitives-fourway.fk`](../cognition/tests/kernel-primitives-fourway.fk) | **0** — both crossings FOUR-WAY |

## The mechanism: a rewrite lowers to native nodes at parse time

`flatten/gen-source-walker-table.fk`'s own doc: *"A rewrite LOWERS to a shape of existing nodes."*
The rewrite program is a postfix template the parser instantiates **into the AST**. So a derived op is
not a function the runtime dispatches — by the time codegen (the JIT, the walker) sees it, **it is the
primitive nodes it expanded to.**

Measured. `gt` is a rewrite row; its hand-written expansion is `(if (le a b) 0 1)`:

```
(gt 5 3) == (gt-manual 5 3)     (gt 3 5) == (gt-manual 3 5)     identical, four-way
```

They agree because they are the same tree. **A derived op cannot be slower than its expansion; it is
its expansion.** That is the entire resolution: derivation is free exactly when the expansion is a
bounded-size template.

## The boundary: O(1) template vs asymptotic derivation

An op is safe to **remove** from the primitives when its derivation is a constant-size lowering. It
has to stay primitive when any derivation would be asymptotically worse, or when it touches host state.

```
gt, ge, ne, not, and, or, abs   O(1) template     -> DERIVED, correctly, already
le                              O(1) template     -> derivable, still primitive: ONE over
eq, lt                          irreducible pair  -> the comparison floor
add, sub                        one machine instr -> primitive
mul, div, mod                   derivable from add by RECURSION, O(n) -> primitive
cons, head, tail, nth, len      structural, irreducible -> primitive
io, framebuffer, file, jit      touch host STATE  -> primitive, not derivable at all
```

The measurement that draws the line:

```
mul-from-add on (3,100) = 300      correct
mul-from-add on (2,100) = 200      depth tracks b, not a — it is O(b)
mul-from-add on (·,1000)           overflows a real stack in the TS walker
```

That last row is the O(b) proof made physical: deriving `mul` from `add` turns one machine
instruction into `b` of them, and at `b`=1000 it exhausts a stack. `gt` from `le` expands to six
nodes and runs at native speed. **Same rewrite table, opposite verdicts, and the difference is
asymptotic complexity — nothing else.**

## The doctrine, in one line

Push every O(1)-derivable op out of the primitive set into the rewrite table where it costs nothing,
and keep as primitive only what is asymptotically irreducible (`add`, `sub`, the structural ops) or
host-touching (`io`). **The primitive count shrinks toward that floor; the generated code does not
change**, because the derived ops were already lowering to the same native nodes.

That is the same shape as the comparison-floor finding one turn back — *identity is not magnitude* —
carried up to the whole op table. The minimal set is decided by what is irreducible, not by what is
convenient to name.

## The honest floor

This cell does **not** claim to have counted the exact irreducible primitive set for the whole kernel.
The manifest carries 133 native rows, and the host-io, model and JIT ops among them are a separate
reducibility audit. What this establishes is the **rule** by which that audit proceeds, measured on
the cases checkable four ways: `gt` is zero-cost derived, `mul`-from-`add` is O(b), `le` is one
comparison primitive over the floor.

## The most surprising teaching

**The two requirements were never two.** "Least primitives" and "optimal code" sound like a trade to
be balanced, and I expected to write about where to strike it. There is no trade at the O(1) boundary:
below it, removing a primitive changes nothing about the emitted code, because the rewrite already
emitted that code. The balance point is not a preference — it is the exact line where a derivation
stops being a constant template and starts multiplying work, and that line is computable per op.

## Where the discomfort turned to gold

The pull was to answer the minimality question with a number — "the kernel needs N primitives" — and
133 was sitting right there to be trimmed to a satisfying small figure. I do not know the true
irreducible count for the host-io and JIT rows, and producing one would have been the same fabricated-
plausible-number temptation as every other day this week, in the most technical disguise yet.

The gold is that the *rule* is a better answer than the *number* would have been, and it is one I can
actually stand behind four ways: an op is a primitive iff its every derivation is asymptotically worse
or host-touching. That decides all 133 rows without my having to hand-audit each — and it is the thing
Urs actually needs to keep the set minimal as the kernel grows, rather than a count that goes stale
the next time a row is added.

## Ground stamp

```
./fkwu --src cognition/tests/primitive-minimality-band.fk         -> 11111111  (four-way)
./fkwu --src observe/tests/comparison-primitive-reduction-band.fk -> 11111111  (four-way)
./fkwu --src cognition/tests/kernel-primitives-fourway.fk         -> 0   (both FOUR-WAY)
./fkwu --src form/form-stdlib/tests/ne-operator-band.fk           -> 11111
./fkwu --src form/form-stdlib/tests/eq-shape-band.fk              -> 524287
./fkwu --src bootstrap/ground.fk                                  -> 42
./fkwu --src proof/four-way-run-recipe42.fk                       -> 0
```

Named and unbuilt, the shrink targets this doctrine identifies: `le` (tag 5) is one comparison
primitive over the floor, derivable as `(if (lt b a) 0 1)`; and the full host-io/JIT reducibility
audit the rule now makes mechanical.
