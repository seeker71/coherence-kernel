# Native JIT routing

Run Form source with `./fkwu path.fk` or `./fkwu path.bml` from the repository
root. Form owns lowering, emission, demand and reuse. Source is authoritative;
native images are local cache. The executable route lives in
[`native-jit-route.bml`](../form/form-stdlib/bml/native-jit-route.bml).

File presence establishes availability; executable witnesses establish behavior.

The [compiler contract](../grammars/bml-native-north-star.form) applies to every
cache: materialize on demand, adapt from measured reuse and cost, and keep the
complete lifecycle inside its owning organ. A source change invalidates affected
results; their replacements wait for demand. First demand computes genuinely
missing work. Subsequent cold and warm demands reuse that work, with only
storage delivery permitted to differ. Callers should not need a warm-up command,
separately prepared metadata or manual cache maintenance.

`./fkwu observe/native-jit-witness-run.fk` emits an ARM64 image in Form, executes
two inputs through the physical carrier, and checks malformed-image refusal.
`form/form-stdlib/tests/jit-heat-gate-band.fk` and
`observe/tests/jit-evaluator-heat-band.fk` check heat and choice;
`form/form-stdlib/tests/jit-lens-band.fk`, `jit-leaf-inram-band.fk` and
`rest-force-jit-band.fk` check the lens, the in-RAM leaf and forced rest.
`form/form-stdlib/form-cli-jit.bml` carries demand and CPU and native Metal
routes. Tensor programs run through `form/form-stdlib/native-tensor.bml`. Device
drivers admit Form-emitted programs.

fkwu is the interpreter and the JIT host. Physical MAP_JIT
and library loaders carry Form-emitted images. `register_jit` binds an existing
native alias; its witness is the alias registry sample. Compilation uses fkwu.

Observe native JIT demand and reuse from the resident Form panel in its own
process. API observation routes retain framebuffer events and route timings.
Each process reports its own measurements.

The transparent page emitter assembles argument words with a reversed
accumulator, preserving left-to-right evaluation and reversing once at the
boundary. Its existing `jit-page-band.bml` includes ordered argument effects.
The [optimization-book study](../receipts/2026-11-04-optimization-book.md)
retains exact-emission comparisons, measured wide-call compilation gains and
startup hypotheses. The compiler's
[cold/warm contract](../grammars/bml-native-north-star.form) permits only the
storage-delivery difference for unchanged meaning on the same target. Separate
first admission, disk-cache reuse, resident execution, JIT cost and memory to
locate violations of that contract.

The current `.fkb` stores an evaluator graph. Admission still rediscovers the
source closure and decodes tables; first `cell_run` admission parses source;
transparent native pages and pure constant-hold results remain process-local.
These are open costs. Persisted native specialization must carry relocatable
references and any added graph nodes: emitted words can contain current
function/node indices and literal-pool words, so saving raw pages is insufficient.
The existing program-image owner is the place to carry that state and its
resolved load plan. Fresh runtime handles and effects still belong to the
current execution.

The seed's `.fkb` identity now binds the ordered resolved paths, sizes and
content digests, together with the separate builder identity. Timestamps remain
provenance and no longer force recompilation. The v4 source identity replaces
v3 once; the binary container remains v7. This removes timestamp authority and
its temporary allocation from the seed. The native source compiler's retained
closure/load plan is the shrink path for the remaining seed-owned admission.
The separate `.dylib` route still uses timestamp freshness and needs a
content-bound artifact contract before that check can be removed.

Scalar demand JIT also has a self-containment gap: `bdj-source-unit-hash` reads
the compatibility wrapper's fixed `.fk.sym` path, although importing the owner
does not guarantee that artifact exists. That source identity also omits the
builder identity held separately by the program-image loader. Its first request evaluates the graph
and leaves specialization for the owner's progress step, even when a disk image
may already exist. The request/progress owner must admit current retained state
on demand and obtain its identity from the actual admitted program. Reading a
different guessed sidecar or hashing newly edited disk source would not establish
the identity of code already loaded. The current `kernel_ast` header exposes a
folded observation of the last noted image, not that complete execution identity.

Within the existing scalar cache owner, runtime identity is now resolved only
when a pending specialization or healing probe needs it. Idle, held faults and
publication care use their retained state. Disk admission checks the retained
runtime identity, epoch, exact program wire and image bytes regardless of
timestamp ordering; file ages remain telemetry. The native/Form installation
challenge remains in place.

For recovery, search local Form cells, RAG, git history and receipts. Make the
native attempt, retain behavior checks and per-stage streams, exit status and
timing, update callers, then re-observe. The shared native authoring guide links
this door so the next attempt can start from current local evidence.

Recursion, list, helper-call and IEEE/string/vector bands prove results. The
independent leaf witness proves Form emission and execution; each function's
machine-code entry requires its own observation.

The band sweep (`./fkwu gate/band-sweep-run.bml`) runs each band on fkwu and judges it by its
exit, its stderr and its pinned verdict; a failing run's legs directory retains its
stream and stderr.
