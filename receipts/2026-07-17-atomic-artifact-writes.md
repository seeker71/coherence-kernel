# 2026-07-17 — artifacts arrive whole: rename() closes the torn-read window

Third piece of the artifact-cache arc (#265 the v4 lane, #271 the wound record). The writer
TRUNC-opened `.fkb`/`.sym` at their final paths and wrote in place, so any concurrent reader
could watch the image mid-build — the "truncated artifact" die class seen in the band sweeps,
and the standing fleet memory's collision shape (shared checkouts, fixed paths, live siblings).

## What changed (`runtime/fkwu-uni.c`)

`fk_src_write_fkb` writes both artifacts to pid-suffixed temp names and `rename()`s them into
place — a reader now sees the whole previous pair or the whole new one, never a partial. The
sym lens lands **before** the image, so a fresh `.fkb` is never visible without its
compile-error record (#271's law). Failure at any step unlinks the temps; nothing half-made
ever occupies a final path.

## Proven, both directions

A 12-second hammer — one process recompiling in a loop (touch + `--src`), one process
executing the image directly in a loop:

| binary | reader outcomes |
|---|---|
| pre-atomic (main) | 115 clean · **1,692 torn** ("truncated artifact" ×1687, "truncated string" ×3, "truncated table string" ×2) |
| atomic (this branch) | **1,993 clean · 0 torn** |

Serial matrix unchanged: degraded cell 1/1 with warning, ledger chain 32767 fresh + cached,
corpus band 511, jit-lower-bmf heal holds at 15. No `.w<pid>` temps left behind.

## Most surprising teaching

While hunting the tear I hammered six concurrent runs of the decode band and got **silently
wrong band scores** (469762047, 503316479, 520093695 against 536870911) — under the pre
binary *and* the atomic one. That corruption isn't the artifact cache at all: the band's own
fixtures write to one fixed `fs-temp-dir` path, so concurrent runs of the *same band* clobber
each other's test files — the exact shape the body's memory already records for organ run
paths ("a live sibling's build clobbers yours"). The artifact tear and the fixture collision
had been masquerading as one transient; they are two seams, and only one closed tonight.
Bands that touch the filesystem deserve pid-scoped temp roots — left named, not fixed, at
02:00.

## Where discomfort became gold

The first race harness (8-way on a small chain, 200 runs) found nothing, and the tempting
readings were both wrong: "the bug doesn't exist" or "the fix needs no proof." Sitting with
the discomfort of an unreproduced observed failure: the window is microseconds wide on a
200 KB buffered write, so the harness had to *hold the window open* — a writer recompiling in
a loop against a reader in a loop. The same evidence that had refused to appear then arrived
1,692 times in twelve seconds. A negative result from a weak harness is not absence; build
the harness that keeps the door open.

## Distillation row offered

*what one word names arriving part-by-part where only whole arrival is safe* → **piecemeal**
(0 hits before the row; near-misses "atomic"/"indivisible"/"wholesale" all 0 but name the
cure — the row keeps the wound's own name, the way the reader saw the image before rename
made arrival indivisible).
