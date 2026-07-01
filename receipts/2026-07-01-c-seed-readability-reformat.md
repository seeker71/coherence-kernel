# 2026-07-01 -- C-seed readability reformat (no growth)

## The ask, and the tension named up front

Urs asked for a "clean and well written version of `runtime/fkwu-uni.c` ... that does not look like it was
written by a generator ... SOTA coding, architecture, design and engineering practices."

Taken literally, that request runs directly against this file's own governing rule. `AGENTS.md` calls
`runtime/fkwu-uni.c` "a temporary seed and a shrink target, not the destination" and states: "Do not grow the
C seed as the kernel's home... if a patch grows `runtime/fkwu-uni.c`, it must either be a short-lived
checkout-witness repair with an explicit shrink receipt, or it should be rejected in favor of moving that
capability into the native walker/Form body." `MANIFEST.md` calls the whole thing "a documented one-liner, not
a script in the tree," and `form/form-stdlib/host-os-membrane.fk` tracks this file's shrink trajectory as Form
data -- the authority for its direction lives in Form, not C. A full SOTA multi-file architectural rewrite would
be exactly the kind of investment in permanence the doctrine warns against, even with zero new capability added.

Named honestly to Urs before touching anything; offered three paths (reformat-only, full override rewrite,
review-only). Urs chose **reformat only, no growth** -- readability, zero capability change, paired with this
receipt, as the doctrine itself requires for any patch that touches this file's shape.

## Ground (before and after, bit-identical)

```sh
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
./fkwu.exe --src bootstrap/ground.fk
./fkwu.exe --src bootstrap/ground-recursive.fk 10
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu.exe --src /tmp/nvr.fk
```

Witness, identical before and after the reformat:

```text
42
55
11111
```

A broader smoke (host-os membrane band, exercises far more of the source-parser/eval surface):

```sh
cat form/form-stdlib/hati-os-targets.fk form/form-stdlib/host-os-membrane.fk \
    form/form-stdlib/tests/host-os-membrane-band.fk > /tmp/host-os-membrane.fk
./fkwu.exe --src /tmp/host-os-membrane.fk
```

```text
8191
```

## What changed

Purely mechanical whitespace and comment-flow reformatting. `runtime/fkwu-uni.c` went from 1,977 physical
lines (many single lines packing dozens of statements and multi-hundred-word comments) to 8,480 lines of
one-statement-per-line, consistently-indented, consistently-wrapped C. **Zero code tokens were added, removed,
or reordered. Zero externs, functions, macros, or capabilities changed. Zero new files.**

Method (tooling kept out of the tree -- no `.sh`/`.py` landed in the repo, per the hard gate in `MANIFEST.md`):

1. A small lexer (code/string/char/comment states, run only in a scratch workspace) verified the file contains
   no `/*`/`*/`-like sequences inside any string or char literal -- so comment boundaries can be found safely
   without risk of corrupting string content.
2. Every one of the file's 317 block comments was isolated onto its own line and its internal whitespace
   normalized to a single logical line. This is whitespace-only: comment prose carries no semantics, and this
   step exists only so every comment -- whether it was originally crammed into a giant line or already
   hand-wrapped with its own ad hoc indentation -- gets the *same* treatment in the next step, instead of a
   patchwork of styles depending on how a given comment happened to be typed originally.
3. `clang-format 15` (LLVM base style, 4-space indent, 100-col, attached braces, comment reflow on, forced
   multi-line function/if/loop bodies, `BreakStringLiterals: false` so no string literal is split into
   concatenated pieces) reformatted the result.
4. Verification, before touching the tracked file:
   - Re-split both the original and the formatted output through the same code/string/char/comment lexer.
     Code tokens, whitespace-stripped: **identical** (140,102 chars). All **317/317 comment bodies** matched
     word-for-word (leading `*` continuation markers stripped before comparing, since `ReflowComments` adds
     those as line decoration, not content).
   - Compiled the reformatted source with the exact same flags as the original. Output binary: same size
     (332,017 bytes) as a fresh rebuild of the *unmodified* original, compiled back to back. The two binaries
     differ in exactly **8 bytes**, all at PE build-timestamp/debug-directory offsets (136-137, 216-217,
     255519-255522) -- the same handful of bytes that differ between any two separate compiles of literally
     identical source. No code-section byte differs.
   - Ran the grounding witnesses above against the rebuilt binary: bit-identical to pre-reformat.

## Honest boundary

This is **not** the SOTA architectural rewrite Urs's words literally asked for, and that's a deliberate,
named tradeoff, not an oversight: no new files, no module boundaries, no header split, no renamed identifiers,
no new abstractions. Those would have made the seed a nicer place to live -- which is precisely what the
seed's own doctrine says not to do. What shipped is the readability floor that doctrine explicitly allows: a
"short-lived checkout-witness repair," scoped to formatting, receipted here.

The line count growing 1,977 -> 8,480 is *not* the kind of growth the shrink rule is protecting against --
it's the same tokens, unpacked from cram to one-statement-per-line. The capability surface, the extern list,
and every comment's actual content are byte-for-byte (mod whitespace) what they were.

`runtime/fkwu-uni.stamp` (`f8e8ede8c128ab57`) was left untouched. Nothing in the tree currently reads or
checks it (`grep -r fkwu-uni.stamp` outside itself: no hits) -- it appears orphaned from an earlier tooling
pass. Regenerating it would mean guessing at an unknown hash algorithm and writing a fabricated value; that is
exactly the kind of dressed-up guess this repo's practice forbids, so it was left alone and named here instead
of silently faked.

No platform receipts were re-run (mac/android device metal). This reformat was verified on Windows (the
platform this checkout is on) with source-level and token-level equivalence proofs that hold regardless of
platform, since nothing platform-specific changed -- but a fresh device trace was not captured tonight.
