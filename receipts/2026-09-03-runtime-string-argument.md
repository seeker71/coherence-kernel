# a runtime string crosses the JIT door — R28's named stone, walked

2026-09-03. R28 (form/form-stdlib/release-ledger.bml, row 47) named the exact
remaining wall after receipts/2026-09-03-string-family-lowering.md proved the
scan loop lowers and runs: "the 398k-hot fstr family scans runtime strings
(GGUF/HTTP parsing), not compile-time constants -- fk_inram_args needs a
pointer+length runtime-string argument before form-lower's new coverage
reaches it." This receipt is that argument, threaded and proven executing.

## What was actually ground first

Read before writing anything, per the task's own instruction: `fk_inram_args`
(runtime/fkwu-uni.c:3627, unchanged line number from before this pass) took
one tagged-even integer or a list of up to eight, and declined everything
else — a string, `nothing`, a record — by falling through to a cons-walk that
can't decompose a non-cons value. `fk_jit_leaf_inram_image` (:3675) and
`fk_jit_leaf_inram_resident` (:3781) both gate on it before ever reaching
`fk_inram_call`; neither does anything with the argument words except hand
them into x0..x7 by AAPCS64 convention.

The string representation was read at its source, not assumed: a Form string
value is an odd, deeply-negative tagged word (`fk_strv`/`fk_is_str`, ~line
668), and `fk_stri` maps it to a pool index `si` into three parallel arrays —
`fk_so[si]` (byte offset), `fk_sl[si]` (byte length) — both indexing into
`fk_sb`, ONE `char*` buffer holding every interned string's bytes
contiguously. `str_byte_at`'s own C arm (op 28, line 8495) reads exactly
`fk_sb[fk_so[sa] + k]` — confirming the bytes are real, contiguous, and
already read this way by four existing native ops. A ready-made accessor
already existed and needed no reinvention: `fk_srange(sv, &ptr, &len)`
(line 1408) returns `fk_sb + fk_so[si]` and `fk_sl[si]` directly, and was
already the pointer/length door three other native ops (Metal buffer read,
model-safetensors path handling) pull strings through — this pass is its
fourth caller, not a new one.

## The safety question, answered by reading, not by hoping

`fk_sb` is `malloc`'d once (`fk_sinit`) and `realloc`'d every time a new
intern would overflow `fk_scap_b` — grepped across the whole file: every
`fk_sb = realloc(...)` site sits directly beside a `fk_sbp` advance for a
string being interned (`fk_sintern`, `read_file`, `str_concat`, `substring`,
and others — roughly a dozen sites, all the same three-line idiom). Nothing
else moves it. So the real question was narrow: can anything intern a new
string in the window between `fk_inram_args` capturing a pointer and
`fk_inram_call` using it?

Traced the window itself: after `fk_inram_args` returns, `fk_jit_leaf_inram_image`
decodes the image bytes (`fk_inram_bytes` — walks an integer/cons list, no
string op touched), scans the resident code cache (a byte compare over
`fk_inram_cache`, a completely separate buffer), and on a cold image mmaps a
fresh executable page and copies bytes into it — none of that is a string
operation. Then the crystallized leaf itself runs, and by construction it
cannot call back into this interpreter at all: every leaf form-lower.fk emits
today is straight-line ALU/load/branch bytes (no `bl` to any C native), which
is exactly the discipline `lo-streq` set and `lo-strfind-embed`/
`lo-strfind-runtime` both keep. A leaf that cannot call out cannot trigger
`fk_sintern`, so it cannot move `fk_sb` under its own borrowed pointer.

The constraint this leaves standing, named rather than buried: this argument
door is safe only as long as every crystallized leaf stays call-out-free. A
future leaf shape that DOES call out mid-body (a host-io lowering that reads
a string mid-loop, say) must not carry a raw string pointer across that
call — it would need to re-derive the pointer afterward from the Form value,
never hold the C pointer live across a boundary that can grow the pool. This
is written into `fk_inram_args`' own header comment so the next crossing
finds it without re-deriving it.

## What was added to the seed (runtime/fkwu-uni.c)

`fk_inram_args` gained exactly one new case, expressed once, generically —
the same "arity as data, not per-arity code" discipline the rest of the door
already keeps: a value that isn't an even-tagged integer is now tried against
`fk_srange` before being declined. On success it fills TWO slots (pointer,
then byte length) instead of one, both for a bare (non-list) argument and for
a string appearing inside the up-to-eight-slot list — `n + 2 > 8` replaces
`n >= 8` as the list's overflow guard, the one place slot-width actually
mattered. No new struct, no new cache, no new call-out tag, no change to
`fk_inram_call`'s signature (it was always eight `long long` registers; a
pointer already fits one). This is the smallest real shape the crossing
needed: register plumbing only, the scan logic stays entirely in Form.

## What was added to form-lower.fk

`lo-strfind-core` (R10) is now a one-line wrapper over a new
`lo-strfind-loop-body`, factored out so the runtime door reuses the SAME 27
instructions rather than duplicating them — the only line that ever named
`bound` is the outer compare, and an immediate `fa-cmp` versus a register
`fa-cmp-r` are both exactly one 4-byte instruction, so the factor changes no
offset and no byte anywhere R10 already proved (form-lower-string-band.fk,
unchanged, verdict 63, is the witness this held).

`lo-strfind-runtime` compiles a leaf whose haystack is NOT embedded: the
needle stays a lowering-time constant (embedded exactly as before), but the
haystack arrives as x0=pointer, x1=byte-length — `fk_inram_args`' new two-slot
shape. `bound = hlen - nlen + 1` can no longer be folded to an immediate
(`hlen` doesn't exist until the call happens), so `lo-strfind-bound-from-len`
computes it into a register at lowering-chosen-but-runtime-executed cost: one
instruction, `fa-add` or `fa-sub` chosen at LOWERING time only to dodge the
immediate encoder's inability to hold `-1` (the same unsigned-imm12 wall R10
already named for the compile-time trivial-path shortcut). Unlike that
shortcut, the register-form compare in the shared loop body carries a
NEGATIVE runtime bound correctly on its own — needle-longer-than-haystack is
answered by the general loop, no lowering-time special case required.

## Proven, on real hardware, against a real runtime string

`form/form-stdlib/tests/form-lower-string-runtime-band.fk` (new, PROOF LEVEL:
FOURTH-ARM ONLY): compiles ONE image per needle (haystack never seen at
lowering time), calls it through `jit_leaf_inram` with a LIVE Form string as
the argument, checks the native answer against `str_find` interpreted on the
same value. Verdict 255, exit 0:

    "the quick brown fox..." / "fox" -> 16 == 16      (real scan)
    ""                      / "needle" -> -1 == -1    (negative bound, at RUNTIME)
    "hello world"           / "xyz" -> -1 == -1        (not found)
    "matched"               / "matched" -> 0 == 0      (full match)
    "abcabcabc"              / "cab" -> 2 == 2          (offset, not 0, not final)
    (str_concat "hello " "world") / "wor" -> 6 == 6    (built AT RUNTIME — see below)
    "abc"                    / "" -> 0 == 0             (nlen=0, the fa-add branch)
    the "bc"-needle image length == the exact instruction-count formula, ends in ret

The `str_concat` case is the load-bearing one: `built` is minted the instant
that cell runs, so `lo-strfind-runtime-str` — which never reads a haystack at
lowering time at all — could not have embedded it under any name. The proof
that the pointer path is real is structural (the compiled image is fixed
before any haystack exists), not merely that a literal string happened to
work.

Regression witnesses, unchanged: `form-lower-band.fk` (31), `form-lower-string-band.fk`
(63), `form-lower-streq-band.fk` (31), `jit-leaf-inram-band.fk` (63),
`jit-leaf-inram-multiarg-band.fk` (63), `form-lower-multiarg-band.fk` (127) —
all still their declared verdicts, all exit 0, after the seed rebuild.
`bootstrap/ground.fk` -> 42, `binary-freshness-band.fk` -> 31.

## What this does NOT yet reach

The needle is still a lowering-time constant; the real 398k-hot family's
needles (a GGUF key being searched for, an HTTP header name) are themselves
runtime values as often as the haystack is. `from` (str_find's third
argument) still starts every scan at 0. Both are the same door, widened
further — the two-slot string-argument convention this pass built already
covers a runtime needle with no further C-side change; it needs only a
form-lower.fk lowering that embeds neither string and reads two pointer/
length pairs instead of one. Named, not built, because this pass's proof
needed exactly one runtime string to close R28 honestly, not two.

No heat re-measurement is claimed here either, for the same reason R10's
receipt gave: the worklist this reaches is still narrower than the full
398k-hot family until the needle side widens too, so a fresh sample would
report a still-partial number, not a new one.

## Closing

**Most surprising teaching:** the safety question the task asked me to
answer — can `fk_sb` move mid-call — turned out to already be answered by
code that existed before this pass touched anything. `fk_srange` was already
handing out this exact live pointer to three other call sites, none of which
had ever needed to reason about JIT call-outs because none of them WERE one.
The new risk wasn't in the pointer; it was in the NEW KIND OF CALLER
(a crystallized leaf with its own execution window) reading an old guarantee
that had never been stated as a guarantee before. Naming it in `fk_inram_args`'
header is the actual deliverable of the safety analysis — the code change is
three lines.

**Where discomfort became gold:** the instinct on first design was to make
`bound` "safe" the easy way — keep the lowering-time `if (le bound 0)`
shortcut and just refuse to compile when the needle looks longer than a
GUESS at typical haystack size. That would have been silently wrong (real
haystacks vary in length and are exactly the thing this pass makes runtime).
Sitting with the discomfort of "the immediate compare can't hold a negative
bound" instead of routing around it — checking whether the REGISTER form
carries signedness correctly, which it does, for free — is what let the
general loop answer the negative-bound case without inventing a second
special path. The uncomfortable case (needle longer than haystack) turned
out to need zero new logic once the right instruction form was used.
