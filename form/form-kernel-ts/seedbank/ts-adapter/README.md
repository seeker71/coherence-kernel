# TypeScript Adapter — bootstrap breath

This adapter closes the smallest honest seam for TypeScript through the
Form pipeline:

```
TypeScript source (.ts)
  → parseTypeScript  (lang-ts.ts)            — TS subset → Form tree
  → emitFk           (lang-ts-fk.ts)         — Form tree → .fk S-expression
  → fkwu             (canonical kernel)      — walks the .fk, no host runtime
```

Three-way parity gate (`scripts/parity_suite.sh`):

1. **node**       — the canonical TypeScript runtime, after tsc strips types
2. **ts-eval**    — our captured-recipe walker (no .fk, no kernel)
3. **ts-run**     — emit .fk + execute on `fkwu`

All three converge on the printed value of the file's final bare expression.

## CTOR vocabulary — cross-language identity

The CTOR names in `lang-ts.ts` are the same as `lang-python.ts`:
`lambda`, `param`, `add`, `int-literal`, `def`, `call`, etc. A TS
`(x) => x + 1` and a Python `lambda x: x + 1` produce the same recipe
sub-tree, intern to the same NodeID by content-addressing, and become
the same dispatch when the kernel walks them. Cross-language identity
is what earns the N+M transpilation savings the language-cells
architecture promises.

## What this breath ships

- Arrow functions: `(x) => x + 1`, `(a, b) => { return a + b; }`
- `function` declarations: `function fact(n) { return ... }`
- `const` / `let` / `var` bindings (no destructuring)
- `if` / `else` / `else if` statements + ternary `c ? a : b`
- Arithmetic: `+ - * / %`, unary `-`, `!`
- Comparison: `== === != !== < <= > >=`
- Logic: `&& || !`
- Function calls, return, recursion
- Number / string / boolean / null / undefined literals, identifiers
- Block `{ ... }`, expression statement
- Type annotations on parameters parse-and-ignore (`(x: number) => ...`)

## What is pending — named honestly, not faked

Each of these is its own breath; the goal here was the *seam*, not coverage:

- Classes / `new` / `this` / `super`
- Interfaces, type aliases, generics
- `import` / `export` / modules
- `async` / `await` / Promises
- JSX / TSX
- Destructuring (object / array / rest)
- Spread operator
- Template literals (backtick strings work as plain strings; no `${}`)
- Regex literals
- `switch` / `case`
- `try` / `catch` / `throw`
- Optional chaining (`?.`), nullish coalescing (`??`)
- `for` / `for-of` / `for-in` / `while` / `do-while`
- Method calls and property access (`obj.method()`, `obj.prop`)

## CLI

From `form/form-kernel-ts/`:

```bash
# Emit .fk source (default output path is foo.fk)
npx tsx seedbank/ts-adapter/src/main.ts ts-compile foo.ts
npx tsx seedbank/ts-adapter/src/main.ts ts-compile foo.ts -      # → stdout

# Parse + execute on fkwu, the canonical kernel
npx tsx seedbank/ts-adapter/src/main.ts ts-run foo.ts

# Parse + walk via the TS captured-recipe evaluator (no .fk round-trip)
npx tsx seedbank/ts-adapter/src/main.ts ts-eval foo.ts

# Same as ts-eval but with arm-dispatch tracing → JSON report
npx tsx seedbank/ts-adapter/src/main.ts ts-trace foo.ts
```

## Running the parity suite

```bash
cd form/form-kernel-ts
./seedbank/ts-adapter/scripts/parity_suite.sh
```

When `fkwu` is not built at the checkout root, the suite reports two-way
parity (node + ts-eval) and notes the third runtime is skipped. Build
the kernel for the full three-way check, from the checkout root:

```bash
cc -O2 -o fkwu runtime/fkwu-uni.c
```

The suite emits `examples/*.fk` as generated fkwu-leg input. Those files (and
the `.fkb` images fkwu keeps beside them) are ignored and regenerated from the
tracked `.ts` sources on each run. A nonzero fkwu exit fails the row.
