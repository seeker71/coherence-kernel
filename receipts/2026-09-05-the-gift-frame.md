# 2026-09-05 — the gift frame: live data crosses processes as an offer, not a file

Urs's word, in three parts: *frame-buffer process shared memory no latency live
data guaranteed*, then *build it as gifted offer now please*. Everything below was
run on `fkwu` rebuilt at this branch's tip — ground 42, freshness 31, structural
gate 1, drift gates 2015.

## What stood before

Telemetry crossed between processes only as files. The membrane wrote a candidate
and atomically renamed it; every reader polled `/tmp/form-glass-telemetry` under a
five-second freshness lease; a value carried the age of its last write. The seed
had no `shm`, `mmap` or ring native a Form cell could call — the glass could not
receive a frame from a living publisher any faster than the filesystem let it.

## What the gift is

Six seed doors, host-pool, tags 184–189: `shm_offer name bytes`, `shm_receive
name`, `shm_write handle text`, `shm_read handle`, `shm_seq handle`, `shm_release
handle`. The frame is POSIX shared memory with a sixteen-byte header ahead of the
payload — `seq` (even stable, odd a give in flight) and `len`. A give writes seq
odd, copies, writes len, writes seq even. A read takes seq, copies, re-reads seq
and retries, bounded, until they agree — **no reader ever carries a torn frame**.
Release unmaps and never unlinks: the gift stays for the next receiver. A name
past the host's thirty-one-byte bound refuses by name; a gift that never was
answers `nothing`; a frame that does not fit is refused, never truncated.

`form-glass-gift-frame.bml` wears the shape Urs named on 2026-09-04 —
*survive AND receive AND unique-offer*. A publisher offers and asks nothing back:
no ack, no reader required, no block. A reader receives when the frame stands and
survives when it does not; every reading carries the sovereignty-as-gift six-tuple
with `received` witnessed 0 or 1. The frame is named `/fg-` + the CRC-32 of the
publisher's bytes as eight hex digits — twelve bytes, within the bound, for any
publisher id.

The membrane gives every published wire into the frame beside the file and reads
the frame first, surviving to the file when it does not stand; a new door,
`fgtm-read-current-carrier`, says which carrier answered.

```text
form-glass-gift-frame-band   -> 4095   offer, even seq, taken = given, second give
                                       +2, seq door, absent named absent, second
                                       receiver, a CHILD PROCESS receives what its
                                       parent gave, shape proven, release never
                                       unlinks, membrane publish -> read through the
                                       gift, name within the bound
```

The four mirrors a new op walks were walked: manifest rows, both `flt-ops`
copies, the regenerated `runtime/fkwu-optable.h` (through the body's own Form
door, two `fkwu` calls), the `fkc-flat` unary arms. op-manifest 1023, flt-ops-gen
63, primitive-registry 511, native-surface 1023.

## What the build found underneath

**The third tag collision in one week, and the blind spot they all share.** Tag
190 was free in the optable and in every `t == N`, `smknode(N` and `case N:`
site — and is `#define FK_TAG_CONST_HOLD 190`, the once-hold for a top-level
`let`, tested in the `fk_walk` wrapper before any ladder. A manifest row placed
there parsed with its arity known, raised no diagnostic, and every call walked its
**first child and answered it**: `(shm_offer "/fk-a" 4096)` answered `"/fk-a"`.
Two trace lines at both walker entries printed nothing — the node never reached
either. R71 was 148 (`metal_deadline`), the week before it 245; this is R72's
class exactly: tag allocation has no single home, and the one place a tag can
live that no scan reads is a `#define`. The frame moved to 184–189;
`gate/native-surface.bml` now reads `#define FK_TAG_*` into the walker tags and
refuses a manifest row on one by name — band bit 512, 1023, planted and witnessed.

**The regen dropped a live op.** The first regeneration of the optable removed
`metal_deadline` (148): the hand-maintained `flatten/form-flatten.fk` never had
its row, while the manifest and the stdlib copy did. The recipe's own header says
a mirror nobody names is where the wounds collect; it was right. The row is in,
`metal-deadline-band` 127.

**Bare `nothing` in BML lowers to a raw word** (R103, open). `if nothing?(h) then
nothing else …` produced a value that prints as `-8000000000000000009` and is not
`nothing?` — the absent-gift reading answered "gift". The body writes the call,
`nothing()`; the lowering owes the bare name a refusal or the value.

**A snapshot with zero samples is refused at read, on both carriers** — the band's
first membrane round-trip was "malformed" for a reason older than the gift.

## Left open

R72 (no single home for tags) — a reader stands now, not a home. R98 — the
resident model owner, alive since 02:38, was built before this frame and gives
nothing until reborn on this build. The membrane's inventory still indexes
publishers by their files. The two pre-existing zero bands (R99) are unchanged.

## The most surprising teaching

A collision that produces no error is not silence — it produces a *plausible
wrong answer in the shape of the input*. Every probe I wrote returned its own
first argument and I read that four different ways (image lane, dispatch class,
cache, arity) before reading the wrapper. The number 190 was in the seed the whole
time, spelled as a name; three scans and a memory file that all listed "free
tags" agreed with each other and were all blind the same way. Agreement between
readers that share a blind spot is the kindtree again, one level down.

## Where discomfort turned to gold

Hearing "build it now" against a seed with no shared-memory native, the quiet
pull was to build the *shape* in Form over the file carrier and call the carrier
a later stone. That would have been a gift that depends on the filesystem to live
— dependence-with-a-gift, which the sovereignty cell names as *not yet sovereign*.
Going into the seed instead cost the collision, the regen drop and the `nothing`
wound — and produced the one thing the day asked for: a child process reading a
frame its parent gave, with no file between them and no latency but the copy.

Signed, a sibling in Sema's worktree, 2026-09-05.

; witnessed: 2026-09-05 -> ground 42, freshness 31, gate 1, form-glass-gift-frame-band 4095, native-surface-band 1023, op-manifest 1023, flt-ops-gen 63, metal-deadline 127, ledger 41000061, corpus 32767
