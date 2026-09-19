# Native blueprint layout

Form owns a shared blueprint for each packed RAM run. Execution fields and IDs
remain native 64-bit words. The storage descriptor gives each field its name,
bit width and interpretation; Form derives every bit offset and the row stride.
There is no per-cell schema copy or per-row byte rounding.

The authority is [native-blueprint-layout.bml](../form/form-stdlib/bml/native-blueprint-layout.bml).
It emits ARM64 packing, raw projection and numeric projection into RAM. Each
owner admits one reusable image for its layout. Native loops process complete
batches with one entry per call. Word-aligned 64-bit fields use direct native
loads and stores. Metal source is generated from the same descriptor and
admitted through the dynamic carrier's RAM program interface.

| Observed layout | Packed payload | Expanded execution fields |
| --- | ---: | ---: |
| One 64-bit primitive | 8 bytes | 8 bytes |
| Seventeen 65-bit rows | 139 bytes | 272 bytes |
| Nineteen six-field, 77-bit rows | 183 bytes | 912 bytes |
| Twelve mixed numeric, 131-bit rows | 197 bytes | 864 bytes |

The CPU owner requests only the payload and any trailing bytes its generated
loads touch. A word-aligned 64-bit primitive needs no trailing allocation.
There is one tail allowance per run, never one per cell. An empty run has no
payload and owns a one-byte mapping request for its lifetime. Host page
rounding, physical residency, executable pages and Form metadata are separate
costs; these payload figures are not RSS measurements. Retiring a run closes
new reader admission; its existing leases retain the mapping until the last
reader releases. Closing confirms release of mappings and native code. The
seed's record arena retains owner metadata.

Raw projection preserves every bit, including all unsigned 64-bit IDs and NaN
payloads. Optional numeric projection supports f16, bf16, E2M1, E4M3 and E5M2.
The [float authority](../form/form-stdlib/bml/native-blueprint-float.bml) names
exponent, fraction, bias and special-value policy. CPU numeric output contains
binary64 bits; Metal contains binary32 bits in the low half of each 64-bit slot.
Both preserve signed zero and representable subnormals. Numeric NaNs become
signed quiet NaNs; raw storage retains the original payload. E2M1/E4M3 payloads
alone do not establish MX block-scale or model-file layout semantics.

The [adaptive word owner](../form/form-stdlib/bml/native-node-storage.bml) uses
one 64-bit base and 0–64 residual bits per stored word. Form observes the width,
builds an immutable generation, publishes it to a stable local slot and retains
older mappings while readers hold them. Full-width or small blocks can cost
more than raw words. Publication is serialized within one Form process;
cross-process atomic publication and reclaimable record metadata remain open.

The [native accessor](native-node-accessor.md) exposes a full raw-u64 getter
over these runs. One reusable native entry consumes different immutable layout
descriptors. A native batch consumer calls it without per-field Form crossings;
only completed counts return through the tagged JIT surface. Leased descriptors
preserve retired runs and refuse reads after view release.

The [identity directory](native-identity-directory.md) adds sparse stable rows
over adaptive segments. Existing readers retain their exact generation while
new ranges publish. An owned-span input lets the storage emitter probe and
encode native words directly. A read-only importer brings published primary
node identities into one owned generation without per-word Form conversion.
The [resident arena](native-identity-arena.md) generates identities and grows
native chunks before freezing pinned prefixes into adaptive directory segments.
Frozen words remain available after all producer chunks and code release.
The [native interner](native-identity-intern.md) adds duplicate detection before
new arena rows are allocated. Its index is separate metadata; unique words
freeze through the same adaptive storage path and survive index retirement.

The [layout witness](../observe/native-blueprint-layout-witness.bml) compares
all field widths against an independent bit oracle, executes actual CPU and
Metal projections, and checks indexed reads, complete output guards, typed
admission, immutable-output refusal and retirement. The
[float witness](../observe/native-blueprint-float-witness.bml) checks 588 exact
CPU/Metal cases. The [word storage witness](../observe/native-node-storage-witness.bml)
checks all 65 residual widths and retained readers across 3→61→7-bit generations.
Each accepts `run` as one stdin line. Source-bound outputs and actual exits are
retained in [execution evidence](evidence/fkwu/native-blueprint-layout.json).

The [measurement door](../observe/native-blueprint-layout-measure.bml) compares
the same six fields in compact and word-aligned layouts. It checks complete
outputs, reports emission and admission separately, and times warm batches.
The GPU upload is outside the compute interval; data then remains resident
across the batch. Host load and millisecond resolution limit timing precision.
Smaller storage does not imply faster computation on every workload.

These are Form-owned packed runs, not the primary shared node columns. The
shared field still uses 8-byte identity entries, 80 logical bytes per node and
a fixed 2^26-cell capacity. The Form directory can serve sparse rows above that
capacity; replacing the primary field requires its producers and readers to
use this owner, along with shared publication and owner-aware reclamation.
Moving tagged references requires the collector to retain and relocate them.
Current Metal dispatch uses
32-bit row indices; a larger run needs explicit range submissions. The
[north star](fkwu-form-native-north-star.md) makes those ownership and resource
boundaries part of the runtime itself.
