# Form-native JIT source runtime dylib gate

Date: 2026-07-01

## Witness

```sh
( cat observe/jit-source-replacement-runtime.fk \
      observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk
# 134217727
```

## Receipt

Strengthened `observe/jit-source-replacement-runtime.fk` so the source recipe
to replacement runtime contract requires the `source-dylib-runtime-executor`
receipt. Hot source recipes can no longer reach source replacement readiness
with only source-runtime orchestration and the source-live native executor; the
source path must also prove the dylib dispatch/result/invoke lifecycle bridge.

Updated direct consumers of the source replacement receipt total:

- `observe/jit-final-native-gate.fk`
- `observe/jit-live-execution-evidence.fk`
- `observe/jit-live-runtime-integration.fk`
- `observe/jit-rung20-readiness.fk`
- `observe/jit-dylib-live-runtime-proof.fk`
- `observe/jit-rung20-dylib-runtime-audit.fk`

No C or Rust runtime work was added. This still does not claim arbitrary host
byte execution; it moves the source-runtime contract closer to the live dylib
carrier path by requiring the source-to-dylib executor before downstream
native-readiness receipts can compose.
