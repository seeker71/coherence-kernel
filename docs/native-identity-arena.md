# Resident native identity arena

Form generates semantic identities as single native 64-bit words and allocates
their rows in owned RAM. The [arena authority](../form/form-stdlib/bml/native-identity-arena.bml)
emits a 1,380-byte Darwin ARM64 image containing a coordinate generator, chunk
allocator, raw-word producer, prefix getter, batch consumer and retirer. No C
source change or static platform link is required.

Admission first makes this image resident and exports its getter. That first
call allocates no data chunks. Subsequent native batches invoke the retained
image; generation, copying, mapping and publication require no per-row Form
callback or primary-node mint. Setup, observations and adaptive freezing still
use Form and seed metadata.

## Production and ownership

`nia-open(first, chunkWords)` creates a process-local owner. `nia-append` accepts
complete raw u64 bytes; `nia-append-span` reads an owned span synchronously.
`nia-generate` accepts four native words per input coordinate: package, level,
type and instance. Field widths come from the [identity authority](native-node-word.md).
The generator validates the entire batch before allocating or writing, then
generates each semantic word directly in its destination. Integer coordinates
retain their signed 63-bit semantics; structured coordinates retain their mode
bit and all 63 field bits. Raw-word append preserves every input bit.

The root holds 12 native words. Each chunk has a six-word header followed by
u64 identities. A complete batch uses the remaining tail capacity when it
fits; otherwise the native producer maps one chunk sized for the larger of
the batch and the owner's current growth preference. It fills that chunk
before publishing it. `nia-chunk-words` changes the preference in RAM, retaining
the admitted image and existing chunks. Allocation follows admitted data and
host resources; no array spans the identity address space.

Rows are owner-local allocation coordinates. Appending the same identity twice
can allocate two rows. These rows are neither canonical interning results nor
the primary kernel's tagged handles. Primary C allocation and readers remain
unchanged, including their fixed 2^26-cell capacity.

The [native interner](native-identity-intern.md) owns a separate arena and adds
exact-u64 duplicate detection. It preserves unique rows across index growth
and reuses this arena's prefix, adaptive freezing and retirement contracts.
`nia-open-image` admits that combined image through the same resource owner.

`nia-acquire` pins a published prefix. Its unique four-word descriptor retains
the first row, count and head pointer. Later tail writes and new chunks remain
outside that prefix. The getter uses the raw-u64 accessor ABI and walks chunks;
`nia-project` serves a batch with one Form entry. The current getter's lookup
cost grows with the number of chunks. Only identity field zero is available.

`nia-freeze` builds adaptive owned segments from a pinned prefix and publishes
them together into the [identity directory](native-identity-directory.md).
The last chunk is clipped to the pinned count even when more rows now occupy
its tail. Borrowed arena spans remain protected by the prefix lease while the
adaptive builder reads them. The directory owns its copies and continues
serving after the entire arena retires. Existing directory row reservations
refuse reassignment.

`nia-release` invalidates the reader descriptor. `nia-close` refuses while
readers or ordinary output regions remain. Once retirement starts, new calls
refuse. Native unmapping advances the owned chain only after each successful
release; failure retains the remaining chain for retry. Root-derived release
state is retained before metadata unmaps, so later observation and cleanup do
not reread freed memory. A failed admission that cannot finish cleanup retains
its closing owner; `nia-active?` distinguishes an admitted owner. Descriptors
and code retire after chunks. Seed record metadata remains process-retained.

## Live signals and execution

The organ reports actual published rows, growth preference, requested mapped
bytes, map/unmap counts, completed batches, last native errno, reader leases,
native entries and code state. Carrier descriptor and output bytes remain
separate. Refused retirement names the outstanding resource. The witness
correlates a release-reader response, applies it, and observes the reader
count fall from three to zero before retirement proceeds. Closed state remains
readable after root and code unmap.

The [execution door](../observe/native-identity-arena-witness.bml) accepts `run`
on stdin. It generates 70 coordinate rows covering every structured field bit,
signed extrema, and distinct integer/structured zero. Fourteen valid-first,
invalid-later batches exercise empty and partially filled owners; publication
counters, existing identities and unused tail bytes stay unchanged. Generated
rows above 2^56 retain exact metadata and raw identities through freezing.

A second run appends 4,113 rows beginning at 67,108,881 through three mappings
and four batches. The measured 4,096-row append and 70-row generation each mint
zero primary nodes. All projected u64 bytes and nonzero output guards match.
Older prefixes retain their boundaries through reads and adaptive freezing.
The sample requests 33,104 chunk bytes and 112 adaptive payload bytes. Those
payloads are unusually repetitive; the figures are not a general compression
ratio, RSS measurement or bandwidth benchmark. Every native mapping releases,
and the frozen directory still serves exact identities after arena retirement.

[Source-bound evidence](evidence/fkwu/native-blueprint-layout.json) retains this
execution alongside nine storage, CPU/Metal, accessor, directory, interner and
primary-import executions, with actual child exits and held source bytes.
Host-unmap failure recovery is source-reviewed; no syscall fault injection is
claimed. Operations are cooperative and serialized within one process. Reader
calls must finish before release. Form row admission ends at exclusive end
2^62−1, exported pointers fit below 2^56, and host mappings round to pages.

## Next boundary

The exact-word interner now preserves canonical rows within a native owner.
The next bridge must resolve native semantic words, kind-sensitive node
equality, tagged runtime handles and physical slots separately. Its native
admission must stand before primary allocation changes. Primary
producers and direct readers can then share that owner, with side-table
lifetimes and collector roots carried explicitly. Concurrent publication needs
ordered generation selection and retained readers; reference-bearing columns
need retention and relocation. Whole-arena release does not establish slot
reuse or primary-field reclamation. These are concrete requirements of the
[Form-native north star](fkwu-form-native-north-star.md).
