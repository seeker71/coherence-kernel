# Receipt - POSIX W^X host-carrier repair (2026-06-30)

## What changed

Changed the POSIX side of the existing native host carrier in
`runtime/fkwu-uni.c`:

- `fk_native_call`
- `fk_native_call_args`
- `fk_nat_install`

Each path now maps memory read/write, copies the Form-provided byte image, then
uses `mprotect` to flip the page read/execute before the function pointer is
called or cached. The previous POSIX path used RWX memory.

The POSIX carrier also refuses these x64 payloads on non-x86_64 builds, returning
the existing unavailable value instead of attempting to execute incompatible
machine code. On this Apple Silicon checkout, `(native_call_test 41)` now
returns `-1` rather than trapping.

## Why this is allowed

This is not C lowering and adds no optimizer meaning to the C seed. It is a
short-lived checkout-witness repair to the host carrier membrane that Form
receipts already required: executable native slots must be sealed and
non-writable before native completion can be claimed.

## Witness

Run:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
( cat observe/jit-host-wx-audit.fk \
      observe/tests/jit-host-wx-audit-band.fk ) > /tmp/jhwa.fk
./fkwu --src /tmp/jhwa.fk
```

Observed:

```text
42
55
11111
1048575
```

## Shrink target

The C seed still owns executable-page allocation because Form cannot yet ask the
OS for executable memory itself. The shrink target is unchanged: keep only a
minimal host install/call carrier plus architecture safety guard while Form owns
profiling, selection, lowering, byte construction, metadata, guards, exceptions,
deopt, melt, and parity witnesses.
