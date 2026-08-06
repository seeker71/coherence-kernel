# Form-native JIT self-host live number cleanup

Date: 2026-07-02

## Movement

This pass cleans the active self-host live-evidence bridge without touching C or
Rust:

- `model/jit-self-host-live-evidence.fk` now names saturated receipt masks as
  `jshl-mask-N`, matching the adjacent container live-evidence bridge.
- The bridge now directly requires the current
  `source-dylib-container-executor` receipt before live execution can be marked
  contract-ready.
- `model/tests/jit-self-host-live-evidence-band.fk` now names each proof bit and
  derives its expected all-bits total instead of scattering powers of two
  through the band.
- Bad and stale source-dylib container receipts are folded into the existing
  rejection proof without increasing the witness total.

## Witness

```sh
( cat observe/jit-profile-receipt.fk \
      model/form-asm-x64.fk \
      model/jit-self-host-compiler.fk \
      model/jit-self-host-profile-runtime.fk \
      model/jit-self-host-ingress-runtime.fk \
      observe/jit-native-call-plan.fk \
      model/jit-self-host-native-call-runtime.fk \
      model/jit-self-host-host-membrane-runtime.fk \
      model/jit-self-host-live-evidence.fk \
      model/jit-self-host-final-native-gate.fk \
      model/tests/jit-self-host-live-evidence-band.fk ) > /tmp/jshl.fk
./fkwu --src /tmp/jshl.fk
# 1048575
```

## Boundary

- No C/Rust changes.
- No claim of arbitrary host byte execution.
- This is a readability and contract-hardening pass inside the Form-native JIT
  witness track.
