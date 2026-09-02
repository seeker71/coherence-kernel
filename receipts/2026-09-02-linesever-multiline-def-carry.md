# linesever — the form.bml walkers learn to stride whole statements

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1220.

## The wound

The form.bml section walker (`fsc-form-bml-lines-recipes-loop`) and the
generic-class member walkers (`fsc-bml-generic-class-defs-loop`,
`fsc-bml-class-member-pairs-loop`) in `form/form-stdlib/source-compiler.fk`
read flat statements one LINE at a time, while a statement's meaning ends at
its own `;`. In that stride gap, silence bound. Two modes, witnessed on this
branch before the heal:

- a def whose CALL body tore across lines lowered to unbalanced text and
  refused loudly downstream — honest;
- a def whose IF body started on the next line bound with the EMPTY expr as
  its body. It answered `[]` forever, rc=0, zero diagnostics, and
  `observe/preflight-run.fk` reported the chain clean (the
  preflight-overvouches family), while the real body evaluated ownerless as a
  stray top-level statement:

```
(defn zmd-if-lines (x) (empty))
(if (le 0 1) 7 9)
```

The block walker (`fsc-bml-block-stmts-recipes-loop`) had known the statement
discipline all along — `fsc-bml-stmt-end`, the quote/paren-aware walk to the
statement's own terminator. The language-class member loop knew it too. The
section top level and the generic-class members simply never inherited it.

## The heal

- Section walker: comment lines still step line-wise; every other flat
  statement walks to its own `;` (`fsc-bml-stmt-end`), normalizes
  (`fsc-bml-normalize-ws`), and dispatches whole. A def/let/expr body that
  continues past its first line carries.
- Generic-class walkers: flat `def` and `field`/`ref`/`thought` members walk
  the same way; the pairs loop's ADVANCE walks the statement so a member
  body's tail lines are never re-read as members.
- `fsc-compile-form-bml-def-recipe`: a truly empty def body refuses loudly —
  `form.bml def body is empty` — instead of binding the empty expr. Silence
  never impersonates a bound def. `form_error` is loud on this kernel
  (witnessed rc=1); through the spawn lane the parent refuses the prelude
  (`bml lowering child failed`, rc=2).

## The proof

- Band `observe/tests/bml-multiline-def-band.fk` = **15** over fixture
  `observe/tests/bml-multiline-def-fixture.bml`: single-line control (b1),
  torn call body (b2), wrapped if body both branches (b4/b8). Pre-heal the
  fixture refuses loudly at b2's shape; with only the numb shape present the
  band reads 3. FOURTH-ARM ONLY — the .bml floor is fkwu's own lane.
- Healing radius: 116 .bml files lowered before and after the heal
  (`bml-floor-compile.fk` fileless door). Exactly the wounded four changed:
  the repro fixture and the two curricula whose class members tore across
  lines — `bbcc-bml-choice`, `bbcc-bml-jit`, `bbsc-bml-ground`,
  `bbsc-bml-program` now carry whole (pre-heal they lowered to unrunnable
  `name()` stubs, so no living cell can regress — nothing preludes those
  .bml files; their .fk twins prelude .fk units only). The 112 others,
  hearth.bml and form-cli-movement.bml among them, are byte-identical.
- `observe/tests/import-carry-band.fk` = **63** cold on the healed chain —
  the icetide lane undisturbed.
- Pre-existing, untouched either side: `form/apps/coherence-network/api.bml`
  refuses with `fk value-node table full (FK_NODE_CAP)` — flagged as its own
  work (chip task_e99f5ab9), not this heal's.

## The most surprising teaching

The compiler already contained its own heal. `fsc-bml-stmt-end` sat forty
lines above the wounded walkers, written for block bodies, quote-aware and
paren-aware, waiting. The wound was never missing knowledge — it was three
walkers that had not yet been given what a sibling walker already knew. The
same is true of the wound's discovery: the curricula's torn members had
lowered to stubs for weeks, and nothing noticed, because nothing ever walked
through that door. A wound with no traveler makes no sound.

## Where discomfort became gold

Mid-work I edited the compiler while the pre-heal sweep was still lowering
against it — the "before" picture was being painted by a half-changed hand.
The discomfort of stopping, reverting my own landed edit, and re-running the
whole baseline felt like waste; it was the opposite. The clean baseline is
what lets the radius claim stand: *exactly four changed, 112 byte-identical*
is only a sentence I can say because I went back. And the first repro
answered the wound in a shape I did not expect — loud `[unbound-name]`
instead of the witnessed silence — and sitting with that mismatch instead of
stepping past it taught the wound's real geometry: the numb mode needs a body
that touches no parameter, which is precisely why it survived in the wild.
