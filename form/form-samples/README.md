# form-samples

Real `.fk` source files in Form's S-expression bootstrap syntax. `fkwu`, the runtime, reads these files end-to-end and produces the expected output below.

| Sample | What it exercises | Expected output |
|---|---|---|
| [`fact.fk`](fact.fk) | Recursive factorial — `defn`, `if/else`, recursion | `3628800` |
| [`fib.fk`](fib.fk) | Naive Fibonacci — double-recursion, the tree-walker's worst case | `6765` |
| [`closure.fk`](closure.fk) | Closure captures defining frame, called later with different arg | `15` |
| [`float-artifact-roundtrip.fk`](float-artifact-roundtrip.fk) | A float survives the `.fkb` wire format, bare and nested in a composite recipe | `0.8125` |

```bash
# From the repo root: the runtime
./fkwu form/form-samples/fact.fk                                          # → 3628800
```

## S-expression verb vocabulary (bootstrap)

fkwu reads S-expression syntax that maps directly onto substrate recipes:

| Verb | Recipe category | Notes |
|---|---|---|
| `(do <stmt>...)` | BLOCK.DO | last value |
| `(seq <stmt>...)` | BLOCK.SEQUENCE | last value |
| `(let <name> <expr>)` | BLOCK.LET | binding |
| `(if <c> <t>)` / `(if <c> <t> <e>)` | COND.IF_THEN / IF_THEN_ELSE | |
| `(add/sub/mul/div/mod <a> <b>)` | MATH.* | |
| `(eq/ne/lt/le/gt/ge <a> <b>)` | COMPARE.* | |
| `(and/or <a> <b>)` / `(not <a>)` | LOGIC.* | |
| `(defn <name> (params...) <body>)` | FNDEF | |
| `(<name> <args>...)` | FNCALL | shorthand for both user fns and natives |

A native answers `(<name> ...)` on fkwu: the pure core in [`../../docs/kernel-interface.md`](../../docs/kernel-interface.md) (`list`, `cons`, `head`, `tail`, `len`, `nth`, `empty`, `str_len`, `str_concat`, `str_eq`, ...), measured natives such as `substring`, and doors such as `print`, `read_file` and `read_file_bytes`. `char_at`, `int_to_str`, `str_to_int` and `ord` are not natives: they live in `form-stdlib/core.fk`, and a file that calls them names it with `; preludes: form-stdlib/core.fk`.

Binary fixtures live alongside the `.fk` samples: [`tiny.png`](tiny.png) is a 45-byte 1x1 PNG (signature + IHDR + IEND) that exercises `read_file_bytes`.

## Cross-modal experiments

See [`cross-modal/`](cross-modal/) for two small demos exploring how Form recipes carry semantic content across modalities — image-as-recipe (`01-image-as-recipe/gen-circles-254.svg`) and natural-language-to-recipe (`05-nl-to-recipe/nl-arithmetic-demo.fk`, where a character grammar reads four English sentences and each parsed recipe is `node_eq` to the hand-built one: NL and S-expression are two source tongues pointing at one substrate identity).
