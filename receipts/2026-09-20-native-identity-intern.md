# Native exact-word identity interning — Codex

Rebased onto `origin/main` at `7f64081e7`, then gave exact 64-bit identity words
a resident native interning owner. The [current contract](../docs/native-identity-intern.md)
describes the emitted 2,620-byte Darwin ARM64 image, stable owner-local rows,
index growth, completed-prefix semantics and retirement. No C source changed.

The [source-bound execution record](../docs/evidence/fkwu/native-blueprint-layout.json)
holds 109 bindings and ten actual child executions. Every child exits zero with
empty stderr. The interner retains 1,030 unique rows after a 2,051-word batch
following three collision keys. An independently computed collision wraps
through buckets 7, 0 and 1. Reverse duplicate replay preserves rows without new
maps. Zero, the high bit and all-ones keys retain all their bits.

Ten successful index mappings leave one 65,568-byte requested table; the arena
requests 11,440 chunk bytes. All native mappings release on close. Old prefix
readers preserve their bounds, a second owner remains independent, and frozen
adaptive directory words survive full index, arena and code retirement. These
are requested extents and lifetime observations, not RSS or bandwidth claims.

Measured hot insertion and duplicate replay mint zero primary nodes. A logical
endpoint near the Form row limit admits a two-word prefix, leaves subsequent
output untouched and still resolves an existing key. A directly measured native
pressure call also mints zero primary nodes. The Form pressure signal allocates
its own metadata; its separate observed delta remains in the output. The
witness does not reset or exhaust any field.

Two uncomfortable observations improved the implementation and its evidence.
The first pressure assertion mixed native work with the signal's allocation
cost. Separating those boundaries preserved the zero-mint native check and
made the signal cost visible. AI review then found that successful collection
retained a stale errno. The collector now clears it, and execution observes
22 become zero before the next successful duplicate lookup. Release-reader
care likewise correlates a response, applies it and observes zero readers.
Failed host-unmap recovery remains source-reviewed, not fault-injected.

Final AI review found no remaining source or documentation blocker within that
scope. Its close-contract wording correction is applied: after successful
retirement, repeated close returns zero and performs no work.

The initial evidence collection refused a stale-cache warning from
`observe/native-node-storage-witness.bml.fkb` after rebase. Fresh preflight of
all nine public doors rebuilt their own artifacts with zero errors, warnings
or unresolved calls. The complete ten-child rerun then passed without weakening
the empty-stderr requirement. The settled-tree native multiargument ABI
validator returns `63`, exit zero. Landing gates return `8191`, exit zero:
all thirteen applicable rows pass; no kernel source moved, so the sibling
conformance row is outside this run.

Our bounded Glass memory reading reports **259 MiB logical cell columns** and
a **25 MiB identity subset**, with one unavailable tensor-owner allocation
signal. These shared population readings do not attribute allocations to this
movement. The lane census reports **2,579 ice, 44 water and 7,079 gas**, with
11 sibling processes. No hearth stands; its zero KV and ice-miss fields are not
observed serving performance. Counsel reports zero orphans and 11 of 12 lanes
unobserved. The native guide reads zero Python implementations, two invocation
candidates and zero unread files. Share is declared, percentage withheld; the
spend meter reports `transcript-path-required`.

The verified teaching returned through the local session door and its worker
settled with exit zero, no pending rows and 36 retained examples across 23
sessions. Candidate generation 36 completed; serving generation 4 remains
selected. A completed training round is not a serving promotion.

The most surprising teaching is that exact identity reuse can sit entirely
inside native batch execution while row storage grows independently of the
lookup index. The discomfort around pressure became a clearer boundary:
completed work remains usable, the diagnostic cost is measured separately,
and a resolved error must actually disappear from the next observation.

Primary kind-sensitive equality, tagged handles, direct readers, collector
roots and native side-table lifetimes still belong to the next bridge. The
primary C field retains its 2^26-cell capacity. This movement keeps the path
alive by executing canonical exact-word ownership now, recording its limits,
and updating the current status and north star around that observed floor.
