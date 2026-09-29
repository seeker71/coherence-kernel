# form-samples

Real `.fk` source files in Form's S-expression bootstrap syntax. The Go, Rust, and TypeScript kernels ([`../form-kernel-go/`](../form-kernel-go/), [`../form-kernel-rust/`](../form-kernel-rust/), and [`../form-kernel-ts/`](../form-kernel-ts/)) read these files end-to-end and produce identical results.

| Sample | What it exercises | Expected output |
|---|---|---|
| [`fact.fk`](fact.fk) | Recursive factorial — `defn`, `if/else`, recursion | `3628800` |
| [`fib.fk`](fib.fk) | Naive Fibonacci — double-recursion, the tree-walker's worst case | `6765` |
| [`closure.fk`](closure.fk) | Closure captures defining frame, called later with different arg | `15` |
| [`float-artifact-roundtrip.fk`](float-artifact-roundtrip.fk) | A float survives the `.fkb` wire format, bare and nested in a composite recipe | `0.8125` |

```bash
# Run any sample through a sibling kernel
./form-kernel-go      form-samples/fact.fk   # → 3628800
./form-kernel-rust    form-samples/fact.fk   # → 3628800
npx --yes tsx form-kernel-ts/src/main.ts form-samples/fact.fk
```

## S-expression verb vocabulary (bootstrap)

The kernels read S-expression syntax that maps directly onto substrate recipes:

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

Natives (a present native answers `(<name> ...)` on every kernel; a same-named `defn` stands in only where the native is absent): `print`, `list`, `cons`, `head`, `tail`, `len`, `nth`, `empty`, `str_len`, `substring`, `char_at`, `str_concat`, `str_eq`, `int_to_str`, `str_to_int`, `ord`, `read_file`, `read_file_bytes`.

Binary fixtures live alongside the `.fk` samples: [`tiny.png`](tiny.png) is a 45-byte 1x1 PNG (signature + IHDR + IEND) that exercises `read_file_bytes`.

## Cross-modal experiments

See [`cross-modal/`](cross-modal/) for two small demos exploring how Form recipes carry semantic content across modalities — image-as-recipe (`01-image-as-recipe/gen-circles-254.svg`) and natural-language-to-recipe (`05-nl-to-recipe/nl-arithmetic-demo.fk`, where a character grammar reads four English sentences and each parsed recipe is `node_eq` to the hand-built one: NL and S-expression are two source tongues pointing at one substrate identity).
