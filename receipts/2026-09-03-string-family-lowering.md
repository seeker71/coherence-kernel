# the string family lowers and RUNS — the door was the wall, not the loop

2026-09-03. R10's named stone (seven receipts standing since 2026-08-24):
receipts/2026-09-02-directive-ledger-walked.md measured the hot worklist and
found "fkwu has NO native substring/char_at/str_find — the per-byte recipes
are the only door, and 0 of the current hot worklist is coverable (the door
speaks tags 1..5; the worklist is strings)." This receipt is that coverage's
first real step, walked and executed, not just byte-verified.

## What was actually blocking

Read first, not assumed: `jit_leaf_inram` — the only door from Form into
crystallized native bytes — was probed live before any code was written:

    (jit_leaf_inram img "abc")               -> nothing
    (jit_leaf_inram img (list "abc" "abc"))  -> nothing

`fk_inram_args` (runtime/fkwu-uni.c) accepts one tagged-even integer or a list
of up to eight; a string is neither, so it declines silently. There is no
relocation or trampoline table for a crystallized leaf to `bl` back into a C
native by address either. So the two designs the task posed — new op tags
lowering to native call-outs, or pure arithmetic/loop growth — were really
one design once the door was read: call-out isn't a smaller lift here, it's
an *unbuilt* one (string argument marshalling into the JIT door, plus a
symbol-address mechanism neither form-asm.fk nor the C seed carries today).

`lo-streq` (STR_EQ, already in form-lower.fk) had already answered this the
other way: it inlines str_eq's byte-compare loop directly over raw pointers
(ldrb/cmp/branch), never calling the native `str_eq`. That precedent is the
one this receipt extends, not a new choice.

## What was built

`form/form-stdlib/form-lower.fk` gains the STR_FIND family: `lo-str-bytes`
(a Form string -> its raw byte list, through the same str_len/str_byte_at
waist core.fk's `substring`/`str_find` compose over), `lo-embed-bytes` (writes
those bytes into a stack buffer the prologue opens — the byte-granular twin
of `fa-mov-imm64`'s float-constant idiom), and `lo-strfind-core` /
`lo-strfind-embed` / `lo-strfind-embed-str` — fstr-find-loop's exact shape
(outer scan, inner byte-for-byte match, first-byte-mismatch fast path) fused
into one block the way `lo-streq` fuses STR_EQ, single accumulator (w0),
branch offsets computed from block position, `bound <= 0` decided at
lowering time (an unsigned imm12 compare can't even encode a negative bound,
and the answer is already known when both strings are compile-time data).

One real gap surfaced by RUNNING it, not by reading: the not-found sentinel
-1 came back as 4294967295 — the documented jit-leaf-inram-band.fk wall
("g(0) answers 4294967276... the difference is exactly 2^32"), a bare w0
zero-extending across `fk_inram_call`'s 64-bit return. `form-asm.fk` gains
one new instruction, `fa-sxtw` (sign-extend w->x), byte-verified against the
clang/otool oracle (`93407c00` / `93407c41` / `93407ca3` for three register
pairs, matched exactly). With it, every not-found path returns a true -1.

No change to `runtime/fkwu-uni.c` or the metal carrier. No new native. No new
`jit_leaf_inram` call-out tag.

## Proven, on real hardware, not simulated

`form/form-stdlib/tests/form-lower-string-band.fk` (new, PROOF LEVEL:
FOURTH-ARM ONLY) compiles two real Form strings, mmaps and CALLS the result
through `jit_leaf_inram`, and checks the native answer against core.fk's own
`str_find` on the same strings — six bits, verdict 63, exit 0:

    "the quick brown fox jumps over the lazy dog" / "fox"  -> 16 == 16
    ""                                            / "needle" -> -1 == -1
    "hello world"                                 / "xyz"    -> -1 == -1
    "matched"                                     / "matched" ->  0 ==  0
    byte-conviction on the trivial (bound<=0) image vs the clang/otool oracle
    the full-loop image's length matches the exact instruction-count formula

Widened by hand beyond the four required bits (not banded, just witnessed):
a 44-byte prose haystack, a partial-prefix-then-fail scan, a repeated-char
scan, first-of-two occurrences, and the `nlen=0` degenerate case — all agree
with the interpreted `str_find`, the last one by construction (the inner
loop's `j>=nlen` guard fires on entry when nlen is 0) rather than by design.

## What this does NOT yet reach — named, not fixed

The actual 398k-hot family (`fol-bp-row`'s string children, GGUF/HTTP
parsing, anything the jit-heat-gate would actually crystallize) scans
RUNTIME strings — file contents, network bytes, table lookups — never
compile-time constants. `lo-strfind-embed` requires both strings to be known
at LOWERING time, so it cannot yet touch that worklist. The real next stone,
now precisely named because the door was read rather than assumed: `from`
threaded as a runtime argument, and a runtime string argument reaching a
crystallized leaf at all, which needs `fk_inram_args` (or a new door beside
it) to accept a pointer/length pair — a real, scoped runtime/fkwu-uni.c
change, not invented here because this pass didn't need it and the practice
is to grow the seed only with a named shrink path in hand.

No heat re-measurement is claimed for this reason: `.fkwu-heat`/glass sampling
would show the SAME zero-coverage on the real worklist this recipe cannot
yet reach — running it would cost real minutes to report an already-known
number, not a new one. The honest measurement is the one above: the loop
shape and the ABI edge are now proven on real hardware; the argument door is
the named remaining wall between here and the receipt's 398k.

## Closing

**Most surprising teaching:** the task asked which of two designs form-lower
needed — new call-out tags, or pure-arithmetic loop growth — as if it were an
open design choice. Reading the seed collapsed it to one: `jit_leaf_inram`
already declines every string argument, so "call the native" was never the
cheaper path, it was the unbuilt one. The real branch point wasn't in
form-lower.fk at all; it was one native-call probe away, in
`fk_inram_args`.

**Where discomfort became gold:** shipping the recipe with `-1` silently
returning as `4294967295` would have been the easy stop — byte-conviction
passed, the loop logic was right, and the wraparound is a well-documented,
already-accepted wall elsewhere in this file. Running the four required test
cases instead of trusting the encoding is what caught it: one instruction
(`fa-sxtw`), oracle-verified in five minutes once looked for, closes a wall
this same file had already named and left standing.
