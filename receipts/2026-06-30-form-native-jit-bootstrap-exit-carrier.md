# Form Native JIT Bootstrap Exit Carrier

Date: 2026-06-30

## Witness

```
( cat observe/jit-bootstrap-exit-carrier.fk observe/tests/jit-bootstrap-exit-carrier-band.fk ) > /tmp/jbec.fk
./fkwu --src /tmp/jbec.fk
# 1048575
```

## Receipt

`observe/jit-bootstrap-exit-carrier.fk` makes the bootstrap boundary explicit:
`runtime/fkwu-uni.c` is a checkout carrier and shrink target, not the surface
where new JIT meaning should accumulate.

The witness composes the existing host-exception, host-ingress, byte-ingress,
args-vector, runtime-stack, and checked-access receipts. It records that the
current Rust walker is proof-only: it has an independent lexer/evaluator and is
valuable for divergence detection, but it does not currently provide arbitrary
byte ingress, argument vector calls, W^X install state, host exception routing,
or executable carrier generation.

## Proved

- a future Rust replacement carrier and a future Form-native carrier are both
  admissible when they provide arbitrary byte ingress, argument vectors, W^X,
  source maps, exception maps, deopt maps, parity, full stack attribution, and
  positive generation;
- the current Rust proof walker remains pending and cannot claim native
  completion;
- C-bootstrap growth rejects instead of becoming the next implementation path;
- missing arbitrary byte ingress, stack attribution, or generation leaves the
  carrier pending;
- no replacement contract rejects;
- a complete replacement carrier routes native, deopt, exception, rewalk, melt,
  unavailable-architecture deopt, and mismatch reject cases.

## Shrink Direction

The next implementation surface should be the replacement carrier contract, not
another C-seed op. If the Rust full kernel is revived as the carrier, it must
enter through this same Form receipt surface and preserve the independent Rust
proof-walker as a small divergence witness.
