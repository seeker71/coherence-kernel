# Receipt — fkwu runs the REAL server cell: native-vs-rented -> 11111, no Go walker (2026-06-29)

**The arc (Urs's corrections):** dropped T_flat (deprecated), then dropped leaning on the Go walker (a proof
sibling, never the runtime). The requirement: **`fkwu` itself runs the Form server cells**, via its own native
source path. Stone 4 gave multi-function + cross-calls. This stone adds the rest the real cell needs — and the
real cell now runs.

## THE RESULT — `fkwu --src observe/native-vs-rented.fk` (NO Go, NO flatten, NO T_flat)

```
native-vs-rented-check -> 11111      (the Go walker gave 11111 — bit-identical)
```

A real committed body cell — 6 functions, cross-calls, 2-arg `nvrk`, `(list 9 7)`, `head`/`tail`/`gt`, five
assertions — runs on the c-bootstrapped runtime and matches the proof walker exactly. The Go walker is now
genuinely unneeded for this cell: it remains only what it always was, a four-way proof sibling.

## Unit witnesses (fkwu --src)

```
(head (list 9 7))            -> 9
(head (tail (list 9 7)))     -> 7
(gt 9 7) / (gt 7 9)          -> 1 / 0
(do (defn k (c b)(if c b 0))) (k 1 42)  -> 42   (2-arg call)
```

## What landed (stone 5)

`runtime/fkwu-uni.c`:
- **Lists in `--src`:** `head`(20)/`tail`(21)/`cons`(19)/`empty`(18); `(list a b ..)` -> nested `cons` ending in
  `empty` (`fk_parse_list`).
- **Comparisons:** `gt`/`lt` lower to `if (le ..) 0 1`; `ge` -> `le(b,a)`. No new runtime op — pure lowering.
- **Multi-arg calls:** arity per function (`fk_fnar`, set before the body so self-recursion reads it). A call
  emits tag 12 (0/1-arg) or **tag 240** (2-arg: push both args, `fp = vsp - 2`). tag 240 is a new `fk_walk`
  handler — a `--src` native-call path; the four-way bands (flattened tables) do not use it, so it introduces no
  band divergence (it is the fkwu source-runner's call carrier, not a band op).

Regression clean: `fac(5)`=120, `g(5)`=12 (stone 4), `(add 40 2)`=42, `do`/`let`=21, numeric-table=42.

## Where this lands the whole arc

The purely-Form server (the new repo) can now be authored in `.fk` and run by **fkwu itself** on Windows — the
sovereignty target, not the Go bootstrap. `native-vs-rented` (the oracle-economy decision cell) is the proof: a
real body cell, full grammar, native on the c-bootstrapped kernel, agreeing with the walker. The remaining body
cells (surprise-receipt, sense-stream, mesh-sense-7w) run on this same grown source-runner as their grammar is
exercised; strings + the string pool are the next surface when a cell needs them.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > nvr.fk
./fkwu.exe --src nvr.fk     # -> 11111
```
