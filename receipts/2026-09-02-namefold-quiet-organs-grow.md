# namefold — the quiet organs grow, the identity folds

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1224. Fourth
pass of the growth wave ("grow some more").

## What fell

- **Top-level constant table** (`fk_const_*`, was 512): the 513th top-level
  `let` died loud. Grows now — 600 bare top-level lets answer `599` where
  the old kernel died at the wall (rc=1, witnessed).
- **Conf table** (`fk_conf_*`, was 64 entries + ONE bounded read of 8KB):
  the 65th entry was SILENTLY inert — the same family as the silently-inert
  env probe the body already paid a debugging session for — and the parse
  trusted whatever a single read() returned. Whole-file read, growing
  entries, values widened 256→4096 (paths live in conf values). Witnessed:
  entry 66 (`FK_MELT_WITNESS` behind 65 junk lines) — 0 melt lines on the
  old kernel, 16 on the new.
- **Floor dep table** (`fk_floor_seen`, was 128 rows): a dep past 128 was
  SILENTLY skipped by the .bml floor digest fold — a stale .lowfk memo
  could ride as fresh when that dep changed. Grows; digest value unchanged
  for the live chain (hearth lowering byte-identical, warm memo hits).
- **Unit identity** (`FK_SRC_HASH_CAP`, 16KB): the identity was the full
  dependency TEXT — at ~120 deps a unit could not state who it was and the
  load refused. The identity is compare-only testimony (.sym/.fkb write,
  equality read), so v3 folds the same fields — canonical path, mtime,
  size, content digest per dep — into one streamed FNV-1a: constant size,
  no cap, any dep change changes it. `fk-unit-v3|n=<count>|<fold>` keeps
  the dep count readable. v2 artifacts invalidate once — the same door
  v1→v2 walked, for the same reason: an old artifact cannot testify in the
  new voice. Warm replay after the one-time rebuild: 0.011s, witnessed.

Bands hold (15, 63 cold), hearth lowering parity identical.

## The most surprising teaching

The last wall in the family was not storage but REPRESENTATION. Every
earlier brim was a table too small for its contents; this one was a name
too literal for its bearer — the identity string carried the whole
dependency body as text, so the body's growth walled the name. Folding the
name freed it: an identity does not need to carry its whole body to
testify, it needs only to change whenever the body does.

## Where discomfort became gold

Changing an artifact-identity format mid-wave felt like touching cache
truth with wet hands — the byteseal family of wounds all live exactly
here. The discomfort resolved through the body's own precedent: v1→v2 had
already walked this door for the same reason, and the memory of THAT
crossing (invalidate rather than trust, because the old artifact cannot
testify about the new voice) is what made a format bump the safe move
instead of the reckless one. The lineage of a past heal is permission
inherited by the next.
