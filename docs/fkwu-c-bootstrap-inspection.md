# fkwu bootstrap ownership inspection

Scope: `runtime/fkwu-uni.c`, its generated opcode table, and the dynamic Metal carrier. The [runtime status](fkwu-kernel-status.md) describes observed capabilities; the [north star](fkwu-form-native-north-star.md) defines the destination.

The complete lexical census indexes every function definition, file-scope declaration and local static, and reconciles the index with the active Clang AST. Source-region labels and migration suggestions support navigation; they do not establish semantic correctness for every symbol.

## Current seed census

| Population | Count |
| --- | ---: |
| Function definitions across source branches | 643 |
| Top-level declaration units | 658 |
| Distinct global names | 374 |
| Individual file-scope global declarations | 395 |
| Local-static declaration units | 58 |
| Preprocessor directives | 304 |
| Unresolved top-level units | 0 |
| Active Darwin Clang functions | 562 |
| Active Darwin Clang globals | 365 |

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
The [current ownership evidence](evidence/fkwu/shared-field-ownership.json)
checks first admission with fresh identities, nested lists, scalar prefixes,
existing shared lists and empty lists. Its Form-native destination is the
context-owned sharing operation; this transport correction adds no function or
global.

## Review contract

For each function and mutable declaration, identify its ABI operation and owner; keep interpretation and policy in Form; carry bulk data through resident spans and completion dependencies; establish behavior and failure witnesses; then delete an implementation when its observed replacement owns every active caller.

Measure seed C, adapters, assembly, globals, exported operations and generated artifacts separately. A smaller seed is progress when ownership, behavior and resource performance remain observable.
