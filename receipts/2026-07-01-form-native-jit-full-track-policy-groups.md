# Form-native JIT full-track policy group gate

Date: 2026-07-01

This receipt tightens the compact full-track sweep without changing its outward
bit total. `observe/jit-full-track-sweep.fk` now requires the grouped policy
receipts already used by the native witness:

- `policy-front-sweep = 31`
- `policy-access-sweep = 7`
- `policy-cache-sweep = 15`

Commands:

```sh
( cat observe/jit-full-track-sweep.fk observe/tests/jit-full-track-sweep-band.fk ) > /tmp/jfts.fk
./fkwu --src /tmp/jfts.fk
# 524287

( cat observe/jit-host-membrane-readiness.fk observe/tests/jit-host-membrane-readiness-band.fk ) > /tmp/jhm.fk
./fkwu --src /tmp/jhm.fk
# 524287

( cat observe/jit-post-ingress-sweep.fk observe/tests/jit-post-ingress-sweep-band.fk ) > /tmp/jpis.fk
./fkwu --src /tmp/jpis.fk
# 1048575
```

The full-track total remains `524287`, but policy readiness now fails if any of
the grouped policy receipts are stale or malformed. This keeps the older
host-membrane and post-ingress receipt totals stable while making the upstream
Form-native optimizer ledger stricter.
