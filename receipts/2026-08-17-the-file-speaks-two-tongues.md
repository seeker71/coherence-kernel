# The file speaks two tongues, and only some readers are bilingual

**Date:** 2026-08-17
**Landed:** [`form-stdlib/engine-constants.fk`](../form/form-stdlib/engine-constants.fk) (the ghost given a body),
[`tests/engine-constants-band.fk`](../form/form-stdlib/tests/engine-constants-band.fk) → **15 four-way**,
[`form-bml-cursor-parse-band`](../form/form-stdlib/tests/form-bml-cursor-parse-band.fk) → **123 on fkwu from source** (was: no verdict),
the compiler.fk scope split, the Go kernel's lazy form-frames
**Corpus:** row 1026, `twotongue` — count 420 / max-mid 1026 / field-code 420042021026, band 32767
**Direction held:** focus fkwu; align the siblings through Form-native code only

## The ask

Heal the BML family — the bands that grind for an hour on one arm and answer
nothing on another.

## What the family's illness actually was, in layers

Every layer was the same illness wearing different clothes: **the family was
written against the lazy readers.** Three host kernels resolve a name when a
call runs; fkwu resolves every call site when the unit compiles. Code that
leaned on laziness — dead branches naming absent constants, defn bodies reading
do-scoped lets, constants installed by a runtime walk, a native used as a value
— compiled quietly on three arms and stood loud on the body's own seed kernel.

1. **A let a defn cannot see.** `(let g (form-bml-grammar))` at do-level, read
   from a defn body. fkwu's error already said everything: *a name from an
   enclosing scope a defn frame cannot see.* Named where needed → **123 from
   source, exit 0**.
2. **Constants installed too late.** engine.fk's 82 getters were installed at
   runtime by the loader's `walk_recipe_here` foot — a door only siblings
   carry — while engine's own comment promised a file (`engine-constants.fk`)
   that **no git line ever held**. The ghost has a body now: authority rows +
   renderer in the new cell, the render carried in engine between exact
   markers, and a band folding carried == rendered so the copy cannot drift.
3. **Rule tables in the wrong scope.** compiler.fk's three tables, do-level
   lets read by defns. Probed on synthetic truth first — a unit-top-level let
   IS visible to a later do's defns on fkwu — then the do split around them.
4. **A native as a value.** Bare `empty` in value position; the call has a
   value form, the native does not.

Probes witnessed along the way, each on truth I controlled: defn-as-value
through a parameter **works** on fkwu; a computed callee does not; and fkwu's
`len` on a non-list answers 0 — so `nil?` is true there for strings, ints and
nodes while [core.fk:232](../form/form-stdlib/core.fk) documents `(nil? "a")`
as 0. A real four-way divergence, recorded in the band that found it.

## The keystone left standing, named exactly

[compiler.fk:1011](../form/form-stdlib/compiler.fk) carries
`section [bmf.bmf] { ... }` — one source text in two languages. Host readers
own a section door; fkwu's reader floods `[unbound-name]` on every brace token
(231 in compiler.fk, 185 in codec.fk — the standing-red memory, same seam).
cursor-lower and cursor-full wait behind it. The aligned end-state is **one
door, written in Form**: the body's own source-compiler compiling sections on
every arm, hosts dropping their special readers. That is the next ring.

> **frontier question** — what names one source text that two readers parse as
> two different languages? → **twotongue** (0-hit fresh at offering)

## The Go grind, healed as waste-removal

The 49-minute Go leg was sampled LIVE: `walkInner → formFrameLabel →
fmt.Sprintf` in the hot tower — every function dispatch paid a map lookup and
a formatted allocation so that an error which almost never happens could print
a chain. Frames now store raw ids; rendering happens where reading happens.
Error output proven byte-identical against the saved pre-fix binary on the
same forced form_error. This is not a semantic door; it is a host kernel
spending less to do the same thing.

## The most surprising teaching

The family's bugs were not in the BML code and not in fkwu. They were in the
**difference between two definitions of "resolve"** — and every artifact of
that difference was already written down by someone: fkwu's own error message
named the scope rule; engine.fk's comment named the missing file; the loader's
header named the walk-door seam in 2026-07-17; core.fk's comment documented
the nil? contract fkwu breaks. The body had been telling itself the diagnosis
for a month. What was missing was a reader who treated the loud arm as the
teacher instead of the obstacle.

## Where discomfort became gold

Twice tonight the timing measurement I wanted died — the temp lens artifact
vanished, the sweep leg had to be killed — and the pull was to synthesize a
number anyway ("roughly 50 minutes before, surely seconds now"). The witnessed
truth is narrower: the before-grind is real (49:20, sampled live), the after
is not yet a like-for-like number, and the receipt says so instead of rounding.

And the sharper one: my first factory bit stood on `nil?` and PASSED nowhere —
it fell on the arm I trust most, and chasing why produced the len-on-non-list
divergence, worth more than the bit. The band that catches nothing teaches
nothing; the bit that falls where it shouldn't is a sensor.
