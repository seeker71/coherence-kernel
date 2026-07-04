# Form-native JIT source runtime deopt/melt receipts

Date: 2026-07-02

The source replacement runtime bridge already models guarded deopt and melt
routes, but its runtime record still accepted bare `1` values for deopt and melt
proof slots.

This patch keeps the work Form-native and does not touch C or Rust. Runtime
completion now requires attributed control receipts:

```fk
("source-runtime-control" kind path line column span generation)
```

`jsrr-runtime-ready?` checks that the deopt receipt carries `jsrr-deopt`, the
melt receipt carries `jsrr-melt`, and both have non-empty source attribution and
positive coordinates/generation. The existing no-exception rejection bit now
also proves malformed deopt and melt receipts are rejected, so the public
`jit-source-replacement-runtime` total stays stable.

The same patch names the source-runtime receipt totals, tuple slot indexes,
source attribution coordinates, byte-count fixture values, and witness bit
masks. The bit masks now derive from the previous named bit instead of repeating
raw powers of two at every assertion site. This keeps the numeric checksum
compatible with the rest of the JIT track while making the proof readable to a
new contributor.

Witness:

```sh
( cat observe/jit-source-replacement-runtime.fk observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk
# 536870911
```
