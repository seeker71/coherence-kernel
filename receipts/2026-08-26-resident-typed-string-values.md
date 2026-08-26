# 2026-08-26 — resident program images retained typed Form strings

The admitted in-memory PIF walker now executes compiler tags 24–30 and 32 as
native Form values: string-pool literal, byte length, equality, concatenation,
byte lookup, substring, search and integer rendering. String byte rows are
decoded through the canonical PIF decoder; no sentinel pool integer escapes as
a runtime string.

Type is carried as execution-created Form data, not guessed afterward. Every
resident result carries a parallel kind tree (`int`, `string`, `nothing`,
`absent`, or `list` with its element kinds); CALL carries the argument kind into
the callee's ARG, CONS extends the tree, and NTH recovers the selected child's
kind. No unavailable `value_kind` reflection participates in the walk. This is
also why present `nothing` and output absence remain different observations.

The canonical micro fixture calls a helper through tag 12 and returns
`"node166"` in 20 visited rows. Budget 19 returns a timeout trace after exactly
19 rows with output-count 0. The symbol-addressed fixture resolves
`unit/string-main`, carries its dependency on `unit/string-helper`, returns the
same typed value in five rows, and reaches the exact timeout cliff at four.

The negative surface is executable, not prose: an invalid pool coordinate,
scalar passed to string length, out-of-range byte coordinate, invalid substring
bounds, string passed to integer rendering, negative list coordinate, and bad
CALL target each produce a distinct loud reason and no output. Empty string,
string `"0"`, scalar `0`, and present `nothing` are observed as different
values.

Evidence:

```text
runtime-program-image-fkb-micro-walker.fk
  preflight balanced; errors 0; warnings 0; unresolved 0
  source/grammar mirrors byte-identical

runtime-program-image-fkb-micro-walker-band.fk
  1073741823, exit 0

runtime-program-image-fkb-symbol-walk.fk
  preflight balanced; errors 0; warnings 0; unresolved 0
  source/grammar mirrors byte-identical

runtime-program-image-fkb-symbol-walk-band.fk
  2147483647, exit 0

runtime-program-image-fkb-symbol-capability-bound-band.fk
  preflight balanced; errors 0; warnings 0; unresolved 0
  262143, exit 0

runtime-program-image-fkb-symbol-observation-band.fk
  preflight balanced; errors 0; warnings 0; unresolved 0
  262143, exit 0
```

No C seed, flattening path, model process, Metal carrier, filesystem lookup, or
artifact selector changed. The exact remaining resident request seam is
comparison and explicit frame state, then NodeID construction/access/equality,
then current file/hash/lookup and source-to-PIF admission. A manually assembled
canonical PIF proves resident semantics; production source-to-PIF-envelope
assembly is still not joined and receives no completion credit here. The
current public walker door seeds its raw top-level input as `int`; accepting a
typed entry envelope is therefore part of the explicit-frame crossing rather
than an unstated capability of this one.

Signed, Codex — sibling, this worktree.

Kept alive: native strings crossed CALL and symbol resolution without being
collapsed into numeric pool sentinels.

The surprising teaching: a canonical byte admission can prove artifact
identity while executable type semantics still need an independent witness.

Discomfort turned to gold when an initially green aggregate was withheld for a
missing reason bit; the missing observation exposed every newly reachable
refusal and made the symbol surface whole.

; witnessed: 2026-08-26 -> micro 1073741823; symbol 2147483647; strict NodeID/current-artifact request still owed
