# Form-native JIT self-host profile container slot gate

Date: 2026-07-01

## Commands

```sh
( cat observe/jit-self-host-completion-sweep.fk \
      observe/tests/jit-self-host-completion-sweep-band.fk ) > /tmp/jshcs.fk
./fkwu --src /tmp/jshcs.fk

( cat observe/jit-native-witness-sweep.fk \
      observe/tests/jit-native-witness-sweep-band.fk ) > /tmp/jnws.fk
./fkwu --src /tmp/jnws.fk

( cat observe/jit-live-runtime-integration.fk \
      observe/tests/jit-live-runtime-integration-band.fk ) > /tmp/jlri.fk
./fkwu --src /tmp/jlri.fk

( cat observe/jit-rung20-readiness.fk \
      observe/tests/jit-rung20-readiness-band.fk ) > /tmp/jr20.fk
./fkwu --src /tmp/jr20.fk
```

## Witness

```text
33554431
33554431
16777215
536870911
```

## Movement

`observe/jit-self-host-completion-sweep.fk` now requires the
`container-profile-live-slot-runtime = 1048575` receipt in addition to the older
hand-modeled `container-live-slot-runtime = 1048575` receipt. The self-host
completion total moves from 16777215 to 33554431 and rejects stale or missing
profile-derived container live-slot evidence before native-witness admission.

Downstream receipt consumers were updated to expect the stronger self-host
completion total. A later native-witness hardening also raised the native
witness sweep total to `33554431` by rejecting the stale self-host completion
receipt:

- `observe/jit-native-witness-sweep.fk`
- `observe/jit-live-runtime-integration.fk`
- `observe/jit-rung20-dylib-runtime-audit.fk`
- `observe/jit-rung20-readiness.fk`

No C or Rust runtime work was added. This makes the profile/register-derived
container live-slot bridge part of the self-host completion ledger instead of a
standalone side witness.
