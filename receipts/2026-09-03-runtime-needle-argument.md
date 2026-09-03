# a runtime needle joins the runtime haystack — R34's named stone, walked

2026-09-03. R34 (form/form-stdlib/release-ledger.bml, row 50) named the exact
remaining wall after receipts/2026-09-03-runtime-string-argument.md (R28)
proved a runtime HAYSTACK reaches the crystallized scan: "the needle side of
str_find still lowers only as a compile-time constant; a runtime needle (a
GGUF key, an HTTP header name) needs the same two-pointer convention R28 gave
the haystack." This receipt is that convention, walked one string further,
plus the `from`-offset widening R28's own closing separately named and left
unbuilt. Continuing that receipt's story, not replacing it — R28 stays
exactly as it reads; nothing there was wrong, and nothing here edits it.

## What was actually ground first

Read before writing anything, per the task's own instruction: R28's receipt
in full, `lo-strfind-runtime`/`lo-strfind-runtime-str`/`lo-strfind-loop-body`
in form/form-stdlib/form-lower.fk, and `fk_inram_args` in
runtime/fkwu-uni.c:3655 (unchanged line number — this pass touches no C).

## The arity question, answered by reading, not by hoping

R34's own ledger row asked implicitly what R28's receipt already answered in
its "what this does NOT yet reach" section: fk_inram_args maps each element
of a Form argument LIST through the same int-or-string test in sequence, so
a list of two strings already yields four slots (2+2) and an optional third
integer (`from`) a fifth — five of AAPCS64's eight, three to spare. Built a
minimal probe first to confirm this before writing any lowering: a bare
`(list stringA stringB intC)` handed to a diagnostic image that returns
`x3` and `x4` directly answered the string's own length and the int
unmodified. **fk_inram_args needed no change and received none.** Rebuilding
the seed with zero C-side edits and re-running R28's own band
(form-lower-string-runtime-band.fk) first, before writing a line of new
form-lower.fk, confirmed the door was already exactly as wide as it needed
to be — the C rebuild in this pass changed no `.c`/`.m` file at all.

## What was added to form-lower.fk

`lo-strfind-loop-body` (already factored once, by R28, to share `boundcheck`
between an immediate and a register outer compare) gained two more
parameters: `innercheck` (the inner loop's own bound — `cmp w5,#nlen` when
the needle is a lowering-time constant, `cmp w5,w10` when it is not, since
`nlen` no longer exists at lowering time once the needle is runtime) and
`initw0` (the loop's own starting `i` — `movz w0,#0` for every existing
caller, unchanged, or `mov w0,w4` for a runtime `from`). Both existing
callers (`lo-strfind-core`, `lo-strfind-runtime`) were updated to pass their
old hardcoded forms explicitly; form-lower-string-band.fk (63) and
form-lower-string-runtime-band.fk (255) are the byte-identical witnesses
this held.

`lo-strfind-runtime-both-core` embeds **neither** string — no
`lo-embed-bytes` call at all — so the compiled image is a fixed length
regardless of what is searched for or in. `bound = hlen-nlen+1` is computed
the fully general way now (`sub w9,w1,w3` then `add w9,w9,#1`, two
instructions, correct for every relationship between the two lengths,
including nlen=0 and hlen<nlen, with no lowering-time special case at all —
neither length is known then). The needle pointer needs no move: the
argument door's third slot (x2) already lands exactly where the loop body
reads `n_base`. `lo-strfind-runtime-both` (from always 0) and
`lo-strfind-runtime-both-from` (from a runtime fifth argument) share this
core, differing by exactly the two parameters named above.

## The bug this pass's own proof caught, not avoided

The first version of `lo-strfind-runtime-both-from` set `i = from` (via
`initw0`) but left `h_cur = h_ptr + 0` unconditionally (the same
`fa-add-x-imm 1 0 0` the plain, always-from-0 lowering correctly uses). This
compiles, runs, and returns a plausible-looking integer — it does not crash
or decline to answer. It is simply wrong: `h_cur` is the pointer `i` indexes, so once
`i` and `h_cur` start out of lockstep, the loop reads bytes at position
`i - from` while believing it is reading position `i`. A search for the
second occurrence of `"abc"` in `"xabcabc"` starting at `from=1` returned
`2` instead of `1` — not a crash, not `nothing`, a wrong number that reads
like a plausible index. Caught by testing the exact case the task specified
("find the SECOND occurrence... by passing a nonzero `from`") against
`str_find` interpreted, not by inspection — the byte-length and structural
checks (c0-c7 below) all passed first, on the WRONG lowering, because none
of them exercised a nonzero `from`.

The fix needed a new primitive: `h_cur = h_ptr + from` is a 64-bit pointer
plus a RUNTIME register offset, which `fa-add-x-imm` cannot encode (its
offset is a lowering-time immediate) and a 32-bit `fa-add-r` cannot safely
carry it either (AArch64 zeros a 32-bit destination's upper bits, which
would truncate the pointer). form-asm.fk gained one new encoder,
`fa-add-x-r (rd rn rm)` — ADD (shifted register), 64-bit form, the exact
X-register sibling of the existing 32-bit `fa-add-r`, same field layout,
`sf` bit set. `lo-strfind-runtime-both-from` now passes
`(fa-add-x-r 1 0 4)` as `hcur-setup`, a new fourth parameter to
`lo-strfind-runtime-both-core` alongside `initw0`, so `i` and `h_cur` always
move in lockstep from a shared `from`.

## Proven, on real hardware, against two real runtime strings

`form/form-stdlib/tests/form-lower-string-both-runtime-band.fk` (new, PROOF
LEVEL: FOURTH-ARM ONLY): compiles each image (`lo-strfind-runtime-both`,
`lo-strfind-runtime-both-from`) exactly ONCE — unlike R28's band, which
compiled one image per needle since the needle was still embedded, neither
image here ever reads a string at lowering time, so one image each is the
whole proof surface. Every haystack and needle is built via `str_concat` at
cell-run time (no test value is a single literal the image could have seen
under another name). Verdict 511, exit 0:

    a real scan at an interior offset            native == interpreted
    empty needle, both sides runtime              native == interpreted
    needle equals haystack (bound == 1)           native == interpreted
    needle longer than haystack (negative bound)  native == interpreted, at RUNTIME
    needle at offset 0                            native == interpreted
    needle at the final valid offset (hlen-nlen)  native == interpreted
    not found                                     native == interpreted
    compositional: both images are 128 bytes      -- fixed, regardless of
                    (32 instructions), fixed          any string above
    the SECOND occurrence, via a nonzero RUNTIME  native == interpreted
    `from`

Regression witnesses, unchanged: `form-lower-band.fk` (31),
`form-lower-string-band.fk` (63), `form-lower-string-runtime-band.fk` (255),
`form-lower-streq-band.fk` (31), `jit-leaf-inram-band.fk` (63),
`jit-leaf-inram-multiarg-band.fk` (63), `form-lower-multiarg-band.fk` (127),
`form-asm-band.fk` (31) — all still their declared verdicts, all exit 0.
`bootstrap/ground.fk` -> 42, `binary-freshness-band.fk` -> 31.

## What this does NOT yet reach

`from` is now threaded, but only alongside a fully-runtime needle
(`lo-strfind-runtime-both-from`) — `lo-strfind-runtime` (R28's runtime
haystack, embedded needle) still starts every scan at 0, unchanged, because
changing it was not needed to close R34 and this pass did not touch it.
`from` itself is never validated against the haystack's own length at
lowering time or runtime (a caller-supplied `from` larger than `hlen` is
handled correctly by the general bound check — proven structurally, not
merely assumed, since `bound` is a runtime register comparison indifferent
to how far `i` starts past it — but a negative `from` was not among the
proven cases here; `str_find`'s own interpreted form does not special-case
one either, so this is parity, not a new gap). No heat re-measurement is
claimed, for the same reason both R10's and R28's receipts gave: the
worklist this reaches is still narrower than the full 398k-hot family until
every hot call site is walked to use these lowerings, which is separate
work from proving the lowering itself correct.

## Closing

**Most surprising teaching:** the compositional and structural proofs (c0
through c7 — byte length, real scans, edge lengths, the negative bound) all
passed cleanly on the FIRST version of the `from`-offset lowering. They
proved nothing was embedded and nothing crashed. None of them could have
caught the lockstep bug, because none of them moved `i` and `h_cur` out of
alignment with each other — every one of those cases implicitly used
`from=0`, where the bug is invisible by construction (`h_ptr+0` is correct
whether or not you remembered `from` at all). The task's own instruction to
prove the second-occurrence case specifically — not "prove `from` exists"
but "prove `from` finds the right answer" — is what surfaced it. A green
suite that never exercises the one input that changed is not evidence for
the part that changed.

**Where discomfort became gold:** the instinct on finding the wrong answer
was to suspect the argument door first (had `fk_inram_args` silently
mis-sliced a five-slot list?) — the more complicated, more alarming
explanation, and the one that would have meant the "no C-side change needed"
finding from earlier in this pass was wrong. Sitting with the discomfort of
re-checking that claim by direct probe (the `x3`/`x4` isolation tests) before
touching the C file at all is what kept the fix scoped to the one line that
was actually wrong: `h_cur`'s own starting offset, three register-widths
away from where the alarm first pointed. The door was exactly as trustworthy
as the earlier reading found it; the bug was entirely form-lower.fk's own,
and small once found at the right layer.
