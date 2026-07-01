# Form-native JIT install/call attempt form carrier gate

Date: 2026-07-01

## Witness

```sh
( cat model/form-asm-x64.fk \
      observe/jit-install-call-attempt.fk \
      observe/tests/jit-install-call-attempt-band.fk ) > /tmp/jica.fk
./fkwu --src /tmp/jica.fk
# 33554431
```

## Receipt

Strengthened `observe/jit-install-call-attempt.fk` so the concrete install/call
attempt packet requires the `form-emitted-carrier-probe` receipt in addition to
the older live carrier probe. The `.dylib` route can no longer compose
install/call evidence through an unrelated black-box carrier status; it must
carry the proof that the observed carrier byte image is emitted by
`model/form-asm-x64.fk`.

Updated the downstream `.dylib` receipts that depend on install/call attempt
totals:

- `observe/jit-carrier-install-call-evidence.fk`
- `observe/jit-dylib-image-manifest.fk`
- `observe/jit-profile-dylib-slot-runtime.fk`
- `observe/jit-dylib-invoke-return-evidence.fk`

No C or Rust runtime work was added. This still does not expose arbitrary host
byte ingress; it tightens the Form-owned route toward the `.dylib` carrier by
requiring Form-emitted byte identity before the install/call packet can pass.
