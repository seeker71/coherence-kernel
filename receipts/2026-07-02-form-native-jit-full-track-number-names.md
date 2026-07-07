# Form-native JIT full-track number names

Date: 2026-07-02

## Witness

```sh
( cat observe/jit-full-track-sweep.fk \
      observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 536870911
```

## Movement

`observe/jit-full-track-sweep.fk` now derives receipt totals from named
bit-width masks instead of repeating anonymous packed totals inline. The final
proof ledger also names each bit by the contract it proves, such as
`jfts-check-install-call-attempt-current` and
`jfts-check-dylib-runtime-container-current`.

This is a Form-only cleanup of the JIT witness surface. It also tightens the
full-track bridge by requiring current `.dylib` memory-envelope, load/bind, and
invoke/return lifecycle receipts directly, and by rejecting bad or stale
lifecycle receipts before the full-track sweep can compose.
