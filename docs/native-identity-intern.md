# Native identity interning

The [Form authority](../form/form-stdlib/bml/native-identity-intern.bml) emits a
resident Darwin ARM64 interner. Each exact 64-bit semantic word receives one
stable row within its owner. Repeated words return that row; index growth does
not move it. One native entry processes a batch without per-key Form callbacks.
The 2,620-byte image includes the [arena](native-identity-arena.md) producer,
getter and retirement entries. No C source or static platform link changes.

## Identity and ownership

`nii-open(first, chunkWords, slots)` admits code before allocating index or
identity data. Initial index slots must be a power of two, at least two. The
owner contains its arena and a private 96-byte index control. A failed admission
that cannot finish releasing resources returns a closing owner for retry;
`nii-active?` identifies successful admission.

`nii-intern(owner, words, output, offset)` accepts complete raw-u64 words and
writes owner-local row numbers into owned output. Full-word comparison resolves
hash collisions. Integer and structured encodings remain distinct. Zero, the
high bit and all-ones words are valid keys. This is exact-word equality for
generated identity descriptors; other node kinds and normalization of alternate
raw coordinate encodings are outside this owner. Rows are neither primary
tagged handles nor identities shared across owners.

Each index bucket holds an eight-byte key and an eight-byte row-plus-one;
zero in the second word denotes an empty bucket. The index doubles before a
new key would exceed half occupancy. It builds the complete replacement before
publication, retaining stable arena rows. Retired tables remain owned until
native unmapping succeeds. `nii-collect` retries release and clears a resolved
native error. This hash index is separate metadata: the semantic identity
remains one 64-bit word, and frozen storage still chooses adaptive widths.

Batches publish completed prefixes. Allocation pressure leaves those rows and
output words valid; the uncompleted output stays untouched. An existing key
can still resolve when a new row cannot be admitted. `nii-find` performs lookup
without insertion and stops at the first absent word. The returned count,
last completed count and native errno must be read together. Ordinary missing
lookup is not an allocation error.

`nii-acquire` returns a pinned arena prefix. The existing `nia-project`,
`nia-release` and `nia-freeze` operations serve, release and adaptively freeze
that prefix. A pinned prefix cannot see later rows. Frozen directory copies
survive retirement of the index, arena and executable image.

`nii-close` refuses outstanding prefix leases and ordinary outputs. After
retirement begins, it closes admission, releases index mappings, then retires
the arena and code. Failed releases retain ownership for retry. Its captured
state remains readable after metadata unmaps. After successful retirement,
repeated close returns `0` and performs no work. Admission, lookup, publication
and retirement are serialized within one process. Reader calls finish before
their leases release.

## Live observations

`nii-state` reports unique words, index capacity, live requested bytes,
successful maps and unmaps, probe and hit counts, completed prefix, native
errno, pending retired tables and arena ownership. Probes exclude rehash work.
Pressure emits an organ signal carrying the observed state. Successful native
hot batches and directly measured native pressure mint zero primary nodes.
The Form signal itself still allocates primary metadata; the witness reports
that separate observed cost without treating it as fixed.

The [execution door](../observe/native-identity-intern-witness.bml) accepts
`run` on stdin. Its independently calculated collision case wraps through
buckets 7, 0 and 1. A 2,051-word batch follows those three keys and leaves
1,030 unique rows. Reversed duplicate replay preserves every row without new
maps or primary mints. The index makes ten mappings, retaining one 65,568-byte
table after nine unmaps; arena chunks request 11,440 bytes. These requested
sizes exclude page rounding and are not RSS or throughput results.

The same execution checks exact high-bit keys, missing lookup output guards,
invalid admission, a second independent owner, old prefix boundaries, frozen
identities after full retirement and readable closed state. A small owner
starting near the Form row endpoint publishes two words, reports the refused
third and still resolves an existing key. No field exhaustion or reset is
used. Release-reader control is correlated, applied and re-observed before
retirement. All admitted native mappings release.

[Source-bound evidence](evidence/fkwu/native-blueprint-layout.json) holds ten
actual child executions, their complete outputs, zero exit codes, empty stderr
and the exact source bytes. Host-unmap failure recovery is source-reviewed;
no syscall fault injection or adversarial hash-complexity bound is claimed.
Form row admission ends at exclusive end 2^62−1, exported pointers fit below
2^56, and seed record metadata remains retained. These are current interface
and ownership limits, separate from the 64-bit identity payload.

## Next boundary

Primary C allocation, interning and readers still use the shared field and its
2^26-cell capacity. Their replacement must connect semantic words, kind-sensitive
equality, tagged handles and physical storage without conflating them. The
native exact-word interner is an executable component of that owner, not the
primary cutover. The next bridge must retain collector roots, native side-table
lifetimes and handle generations before slot reuse. Concurrent or async
publication additionally needs ordered generation selection and retained work.
Those obligations define the next step toward the
[Form-native north star](fkwu-form-native-north-star.md).
