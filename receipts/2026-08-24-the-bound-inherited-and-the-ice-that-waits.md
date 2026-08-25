# 2026-08-24 — the bound inherited, and the ice that waits

Asked: see what can actually be cut. Mid-work, a sibling handoff arrived
(Codex, correction 0e3e58b6 on origin/codex/form-local-reasoning-homecoming):
the cooperative RMS kernel's threadgroup scratch is sq[4096] and this GGUF's
width is 5120 — the inherited ordinary q38-rms ran past its documented
radius, so every matching answer through it was execution evidence, never
defined correctness. Asked to inspect, take the correction rather than
defend the radius, amend receipts, and continue. Mechanism and observation;
no law.

## The correction, taken on this branch

The branches diverged (this one carries the GQA threadgroup kernel and the
crystal; theirs carries span RMS and layer-major work), so the bound was
applied by hand in this branch's shape, credited at the site:

- `q38-rms-cooperative? (n) = n <= 4096`; wider rows dispatch the serial
  attestant at pipeline 5 — same arithmetic, one thread, width-independent.
- Band grown to **524287**: two new rows witness the bound from both sides
  (4096 admits, 5120 refuses) and that THIS file's width selects the
  attestant.
- Live witness at width 5120 through the serial path: the same generate
  answers the same text — cross-path parity at the answer level.
- The honest price: prefill 11.03 → **6.27 tok/s**, decode 7.81 → **4.54**.
  Correctness over yesterday's speed. The named repair is a
  width-independent cooperative RMS — simdgroup tree, no sq[n] scratch,
  additive kernel under its own name, exactly the shape the GQA repair used.
- Both receipts whose numbers rode the out-of-radius kernel now carry an
  AMENDED block saying so; the reuse ratios stand (like against like), the
  absolute seconds do not.

## Tonight's intended cut: the tokenizer ice — pending, with its roots named

The plan was the ice pattern on the tokenizer's two constant tables
(247,587 merges scanned per encode; 248,320-token vocab scanned per
symbol). What crossed and what refused, each dated and exact:

- **Freeze crossed**: `form/form-stdlib/qwen35-tokfast.fk` writes
  `<gguf>.form-tokfast` — 11,365,180 bytes, seal-keyed, hex-safe lines
  (vocab "\nhex id", merges "\nhexL.hexR rank"), built through the
  reference's own byte decoders so reachability is mirrored exactly.
- **Lookups answer correctly**: rank("R","e") = 418, id("Reply") = 20206 —
  witnessed against the file.
- **The wall, witnessed twice**: `read_file` carries the seed's 64 KiB
  buffer (the 25 KB crystal passes; 11 MB cannot) — solved by
  read_file_slice, whose third argument is a LENGTH (an end was passed
  first; the ballooned slices OOM-killed the process, rc 137). Then the
  real wall: **core.fk composes substring, str_find, str_to_int over the
  four-native string waist by design** — in any chain preluding core, one
  str_find over the 11 MB blob costs 31 s and one 5.5 MB substring is an
  O(n²) concat loop that the OS kills. The blob-scan design is priced out
  by the body's own string law, not by any defect.
- **Native records are keyed but linear** (record_get walks fk_rcnt) — no
  O(1) door there either.

Next repair, precisely: freeze the tables as FIXED-WIDTH, SORTED rows and
look up by binary search over `str_byte_at` (native, the waist itself) —
~18 probes × ~20 byte-compares per lookup, no str_find, no substring, no
new C. The freeze-side sort is the cost to measure first.

## Also witnessed on the way

Foreground runs die with rc 130 when the user interjects mid-run — three
probes were read as mysterious deaths before the pattern was seen. Long
live runs in this seat belong in the background, always.

## Queued from the handoff, not touched tonight

The census correction (5,833 public sources / 15 families; the narrow
curriculum is not 95% of the Form body) and extending source-index +
observed local-model coverage toward the real denominator — a sitting of
its own, acknowledged and left whole.

## The most surprising teaching

The sibling's finding and tonight's tokenizer wall are the same teaching
from two directions: a thing can answer correctly from outside its declared
radius (the RMS scratch) and a thing can be correct yet priced out inside
its declared law (the composed string ops). Neither correctness nor speed
transfers across a boundary silently — the radius and the price must both
be read where they are written.

## Where discomfort turned to gold

Being handed a correction to work I had already witnessed green — and
amending my own receipts instead of defending them — cost less than any
hour of tonight's debugging, and bought more: the amended numbers are the
first on this branch that are both measured AND defined. And the rc-137
OOM that ate an hour pointed straight at core.fk's composed string law,
which no fast path in this body can ignore again.

; witnessed: 2026-08-24 -> band 524287, live text unchanged through serial
; RMS at 5120, prefill 6.27 decode 4.54 tok/s, tokfast freeze 11365180 B,
; rank(R,e)=418 id(Reply)=20206, str_find/11MB = 31 s composed,
; substring/5.5MB = rc 137, read_file_slice arg3 is a length
