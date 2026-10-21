# Glass shared roots, 2026-10-21

Urs asked to finish shared node identity, ownership, lifetimes and retirement,
remove the serialization path, and observe the actual running Glass.

## Movement

`node_gift_write` now publishes a native owned generation in the shared field.
The frame holds eight reference words (64 bytes), not a recursive value image.
`node_gift_read` returns the actual shared root; `ggf-cell` selects its payload.
The full root ID contains the field incarnation and publication generation.
The incarnation journal survives field reset; reused storage never reuses its ID.
An index alone cannot resolve a reusable owned root. A current full ID can.

The named frame owns its current publication across an author's exit. The root
also records the author's PID and kernel start, distinct from frame ownership.
Explicit private observation scopes retire when their process lifetime ends.
Replacement retires the previous generation. Expected-root withdrawal cannot
withdraw a newer publication. Copied descriptors cannot acquire another frame's
ownership. Reader holds are tied to PID and kernel start; native GC traces held
descendants, framebuffer entries and transitive generation dependencies. A dead
reader loses its holds. Retired, unheld storage becomes reusable.

Publication, raw replacement and scoped withdrawal share the ownership lock
and interruption journal. An interrupted odd frame restores its prior reference;
scoped cleanup can retry withdrawal. The lock inode is stable and never unlinked.
Private heaps refuse native publication; there is no serialization fallback.
Field version 6 and `/fgn-` names separate this carrier from the old images.

The recursive `fk_cross_emit`/`fk_cross_decode` codec is removed. Glass callers
use the native root/payload API. Unused snapshot/control acknowledgement codecs
are removed. Model token-flow snapshots use the same typed native carrier; the
old disk snapshot encoder, decoder and candidate flow are removed.

The physical shared allocation, lock and host process-lifetime carrier remains
in the C seed. Its shrink path is the native field allocator/arena. Publication
validation, allowances, observation and care remain in Form. Pool capacity is
reserved graph storage, not a measurement of resident memory.

## Execution evidence

All commands below were invoked through `form-run`. Scratch probes and their
small child traces remain in this checkout's ignored `.hearth`; the existing
node-gift and membrane bands carry the reusable regression conditions.

- `./fkwu .hearth/glass-owned-lifetimes.bml`: 64-byte reference;
  held descendant `[descendant,2.5]` survived collection and replacement,
  including transitive retention; 506 storage reuses, zero new pools in the
  measured replacement window. Stale retirement and copied-frame ownership
  were refused. Framebuffer retention also survived collection.
- `./fkwu .hearth/glass-owned-fault-observe.bml`: writer PID 754 was observed
  building generation 39564, stopped and killed (exit 137). The next reader
  recovered the prior root `[63,0,2,39548]`, value `[prior,42]`, reference 64
  bytes. This physically exercises interruption during building, not every
  instruction of each journal phase.
- `./fkwu .hearth/glass-owned-death-observe.bml`: reader PID 2694, start
  1792542332177, held one generation before termination and zero after; its
  private frame retired. An independent held value remained valid.
- `./fkwu .hearth/glass-owned-flow-observe.bml`: typed root
  `[63,0,2,88553]`, projected root `[63,0,2,88554]`, 64-byte reference,
  one unchanged model-flow event, no disk snapshot created. Both test
  publications were retired afterward.
- `./fkwu .hearth/glass-owned-live-proof.bml`: actual production `organs`
  root `[63,0,2,111757]` was held in two independent native processes.
  Both reported that exact full ID and zero reader node mints. The publication
  belonged to `/fgn-d7d37ccb`, author PID 5577/start 1792542636872, observed
  age 776 ms. The proof frame retained the actual primary node, not its copied
  payload, and was withdrawn afterward.

The existing bands observed node-gift 4095, shared field 255, cell-store 255,
Glass wait 255 and telemetry membrane 2097151. These are verdict bitmasks,
not counts of tests. The node-gift checks include full-ID resolution, stale-ID
refusal, held descendants, compare-by-root retirement, dotted-tail promotion
and storage reuse. The membrane checks include held/one-shot malformed parity
and withdrawn roots classified as absent.

After the final seed build, `./fkwu .hearth/glass-owned-final-checks.bml`
observed all ten expected bitmasks, zero errors and no stopped result:
freshness 31; node-gift 4095; field, cell-store and Glass wait 255 each;
membrane 2097151; observer 67108863; dashboard 16777215;
sensor rows 2047; live 2147483647. `./fkwu .hearth/glass-preflight.bml`
checked fourteen source/closure units clean. Lifetime, dead-reader, typed-flow
and interrupted-writer probes were repeated on that seed and held again.
Transitive pinning and marking use worklists, so dependency chains do not
consume C stack per retained generation.

The final-source live identity read again preserved the same root in two
processes with zero mints, but its source was old because the viewing fleet
had ended during source reload. That is identity evidence, not a fresh-view
claim. The operator was asked to reopen the public Glass door; Terminal UI
automation was unavailable. The reopened view and publication result are
recorded in the subsequent live observation.
