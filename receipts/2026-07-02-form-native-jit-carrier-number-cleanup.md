# Form-native JIT carrier ledger number cleanup

Date: 2026-07-02

## Movement

`observe/jit-carrier-install-call-evidence.fk` no longer encodes the final
carrier/install/call evidence surface as raw packed integers:

- receipt totals are derived from named bit-width masks with `jcice-mask`
- bad receipt totals use `jcice-one-short`
- the stale source-dylib container receipt is named as a stale bit-width mask
- the saturated witness bits now have semantic names such as
  `jcice-bit-bad-dylib-call-chain-rejects` and
  `jcice-bit-bad-container-or-cache-rejects`

This is a Form-only readability cleanup of a top-level JIT live-execution
ledger. It does not add C/Rust work and does not claim arbitrary host byte
execution.

## Witness

```sh
( cat observe/jit-carrier-install-call-evidence.fk \
      observe/tests/jit-carrier-install-call-evidence-band.fk ) > /tmp/jcice.fk
./fkwu --src /tmp/jcice.fk
# 4294967295
```
