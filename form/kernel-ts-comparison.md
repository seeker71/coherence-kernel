# TypeScript proof interpreter

The TypeScript kernel walks Form recipes for cross-kernel conformance.
[`src/kernel.ts`](form-kernel-ts/src/kernel.ts) owns that proof interpreter.
From the repository root, run the shared proof door:

```sh
form/validate.sh form/form-stdlib/core.fk form/form-stdlib/tests/native-recursion-band.fk
```

Run native Form programs with `./fkwu path.fk` or `./fkwu path.bml`.
[`Native JIT routing`](../docs/native-jit-routing.md) identifies the Form compiler,
emitter, demand policy and executable measurements. A proof interpreter's
timings describe that interpreter; native performance comes from the native witness.
