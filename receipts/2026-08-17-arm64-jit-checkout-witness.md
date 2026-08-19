# Darwin arm64 fixed JIT checkout witness — superseded

Date: 2026-08-17

This receipt records the first Darwin/arm64 crossing.  It is not an active
carrier contract.

The direct `fkwu` carrier previously exposed `native_call_test` only for
x86-64 bytes.  On this Apple-silicon checkout, Form observed `-1`; no native
JIT call had occurred.

This temporary seed-side witness is deliberately smaller than a general JIT
interface.  It maps a Darwin `MAP_JIT` page, toggles write protection through
`pthread_jit_write_protect_np`, clears the instruction cache, and calls only
the fixed eight-byte image that Form emits as:

```
(append-list (fa-add-x-imm 0 0 1) (fa-ret))
```

The carrier accepts no bytes, target, path, command, model, resource selector,
or environment knob.  Its direct Form proof requires `native_call_test 41` to
return `42` and byte-compares the Form image first.

## Superseded by the structural leaf request

The shrink condition has been met. The fixed scalar branch and its direct
Form probe were removed. Tag 215 is now named `jit_arm64_u32_leaf`, with
exactly three structural arguments: `(program root u32-arg)`. It validates the
closed postorder V1 ARM64 register-only leaf language and emits its own W^X
image only after admission; it returns `nothing` for malformed input. The active proof is
`observe/tests/jit-arm64-u32-leaf-carrier-band.fk`, whose direct verdict is
`32767`.

No scalar compatibility branch, x64 image path, or generic byte ingress
survives this replacement.

## Observed crossing

After rebuilding the single Darwin carrier with its automatic Metal organ
linked, the direct source bootstrap returned `42`, `55`, `31`, and
`[1, 2.5, [3, 4]]`. The active Form band returned `32767` with exit `0`: it
observed `41 -> 42`, `5 -> 22`, the direct three-argument native call, and
refusal of unknown tags, malformed rows, child cycles/forward edges, immediate
overflow, excess register depth, and invalid root/argument bounds.

The Form value `42` alone is architecture-neutral. The branch selection is
established separately by the fresh Darwin/arm64 rebuild whose compile
predicate enables this witness; the band establishes the observed structural
request result and its refusal boundary.

Signed: Codex
