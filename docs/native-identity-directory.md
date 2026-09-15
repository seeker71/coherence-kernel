# Native identity directory

Form owns a sparse directory over immutable, adaptive identity segments. A
stable row selects a full unsigned 64-bit identity through the same native ABI
as the [blueprint accessor](native-node-accessor.md). Adding a distant row
allocates its segment and directory metadata; it does not allocate every
intervening row. The C primary allocator and its direct readers remain in use.
Identity is field zero; other fields return unavailable without writing output.

The [directory authority](../form/form-stdlib/bml/native-identity-directory.bml)
emits one 312-byte ARM64 image into RAM. Its directory entry uses unsigned binary
search and tail-calls the segment getter. The getter reconstructs the word from
an eight-byte base and 0–64 residual bits. Full words stay in native registers
and owned RAM. The batch consumer makes one Form entry for many row queries;
only the completed prefix count returns through the tagged result surface.

Each directory descriptor contains four native words: active state, segment
count, table address and generation. Each immutable table row contains first
row, row count, getter address and segment descriptor address. A segment
descriptor contains active state, count, width and data address. This metadata
is shared across a segment, with no per-identity descriptor.

`ndi-append` admits complete raw words; `ndi-append-span` admits an owned native
span. The adaptive owner probes and encodes that span synchronously without
copying its words through a Form string. Admission validates nonoverlapping
ranges before building a complete candidate. Publishing replaces the current
directory only after its table and segments stand. `ndi-remove` retires a whole
range. Removed ranges remain reserved for the owner lifetime so an existing
handle cannot silently name another identity.

`ndi-acquire` pins one generation. New generations share unchanged segments;
a segment remains mapped while the current generation or any pinned retired
generation references it.
Released views first invalidate their own descriptors. Their descriptors stay
mapped until owner close, while retired tables and unneeded payloads unmap.
`ndi-project` reads a pinned view; `ndi-project-current` reads the current root
synchronously. Closing refuses while views or ordinary output mappings remain.
Admitted retirement blocks new calls and retains failed releases for retry.

The organ signals its actual generation, segment count and live readers.
Carrier descriptor/output bytes and native calls are separate from adaptive
payload bytes and probe/codec calls. These are requested extents, not RSS;
seed metadata and platform admission costs are outside those counters.
A refused close offers release of the needed reader.
The witness correlates that response, releases the reader and observes the
retired table and payload becoming unmapped, with the payload byte signal
decreasing by the exact released span.

## Published primary identities

The [primary importer](../form/form-stdlib/bml/native-primary-identity.bml)
connects actual published kernel node handles to this directory. Form emits
the Darwin ARM64 shared-memory open, object-size and read-only mapping calls.
No C source or static platform link is added.

The importer checks the calling kernel's shared-field home and layout, accepts
node handles, deduplicates their physical slots, and groups adjacent selected
slots. It validates the shared header and object extent, then reads only those
selected identities into owned adaptive segments. All source mappings close
before one publication makes the complete batch visible. The imported directory
continues serving the exact words after the importer closes.

The shared node counter describes reservations, including unfinished nodes.
It cannot establish publication. This importer therefore requires actual
published handles and never scans a counter-derived population. Non-field
handles refuse before the importer opens primary-field objects. Collector-bearing category, children
and value columns do not enter this storage.

## Executed scope

The [directory witness](../observe/native-identity-directory-witness.bml) covers
65 residual widths, 130 raw fields, 69 generations, high-bit bases, retained
readers, sparse gaps, retired-range refusal and complete output guards. It
publishes row **67,108,881** with only its selected segments, beyond the C field's
2^26 row limit. The [primary witness](../observe/native-primary-identity-witness.bml)
imports 25 published handles in one generation, checks all 200 raw bytes against
independent coordinate encoding, and reads them after source-owner retirement.
Both accept `run` on stdin. The primary witness also accepts `private` when
launched from an owned working directory whose `fkwu.conf` contains
`FK_FIELD_OFF 1` and whose `form/` and `observe/` paths reach this checkout's
source closure; the refusal must occur before the importer opens primary-field
objects. This mode selects the kernel's per-process node home; it does not
establish private-heap execution.

[Source-bound evidence](evidence/fkwu/native-blueprint-layout.json) retains nine
actual child executions, including non-field-home refusal and the existing
CPU/Metal layout and float checks. Every child exits zero, has empty stderr and
releases its native owners.

The [resident arena](native-identity-arena.md) generates and allocates identities
in native RAM, then freezes a pinned prefix into this directory in one
publication. Native generation and a 4,096-row append mint no primary nodes in
the measured hot calls. The directory retains its owned adaptive words after
complete arena retirement. Arena rows remain owner-local allocation coordinates.

Publication and release are serialized in one Form process. The current root
is a copied 32-byte descriptor, not an atomic publication for concurrent readers.
Async ownership requires an immutable generation pointer with ordering and
reader leases. External field reset or truncation must not overlap a source
lease; layout version alone does not identify a shared object's generation.

Raw identities retain all 64 bits. Form range admission uses its current signed
integer surface: nonnegative first row, positive count, and exclusive end no
greater than 2^62−1. Exported native pointers must fit below 2^56. These are
current interface limits, separate from payload width and physical capacity.
Per-segment mappings incur host page rounding; compact payload bytes are not
RSS. Form record metadata and retired range reservations remain retained until
the seed process ends. The execution establishes correct access and lifetime,
not a general bandwidth improvement.

The next boundary is canonical interning and the primary producer/read path.
The arena admits its implementation before allocating data. Primary cutover
must preserve that ordering while retaining tagged handles and native side-table
ownership. C allocation and identity readers can then move behind the Form
owner. Reference-bearing columns additionally need
collector root retention and relocation; slot reuse needs handle generations
and side-table ownership. The [north star](fkwu-form-native-north-star.md) keeps
these obligations with the resource owner.
