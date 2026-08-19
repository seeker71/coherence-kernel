# ARM64 live native-call carrier

The Form JIT's host membrane could map RW memory, seal it RX, and call it only
on x86-64. On this ARM64 body `native_call_test` therefore reported
unavailability even though macOS provides the same W^X carrier.

This checkout-witness repair admits ARM64 to the existing POSIX install/call
door and gives the carrier probe the ARM64 image for `f(a) = a + 1`:

`add x0, x0, #1; ret`

This is deliberately not called a general ARM64 JIT. It witnesses real native
installation and execution on the current host. Recipe lowering is still
x86-64-specific and is the next boundary to move into Form-owned emitter data.

Shrink target: the architecture byte images and lowering policy belong in Form
cells. The C seed retains only the irreducible host membrane that maps supplied
bytes RW, seals them RX, calls them, and returns the observed value.

Observed after a fresh rebuild, using direct `.fk` invocation:

- `bootstrap/ground.fk` -> `42`
- `bootstrap/ground-recursive.fk 10` -> `55`
- `binary-freshness-band.fk` -> `15`
- `bootstrap/ground-numeric-list.fk` -> `[1, 2.5, [3, 4]]`
- `jit-live-carrier-probe-check` -> `1048575` (all 20 bits, including
  live completeness, present)

The generic args-vector installer remains architecture-gated to x86-64. That
boundary is intentional until the recipe emitter labels and emits ARM64 images;
otherwise an x86 image could be installed and executed on ARM64.

; witnessed: 2026-07-23 -> ARM64 install/call probe live; recipe JIT architecture still pending
