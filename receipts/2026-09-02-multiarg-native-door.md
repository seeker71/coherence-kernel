# the multi-arg native door — first rung of the string-crystallize course

2026-09-02 afternoon. "Next tune." The heat worklist names the per-byte
string family; crystallizing fstr-find-loop (byte loads, arithmetic,
branches, tail recursion, NO allocation — the perfect first string
candidate) needs four rungs: multi-arg through the native door,
string-pointer passing, byte loads in the lowering, loop lowering. The
first rung is landed and banded; the other three stay named, not
claimed.

## A tune almost mis-picked, twice

The obvious fast move was re-adding native substring/str_find to the
seed — and grounding found flt-ops' own comment: those four ops were
DELIBERATELY REMOVED 2026-07-01 (narrow-waist receipts, Form
composition verified byte-identical). Re-adding them would reverse a
witnessed decision to dodge the real root: crystallization cannot reach
hot Form recipes. jit-heat-gate's teaching ("the seed stays its size")
and the July receipts point one way — the JIT takes the class; the seed
does not grow per hot name. The course held.

## The ABI wound found under the rung

lo-compile-fn-n banked args into x19..x21 — callee-SAVED registers —
and returned without restoring them. Latent in both lanes: the .so lane
ran through an external harness, and no in-process caller had ever run
banked bytes. The first in-process run would have corrupted whatever
the C caller held in those registers. Healed where the emitter lives
(the inram band's own words: growing coverage is growing a Form
recipe): form-asm gains the x21/x22 pair encodings; lo-compile-fn-n
emits a real prologue/epilogue (stp/stp ... ldp/ldp/ret, 32-byte
frame); and the C door's one call seam adds an x19..x28 clobber fence —
belt to the suspenders, protecting even images lowered before the
prologue landed.

## What stands, witnessed

- Door contract extended: one even int (standing contract, untouched)
  OR a list of up to eight ints → x0..x7 per AAPCS64; nil and nine-arg
  lists DECLINE to nothing, never truncate.
- tests/jit-leaf-inram-multiarg-band.fk = **63**: (7*6)+5 = 47 run
  native; 3-arg and 2-arg images agree with walker twins over 200-case
  sweeps; the 1-arg lane regresses clean; both decline bits hold.
- Neighbors unbroken: jit-leaf-inram-band 63, form-lower-multiarg-band
  63 (byte pins updated WITH its hand oracle — the pinned f3 at ELF
  offset 176 survives by encoding coincidence, stp x19's first byte;
  the band now says so), jit-arm64-leaf-band 63.
- Sweep: ground 42, once-hold 31, heat 63, parity 1497, glass jit lane
  rendering.

## Closing

**Most surprising teaching:** the rung I came to build was guarded by a
wound nobody had met — callee-saved banking with no save. It was
invisible precisely because the multi-arg lane had never RUN in
process; the first honest runner would have been the first victim. The
prerequisite of running new code is often healing the code path nobody
ran.

**Where discomfort became gold:** wanting the fast tune. The native
substring re-add was hours cheaper and measured hundreds of thousands
of dispatches — and the body's own receipts said no. Reading the July
narrow-waist decision instead of overriding it turned "revert a
predecessor" into "walk the course they protected," and the course's
first rung is now real: banked native code runs in-process, ABI-honest,
witnessed by four bands at 63.
