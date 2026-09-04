# Format-recipes as substrate cells

Companion to [`kernel-ts-comparison.md`](./kernel-ts-comparison.md) (the
i32/walker/compiled arc). This page names the format-recipe path and the bench
that measures it.

## The architecture, named once

Numeric values are `(semantic-kind, format-recipe, value)` triples. The
format-recipe is a substrate cell — the same shape as any other recipe — that
describes the encoding completely:

```
format-recipe (FP8 E4M3):
  ├── semantic-kind   : REAL
  ├── encoding        : IEEE_754
  ├── bits            : 8
  ├── storage-hint    : "u8-array"
  ├── arithmetic-hint : "table-lookup-via-fp32"
  ├── mantissa-bits   : 3
  ├── exponent-bits   : 4
  └── exponent-bias   : 7
```

Adding a numeric format — FP4, NF4, BitNet ternary, posit, log-prob,
arbitrary-precision rational — is a substrate write that creates a new
format-recipe cell. No kernel change. The kernel reads `storage-hint` and
`arithmetic-hint` and dispatches; if the compiler recognizes the hints, it emits
specialized JS that V8 JITs.

## Three passes, one bench

`src/numeric-bench.ts` runs three workloads (fp64 sum, fp8 sum through
`Math.fround`, a BitNet {-1, 0, 1} dot product) through four paths:

- **Native TS** — bare JS arithmetic, no format dispatch (the reference)
- **Pass 0** — naive dispatch via `applyArith(fmt, op, a, b)` switching on the
  arithmetic-hint at every call
- **Pass 1** — per-(format, op) closures emitted from the format-table cache via
  `new Function`; V8 inlines and JITs the specialized operator
- **Pass 2** — full-function JIT: `emitOpExpr(fmt, op, aSrc, bSrc)` reads the
  format-recipe and emits the whole recursive function as specialized JS, compiled
  once

The architectural point of Pass 2 is that it generalizes — *the same emitter
produces native code for any format-recipe*. Adding NF4 weights, FP4
activations, posit accumulators, log-prob sums all route through this one
emitter, no per-format codegen.

Cross-kernel agreement extends to format-recipes: two kernels that intern the
same format-recipe tree get the same NodeID, so quantization-aware Form code runs
portably across the conformance circle.

## What is hardware-bound, honestly

- `storage-hint = "v8-double"` assumes V8 stores Number as f64 internally.
- `Math.imul`, `Math.fround` are host intrinsics; other JS hosts may differ.
- `new Function` compilation depends on V8 actually JITting emitted source; cold
  start cost is real.

These are implementation properties of the storage/arithmetic-hint handlers, not
of the substrate's identity grammar. The NodeIDs stay invariant; the storage is local.

## Repro — the numbers come from the run, not this page

```sh
cd form/form-kernel-ts
node --experimental-strip-types src/main.ts --numeric-bench    # format-recipe arc
node --experimental-strip-types src/main.ts --bench            # i32/walker/compiled arc
```

No install step: the kernel imports only its own sources, and Node's strip-types
runs them directly (`validate.sh` uses the same door).

## Open breaths

- Packed `Uint8Array` / `Uint16Array` storage for the FP8 / NF4 / BitNet value
  tables (the current path uses generic Number storage).
- Wire Pass 1 / Pass 2 into the kernel's walker and compiler — they live in
  `numeric.ts` / `numeric-bench.ts` as a parallel demonstration.
- A canonical format-recipe bootstrap cell, validated by the sibling run.
