# The field was full of one NodeID

2026-09-14, M4 Max, Hati Suci. On the 13th the host field filled its 2^26 node cells, and drift read 16191 of 16383.
I asked Urs whether to reset it. His answer: 2^26 cells is not feasible, think about it before doing anything. A
reset had already been made at 23:15 that night. This is what the thinking found.

## What the field held

- **Only the node column grows.** A read-only probe of the field's header, about ten hours after the reset:
  - nodes 27,477,134 of 67,108,864 (40.9%)
  - pairs 0.2%, strings 0.1%, string bytes 0.1%, floats 0
- **With the glass running, the node column grew about ten thousand cells a second.** That is full again in about
  an hour.
- **The cells were one value.** A second probe read them:
  - 99.7% were make_nodeid cells.
  - Of the newest two million, 1,992,684 carried the same NodeID, [1 2 99 1759]. That is
    form-glass-observer.bml's fgo-metric-field-category, which the glass asks for once per metric sample.
- **make_nodeid was the one intern with no lookup.** fk_field_intern_node filled a fresh cell for every NodeID, and
  the private-table path did the same. A NodeID is identity by content: eq has compared its four coordinates since
  2026-08-30. So every one of those cells was a copy of a value the field already held.

## Carried

- **make_nodeid looks its coordinates up**, in the field's shared hash and in private tables, as every other intern
  does. A repeat answers the cell already there.
- **The lookup key is the hash fk_deep_hash_node already gives a NodeID's cell**, so no hash value anywhere moves.
- **Nothing else changes for a program.** eq, node_eq and value_eq already compared coordinates, and every
  coordinate reader answers as before.
- **nodeid-one-cell-band, registered at 7** (FOURTH-ARM ONLY).
- **repeatmint is row 1538.** farwin, held while the field was full, lands beside it as 1537.

Witnessed:
- **A fresh coordinate minted 1,000 times** leaves 1,000 field cells on the old kernel and one on this one.
- **With the field turned off** (FK_FIELD_OFF):
  - the same NodeID 3,000 times mints 3,000 private cells on the old kernel and one on this one;
  - 100 distinct coordinates mint 100 on both;
  - the field is untouched.
- **The band** reads 7 here and 6 on the old kernel.
- **The 41 registered bands that call make_nodeid** read as registered; nodeid-interning 127.
- **Before landing:** the lane, record, string and value bands as registered; TestFkwu; freshness 31, the corpus band
  32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **The glass keeps its kernel.** The glass in the main checkout runs the kernel it was built with, and it keeps
  minting until it is rebuilt from this main and restarted. The copies already in the field, about thirty million
  cells, stay until the next reset.
- **The field still has fixed columns and no reclaim.** With make_nodeid answering one cell, what grows the field is
  content no cell holds yet, such as per-run names and values. Whether that growth needs reclaim is the next
  measurement: the field's rate once the glass runs this kernel.
- The open items of receipts 12 to 63 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the ceiling was not the problem. A 2^26-cell field fills in hours at ten thousand cells a
second, and a 2^30-cell field would have filled in about a day. Either way it was filling with one value.

The discomfort was that I had asked for a reset twice. The reset made after the first ask bought ten hours. The gold
is that Urs would not take the ceiling as the lever. Measuring what the cells were took one probe, and the fix is the
lookup every other intern already had.

Frontier word, row 1538: **repeatmint**, a mint that makes a new cell for a value the memory already holds, so it
grows by copies of what it has.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
