# Form-native JIT source runtime consumes dylib lifecycle receipts

Date: 2026-07-02

## Movement

`observe/jit-source-replacement-runtime.fk` now directly requires the concrete
`.dylib` lifecycle receipts before source replacement can pass:

- `dylib-memory-envelope`
- `dylib-load-bind-runtime`
- `dylib-invoke-return-evidence`

Bad and stale lifecycle receipts are folded into the existing source-dylib
rejection lanes, so the public source-runtime witness remains stable while the
source-to-replacement bridge can no longer rely only on aggregate source-dylib
executor receipts.

No C or Rust runtime work was added. This does not claim arbitrary host byte
execution; it tightens the Form-owned readiness contract before that live
boundary.

## Witness

```sh
( cat observe/jit-source-replacement-runtime.fk \
      observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk
# 536870911
```
