# Warning and Advisory Repair

Date: 2026-07-08

Scope: checkout witness health only. No new runtime meaning was added to the C
seed.

## What Was Repaired

- `runtime/fkwu-uni.c` no longer emits the checkout compile warnings for the
  `fread` declaration or `getsockname` pointer type.
- `form/form-stdlib/fkc-table-serialize.fk` now emits a portable
  `fk_socklen_t` for generated socket code, so regenerated bootstrap C does not
  reintroduce the `getsockname` pointer warning.
- `form/form-stdlib/bootstrap/fkwu-uni.c` and its stamp were regenerated from
  the Form emitter chain.
- `form/form-stdlib/bootstrap/fkwu-darwin-arm64` and its stamp were refreshed so
  clean Darwin/arm64 validation uses the committed fourth-arm witness instead of
  rebuilding it.
- `form/form-stdlib/bootstrap/form-cli-emitted.c` received the same generated
  socket type repair and compiles warning-free with `cc -O2 -c`.
- `form/scripts/validate_fkwu_native_surface.py` now reports same-tag same-arity
  native op aliases as aliases, not warnings. Mixed-arity conflicts and missing
  coverage remain diagnostics.
- Successful source compilation no longer prints a warning/advisory for pending
  `.dylib` emission. The `.fkb/.sym` artifacts are the current successful output;
  the `.dylib` fallback note is still observable with `FK_ARTIFACT_TRACE=1`.

## Witness

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
# no warnings

cd form && ./validate.sh form-stdlib/tests/source-runtime-release-metrics-band.fk
# validate_fkwu_native_surface: OK (... aliases=19, warnings=0)
# no stale bootstrap uni.c advisory
# source-runtime-release-metrics-band -> 131071

cc -O2 -c -o /tmp/form-cli-warning-check.o form/form-stdlib/bootstrap/form-cli-emitted.c
# no warnings
```

This is still a shrink target: the C file remains the temporary checkout seed.
The repair keeps the witness quiet and honest while runtime meaning continues to
move into Form/native-walker cells.
