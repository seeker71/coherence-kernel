# fkwu bootstrap ownership inspection

Scope: `runtime/fkwu-uni.c`, its generated opcode table, and the dynamic Metal carrier. The [runtime status](fkwu-kernel-status.md) describes observed capabilities; the [north star](fkwu-form-native-north-star.md) defines the destination.

The complete lexical census indexes every function definition, file-scope declaration and local static, and reconciles the index with the active Clang AST. Source-region labels and migration suggestions support navigation; they do not establish semantic correctness for every symbol.

## Current seed census

The inspected seed contains 896,371 bytes and 21,681 source lines. The census
includes every conditional branch; the Clang reconciliation describes the
active Darwin build.

| Population | Count |
| --- | ---: |
| Function definitions across source branches | 672 |
| Distinct function names across source branches | 607 |
| Top-level declaration units | 698 |
| Distinct global names | 379 |
| Individual file-scope global declarations | 400 |
| Local-static declaration units | 64 |
| Preprocessor directives | 330 |
| Unresolved top-level units | 0 |
| Active Darwin Clang functions | 573 |
| Active Darwin Clang globals | 368 |

The active AST additionally sees `__sigbits` from host headers and `fk_optab`, `fk_optab_n`, `fk_rwtab`, `fk_rwtab_n` from the generated opcode header. Conditional source branches and the active platform are different populations.

The current [per-symbol audit](evidence/fkwu/c-bootstrap-audit.json) carries positions, signatures, declarations, call-shaped tokens, references matching global names, numeric tokens and ownership/migration information. Matched names may be shadowed; call-shaped tokens may be macros or indirect calls. Missing or reordered region anchors prevent publication.

## Executable inspection

| Native organ | Responsibility |
| --- | --- |
| `form/form-stdlib/bml/c-bootstrap-inspection.bml` | Lexical units and complete per-symbol records |
| `observe/c-bootstrap-inspection-run.fk` | Source census |
| `observe/c-bootstrap-outline-run.fk` | Function/global outline |
| `observe/c-bootstrap-review-source-run.fk` | Source reading by unit |
| `observe/c-bootstrap-audit-run.fk` | Full audit and active-AST reconciliation |
| `observe/tests/c-bootstrap-inspection-band.fk` | Parser and census behavior |

## Handwritten ownership and native destination

| Area | Present responsibility | Form-native destination |
| --- | --- | --- |
| Values, pools and collection | Process-wide storage, roots and tagged representation | Context-owned values, roots, layouts and collection policy |
| Parsing and source admission | Direct-source parser, diagnostics, compilation and images | Form compiler producing identified native modules in RAM |
| Executable memory | Allocation, protection, instruction-cache synchronization and entry | Versioned executable spans with final-lease release |
| Metal carrier | Platform APIs, buffers, completion and pipeline residency | Form resource, queue, program and destruction policy |
| Host dispatch | Opcode binding and platform operations | Minimal versioned calls with Form batching and interpretation |
| Files, mapped stores and sharing | Open/map/read/write/sync/unmap and mixed store behavior | Form naming, ownership transfer, consistency and persistence |
| Processes and networking | Host operations and remaining protocol behavior | Form lifecycle, framing, routing, deadlines and retry policy |
| Audio and media | Capture/playback and remaining format behavior | Form stream ownership, decoding, scheduling and interpretation |
| Numeric/device paths | Device entry and remaining operator choices | Explicit Form admission, layouts, batching and comparison |
| Portability | Platform ABI declarations and implementations | Separate ABI families sharing a Form value/module contract |

The Form reference walker carries lexical frames, retained source and explicit run memory. Form contexts own their function/source roots, run-memory contents and CPU admission ledgers. The RAM image table grows with simultaneous residents; image mappings use checked page-rounded lengths. Final release checks physical unmapping before surrendering the admission. Raw unowned cache entries can be replaced; resident entries remain callable until release.

Seed intern pools, record metadata, parser state and other globals still require ownership migration. Cooperative Form resource owners do not establish protected address spaces or independently destroyable seed contexts. The current tests exercise 320 simultaneous CPU admissions and a 16,392-byte image; the absence of a fixed table or image ceiling does not promise unlimited physical memory.

The seed's field-sharing transfer preserves complete mixed local/shared lists.
The [source-bound ownership evidence](evidence/fkwu/shared-field-ownership.json)
checks first admission with fresh identities, nested lists, scalar prefixes,
existing shared lists and empty lists. Its Form-native destination is the
context-owned sharing operation; this transport correction adds no function or
global.

The process observer still parses platform records in C. Its argument result
holds at most 64 words; the Linux reader uses a single read of at most 65,535
bytes. The result does not distinguish a complete argument vector from these
truncations. Darwin process fields use fixed platform structure offsets and
bounded buffers. A Form-owned observer must carry exact byte extents, field
availability and completion before treating those rows as complete process
identity. The roster keeps a process-lifetime mapping with 256 slots and
compares process IDs when claiming or clearing a slot; this is not a general
generation-qualified context capability.

Opening this process's live page first unlinks its own PID-derived name, so a
recycled PID can receive a newly sized object while existing readers retain
their mappings. The unlink result is unchecked. If unlink fails and an existing
object cannot grow, the shared-memory opener can still return an undersized
mapping. The fresh-page behavior therefore does not establish complete failure
handling or a generation-qualified observation contract.

The Windows branch has additional unobserved ownership and status boundaries.
Its 64-entry process table replaces slot zero when full, closing a retained
handle before its child's status is collected. Its wait adapter ignores the
wait and exit-code query results; its flag adapter returns success without
performing an operation. Redirected spawning temporarily changes the parent's
standard descriptors, without checking every duplication or restoration.
Its formatted output has a fixed 4,095-byte payload bound. These source findings
do not establish Windows runtime correctness. Platform ABI calls remain a seed
shrink target: Form must own admission, complete transfer, failure disposition,
process handles and retryable release, with actual Windows witnesses before
claiming that contract on that host.

Ownership observations apply to their recorded source identities and executions.
They do not establish a semantic audit of every function in this census.

## Review contract

For each function and mutable declaration, identify its ABI operation and owner; keep interpretation and policy in Form; carry bulk data through resident spans and completion dependencies; establish behavior and failure witnesses; then delete an implementation when its observed replacement owns every active caller.

Measure seed C, adapters, assembly, globals, exported operations and generated artifacts separately. A smaller seed is progress when ownership, behavior and resource performance remain observable.
