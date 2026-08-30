# High-BML native resident floor

## Crossing

`form-cli-resident-turnwheel-xtal.fk` and its executable-BML source have left
the living path. The authority is now
`form/form-stdlib/bml/form-cli-resident-turnwheel.bml`; the resident carries
one compiled cache built from that source at birth:

```
high BML declaration
  -> scannerless Form byte cursor / structural source NodeID
  -> equality + conditional program images
  -> jonb registry / jit_leaf_inram
  -> retained native call on every resident row
```

The cache names its BML source NodeID, signal and phase maps, and three
structural images: terminality, phase runnability, and stop carriage. New BML
source gives a new NodeID. The first call to an image births native code; later
calls carry the returned registry and re-enter the same native page. A malformed
declaration or unavailable page returns a typed refusal. There is no
interpreter, emitted-source, or legacy-xtal fallback on this resident decision
path.

The BML phase terminals remain fixed grammar words only after the cache has
verified the same surfaces in the high declaration. A changed high-BML phase
surface therefore refuses resident birth rather than allowing the runtime to
quietly speak an old phase vocabulary.

## Evidence

Fresh source preflight was clean for the native compiler, turnwheel, and BML
band. Direct `fkwu` runs returned:

- `form-stdlib/tests/form-cli-resident-turnwheel-bml-band.fk` → `65535`;
- `form-stdlib/tests/form-cli-resident-turnwheel-band.fk` → `65535` with live
  framebuffer stage frames and both dynamic recipe routes asserted `native`;
- `form-stdlib/tests/jit-once-born-band.fk` → `32767`, including equality and
  conditional semantics on the Form route;
- `form-stdlib/tests/form-lower-condgen-band.fk` → `15`.

The BML band proves all of: complete BML-grammar parse over the live cursor,
no legacy BML or `xtal` file, cache source identity stability, first-birth and
hot native calls for all three images, `nothing` distinct from `0` and `1`, a
direct resident terminal call (`cut=1`, `undo=0`), and typed refusal for an
incomplete declaration.

## What the error gave back

The first guard was written as wide `or` / `and` forms. Form's source grammar
defines those as binary; the extra operands desynchronized the reader and
showed unbound local names. The repair is not an added parser exception or a
larger fixed operand table: `fcrtn-values-present?` and `fcrtn-all-true?` fold
structural lists. The compiler now accepts an unbounded observation row without
silently losing operands.

The earlier receipt recorded the then-real `xtal` crossing. This receipt does
not rewrite that history; it records the successor in which the transitional
mirror no longer participates in the resident.

The next honest gap is broader BML lowering: this native compiler owns the
resident declaration family today. The general BML grammar is witnessed by the
scannerless BML band, while arbitrary high-BML AST lowering is a separate
compiler movement—not something this resident claims to have completed.

— Codex, 2026-08-30

; witnessed: 2026-08-30 -> BML-band 65535, turnwheel-band 65535, JONB-band 32767, condgen-band 15
