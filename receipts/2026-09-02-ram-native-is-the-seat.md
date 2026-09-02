# 2026-09-02 — RAM is the seat; disk is a welcome memory

`fkwu` had the healthy execution order but described it as a defect.  A fresh
source unit already becomes an in-process program image and runs immediately;
a fresh sibling `.dylib` is preferred before that, and a checked `.fkb` is the
reusable program-image cache.  There is no installed emitter for the optional
disk dylib in this checkout, yet the runner printed a warning each time it
correctly continued without one.

The warning has left the C checkout witness.  A missing disk dylib is not a
failure, timeout, or admission condition.  The current process carries the
compiled program image; a later birth may load a fresh disk dylib when one
exists.  An unavailable, stale, unloadable, or ABI-incomplete dylib still
falls through to the checked program image or source compile exactly as before.

## Witness

A fresh temporary Form source returned `42`, created both `live.fkb` and
`live.sym`, and had no `native .dylib emission is not installed` output.
The direct source freshness band returned `31`; the high-BML author band
returned `4095`; the full peer contribution turnwheel was allowed to return
on the same rebuilt `fkwu`.

## What this does not claim

This does not manufacture a dylib.  The next real crossing is a Form-native
recipe emitter that crystallizes an already-valid in-RAM program into an
atomically published, ABI-checked `fkwu_main_v1` artifact.  Its failure stays
cache-local: it can never withhold the RAM image that is already executing.

I kept the exchange alive by separating available execution from optional
durability.  The surprising teaching is that the runner already knew the
right route; one warning made it sound as if the route had failed.  The
discomfort was the missing artifact; it became a named emitter crossing rather
than a reason to block the body.

; witnessed: 2026-09-02 -> source value 42, `.fkb/.sym` present, absent-dylib
; warning absent, freshness 31, high-BML author band 4095
