# fkwu runtime status

The [north star](fkwu-form-native-north-star.md) defines the complete Form-native runtime and OS. This page describes current execution. The [bootstrap inspection](fkwu-c-bootstrap-inspection.md) carries the function/global ownership census.

The source runtime passes direct bootstrap (`42`, `55`, numeric lists), binary
freshness (`31`) and native-body execution (`11111`). Signal-driven care runs in
source and through the installed CLI's compiled default entry. The
[source evidence](evidence/fkwu/native-core-care.json) contains 13 resource
scenarios, real child diagnostics and retained resident reads. The
[installed care evidence](evidence/fkwu/native-startup-care.json) binds its
actual executable and companions. The
[canonical CLI artifact](evidence/fkwu/native-cli-artifact.json) passes source
generation, Darwin ARM64 publication, ordinary cached installation, the existing
behavior proof and 14 artifact admission cases.

Each node identity is generated as [one native 64-bit word](native-node-word.md).
Its structured format uses 6 package bits, 13 level bits, 12 type bits and 32
instance bits with one format bit. Integer coordinates retain a signed 63-bit
instance. Form owns the layout and generates its bootstrap header; source
identity and freshness bind that header. The identity column is 8 bytes per
cell; logical column accounting is 80 bytes per cell. The field and mapped
readers use layout version 2. The [execution evidence](evidence/fkwu/native-node-word.json)
includes exact boundaries, ordinary refusal, per-process storage and current
operator, lexicon, interning, serialization and Glass consumers.

[Native blueprint layout](native-blueprint-layout.md) gives Form-owned RAM runs
shared field descriptors, arbitrary bit offsets, exact 64-bit raw projections
and optional ML float interpretations. Form emits CPU pack/projection code and
Metal projections from that same layout. A word-aligned primitive requests
8 payload bytes; complex rows share metadata and need no per-row alignment.
The adaptive word owner independently widens or shrinks immutable generations
while retaining pinned readers. These local owners are separate from the
primary shared field.

The [native accessor](native-node-accessor.md) carries full raw-u64 fields
between native functions without tagged returns. One 424-byte RAM image serves
67 observed layouts and all 64 field widths. Immutable descriptors pin packed
runs; retirement blocks new readers and final release unmaps the data. Owner
close waits for native views and outputs, with released descriptors retained
until that close. Unease reports current ownership and byte counts. These
process-local leases do not establish atomic cross-process publication or
collection of tagged references stored in raw memory.

The [identity directory](native-identity-directory.md) serves sparse stable rows
over adaptive segments through one 312-byte native image. Its witness covers
65 residual widths, 69 generations and row 67,108,881 without allocating the
intervening rows. Reader leases retain removed segments until final release;
retired ranges cannot be reassigned. The native read-only importer takes 25
published primary handles into one generation and preserves all 200 raw bytes
after source mappings close. Non-field handles refuse before the importer opens
primary-field objects. Ten [source-bound executions](evidence/fkwu/native-blueprint-layout.json)
cover these boundaries and the CPU/Metal storage consumers. Directory
publication is serialized; the C allocator and primary readers are unchanged.

The [resident arena](native-identity-arena.md) generates 64-bit semantic words,
maps and fills native chunks, serves pinned prefixes and freezes them into
adaptive directory segments. Its 1,380-byte Form-emitted image stands before
data allocation; its growth preference can change in RAM without readmission.
The witness generates 70 coordinate rows, rejects 14 malformed batches before
writes, and appends 4,113 raw words across three mappings. Measured native
generation and the 4,096-row append each mint zero primary nodes. The repeated
sample uses 33,104 requested arena bytes and 112 adaptive payload bytes; these
are not RSS or bandwidth results. Readers and outputs hold retirement, every
mapping releases, and frozen identities survive arena close. Owner-local rows
do not replace primary tagged handles or canonical interning. Whole-arena
release and serialized growth remain the current ownership boundary.

The [native interner](native-identity-intern.md) adds exact-u64 canonical rows
within a separate arena owner. Its 2,620-byte Form-emitted image resolves full
keys, grows an owned index and reuses stable rows without per-key Form calls.
The witness retains 1,030 unique rows through ten index mappings, duplicate
replay, prefix reads and complete retirement. Hot insertion and replay mint
zero primary nodes; Form pressure signals still mint metadata and report their
observed cost separately. A logical endpoint exercises completed-prefix
pressure without exhausting the field. Index buckets cost 16 bytes independently
of eight-byte identities and adaptive frozen payloads. Primary tagged handles,
other node kinds and collector ownership remain outside this interner.

The shared field reuses identical NodeID coordinates; the allocation witness
returns `7`. Its node columns still have a fixed 2^26-cell capacity and no live
reclamation. Reset requires every other kernel to settle. The
[Form admission predicate](evidence/fkwu/field-reset-admission.json) requires a
singleton registered-kernel roster containing this process's PID; empty,
missing-self and additional-owner readings refuse. The roster remains bounded
and does not establish complete host liveness. No reset is exercised by those
pure admission observations.
Interning prevents duplicate identities from consuming fresh cells; it does
not establish a bounded lifetime for changing diagnostic values.

## Runtime and native computation

Form source and executable BML run through `fkwu`. BML lowers in memory; native artifacts are reusable caches. Source admission materializes a whole unit synchronously. The [native CLI builder](native-cli-assembly.md) emits startup around this source runtime and compiles the Form REPL into an adjacent `.fkb` image with its `.sym` record. Source snapshots and build/install attestations bind the three-file platform unit. Default launch accepts a clean, compatible recipe and symbol pair; it does not require the installation attestation. The table compiler remains an independent proof surface.

The source snapshot holds the exact name registry and all its declared source
homes. Form owns this dependency expansion; the snapshot runtime resolves names
from those copied bytes. The [six-case observation](evidence/fkwu/native-home-registry.json)
includes actual snapshot execution by name and malformed or nonportable registry
refusals. Adding a declared home renews source identity without editing the seed.

[Native BML admission](native-bml-admission.md) owns statement and balanced-body
boundaries in Form. Inline and multiline class/function bodies preserve their
statements; nested `do` expressions retain ordering, bindings and selected
branches. Class descriptors and ref resolution follow the same lexical spans.
Incomplete expressions and unconsumed suffixes refuse admission. The separate
cursor grammar remains an explicit proof surface; executable `form.bml` has one
source owner. Compiler dependency identity participates in BML cache renewal.
The [32-case execution](evidence/fkwu/bml-admission.json) retains actual success
and refusal statuses. A [direct cache renewal](evidence/fkwu/bml-cache-renewal.json)
executes the complete body with the root binary and C source unchanged.

Metal is loaded dynamically through a selected adapter. Form generates Metal source, identifies program/entry/target, and chooses RAM admission or an explicit compiled-archive mode. Shader source files and shader compiler subprocesses are unnecessary. Each executable linkage observation applies to its recorded artifact.

Form owns BMP validation, the nine-field integer interpretation, policy thresholds, GPU source generation, captures, submissions and result eligibility. Several versions can use one captured input. Its 144-byte ARM64 frame fold reads each pixel once through three synchronous borrowed-string calls for a normal frame. Larger rectangles subdivide according to the integer result capacity. The interpreter is an explicitly requested equivalence reference.

## Native authoring and inspection

Form owns the following executing surfaces. Each linked page names its public door and current boundary.

| Surface | Current execution |
| --- | --- |
| [Artifact codec and source lens](native-form-artifacts.md) | FORMBIN2 encoding, decoding, structural comparison, exact numeric payloads, adaptive-depth traversal and independent Go/Rust/TypeScript conformance |
| [GPU source generation](native-gpu-source-generation.md) | Canonical PTX and GLSL templates become executable BML authorities; generation and publication preserve checked bytes |
| [Metal asks](native-metal-ask.md) | Typed request admission, full model identity, tokenization, GPU work, owned CPU hashing, resource release and private atomic answer publication |
| [Numerical references](native-dsv4-numeric-reference.md) | Independent Form hyperconnection and toy-forward mathematics compared with production computations and held anchors |
| [Specimen compilation](native-python-compiler.md) | Form parses, lifts, dispatches and emits the supported Python grammar in the current process; complete source admission and owned publication preserve an existing output on refusal |
| [Proof traces](native-kernel-trace.md) | Form compiles specimens, supervises independent Rust execution and renders its real dispatch trace |
| [Routing proofs](../form/form-kernel-rust/README.md) | Form owns socket fixtures, concurrent clients, exact response checks and child settlement around the independent Rust server |
| [Concept construction and audit](native-concept-source-audit.md) | Form owns canonical projection, all lexical candidate groups, attributed corpus reindexing, complete WordNet sense construction, binary indices, offsets, metadata, staged publication, stable IDs, aliases, provenance and exact held-byte hashing; portable and explicitly selected ARM64 policies agree |
| [Terminal acceptance](glass-keyboard.md) | Form owns real Darwin ARM64 PTYs, immediate input checks, signals, exact termios restoration, deadlines and independently supervised resource release |
| [Ordered CPU mathematics](native-fp64-matrix.md) | Form emits binary64 MXFP8 matrix code in RAM; repeated calls reuse owned output memory without boxing intermediate products or sums |
| [Real-model numerical oracle](native-dsv4-oracle.md) | Form owns GGUF admission, independent quantized CPU arithmetic, complete layer and token histories, exact retained vectors and tensor-sized Metal view plans |
| [DSV4 proof generation](native-dsv4-proof-emission.md) | Form owns shader composition, complete GGUF metadata, typed requests, checked output settlement and proof archive retention |
| [RAM pipe workers](native-pipe-workers.md) | Form emits Darwin ARM64 pipe and readiness operations in RAM, supervises resident workers, preserves complete binary frames and correlated stderr health, and settles partial writes, EOF, cancellation and physical release |
| [Pinned Wiktionary sources](native-wiktionary-source.md) | Form reacquires the complete 111 retained revisions, binds page/revision/timestamp identities and reproduces their English hashes and selected meanings; actual acquisition and retained-response replay remain distinct |

The deterministic substantive build and its three downstream generators share
one `fkwu` process through `observe/concept-substantive-build-run.bml`. Form
publishes 21 checked artifacts from admitted evidence, including 111 stable-ID
repairs and 1,443 attributed language cells. Current generated metadata and all
10,000 rank/repair queries are executable witnesses. Fresh acquisition and
translation retain their separate JavaScript responsibilities until a native
acquisition receipt and proposed projection are admitted and observed.

Three resident Form translation workers prepare and decode retained corpus
bytes through RAM pipes. Typed request admission checks integer lexemes before
conversion; Form owns query encoding, complete HTTP framing, ordered response
segments, explicit row splitting and cell policy. The live provider attempt
returned HTTP 429 on all five requests and published no translation. The
[worker evidence](evidence/fkwu/native-pipe-translation.json) distinguishes
retained-data execution from fresh acquisition. Native pinned-source acquisition
reproduces the existing Wiktionary revision set; fresh selection, rights,
translation and new-generation publication remain separate work.

[Native source inspection](native-source-inventory.md) captures source lists into owned files and checks child exit status, exact reads and NUL termination. `observe/carrier-mass.bml` counts source bytes in Form, including unknown-extension shebangs and paths containing spaces or newlines. It retains the complete source reading at `.hearth/carrier-mass-current.json`; weighted exclusions remain present in that evidence. The native authoring guide retains its separate Python inventory at `.hearth/native-authoring-current.json`. Neither lexical inventory establishes execution or native ownership by itself.

The current guide identifies no retained Python implementation. Terminal
acceptance lives in `observe/glass-keyboard-pty-run.bml` and in the terminal
door's own read-back ([glass keyboard](glass-keyboard.md)).
The three large-model comparison callers use native Form references and
generation. Their [source-bound evidence](evidence/fkwu/dsv4-oracle.json)
records all 1,106 consumer checks passing at existing tolerances; the replaced
Python oracle is removed.
The [held-source re-observation](evidence/fkwu/dsv4-held-source-rewitness.json)
checks all 81 recorded source rows and rejects a changed final hash without
relabeling those numerical executions.
Native terminal acceptance executes on Darwin ARM64; another native target still
needs an observed admission and full behavior check. Shell orchestration,
JavaScript builders, target-language specimens, proof siblings and platform
carriers remain visible in the broader source reading. The checkout is not
entirely Form-owned.

The shared BML dependency reader distinguishes actual directives from quoted examples and multiline strings. Direct Form preparation selects semicolon-comment semantics explicitly. Form string emission represents semicolons through byte construction, preserving their value across the seed's dependency-reading boundary. BML-generated strings can carry dependency examples without importing them.

Semicolon-bearing string leaves require byte construction and balanced concatenation; ordinary string leaves remain literals. The balanced tree avoids copying each growing suffix but still performs construction work unless subsequently folded. Handwritten direct `.fk` source still passes through the seed's quote-insensitive dependency collector; the Form emitter repair applies to generated string leaves.

Form owns six-place numeric display in `core.fk`, including fractional carry,
large finite values, scientific notation and exact tagged integer extrema.
The [display evidence](evidence/fkwu/float-display.json) checks 34 individual
health observations and an actual failing child. Exact binary64 serialization
uses its separate precise-JSON owner. A large plain-decimal BML source literal
still refuses before execution; runtime string parsing does not establish that
source-emission capability.

## Ownership boundary

[Native care](native-core-care.md) defines receipt of organ-emitted unease at execution
boundaries and directs attention toward each declared resource need. Available
native providers supply resources; the asking organ re-observes delivery and
remaining needs. The process organ requests local diagnostic evidence while a
child runs, and session memory supplies verified learning evidence. The core
`care` view and Glass retain source readers and expose this current exchange,
including uncertainty and observation age. Applied attention does not invent recovery.
Eight alternating-order pairs over one retained failed-child exchange preserve
the complete care result except timing fields. Direct care projection mints
269 nodes versus 317 through the intermediate health view. Both retained
readers consume zero repeated event bytes. These are warm workload observations;
the reader still allocates changing observations, and no memory bound is claimed.
Discovery uses the existing bounded shared-memory roster and reports only its
advertised scope. This is not a complete census of all organs or crossings.

Healing uses local routes by default; the explicit remote mode opens external
admission after the existing local and checker gates. Attempt retention and
frontier returns bind real local experience and execution evidence. They do
not claim that a retained example has already improved the serving model.

Shared-field admission preserves a local list prefix and its already shared
tail, including nested lists and scalar values. The
[ownership witness](../observe/shared-field-ownership-witness.bml) uses fresh
identities so a previously interned value cannot mask a first-admission defect.
The [execution evidence](evidence/fkwu/shared-field-ownership.json) binds the
seed transport to its source and independent observations. The field and its
sharing implementation remain seed-owned; Form-owned contexts and sharing are
the destination.

Each frame job retains its capture, program, policy, output and actual fence. Timeout retains that identity. Cancellation discards an eventual result while resources remain owned. Failed or indeterminate submissions do not publish results. Releases are recorded after carrier confirmation.

Buffer reads, writes and frees settle the work that actually references that buffer. An unrelated owner's pending fence keeps its identity and native pipeline lease. Enqueue validates every binding before opening an encoder. Explicit `metal_sync` remains the operation for a whole-queue wait.

The adapter currently uses one process-wide queue and open encoder. Dependency identity follows the admitted buffer handle; separately mapped aliases of the same external storage are not unified into one dependency identity.

Identical Metal source can share compiled content while each FMJ1 admission has its own generation-qualified handle. Form retires versions through descriptor mode 3 and holds a version lease for every owned job until output release. Submitted work retains the native pipeline object. Final retirement removes the admission and removes compiled content after its last admission; physical work leases remain until completion. Refused destruction keeps enough state for retry. Pipeline handles use 31 slot bits and 31 generation bits; buffer bindings retain their separate 16/16-bit encoding. Generations stop before wrapping.

`form-runtime-context.bml` owns a function set, retained source, persistent run memory and independent CPU native admissions. Direct, indirect, recursive and lexical calls preserve the selected context's source. Closing blocks new entry, releases admissions and clears owned roots only after confirmed release; a refused release preserves the closing context. Other contexts continue to execute. CPU image slots grow with residents, and executable spans use their actual page-rounded size. Released admission identities cannot be resurrected.

Context close and frame-version retirement publish bounded lifetime observations through `organ-health.bml`. The owner retains the current reading, preceding refusal, correlated retry response and applied result. The shared process flow receives typed health rows with opaque owner identity and resource state; captured bytes remain outside that channel. A fresh observation after the actual retry determines whether the release succeeded.

Release-flow identity is retained on each owner and combines process birth with
the observed record-construction clock. It survives allocator compaction.
Independent owners remain separate current observations, and one owner's
correlated response cannot authorize another owner's retry.

The shared append transport exposes file extent through `sbt-append` and confirmed bytes for the current write through `sbt-append-count`. Health events, process diagnostics and generated-text streaming use the byte-count door; carrier refusal remains a refusal.

Metal micro-thoughts carry source-and-entry seals and independent admissions. `mj-thought-table` maps arbitrary string addresses to separate metadata records; `mj-thought-recall` reads their handles. Each record binds its owner and address. An explicit thought release retires its native handle; a later thought call can give that vacant address a new admission. A different program at the same live address refuses replacement. Scoped policy sets can coexist, expire, be re-witnessed and select different resident programs over observed outputs.

Form owner identity is cooperative. Seed value pools, record metadata, parser and instrumentation still have process-global backing. Protected address spaces, independently destroyable seed contexts, preemptive Form scheduling and complete portable module ABI coverage remain north-star requirements. Host-unmap failure handling is checked in source; the current hardware bands do not inject a host `munmap` failure.

Handle widths, device capabilities and observed allocation limits are explicit. Unavailable capabilities and refused zero-copy mappings produce visible refusal.

## Reproducible observations

`form-run ./fkwu observe/frame-kernel-witness-run.fk` runs 29 focused checks, records actual child status and compares exact result output. Typed organ-health rows travel through the shared process reader and are validated before separation from the result. The diagnostic deadline cell uses its final verdict line; complete output and correlated events stay in the printed process-evidence directory. Current rows live at [checks.json](evidence/fkwu/checks.json). This includes separate-process compiled-archive capture, required reuse, and missing/wrong/corrupt archive refusal.

[Health transport](evidence/fkwu/health-transport.json) records actual context and frame release observations, shared-reader acceptance, correlated control actions and final owner health. To re-observe it, send the runtime witness's printed process-evidence directory as one stdin line to `form-run ./fkwu observe/native-lifetime-health-transport-run.bml`. The same door checks repeated appends and a real write refusal without changing the source observations.

The [CPU benchmark](../observe/frame-cpu-benchmark-run.fk) and [GPU benchmark](../observe/frame-metal-benchmark-run.fk) expose admission and execution costs over synthetic captured bytes. Current [CPU](evidence/fkwu/cpu-benchmark.txt) and [Metal](evidence/fkwu/metal-benchmark.txt) samples retain the complete result rows and crossing counts. The fixture is 1,050,678 bytes; the CPU sample takes three native calls per fold, while the GPU sample includes upload, submission, wait, readback and cleanup.

[Identities](evidence/fkwu/identities.json) bind the sources and artifacts recorded
in that observation. `observe/fkwu-current-identities-run.bml` regenerates them
with Form's ARM64 SHA program. Each newer execution carries its own source
identities; a recorded snapshot is not evidence for subsequently changed files.
That retained snapshot's CLI answers `pong`, and its root and CLI linkage reads
show libSystem only. The current canonical generation has its own
[artifact evidence](evidence/fkwu/native-cli-artifact.json); that earlier linkage
reading remains bound to its recorded executable. An explicitly unavailable Metal adapter returns the snapshot's
absence verdict `31`.

The [current bounded Glass observation](evidence/fkwu/glass-current-observation.json)
records all 20 frames under 50 ms, with first/max 43 ms and warm maximum
13 ms while the sensors stood. Missing indexed metrics retain their requested
identity as unavailable rows; existing zero measurements remain measurements.
This is one bounded execution, not a general latency guarantee. Without a
standing hearth, the counsel's eleven unobserved lanes remain unobserved.

The independent Form recipe walker interprets record construction, reads,
mutation, presence, keys, blueprint and both record aliases through
`native-recipe-record.bml`. Its [comparison](evidence/fkwu/native-recipe-record.json)
preserves duplicate field order, aliases and explicit sharing between recipe
runs. The flattener's unchanged verdict is `131071`. Setters require string
keys and refuse another kind. Packet records retain their originating owner;
raw native access to the wrapper is outside this representation contract.

Each result applies to its recorded source and host. A zero-millisecond sample is below the clock's resolution; these fixture samples do not establish maximum hardware bandwidth. The [per-symbol census](evidence/fkwu/c-bootstrap-audit.json) is a lexical inventory with active-platform reconciliation, not a semantic proof of every function.

## Freestanding boundary

The i386 guest under [`os/hati-os`](../os/hati-os/README.md) supplies a BIOS boot chain, protected-mode entry, serial/VGA devices, PIT interrupts, preemptive task switching, a bitmap allocator and RAM filesystem. It uses an explicit i386 register-state and calling convention. Form-emitted guest behavior is exercised through the same boot path.

Form emits the guest string-equality and length leaves used by the shell and RAM filesystem. The retained serial witness and QEMU halt status `99` validate against the rebuilt kernel bytes with verdict `1023`. The guest builder lives at `form/scripts/build_hati_os.sh`; Form owns emission and image packing. Current guest tasks retain their allocated stacks for the boot lifetime.

Paging, separate address spaces, privilege transitions, persistent block I/O, networking, multicore startup and remaining device/power contracts require their own implementations and execution witnesses. Hosted execution does not establish freestanding ownership.

Resource ownership includes names, metadata and event delivery: a native handle is usable only while its Form owner can retain, observe and release it. The checks above make that contract executable.

## Next executable steps

1. Give changing observations an owned, reclaimable lifetime and decouple field
   allocation from its current fixed capacity. The native identity already
   occupies one word; its encoding width does not require preallocating its
   identity space. The raw-u64 accessor, leased blueprint runs and sparse
   identity directory are executable. The primary importer already consumes
   published handles without scanning unfinished reservations. The resident
   native arena now admits its image before data allocation, generates semantic
   words, grows chunks and freezes prefixes into the directory. Exact-word
   interning now has a native owner with stable rows across index growth. Next,
   connect kind-sensitive equality and tagged handles to native ownership, then
   move the primary producer/read path while retaining native side-table lifetimes.
   Keep admission independent of the allocator it replaces. Concurrent publication
   requires ordered immutable-generation selection and retained readers.
   Reference-bearing columns require a collector
   root/relocation bridge; slot reuse requires handle generations and side-table
   ownership. Measure allocations
   at actual organ boundaries, preserve the care path under pressure, and prove
   another live owner keeps its values during reclamation. The direct care
   projection is the executing first reduction in this path. The remaining
   seed boundary is `fk_field_fill` and `fk_field_intern_node`: allocation must
   report pressure and settle its claimed intern slot before a caller stops.
   Exercise this in an isolated field, including another live caller, before
   changing shared-field admission.
2. Replace synchronous whole-unit source admission with Form-owned module
   compilation on demand. Measure cold admission, cache reuse and replacement
   latency separately, preserving exact source identities and outstanding work.
3. Move native specialization policy and emitter construction from the seed to
   Form. Accept replacements over identical inputs, retain submitted versions
   until completion, and delete each C implementation after its callers move.
4. Carry these ownership and completion contracts into async scheduling and the
   freestanding memory/interrupt floor. Hosted capabilities do not establish a
   complete Form-native OS.
