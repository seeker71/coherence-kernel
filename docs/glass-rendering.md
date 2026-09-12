# Native buffered Glass

The renderer retains its front/back semantic frames. Successful output commits
the back frame; failed output retains the front frame for retry. An unchanged
frame emits no bytes. Equal-width ASCII segments update in place; changed widths
and non-ASCII text use a whole-row fallback. Styles reset before changing so
background and weight do not leak between segments.

A frame is built as Form bytes before writing. It uses absolute row positions,
not line feeds, and a fresh renderer paints over the previous screen without a
separate blank-screen clear. Cursor movement is bracketed inside a
[DEC 2026 synchronized update](https://gist.github.com/christianparpart/d8a62cc1ab659194337d73e399004036).
Terminal support determines whether intermediate writes are visually batched;
the Form buffer and changed-region suppression work independently of that support.
No Rich, Python renderer, shell renderer, or new external service is involved.

## Aligned panel regions

Two-column views clip each panel to its allocated width before joining them.
The left region keeps that width even for a short, empty, or exhausted row;
neutral padding carries no measurement or source identity. The three-column
gutter therefore starts at the same position on every row. Original segment
styles, channels, node IDs, and evidence remain attached to their text.

`./fkwu form/form-stdlib/tests/form-glass-column-alignment-band.fk` returns
4095. It covers short and absent rows, mixed string/byte segments, overflow,
and the Help, Overview, Memory, and Recipes layouts at different widths.
This establishes the ASCII telemetry column contract. General Unicode
display-cell measurement is still a separate seam: byte-safe clipping does
not measure wide glyphs, combining sequences, or terminal-dependent shaping.

## Bookkeeping belongs at the producing boundary

Shared-memory mapping counts and bytes are updated when a mapping opens or
closes. `kernel_stat` and the kernel live page read that ledger in constant time;
they do not scan the history of released handles. Live-page word 10 counts
currently mapped gifts, consistent with `kernel_stat 41`; word 11 and key 42
count their actual mapped bytes, including the carrier header and host rounding.
Repeated or refused releases cannot decrement them twice. Native Form owns the
observation policy and renderer; this small carrier repair removes repeated
scans and consolidates unmapping (four fewer C lines).

`observe/gift-bookkeeping-cost.bml` measures 20,000 counter reads before and
after 20,000 real receive/release cycles. `tests/gift-bookkeeping-band.fk`
under `form/form-stdlib/` verifies the mapping lifecycle, including kernel-page
readers. This measures bookkeeping cost, not end-to-end screen latency.

Glass reads retained shared-memory handles and decodes changed snapshots. The
producing organ owns counters at the operation boundary and publishes coherent
snapshots at its useful cadence. Durable coding checkpoints belong after a
complete role/tool transition; verified lessons belong after successful checks,
not in the renderer or on each generated token.

`./fkwu observe/form-glass-presentation-current.bml` reads only the shared last
presentation and cadence. It reports collection-start-to-terminal-write time,
terminal-write time, generation, and age. A generation advances only after a
successful terminal write; staging, unchanged frames and failed writes cannot
claim presentation. Cadence work includes terminal output. This is a write
acknowledgment, not display-photon time or upstream microphone/model latency.
The pure presentation band returns 31.

Run `./fkwu observe/form-glass-run.fk` in the viewing terminal. Its prelude-free
native bootstrap checks kernel freshness with closed stdin before admitting BML.
A failed check rebuilds the committed checkout seed into a temporary executable,
publishes it after successful compilation, and rechecks freshness before starting
`observe/form-glass-supervisor-run.fk`. Compiler diagnostics remain visible; a
failed compile retains the existing binary. This early Form layer needs no core
formatting primitives from the newer kernel it may be rebuilding. Relaunching the
public door replaces already-running children after a native binary repair.
Thereafter a dependency change renews the renderer and the three sensor processes;
a renderer-only pool renewal keeps sensors and skips repeated admission output.

Bounded checks (each also has its printed verdict):

```text
./fkwu form/form-stdlib/tests/form-glass-render-batch-band.fk  # 4095
./fkwu form/form-stdlib/tests/form-glass-heal-band.fk          # 262143
./fkwu observe/form-glass-jit-hold-current-run.fk             # checks=4095/4095
./fkwu observe/form-glass-frame-budget-run.fk                 # measured, not fixed
```

`DOING dsk=` is placed first so a narrow view retains it. `cpuT` is cumulative CPU
time, not utilization. Owner CPU carries microseconds; absent or stale values
show `?`. Retained detailed tiles still carry last-observed evidence. Organ
coverage requires the exact census identities, not just an equal row count.

## Frame-local metric memory

`fgd-metric-index(rows)` builds a reclaimable native keyed trie. Its depth follows
the row count; keys are exact bytes, not a fixed field list. The first row for an
ID wins, including an unavailable row; absent IDs return an empty list. Old index
snapshots stay unchanged when another frame is built. Dropping a frame lets its
index and rows be reclaimed, unlike a kernel record whose fields remain rooted
for the process lifetime.

The live loop retains dynamic key-to-position tries in 32-row cache chunks.
Unchanged keys reuse their trie while values and ages come from the new frame.
Insertion, removal, reordering, and malformed-row changes rebuild the affected
chunk. The granule is not a field schema or capacity limit. Exact byte equality,
hash-collision handling, and first-row precedence apply across chunks. The
unfiltered atlas shares this index with the flow calculation; filtered views
build an index from their own rows. Sample deduplication uses an exact-key trie
instead of repeatedly scanning all preceding samples. The cached-index band
returns 65535, including cross-chunk precedence, unaffected-chunk reuse, and
the native bounded-integer hash's equivalence to the established djb2 encoding.

Snapshot identity and first-occurrence sample positions are also retained across
value-only changes. A changed publisher, ID, model kind, or physical handle
invalidates the identity proof; duplicates use the existing newest-snapshot and
physical-alias fold. The sample-index band returns 8191. Tries allow a small
bucket load to avoid copying unnecessarily deep sparse paths; full-key comparison
still decides equality. These are reclaimable observer caches, not disk ledgers.

Rows whose age has not changed are retained directly. When a row must be
restamped, the encoding copies its unchanged slots without redispatching every
field accessor. Observation time, sequence, projection node, optional producer
node, unavailable age, and backward-clock indication retain their meanings.
`form-glass-row-age-band.fk` returns 31; the observer band returns 67108863.

The physical frame-work band uses a child-PID namespace for its output frames
and control channel. It reads existing producer frames but cannot overwrite
live Glass cadence, presentation, or supervisor state, and cannot request work
from the model owner. Its rows and dispatch counter belong to the same child.
It warms 200 ticks before the four-second measurement. Its original limits,
including fewer than 1,500,000 dispatches per tick, remain unchanged.

```text
./fkwu form/form-stdlib/tests/form-glass-metric-index-band.fk  # 65535
./fkwu observe/form-glass-metric-index-measure.bml
```

At the measurement door, enter `map` followed by Enter. Repeat in a separate
invocation with `record` to measure the previous implementation. Each bounded
run checks 256,000 lookups over 1,000 changing frames and prints elapsed time,
heap capacity, reclamations, and permanent-record allocations. No live sensor,
microphone, or renderer is started, and no transcript is read. Compare repeated
warm runs: bounded memory does not by itself establish lower CPU cost.

Transcript bodies now pass directly into `ftc-byte-text` segments. The native
wrapper walks only visible spans, retains UTF-8 character boundaries, replaces
terminal controls with spaces, and leaves off-screen suffixes untouched. It does
not build every growing text prefix as an interned string. The string-returning
`fglm-clean` remains available to bounded callers, outside the live render path.
`./fkwu form/form-stdlib/tests/form-glass-transcript-bytes-band.fk` checks this
contract, including unchanged-frame suppression; the expected verdict is 1023.

After a successful local frame publication, Glass renders the same Form rows
using the carrier's acknowledged sequence. Its frame evidence explicitly names
publication, not readback. A refused write still reads the prior shared frame
with its original age. This removes a per-frame encode/decode round trip without
changing the published frame or external readers. The pure local-publication
band (`form/form-stdlib/tests/form-glass-local-publication-band.fk`) returns 255.

## Stable, locally owned transcripts

The ear's shared-memory publisher is scoped to the native body working directory,
the same boundary as its `.hearth` spool and owner lease. Two checkouts may each
have a live ear, but neither can overwrite the other's transcript frame. Glass
does not fall back to the old unscoped ear name. Machine-wide sensor names are
unchanged. `fgsr-publisher-in("ear", bodyRoot)` names an explicit body's channel;
ordinary producers and readers use `fgsr-publisher("ear")` in that body root.
Run transcript inspection there too. This uses native `host_cwd`, not a process
launched to discover the directory.

The live reader retains its last coherent ear snapshot in the frame cache.
Unchanged sequence words skip decoding, and an overlapping or invalid read cannot
briefly remove its transcript slots. The original clock stays attached: missed
publications become visibly stale, while a coherent sleep frame clears the text.

`./fkwu form/form-stdlib/tests/form-glass-transcript-isolation-band.fk` returns
4095. Two private native shared-memory gifts exercise competing publications,
retained invalid reads, genuine new speech, stale evidence, explicit sleep,
and mapping release. Tests never offer a living ear's name or capture speech.

The ear retains one latest original and one translation per target language,
not an ever-growing transcript history. A detected source-language change
replaces the original slot. Quiet frames leave that text in place. A
growing heartbeat with unchanged words keeps its source timestamp; corrections
replace words, and a newer final replaces its partial. Older source timestamps
cannot rewind a newer slot. Selected languages share the available panel height
in stable two-to-five-row slots, including headings. Slot height depends on
language count and panel size, not text length or partial/final state. Catalog
order keeps the default languages ahead of a newly detected source language.
Clipped text ends with an ellipsis inside its slot; evidence and partial/final
labels remain visible. More selected languages than the physical panel can
hold still require filtering or viewport navigation.

The sensor passes a checkout-local owner lease through the workers' native
stdin file. The input is three lines: duration, language codes, owner token.
Empty duration/languages use existing defaults; an empty owner supports bounded
standalone callers. A worker checks its owner before model admission, between
model rounds and before publishing translations. Retirement ends it without
writing a `done` marker into its successor's segment stream. The model workers
spawn no bell, shell or timer processes: a 20 ms native idle wait checks the
authoritative spool, deadline and owner. This interval is not an end-to-end
latency promise; model and scheduler time remain separate.

An agreed translation prefix is reused for an extending source. A correction
to earlier source words discards that forced prefix so the translation can
correct itself. This does not establish general translation accuracy.

```text
./fkwu form/form-stdlib/tests/ear-transcript-flow-band.fk  # 524287
./fkwu form/form-stdlib/tests/form-glass-meaning-ui-band.fk  # 8191
```

These bands use synthetic rows and do not open a microphone or publish a live
frame. Live inspection should retain only counts, ages and stage timings in
receipts, never captured speech.
