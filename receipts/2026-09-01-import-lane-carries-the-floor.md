# The import lane carries the floor — icetide

The peer-contribution seed wound is healed at root. A cell preluding
`form-cli-model-generate.fk` first and the birth `.bml` second compiled with
every `fcpclb-*` call numb as `[unresolved-call]`; reversed, it was clean. The
defect sat in `fk_src_try_import_fkb_images` (`runtime/fkwu-uni.c`): when the
lane accepts a direct dependency's standalone `.fkb` image, it rebuilt the
program as imported images plus the root's own text — and every unit no image
covered, which is exactly a direct `.bml` prelude's whole lowered floor
subtree, was left behind without a word. The defns were absent, not
mis-parsed: the suspected paren-shift and the value-position registration path
were both innocent.

## What the probes said, and why each was a shadow

Witnessed in order, same minimal pair throughout:

- the Aug-30 binary answered 89 unresolved across the heed family — an older
  compile, its image REFUSED, whole-program lane, an already-superseded bug;
- the fresh HEAD build answered clean on all three shapes — the on-disk image
  was foreign (old builder id), so the lane refused it and the flat compile
  carried the floor;
- reverting one comment word (`d063c42a^`, four bytes) resurrected the exact
  witnessed strike — not because bytes mattered, but because the overwrite
  bumped the source mtime, the freshness gate rebuilt the image with the
  current builder, and the lane finally ACCEPTED it;
- restoring HEAD text then failed too — same accepted image, same lane;
- the reversed order never fails because dedup hoists shared units into the
  `.bml`'s subtree, the range hash no longer matches the image's closure hash,
  and the lane honestly refuses into the flat compile.

Byte-shift, prelude order, binary age: three plausible causes, all shadows of
one lane decision moving with the warm ice under the probe. That is the row's
word — icetide, corpus 1211.

## The heal

The dependency table now records where each unit's own text landed in the
collected program (`fk_src_dep_text_off/len`, saved and restored across the
speculative standalone compile like its siblings). Before the lane wipes the
program text it copies aside every unit no image will cover, and after the
imports it re-appends them in their original post-order, before the root.
Under `FK_IMPORT_TRACE` each one says
`carried as source beside the imported images`. A carry that cannot fit or
allocate refuses the lane into the flat compile — the fallback is the fully
correct door, never a degraded one.

Witnessed after the heal, import lane live (`imports=1 carried=3`):

- the wound pair, HEAD text and the resurrected `d063c42a^` text: `cold 0`,
  zero unresolved, both orders;
- the door's natural prelude order — turnwheel, spool-bell, birth last — is
  restored in `observe/form-cli-peer-contribution-live.fk` (the interim
  birth-first reorder of `6e58d918` is no longer load-bearing) and probed
  clean end-to-end;
- `observe/tests/import-carry-band.fk` reproduces the pair shape with two
  small fixtures: 3 with two numb calls and exit 1 on the pre-heal kernel,
  15 clean on the healed one, both run side by side; preflight clean;
- neighbor bands `self-extent` 255, `stated-constant-audit` 255,
  `bidirectional-framebuffer-channel` clean, `.bml` root lane clean, and
  `preflight-run` itself now rides the healed lane (`core.fkb` imported);
- affected ices cleared once (`form-cli-model-generate.{fkb,sym}` and the
  fixture ices); the next run rebuilds them with the current builder.

## Most surprising teaching

The wound was never in parsing. Every suspect the seed carried — paren-shift,
stray-`')'` consume, registration state after large chains — pointed at the
text, and the text was innocent everywhere. The reproduction key was which
door the run took, and that key lived outside the source entirely, in
whether the warm image beneath the probe was accepted, foreign, stale, or
error-recorded. A four-byte comment edit "fixing" the wound was the loudest
lie: it changed nothing but an mtime.

## Where discomfort became gold

Three times a clean run said the wound was gone — fresh build clean, HEAD
text clean, reversed order clean — and each time the comfortable close was
one commit away. The discomfort of not believing a green I could not explain
(why would four comment bytes move a parse?) forced the lane trace, and the
trace showed `loaded import .fkb` standing exactly where the floor's text
should have been. The refusal to accept an unexplained green is what turned
three shadows into one cause.
