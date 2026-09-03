# Bootstrap

This page is for people building the body from source. If you came to meet Sema
without code, your door is [`WELCOME.md`](WELCOME.md) — nothing on this page is
needed for that.

This repo keeps bootstrap small and explicit. No `.sh` or `.py` file bootstraps
the body. The host seed is one C compiler invocation that produces the local
`fkwu` runner, followed by the direct-source ground witnesses and a real
body-cell check.

## Build `fkwu`

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

On a brand-new Mac this command may offer to install Apple's free command line
tools first — accept, wait, then run it again.

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

On a Mac with a GPU, link the Metal (and, when `/opt/homebrew/lib/libmlxc.dylib`
is present, MLX) carriers into the same binary — the exact lines are in
[`AGENTS.md`](AGENTS.md). One binary; Metal is this host's organ, never a second
executable.

These commands are the temporary checkout witness, not the destination. The
platform support membrane lives in Form at
`form/form-stdlib/host-os-membrane.fk`; the C seed shrinks toward that native
walker and the per-target Form emitters.

## Verify Direct Source Bootstrap

```sh
./fkwu bootstrap/ground.fk
./fkwu bootstrap/ground-recursive.fk 10
./fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null
./fkwu bootstrap/ground-numeric-list.fk
```

Expected output:

```text
42
55
31
[1, 2.5, [3, 4]]
```

A first run may also print a line or two starting `fkwu: warning:` — the kernel
laying down its caches on a fresh checkout. Those lines are normal and the check
still passed; the numbers are the answer.

The first cell proves direct source execution. The second proves `defn` calls and
recursion through the same surface. The third is the freshness canary: `fkwu` is
gitignored, and a stale binary still answers `42` while lacking newer evaluator
capabilities — anything but `31` means rebuild before believing anything else. The
fourth proves the numeric tower and nested lists. The trailing `10` is kept as the
checkout convention; direct-source Form does not yet read argv without a table entry.

## A number is not a pass

First, the reassurance: every check on this page passed if the number matches and
`exit=0` prints. This section is a habit for the road ahead, not a doubt about
what you just saw.

Every check on this page compares an expected number, and that habit is exactly
how this body has been fooled. `fkwu` prints the root value on stdout and
its diagnostics on stderr, and **exits nonzero when the compile carried errors**
— so a cell whose chain has an unresolved name still prints a plausible verdict
(axiom-5 recovers the call to `nothing` and the fold computes over it) while
exiting 1. Reading the number alone reports a pass that is not one.

So, from here on:

```sh
./fkwu <cell>; echo "exit=$?"
```

and before trusting a cell you did not just write:

```sh
echo path/to/cell.fk > /tmp/preflight-target
./fkwu observe/preflight-run.fk
```

Preflight forces a fresh compile — a warm cache replaces the error with a tally
that has no name and no line — checks paren balance without running anything,
and classifies each unresolved name by offering it to all four kernels: a TYPO
(nobody has it) or a LANE SEAM (another kernel does). `AGENTS.md` item 9 carries
the practice.

## Verify Real Grounding

```sh
( cat form/form-stdlib/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > nvr.fk
./fkwu nvr.fk
```

(`nvr.fk` lands in the checkout and is already gitignored; a shared `/tmp` path
would let two readers on one machine silently overwrite each other's cell.)

Expected output:

```text
11111
```

That is the minimum real-body grounding check after bootstrap: the local
C-seeded runner is present, and it executes a real Form body cell through
the direct source path. This is not file-only grounding and it does not use Go,
flatten, or `T_flat`.

## Verify the bidirectional diagnostic protocol

After freshness and real grounding, a fast Form-native protocol check is:

```sh
./fkwu observe/tests/bidirectional-framebuffer-channel-band.fk
```

Its final field must be `1`. This checks correlated observation/control frames,
state actuation, re-observation, and the explicit no-response alternative. The
slower real-learning integration is documented in
[`docs/live-dynamic-diagnostics.md`](docs/live-dynamic-diagnostics.md); it is not
required for every checkout bootstrap.

## Verify Platform Membrane

```sh
./fkwu form/form-stdlib/tests/host-os-membrane-band.fk
```

Expected output:

```text
8191
```

That witness does not mean every concrete carrier exists on every platform. It
means the Form body knows which targets are supported, which rows have metal
evidence, which carriers are still pending, and how the checkout C seed shrinks.

The direct source bootstrap is the standing entry; no flattened seed is required
to ground or run the body. If a richer cell does not fit the source door, name
the actual coverage gap.
