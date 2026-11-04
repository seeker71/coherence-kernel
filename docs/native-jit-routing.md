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
v3 once. Container v8 embeds compile errors, execution refusal and a complete
image checksum. Source, import and direct execution share that admission;
`.sym` is an optional inspection lens. Selected imports consume the exact
validated snapshot once, and covered dependencies need no separate image.
An invalid image emits its cause, is repaired at its owner and is re-admitted
before the correlated repair event reports health. The native source compiler's retained
closure/load plan is the shrink path for the remaining seed-owned admission.
Source startup uses the content-bound image and RAM JIT; it no longer probes
or selects a sibling `.dylib` by timestamp. Explicit `.dylib` execution remains
a direct native operation: failed loading or a missing ABI symbol ends that
operation with its diagnostic, and does not execute source or an image instead.

Scalar demand JIT reads its executing definition's immutable admission through
`kernel_ast(host_pid(), ["identity"])`. Its fingerprint binds the admitted
builder and source closure with the emitter ABI. It requires no `.sym` sibling
or fresh disk hash. A new eligible request selects a current disk image
immediately; ordinary absence or changed identity leaves one owned specialization
pending. Retained pages, held faults and publication care use their current
state. Missing admission identity retains the original request as a fault with
no alternate answer; a correlated identity-available event can resume its
verification. The source path supplies telemetry, while the offered graph and
immutable admission supply authority; an absent source file does not block reuse
or publication. Disk admission compares runtime identity, epoch, exact program wire and
image integrity; timestamps remain telemetry. The v5 scalar artifact retains a
SHA-256 sealed admission binding those fields to the completed native/Form
challenge's argument and equal result. Publication care carries the same sealed
record through retries.

A corrupt scalar cache produces an exact failure observation and no alternate
answer. The owner publishes that failure, repairs and rechecks the cache through
its cause-bound healing flow, publishes the matching healing event, and resumes
the retained request. Native challenge disagreement or carrier loss also retains
attention without interpreting a replacement answer. Publication acknowledgements
preserve later executable state and ordered observations. The existing
`bml-demand-jit-guard-band.fk` observes these paths, including actual failure and
healing publication and the resumed native answer.

Compilation performs the native/Form challenge once. A validated disk admission
and a resident request each invoke the native page once, without repeating Form
evaluation. This retains the evidence of one checked point; it does not prove
equivalence for every input. Unchecked legacy artifacts, incomplete admissions
and changed sealed bytes follow the same failure and healing flow. Disk integrity
verification and decimal image decoding still add computation after a restart.
SHA compression uses the existing Form-emitted native implementation; its image
and round constants are emitted on first compression and retained by that owner.
Their persistence across restarts remains open, alongside integrity verification
and decoding costs, before the storage-only contract is met.

Metal archive retention belongs to `form/form-stdlib/bml/metal-jit.bml`.
One `.metalbin` contains the native archive, Form's source/entry/target identity
and a seal of the archive bytes. Admission verifies both before loading the
embedded library through the public Metal API. On Apple M4 Max with macOS
26.3.1, a generated `f(n)=3n+7` compiled once on first demand; a new process
loaded its archive with zero MSL source compilations and returned `f(5)=22`.
Missing, corrupt or mismatched archives enter the existing organ-care flow,
which creates the directory, captures, seals and atomically publishes the
replacement, then re-observes strict admission. Failed retention remains a
failure with the original request and findings held. A correlated resource
response through `mj-cache-hear` resumes that request; unchanged demand does
not repeat failed care. This owner does not yet enroll a filesystem watcher.
Explicit archive-hit-only admission remains strict.

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
