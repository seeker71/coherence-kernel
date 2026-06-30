# Bootstrap

This repo keeps bootstrap small and explicit. There is no `.sh` or `.py`
bootstrap in the tree. The host seed is one C compiler invocation that produces
the local `fkwu` runner, followed by two direct-source bootstrap witnesses and a
real body-cell check.

## Build `fkwu`

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

`fkwu` is intentionally ignored by git. Build it in the repo root when you enter
a fresh checkout.

Supported checkout-witness build rows:

```sh
# macOS / Linux
cc -O2 -o fkwu runtime/fkwu-uni.c

# Windows, mingw-w64 / TDM-GCC
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp

# Android arm64, off-device with the Android NDK
aarch64-linux-android34-clang -O2 -pthread runtime/fkwu-uni.c -o fkwu-android
```

These commands are the temporary checkout witness, not the destination. The
platform support membrane lives in Form at
`form/form-stdlib/host-os-membrane.fk`; the C seed shrinks toward that native
walker and the per-target Form emitters.

## Verify Direct Source Bootstrap

```sh
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Expected output:

```text
42
55
```

The first cell proves direct source execution. The second proves `defn` calls and
recursion through the same surface. The trailing `10` is kept as the checkout
convention; direct-source Form does not yet read argv without a table entry.

## Verify Real Grounding

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Expected output:

```text
11111
```

That is the minimum real-body grounding check after bootstrap: the local
C-bootstrapped runner is present, and it executes a real Form body cell through
the direct source path. This is not file-only grounding and it does not use Go,
flatten, or `T_flat`.

## Verify Platform Membrane

```sh
cat form/form-stdlib/hati-os-targets.fk \
    form/form-stdlib/host-os-membrane.fk \
    form/form-stdlib/tests/host-os-membrane-band.fk > /tmp/host-os-membrane.fk
./fkwu --src /tmp/host-os-membrane.fk
```

Expected output:

```text
8191
```

That witness does not mean every concrete carrier exists on every platform. It
means the Form body knows which targets are supported, which rows have metal
evidence, which carriers are still pending, and how the checkout C seed shrinks.

Large composed cells should stay in bounded witness slices while the checkout C
runner remains the seed. For example, the speech carrier run and carrier-gated
A/B proofs are separate direct-source bundles; joining every speech dependency
into one aggregate crosses the current seed's function table. Do not grow that
table for grounding. The destination is the native walker and Form-owned
emitters, not a larger C seed.

## What This Does Not Claim

The direct source bootstrap is the standing entry; no flattened
`form-eval-cli-loop.tbl` seed is required to ground or run the body.

The optional flattened `form-eval-cli-loop` path is a cache/parity door for the
Form meta-evaluator, not a gate for running the body. If a richer cell does not
fit the current `--src` surface, name the actual coverage gap.
