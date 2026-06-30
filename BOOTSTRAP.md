# Bootstrap

This repo keeps bootstrap small and explicit. There is no `.sh` or `.py`
bootstrap in the tree. The host seed is one C compiler invocation that produces
the local `fkwu` runner, followed by a direct source execution check against a
real body cell.

## Build `fkwu`

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

`fkwu` is intentionally ignored by git. Build it in the repo root when you enter
a fresh checkout.

## Verify Real Grounding

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Expected output:

```text
11111
```

That is the minimum grounding check: the local C-bootstrapped runner is present,
and it executes a real Form body cell through the direct source path. This is not
file-only grounding and it does not use Go, flatten, or `T_flat`.

## What This Does Not Claim

Do not require, invent, or wait for a flattened `form-eval-cli-loop.tbl` seed.
That framing is obsolete: the direct source bootstrap is the standing entry.

The optional flattened `form-eval-cli-loop` path is a cache/parity door for the
Form meta-evaluator, not a gate for running the body. If a richer cell does not
fit the current `--src` surface, name the actual coverage gap instead of
claiming a missing table seed.
