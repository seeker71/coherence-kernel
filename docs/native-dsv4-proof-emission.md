# Native DSV4 proof emission

`observe/dsv4-proof-emission-run.bml` loads the existing Form shader authorities
in the current `fkwu` process. It emits source and GGUF metadata without a Go
process, a shell dependency walk, generated Form source, or GPU admission.
The three DSV4 GPU proof callers use this door for their shader, manifest and
compression-ratio inputs. Their device compilation and numerical checks remain
separate consumers of those complete inputs.

The request contains five NUL-terminated strings:

```
DSE1, operation, subject, detail, END
```

An optional final LF follows the last NUL. The same five strings may arrive as
a JSON array preceded by its exact eight-byte little-endian byte length. One
complete request is required. The shared native input owner observes real file
offset/extent or pipe queue/HUP, then releases its code and mappings before
request dispatch. The executable input/output owner is currently observed on
Darwin ARM64. Model paths preserve spaces, quotes, backslashes, Unicode and
literal newlines. They are data, never generated source.

| Operation | Subject | Detail |
| --- | --- | --- |
| `shader` | One entry below | Empty, or its admitted precision selection |
| `manifest` | GGUF path | Empty |
| `array` | GGUF path | Complete metadata key |

The shader entries are `dsv4-embed-msl`, `dsv4-mla-unit`,
`dsv4-mx8-matvec-msl`, `dsv4-mla-core-msl`, `dsv4-hc-unit`,
`dsv4-mx4-matvec-msl`, `dsv4-iq2-matvec-msl`, `dsv4-router-f16-msl`,
`dsv4-ffn-unit`, `dsv4-stack-ffn-unit` and `dsv4-stack-kv-unit`.
Only `dsv4-hc-unit` admits `hc-precision`; only `dsv4-mla-core-msl` admits
`core-precision`. These append the corresponding public
`metal-fp32-compensated.bml` fragment to the exact base source. The source
composition has one home, shared by the existing wrappers and this door.

Metadata reading has no fixed header-size cap. It validates GGUF version 3,
complete metadata and tensor directories, positive dimensions, header alignment,
supported block pricing, checked products and file extents. The complete saved
header is validated again in memory before rendering. Up to four tensor
dimensions and the sixteen types priced by `gguf-manifest.fk` are admitted;
unpriced types and overflowing or incomplete geometry refuse before output.
File-size consistency is observed; concurrent same-size payload mutation is
outside this metadata inspection contract.

The manifest retains the `MAGICOK`, `VERSION`, `NTENSORS`, `NKV`, `KV`,
`TINFOSTART`, `TINFOEND`, `DATABASE` and `T` rows. Arrays in a manifest carry
their type and count. A requested array emits every supported integer or finite
F32 value, followed by `END`; unsupported element types refuse before `ARR`.
`ABSENT` and `NOTARRAY` are distinct complete miss responses, and callers
requiring an array must require its `ARR` and `END` rows.

Unsigned integer wire values retain all 64 bits. Signed integer metadata keeps
the existing explicitly unsigned `u` representation. Scalar finite F32 and F64
values use shortest round-trip decimal digits with the existing scientific
layout; F64 has a `KVF64` row. Signed zero and subnormals remain distinct.
Nonfinite scalar values and selected array values refuse before a data stream.

Keys and tensor names must fit a whitespace-delimited token. Single-line UTF8
strings retain `KVSTR`. A string containing NUL, CR or LF uses
`KVSTRJSON` followed by one JSON string, preserving every original byte after
decoding. The current model's multiline chat template uses this representation;
numeric, tensor and compression-ratio rows retain their exact consumer bytes.

Every payload and its framing are assembled in memory before a checked native
descriptor write. Partial writes and interruptions resume at the unconsumed
offset; a blocked nonblocking descriptor waits for readiness. The writer
borrows stdout, restores its original SIGPIPE flag, and releases its owned code
and memory. Failed restoration retains the owner and prior flag for settlement
retry without rewriting accepted bytes. Failed release stays explicit.
Completion or refusal travels as correlated organ-health rows on stderr with
byte counts, error and release state. A successful return requires complete
payload output and settled ownership. For shaders, manifests and present
arrays, the runner's final `0` follows the payload's `END`. Complete `ABSENT`
and `NOTARRAY` miss responses return `0` after their miss row without `END`.

The observed source comparison covers all eleven base shader streams and both
precision compositions. All unchanged manifest rows and the 443-byte ratio
stream match retained independent outputs; the chat-template JSON decodes to
the exact original string. Boundary observations cover a metadata array larger
than 16 MB, complete framing and unusual paths, 64-bit values, F32/F64 numeric
boundaries, malformed and truncated files, output refusal and owner settlement.

The north star is one Form source owner, complete RAM requests and outputs,
dynamic compiler selection and reusable native programs. The remaining GPU
proof scripts still own device compilation and their existing numerical
consumers; this emission door does not claim those host responsibilities.
