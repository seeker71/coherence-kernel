# Receipt - source replacement requires the container dylib executor (2026-07-02)

## Movement

`observe/jit-source-replacement-runtime.fk` now names the
`source-dylib-container-executor` receipt and requires it before the source
recipe to replacement runtime contract can pass.

`observe/jit-full-track-sweep.fk` also stops repeating raw receipt totals in
every constructor. It now has named receipt slots, named current/stale totals,
and a `jfts-current-receipt` helper so the full-track ledger reads as a list of
named dependencies instead of duplicated numbers.

The public witness total remains stable at `536870911`. The existing
source-dylib rejection bits now also cover bad and stale container executor
receipts, so downstream gates do not churn just because this dependency became
explicit.

## Why

The JIT track had already built the Form-native container `.dylib` bridge for
list, array, dict, hashmap, and red-black-tree access, but source replacement
still named only the general source-dylib runtime executor. That left one more
hard-coded implicit edge in the live path.

This round replaces that implicit number with named totals and receipt helpers:

- `source-dylib-container-executor = 4194303`
- stale pre-current container executor receipt = `2097151`
- one-short bad container executor receipt = `4194302`

It also starts the broader cleanup the repo needs: receipt numbers still exist
because they are witness totals, but they now live behind named Form functions
at the ledger boundary instead of being sprinkled through every call site.

## Witness

```sh
( cat observe/jit-source-replacement-runtime.fk \
      observe/tests/jit-source-replacement-runtime-band.fk ) > /tmp/jsrr.fk
./fkwu --src /tmp/jsrr.fk
# -> 536870911

( cat observe/jit-full-track-sweep.fk \
      observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# -> 536870911
```
