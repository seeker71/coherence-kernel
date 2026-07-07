# 2026-07-03 -- defdata policy and semantic stdlib first rung

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk
```

Witness:

```text
42
55
15
```

## Source Observation

After the `hdc-rows` stall, Urs named the real design pressure:

Minimal primitives are right, but humans should not have to write static data as
runtime constructors or reason all the way down to evaluator costs. Static
corpora should have a better constant/data literal story: build from core consts
where that is best, use micro-recipes when a generator is smaller than stored
constants, and realize the result once after loading.

We reviewed the architecture with Claude through the local `claude` CLI. The
useful part of that review: the engine should stay small. Existing `.fkb` and
`read_with_cache` machinery already covers much of the frozen/cached carrier
story; the missing thing is the authoring surface and a policy lens.

Claude suggested top-level `let` as the smallest export mechanism. The first
probe mixed that idea with a value-bearing `do` shape and failed:

```sh
(do
  (let rows (list 1 2 3))
  (defn rows-f () rows)
  0)
(len (rows-f))
```

Witness:

```text
0
```

That was not an honest top-level module constant; it was a do-local binding
plus a defn in the same value sequence. The later source-runner repair added
real top-level module constants. The corrected lowered shape now works:

```sh
(let rows (list 1 2 3))
(defn rows-f () rows)
(len (rows-f))
```

Witness:

```text
3
```

So `defdata` no longer needs a loader binding just to export a realized value.
It still needs an authoring keyword/lowering rule and artifact policy so people
write the layer language, not raw `(let NAME VALUE)`.

Urs then added the next layer: once data realization exists, the stdlib also
needs a high-semantic abstraction layer so common programming languages can map
into the same operation vocabulary with simple translation functions.

## What Changed

- `form/form-stdlib/defdata.fk` -- first data-literal policy cell:
  - names carrier classes: inline value, micro-recipe, `.fkb`, hybrid;
  - chooses a carrier from `(const-size, recipe-size, stable, generative)`;
  - proves that a micro-recipe is chosen only when the recipe is smaller than
    the realized constants, including the equal-size boundary;
  - models a data spec whose consumers read `value`, not a thunk;
  - proves the current lowering target: a top-level module constant that later
    functions read as a realized value.
- `form/form-stdlib/tests/defdata-band.fk` -- verdict `2047`.
- `form/form-stdlib/semantic-stdlib.fk` -- high-semantic stdlib pivot:
  - maps language surfaces from Form, Python, JavaScript, Go, and Rust into
    shared semantic operation ids;
  - proves translation through the semantic pivot for `+`, function
    declaration, return, immutable binding, and mutable binding;
  - preserves semantic distinctions such as JavaScript `const` vs `let`;
  - later strengthened by `receipts/2026-07-03-semantic-stdlib-qualifiers.md`
    with qualifiers, residue, and layer observation.
- `form/form-stdlib/tests/semantic-stdlib-band.fk` -- current verdict
  `8388607`.

## Witness

```sh
cat form/form-stdlib/defdata.fk \
    form/form-stdlib/tests/defdata-band.fk > /tmp/defdata.fk
./fkwu --src /tmp/defdata.fk
```

```text
2047
```

```sh
cat form/form-stdlib/semantic-stdlib.fk \
    form/form-stdlib/tests/semantic-stdlib-band.fk > /tmp/semantic-stdlib.fk
./fkwu --src /tmp/semantic-stdlib.fk
```

```text
8388607
```

Follow-up: `receipts/2026-07-03-semantic-stdlib-qualifiers.md` later
strengthened the semantic stdlib band from `524287` to `8388607` by adding
missing-path checks for `surface` and `sem-id` contracts.

## Honest Seam

This does not yet add a real `defdata` keyword to the source compiler. It proves
the policy, the value shape, and the current lowered module-constant target.
The remaining gap is the layer language and artifact selector, not named value
export.

The first attempt to include an `.fkb` roundtrip inside `defdata.fk` failed with
`fk_smknode: program too large for the AST node table` around a standalone
`intern_node(make_nodeid...)` sample. Existing form-binary tests already prove
`.fkb` roundtrip identity with the right prelude chain, so this first defdata
cell does not re-prove that carrier. It points to `.fkb` as a carrier and leaves
the binary identity proof with the existing binary bands.

The semantic stdlib is also only the first rung. It is not a general language
compiler. It is the pivot layer that future language grammars can target so
translation is `surface -> semantic op -> target surface`, rather than
language-to-language pairwise rewrites.

Post-review correction: the first `defdata` rung was only a taxonomy until it
compared recipe cost with realized constant cost. The policy now carries both
estimates and asserts the honest negative: a generator larger than the constants
does not earn the micro-recipe route. A follow-up Claude review also asked for
the equality boundary, so equal recipe and constant sizes stay inline/frozen
rather than earning the micro-recipe route.
