# 2026-07-18 — thirteen programming-language lenses gain a native, bounded floor

## The gap named

The body already held several serious code artifacts, but not one direct-source
Form registry spanning the common programming surfaces requested here:

- `form/form-kernel-ts/src/languages.ts` defines the substrate language-cell
  architecture and generic parse/emit walkers. Python, TypeScript, Go, and Rust
  have host-TypeScript populations.
- `form/form-stdlib/seedbank/emits/` carries Form emission templates for those
  same four languages.
- `form/form-stdlib/lang-common.fk` carries a shared imperative statement and
  expression grammar.
- `form/form-stdlib/source-compiler-text-lens.fk` emits Form source from compiled
  recipes; it is not an arbitrary foreign-language emitter.

Those are real foundations. They did not amount to thirteen native parsers or
generators, and this receipt does not rename them as such.

## What entered

`cognition/pl-lenses-13.fk` is a pure-Form organ with exactly thirteen registry
rows:

| ID | Surface | Fixture evidence used by detection | Exact generated `add` function |
|---|---|---|---|
| `python` | Python | `from pathlib`, `def`, `print`, f-string | `def combine(a, b):\n    return a + b\n` |
| `javascript` | JavaScript | `const`, arrow, `console.log` | `function combine(a, b) {\n  return a + b;\n}\n` |
| `typescript` | TypeScript | `interface`, typed fields, typed arrow | `function combine(a: number, b: number): number {\n  return a + b;\n}\n` |
| `java` | Java | class, `public static`, `System.out` | `static int combine(int a, int b) {\n  return a + b;\n}\n` |
| `c` | C | `stdio.h`, `int main`, `printf` | `int combine(int a, int b) {\n  return a + b;\n}\n` |
| `cpp` | C++ | `iostream`, `std::`, `cout` | `int combine(int a, int b) {\n  return a + b;\n}\n` |
| `csharp` | C# | `using System`, namespace, `Console.WriteLine` | `static int combine(int a, int b) {\n  return a + b;\n}\n` |
| `go` | Go | package, `func`, `fmt` | `func combine(a int, b int) int {\n\treturn a + b\n}\n` |
| `rust` | Rust | `fn main`, `let mut`, `println!` | `fn combine(a: i64, b: i64) -> i64 {\n    a + b\n}\n` |
| `ruby` | Ruby | class, `def`/`end`, `puts` | `def combine(a, b)\n  a + b\nend\n` |
| `php` | PHP | `<?php`, dollar variables, `echo` | `function combine($a, $b) {\n  return $a + $b;\n}\n` |
| `swift` | Swift | Foundation, `func`, `let` | `func combine(_ a: Int, _ b: Int) -> Int {\n    return a + b\n}\n` |
| `kotlin` | Kotlin | data class, `val`, `fun main`, `println` | `fun combine(a: Int, b: Int): Int {\n    return a + b\n}\n` |

Every fixture above is stored as actual multi-line code in its registry row.
Detection counts lexical evidence for every candidate and returns the
highest-scoring language. The band feeds all thirteen stored fixtures back
through the detector; all thirteen return their own IDs. Unmarked prose returns
`unknown`.

Generation begins from one language-neutral row:

```text
(binary-spec "combine" "a" "b" "add")
```

The same row emits all thirteen exact strings above. `sub` and `mul` are also
executable; the band observes Swift subtraction and Ruby multiplication.
Division declines with an empty result because integer-division semantics differ
between these languages. An unregistered target declines the same way.

There are two intentional emission doors:

- `pl13-generate` returns the exact annotation-free strings in the table.
- `pl13-generate-detectable` returns valid source prefixed by a language-native
  comment such as `// pl13-language[cpp]` or `# pl13-language[ruby]`. PHP is
  wrapped in `<?php ... ?>` so its comment remains code rather than page text.

The bounded marker syntax matters: `pl13-language[c]` is not a substring of
`pl13-language[cpp]` or `pl13-language[csharp]`. The twentieth band bit feeds
the annotated output for every registry row back into `pl13-detect` and requires
all thirteen results to equal their target IDs.

## Honest capability boundary

The executable capability vector is:

```text
[1, 1, 0, 0, 0, 0, 0, 0, 0]
```

Its fields are, in order:

```text
lexical detection
bounded add/sub/mul generation
full parsing
AST round-trip
type checking
compilation
execution
arbitrary transpilation
identifier escaping
```

Only the first two are present. In particular:

- lexical scoring can be ambiguous or fooled; it is evidence, not a parse;
- generation accepts already-valid identifiers and only a two-argument numeric
  function recipe;
- emitted source was compared byte-for-byte in Form, not compiled by thirteen
  external toolchains;
- no claim is made that two arbitrary programs share semantics;
- the TypeScript-hosted language cells remain distinct prior art, not silently
  counted as fkwu-native thirteen-language parsers.

## Observation

`cognition/tests/pl-lenses-13-band.fk` returned the complete twenty-bit band:

```text
fkwu       1048575
Go         1048575
Rust       1048575
TypeScript 1048575
```

Commands:

```sh
./fkwu --src /tmp/pl-lenses-13-band.fk
walkers/go/walker form/form-stdlib/core.fk cognition/pl-lenses-13.fk cognition/tests/pl-lenses-13-band.fk
walkers/rust/target/release/form-walker-rust form/form-stdlib/core.fk cognition/pl-lenses-13.fk cognition/tests/pl-lenses-13-band.fk
npx --yes tsx walkers/ts/main.ts form/form-stdlib/core.fk cognition/pl-lenses-13.fk cognition/tests/pl-lenses-13-band.fk
```

The independent readers caught two defects during the walk: a pair of missing
closing delimiters that fkwu tolerated, and a test-only dependence on
`value_eq` absent from the minimal Rust walker. The final band uses explicit
Form-native recursive list equality and has balanced structure. The first
detectable-generation attempt also exposed prefix collision between C, C++, and
C# sentinels; bounded bracket markers turned that discomfort into the final
all-thirteen re-entry proof.

## What the attempt taught

The transferable center was not thirteen large parsers. It was one explicit
language-neutral function recipe, one evidence-scored registry, and thirteen
small honest surface transformations. That is enough to observe a real common
waist without pretending the much larger semantic layer has arrived.
