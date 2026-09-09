# Numeric format recipes

Numeric values carry a semantic kind, a format recipe, and a value. Format
recipes describe encoding, bit width, storage and arithmetic hints. Their
content identity is shared across the proof interpreters.

[`numeric.ts`](form-kernel-ts/src/numeric.ts) interprets those hints through
`applyArith`. `FormatTable` caches handlers that call this arithmetic path.
This is the numeric conformance surface.

Native compilation and performance witnesses use Form's
[native JIT route](../docs/native-jit-routing.md). Run
`./fkwu observe/native-jit-witness-run.fk` from the repository root for the
emission and execution witness. Numeric representation and whole-function
machine-code coverage require their own behavior and dispatch observations.
