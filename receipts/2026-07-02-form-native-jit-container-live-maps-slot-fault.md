# Form-native JIT container live maps and slot-fault bridge

Date: 2026-07-02

## Witness

```sh
( cat observe/jit-profile-receipt.fk \
      model/jit-container-lowering.fk \
      model/jit-container-backend.fk \
      model/jit-container-byteplan.fk \
      observe/jit-loader-contract.fk \
      model/jit-container-loader.fk \
      model/jit-container-profile-runtime.fk \
      model/jit-container-replacement-runtime.fk \
      model/jit-self-host-container-live-evidence.fk \
      model/tests/jit-self-host-container-live-evidence-band.fk ) > /tmp/jshcl.fk
./fkwu --src /tmp/jshcl.fk
# 4194303
```

## Movement

`model/jit-self-host-container-live-evidence.fk` now names the live-evidence
receipt surface for list, array, dict, hashmap, and red-black-tree replacement
payloads. The container bridge requires current carrier/install/call evidence,
live-execution evidence, and slot-runtime fault evidence before a complete
container live proof can count.

The band now carries source, maps, slot-fault, no-C-growth, and generation
through the same live proof shape as the self-host bridge. It rejects missing
carrier/install/call facts, missing source/maps/slot-fault, bad live receipts,
missing receipt source/maps, uninstalled slots, and bad static tickets while
preserving the public witness total.

This is Form-only and does not claim arbitrary host byte execution.
