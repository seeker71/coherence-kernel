# Form-native JIT full-track current dylib receipts

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-full-track-sweep.fk \
      observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk

( cat observe/jit-host-membrane-readiness.fk \
      observe/tests/jit-host-membrane-readiness-band.fk ) > /tmp/jhm.fk
./fkwu --src /tmp/jhm.fk

( cat observe/jit-post-ingress-sweep.fk \
      observe/tests/jit-post-ingress-sweep-band.fk ) > /tmp/jpis.fk
./fkwu --src /tmp/jpis.fk
```

## Witness

```text
67108863
524287
1048575
```

## Movement

`observe/jit-full-track-sweep.fk` now requires the current dylib carrier path:
`dylib-carrier-abi = 4194303`, `install-call-attempt = 67108863`,
`dylib-image-manifest = 33554431`, `dylib-slot-runtime = 524287`,
`carrier-install-call-evidence = 4294967295`, and
`dylib-live-proof-suite = 536870911`.

The full-track sweep rejects the stale pre-div dylib carrier, install-attempt,
image, and slot receipts. Its witness moves from `524287` to `67108863`.

`observe/jit-host-membrane-readiness.fk` and
`observe/jit-post-ingress-sweep.fk` now consume that stronger full-track receipt
while preserving their established totals. No C, Rust, TypeScript, or bootstrap
carrier code was changed.
