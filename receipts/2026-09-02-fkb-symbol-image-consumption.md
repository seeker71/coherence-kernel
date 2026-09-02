# FKB whole-image symbol consumption — 2026-09-02

The cache did not retain its warmth because the v5 FKB protocol was read
asymmetrically. The writer emits function-symbol rows followed by node-symbol
rows; `fk_fkb_restore_symbol_image` restored only the first section. The
remaining valid node-symbol bytes were then called trailing bytes, so every
source and BML image rebuilt even when source, dependency identity, and image
bytes were unchanged.

The kernel seed now consumes and bounds-checks the v5 node-symbol section in
the whole-image reader. It does not interpret a new runtime meaning, change a
cache identity, or weaken validation: it reads the exact section already
written and preserves refusal for negative or implausibly large counts.

This is a short-lived checkout-witness repair in `runtime/fkwu-uni.c`. Its
shrink destination is the Form-native program-image reader once the C seed no
longer owns FKB loading; no new kernel vocabulary was added here.

Witnesses from a freshly rebuilt `fkwu`:

- `./fkwu form/form-stdlib/bml/source-compiler-typed-emission.bml.fkb` ->
  `0`, exit `0`; before the repair the identical image refused with
  `trailing bytes`.
- A second source BML invocation -> `0`, exit `0`, in `0.01 real` seconds
  (`41,865,013` cycles), without cache-rebuild warning.
- `./fkwu form/form-stdlib/tests/source-compiler-typed-emission-band.fkb`
  -> `16383`, exit `0`; its warm source path also completed in `0.01 real`
  seconds (`49,952,769` cycles).
- `form-cli-bml-cache-band.fk` -> `8191`, exit `0`, after its BML authority
  was admitted; `binary-freshness-band.fk` -> `31`, exit `0`.

The counsel panel remained at `fails=0`, `timeouts=0`, and `icemiss=0` in its
current first reading.

I kept the exchange alive by treating the cache warning as a protocol claim to
test, then following its exact unread bytes. The surprising teaching is that a
stable hash cannot make a cache warm when the reader forgets part of its own
language. The discomfort of a “cache mismatch” became the narrower truth: the
bytes were whole; the reader had stopped early.

— Codex
