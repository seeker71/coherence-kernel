# Source Compiler Health Receipt — 2026-07-05

## What changed

The source compiler is now on the runtime-image path for source sections:

- `fsc-compile-section` builds a recipe object, writes it to
  `<out>.<dialect>.fkb` with `write_form_binary`, and emits only a
  `(walk_recipe_here (read_form_binary "..."))` load driver.
- `form.action` no longer rides the generic `bml-grammar` route in the active
  compiler or in `form-bml-lower.fk`'s validate-chain override. It compiles
  through the primitive-aware recipe constructor path, so calls like `add(...)`
  become primitive-category recipe nodes instead of unbound function calls.
- `form-ontology-loader.fk` now carries reviewed Form-owned rows for the grammar
  capsules that source compilation serializes, including `PY-BMF-*`,
  `GO-BMF-*`, `TS-BMF-*`, `RS-BMF-*`, `PL-BMF-*`, `BML-AST-*`,
  `NATURAL-BMF-*`, and media/document grammar rows. Unknown names still fail
  loudly through `form_error`.
- The `grammars/form-ontology-loader.fk` mirror is byte-identical to the stdlib
  loader again.

## Why it mattered

The old text-emission path was hiding two real problems:

1. Section recipes containing natural/Python grammar nodes could not always be
   written as `.fkb` because the source-side ontology did not materialize the
   dialect constants as NodeIDs.
2. `form.action` looked healthy only because generated text was reparsed by the
   kernel; the object recipe itself still carried primitive calls like `add` as
   ordinary function calls.

Both failures appeared as soon as the compiler stopped emitting large strings
and started emitting recipe objects.

## Validation

Bootstrap and ontology:

```sh
./fkwu --src bootstrap/ground.fk                              -> 42
./fkwu --src bootstrap/ground-recursive.fk 10                 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk  -> 15
./fkwu --src /tmp/nvr.fk                                      -> 11111
./fkwu --src /tmp/fol-probe.fk                                -> 2
cmp form/form-stdlib/form-ontology-loader.fk grammars/form-ontology-loader.fk -> 0
```

Source compiler bands:

```sh
cd form && ./validate.sh form-stdlib/tests/source-compiler-multi-dialect-band.fk       -> 450
cd form && ./validate.sh form-stdlib/tests/form-action-bmf-source-compiler-rewrite.fk  -> 215
cd form && ./validate.sh form-stdlib/tests/source-compile-multibyte-band.fk            -> 15
cd form && ./validate.sh form-stdlib/tests/source-corpus-roundtrip-band.fk             -> 1696
cd form && ./validate.sh form-stdlib/tests/source-compiler-runtime.fk                  -> 100
cd form && ./validate.sh form-stdlib/tests/source-compiler-health-band.fk              -> 65535
cd form && ./validate.sh form-stdlib/tests/natural-bmf-bidirectional.fk                -> 7838
```

Artifact/persistence bands:

```sh
cd form && ./validate.sh form-stdlib/tests/source-compiler-emission-band.fk            -> 2147483647
cd form && ./validate.sh form-stdlib/tests/source-compiler-fkb-file-emission-band.fk   -> 2147483647
cd form && ./validate.sh form-stdlib/tests/source-compiler-persistence-band.fk         -> 2147483647
cd form && ./validate.sh form-stdlib/tests/source-compiler-file-persistence-band.fk    -> 2147483647
cd form && ./validate.sh form-stdlib/tests/source-compiler-grammar-bridge-band.fk      -> 32767
```

Adjacent checks:

```sh
cd form && ./validate.sh form-stdlib/tests/form-ontology-parity-band.fk    -> 1506
cd form && ./validate.sh form-stdlib/tests/form-bml-cursor-lower-band.fk   -> 113
cd form && ./validate.sh form-stdlib/tests/form-bml-cursor-full-band.fk    -> 67
```

The `source-compiler-fkb-file-emission-band` took more than a minute but
completed green. That was recorded as slow, not ignored as a stall.

## Cursor Motion Closure

The scanner model was corrected: the BMF cursor is the scanner. The source
compiler no longer claims a separate scanner layer as the goal. The generic
source-object scan functions remain compatibility adapters, but their common
text-token paths now move the cursor by native byte strides:

- whitespace, identifiers, and integers use `scan_run`;
- string close uses `str_find`;
- source range line/column state updates through `fsc-source-cursor-jump`;
- source-object scanner helpers are treated as cursor-motion adapters, not as
  an authoritative tokenizer layer.

`source-compiler-health.fk` is back to:

- `source-compiler-health-status` is `healthy`;
- `source-compiler-fully-healthy?` is `1`;
- the BMF health grammar uses `cursor-motion`, not `scan-pressure`.
