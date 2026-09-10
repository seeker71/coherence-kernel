# fkwu runtime status

The [north star](fkwu-form-native-north-star.md) defines the complete Form-native runtime and OS. This page describes current execution. The [bootstrap inspection](fkwu-c-bootstrap-inspection.md) carries the function/global ownership census.

## Runtime and native computation

Form source and executable BML run through `fkwu`. BML lowers in memory; native artifacts are reusable caches. Source admission materializes a whole unit synchronously. The Form table compiler generates the bootstrap table and emitted CLI carrier, with source and generation identities checked by the canonical builder.

Metal is loaded dynamically through a selected adapter. The root and emitted CLI executable link only libSystem on this Darwin host. Form generates Metal source, identifies program/entry/target, and chooses RAM admission or an explicit compiled-archive mode. Shader source files and shader compiler subprocesses are unnecessary.

Form owns BMP validation, the nine-field integer interpretation, policy thresholds, GPU source generation, captures, submissions and result eligibility. Several versions can use one captured input. Its 144-byte ARM64 frame fold reads each pixel once through three synchronous borrowed-string calls for a normal frame. Larger rectangles subdivide according to the integer result capacity. The interpreter is an explicitly requested equivalence reference.

## Native authoring and inspection

Form owns the following executing surfaces. Each linked page names its public door and current boundary.

| Surface | Current execution |
| --- | --- |
| [Artifact codec and source lens](native-form-artifacts.md) | FORMBIN2 encoding, decoding, structural comparison, exact numeric payloads, adaptive-depth traversal and independent Go/Rust/TypeScript conformance |
| [GPU source generation](native-gpu-source-generation.md) | Canonical PTX and GLSL templates become executable BML authorities; generation and publication preserve checked bytes |
| [Metal asks](native-metal-ask.md) | Typed request admission, full model identity, tokenization, GPU work, owned CPU hashing, resource release and private atomic answer publication |
| [Numerical references](native-dsv4-numeric-reference.md) | Independent Form hyperconnection and toy-forward mathematics compared with production computations and held anchors |
| [Proof traces](native-kernel-trace.md) | Native preparation, supervision and rendering of real Rust specimen traces; Python grammar compiler execution remains a Rust proof responsibility |
| [Routing proofs](../form/form-kernel-rust/README.md) | Form owns socket fixtures, concurrent clients, exact response checks and child settlement around the independent Rust server |

[Native source inspection](native-source-inventory.md) captures source lists into owned files and checks child exit status, exact reads and NUL termination. `observe/carrier-mass.bml` counts source bytes in Form, including unknown-extension shebangs and paths containing spaces or newlines. It retains the complete source reading at `.hearth/carrier-mass-current.json`; weighted exclusions remain present in that evidence. The native authoring guide retains its separate Python inventory at `.hearth/native-authoring-current.json`. Neither lexical inventory establishes execution or native ownership by itself.

The current guide identifies two Python implementations: `dsv4-mla-core-oracle.py` supplies the active large-model numerical reference, and `test_glass_keyboard_pty.py` supplies real terminal acceptance checks. Their replacement still requires equivalent observed native behavior. Shell orchestration, target-language specimens, proof siblings and platform carriers also remain visible in the broader source reading. The checkout is not entirely Form-owned.

The shared BML dependency reader distinguishes actual directives from quoted examples and multiline strings. Direct Form preparation selects semicolon-comment semantics explicitly. Form string emission represents semicolons through byte construction, preserving their value across the seed's dependency-reading boundary. BML-generated strings can carry dependency examples without importing them.

Semicolon-bearing string leaves require byte construction and balanced concatenation; ordinary string leaves remain literals. The balanced tree avoids copying each growing suffix but still performs construction work unless subsequently folded. Handwritten direct `.fk` source still passes through the seed's quote-insensitive dependency collector; the Form emitter repair applies to generated string leaves.

## Ownership boundary

Each frame job retains its capture, program, policy, output and actual fence. Timeout retains that identity. Cancellation discards an eventual result while resources remain owned. Failed or indeterminate submissions do not publish results. Releases are recorded after carrier confirmation.

Buffer reads, writes and frees settle the work that actually references that buffer. An unrelated owner's pending fence keeps its identity and native pipeline lease. Enqueue validates every binding before opening an encoder. Explicit `metal_sync` remains the operation for a whole-queue wait.

The adapter currently uses one process-wide queue and open encoder. Dependency identity follows the admitted buffer handle; separately mapped aliases of the same external storage are not unified into one dependency identity.

Identical Metal source can share compiled content while each FMJ1 admission has its own generation-qualified handle. Form retires versions through descriptor mode 3 and holds a version lease for every owned job until output release. Submitted work retains the native pipeline object. Final retirement removes the admission and removes compiled content after its last admission; physical work leases remain until completion. Refused destruction keeps enough state for retry. Pipeline handles use 31 slot bits and 31 generation bits; buffer bindings retain their separate 16/16-bit encoding. Generations stop before wrapping.

`form-runtime-context.bml` owns a function set, retained source, persistent run memory and independent CPU native admissions. Direct, indirect, recursive and lexical calls preserve the selected context's source. Closing blocks new entry, releases admissions and clears owned roots only after confirmed release; a refused release preserves the closing context. Other contexts continue to execute. CPU image slots grow with residents, and executable spans use their actual page-rounded size. Released admission identities cannot be resurrected.

Context close and frame-version retirement publish bounded lifetime observations through `organ-health.bml`. The owner retains the current reading, preceding refusal, correlated retry response and applied result. The shared process flow receives typed health rows with opaque owner identity and resource state; captured bytes remain outside that channel. A fresh observation after the actual retry determines whether the release succeeded.

The shared append transport exposes file extent through `sbt-append` and confirmed bytes for the current write through `sbt-append-count`. Health events, process diagnostics and generated-text streaming use the byte-count door; carrier refusal remains a refusal.

Metal micro-thoughts carry source-and-entry seals and independent admissions. `mj-thought-table` maps arbitrary string addresses to separate metadata records; `mj-thought-recall` reads their handles. Each record binds its owner and address. An explicit thought release retires its native handle; a later thought call can give that vacant address a new admission. A different program at the same live address refuses replacement. Scoped policy sets can coexist, expire, be re-witnessed and select different resident programs over observed outputs.

Form owner identity is cooperative. Seed value pools, record metadata, parser and instrumentation still have process-global backing. Protected address spaces, independently destroyable seed contexts, preemptive Form scheduling and complete portable module ABI coverage remain north-star requirements. Host-unmap failure handling is checked in source; the current hardware bands do not inject a host `munmap` failure.

Handle widths, device capabilities and observed allocation limits are explicit. Unavailable capabilities and refused zero-copy mappings produce visible refusal.

## Reproducible observations

`form-run ./fkwu observe/frame-kernel-witness-run.fk` runs 29 focused checks, records actual child status and compares exact result output. Typed organ-health rows travel through the shared process reader and are validated before separation from the result. The diagnostic deadline cell uses its final verdict line; complete output and correlated events stay in the printed process-evidence directory. Current rows live at [checks.json](evidence/fkwu/checks.json). This includes separate-process compiled-archive capture, required reuse, and missing/wrong/corrupt archive refusal.

[Health transport](evidence/fkwu/health-transport.json) records actual context and frame release observations, shared-reader acceptance, correlated control actions and final owner health. To re-observe it, send the runtime witness's printed process-evidence directory as one stdin line to `form-run ./fkwu observe/native-lifetime-health-transport-run.bml`. The same door checks repeated appends and a real write refusal without changing the source observations.

The [CPU benchmark](../observe/frame-cpu-benchmark-run.fk) and [GPU benchmark](../observe/frame-metal-benchmark-run.fk) expose admission and execution costs over synthetic captured bytes. Current [CPU](evidence/fkwu/cpu-benchmark.txt) and [Metal](evidence/fkwu/metal-benchmark.txt) samples retain the complete result rows and crossing counts. The fixture is 1,050,678 bytes; the CPU sample takes three native calls per fold, while the GPU sample includes upload, submission, wait, readback and cleanup.

[Identities](evidence/fkwu/identities.json) bind current sources and artifacts. `observe/fkwu-current-identities-run.bml` regenerates them with Form's ARM64 SHA program. The canonical CLI build answers `pong`; root and CLI linkage reads show libSystem only. An explicitly unavailable Metal adapter returns the absence verdict `31`. The glass's observed first frame is 33 ms, with 33 kernel operations; without a standing hearth, the counsel's 11 unobserved lanes remain unobserved.

Each result applies to its recorded source and host. A zero-millisecond sample is below the clock's resolution; these fixture samples do not establish maximum hardware bandwidth. The [per-symbol census](evidence/fkwu/c-bootstrap-audit.json) is a lexical inventory with active-platform reconciliation, not a semantic proof of every function.

## Freestanding boundary

The i386 guest under [`os/hati-os`](../os/hati-os/README.md) supplies a BIOS boot chain, protected-mode entry, serial/VGA devices, PIT interrupts, preemptive task switching, a bitmap allocator and RAM filesystem. It uses an explicit i386 register-state and calling convention. Form-emitted guest behavior is exercised through the same boot path.

Form emits the guest string-equality and length leaves used by the shell and RAM filesystem. The retained serial witness and QEMU halt status `99` validate against the rebuilt kernel bytes with verdict `1023`. The guest builder lives at `form/scripts/build_hati_os.sh`; Form owns emission and image packing. Current guest tasks retain their allocated stacks for the boot lifetime.

Paging, separate address spaces, privilege transitions, persistent block I/O, networking, multicore startup and remaining device/power contracts require their own implementations and execution witnesses. Hosted execution does not establish freestanding ownership.

Resource ownership includes names, metadata and event delivery: a native handle is usable only while its Form owner can retain, observe and release it. The checks above make that contract executable.
