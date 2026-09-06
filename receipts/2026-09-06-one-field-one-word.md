# 2026-09-06 — one field, one word

Urs, before noon: "multiple processes can share the same memory for shared
nodes, so not each process needs its own copy."

## What stands

Every `fkwu` on this host now opens the *same* store — `/fg-field-<letter>`:
the node columns (kind, category, kids, value, NodeID, source pointer, hash
memo), a shared intern index, a shared pair arena for the kids of shared cells,
a shared string pool and a shared float pool for the values shared cells carry,
and a header whose counters every kernel claims with an atomic add. Interning
is one door for every kind: hash by content — strings by bytes, floats by
bits, composites by their children's content — probe the shared index, compare,
and either take the cell another kernel already made or claim the slot, fill,
publish. A composite interned in one process and again in another is one cell
with one word:

```
field-band  child-word -81821  my-word -81821  field-nodes 40910 40910  store 2
```

The field's node count does not move when the second process interns; the
float node reads 3.25 through the kernel and through the field reader; the
child's category text reads from the shared column. `field-band` 255, twice.
The private cons heap, private strings and floats stay per process for
transient values; a value becomes shared the moment a shared cell carries it,
and every read dispatches by index range — 2⁴⁰ and above is the field — behind
the same words the private tables use, through eight accessor doors the whole
seed now goes through. The per-kernel store of the morning is the fallback
when the host offers no shared memory. The field persists across processes,
as a host memory should; `observe/field-reset-run.fk` starts it over when no
other kernel is alive. Live page word 22 reads 2. The glass frame path is
still 10 ms mean; every glass band, the corpus band and `python-bmf-grammar`
answer as before; resident size unchanged.

Ledger R121 released. Corpus 1289 *onefield*.

## The most surprising teaching

Two processes probed different slots for the same string because
`fk_deep_hash` hashed the *word* — the private pool index — not the bytes. In
one process a word is its content; across two it is only an address. Every
hash, every equality and every "same" in the seed had been leaning on that
identity without saying so, and the field is the first place where the lean
showed. And a smaller one under it: `intern_trivial_float` takes a string;
three probes fed it a box and interned 0.0 into the field, then read their own
zero back as a bug in the door.

## Where discomfort turned to gold

The morning's per-kernel store was proof that the surface could be shared; this
noon's ask was that there be *one*. The pull was to keep both live —
per-kernel columns and a field mirror. The gold was letting the per-kernel
store become the fallback and making the field the store: fewer segments, one
truth, and the floats that were boxed at parse time — before anything else
asked for shared memory — became the door through which the field opens, so
that nothing private ever starts first.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, field-band 255, live word 22 = 2, frame path 10 ms mean
