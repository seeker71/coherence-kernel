# Form-native JIT dylib image manifest

Date: 2026-07-01

## Witness

```sh
( cat model/form-asm-x64.fk \
      observe/jit-dylib-carrier-abi.fk \
      observe/jit-dylib-image-manifest.fk \
      observe/tests/jit-dylib-image-manifest-band.fk ) > /tmp/jdim.fk
./fkwu --src /tmp/jdim.fk
# 16777215
```

## Receipt

Added `observe/jit-dylib-image-manifest.fk` and its band test. The cell makes
the loadable `.dylib` carrier artifact explicit in Form: exported symbols, ABI,
byte image, imports, relocations, text permissions, sealing, source/fault/deopt
maps, owner, no-C-growth, and generation are all checked before the carrier
ledger can advance.

The manifest recognizes the old add1 probe bytes, but live routing uses the
checked array/field payloads accepted by the existing carrier ABI. Valid images
route native/deopt/exception/rewalk/melt through the carrier ABI. C-lowering
owners, missing source maps, writable text, zero generation, unknown imports,
out-of-bounds relocations, missing required exports, and stale slot-fault
receipts reject.

No C or Rust runtime work was added. The `.dylib` remains a carrier/output
artifact whose meaning is described and gated by Form.
