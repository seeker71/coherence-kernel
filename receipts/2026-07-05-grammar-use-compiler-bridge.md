# 2026-07-05 -- Grammar use compiler bridge

## Why

The three-day intent was not merely to create grammars. It was to create
grammars and use them as load-bearing language surfaces.

The audit found a real mismatch:

- `defdata-language`, `defdata-recipe-language`, `domain-grammar-core`,
  `grammar-authoring-language`, `form-definition-language`, and
  `sibling-ref-authoring-language` existed as grammar surfaces.
- The newer source/artifact compiler layers still accepted caller-supplied
  emissions without requiring one of those grammar surfaces to admit the source.
- Several grammar bands were not honestly protected by the standard
  `form/validate.sh <band>` path because multiline prelude headers only loaded
  the first line.

So the layer stack had grammar islands, but the compiler lane was still mostly
low-level Form/artifact plumbing.

## Repairs

Added:

- `form/form-stdlib/source-compiler-grammar-bridge.fk`
- `grammars/source-compiler-grammar-bridge.fk`
- `form/form-stdlib/tests/source-compiler-grammar-bridge-band.fk`

The bridge makes `form-definition-language` load-bearing for a source compiler
emission:

```text
module calc { data rows = [40,2]; fn answer() = add(40,2); }
```

must parse through the scannerless BMF grammar, lower to:

```text
(let rows (list 40 2))
(defn answer () (add 40 2))
```

and only then may delegate to `source-compiler-emission`. Bad grammar refuses
the bridge. Bad nested emission investigates.

Also repaired the grammar witnesses exposed by normal validation:

- single-line prelude headers for the grammar bands that depended on multiple
  preludes;
- missing outer closes in grammar band files;
- `defdata.fk` had `0` accidentally inside `defdata-check`;
- `defdata-language` and `defdata-recipe-language` emitted unregistered
  blueprint names instead of the registered `fncall` carrier;
- `form-definition-language` used a fragile helper name that crossed kernels as
  a zero-arg call in Go/Rust;
- `sibling-ref-authoring-language` used `node_eq` on list-shaped lowered values
  instead of structural `value_eq`;
- `source-runner-admission` now records `source-compiler-grammar-bridge-band`
  as a current green observation.

## Witness

```text
cd form && ./validate.sh form-stdlib/tests/defdata-band.fk
-> 2047

cd form && ./validate.sh form-stdlib/tests/defdata-language-band.fk
-> 8191

cd form && ./validate.sh form-stdlib/tests/defdata-recipe-language-band.fk
-> 134217727

cd form && ./validate.sh form-stdlib/tests/form-definition-language-band.fk
-> 65535

cd form && ./validate.sh form-stdlib/tests/source-compiler-grammar-bridge-band.fk
-> 32767

cd form && ./validate.sh form-stdlib/tests/domain-grammar-core-band.fk
-> 268435455

cd form && ./validate.sh form-stdlib/tests/grammar-authoring-language-band.fk
-> 134217727

cd form && ./validate.sh form-stdlib/tests/domain-semantic-bridge-band.fk
-> 268435455

cd form && ./validate.sh form-stdlib/tests/sibling-ref-authoring-language-band.fk
-> 2147483647

cd form && ./validate.sh form-stdlib/tests/source-compiler-multi-dialect-band.fk
-> 0
```

No OOM-killed process occurred during this pass. The failures that did occur
were investigated and repaired rather than ignored.

## Lesson

The altitude problem was real. We had grammar surfaces, but too many downstream
layers still had a low-level hand-built artifact shape. The new bridge is not
the final compiler; it is the first guide that prevents the source compiler
lane from bypassing a high-level grammar surface.

The next stronger move is to make the bridge produce the `.fkb` program image
directly from the lowered source/image recipe instead of accepting a supplied
PIF envelope.
