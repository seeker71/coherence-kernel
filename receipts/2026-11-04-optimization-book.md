# Algorithms for Optimization applied to Form

The second-edition preview supplied an experimental discipline for the kernel,
JIT and startup work. Argument emission now scales linearly; validated scalar
and Metal images are reused on demand; failures retain their operation through
repair. Whole-program storage-only cold/warm behavior is not yet established.

## Current execution round

This round follows the user's instructions to execute, remove fallbacks and handle
failures in the nervous system. Earlier open findings below describe their original
observations; this section records their current state.

- Container v8 holds compile errors, refusal and a whole-image checksum. `.sym`
  is optional. Source, imports and direct execution share admission; selected
  imports consume one validated snapshot. Empty, truncated and corrupt images
  emit their cause. Source-owned repair closes the correlated failure only after
  publication is re-admitted; direct invalid images return no substitute answer.
- Scalar JIT reads immutable admitted builder/closure identity through `kernel_ast`.
  Missing mandatory identity retains a fault and no answer. A source pathname is
  telemetry, no longer another authority or a required file. The v5 artifact seals
  the completed native/Form point challenge. Disk hits invoke native once with
  zero Form evaluations. Care retains the original request, completed challenge,
  completed seal and original answer through recovery. The guard observes failure
  publication, healing publication, retained answer 7 and a distinct native input
  returning 8. The admission proves its checked point, not all inputs.
- Metal archives hold the compiled library, program identity and integrity seal.
  Generated `f(n)=3n+7` returns 22 for input 5 across processes with source-compile
  counts 1 then 0. Invalid archives are refused and repaired. Blocked publication
  remains pending with no answer; a correlated resource response resumes the same
  operation. Directory creation and atomic publication belong to the existing
  owner. Blocked destinations no longer crash Metal serialization.
- The timestamp-based implicit sibling `.dylib` route is removed. Explicit native
  load failure remains a failure and releases a rejected ABI handle. CLI v3 binds
  binary, image and source identity without a symbols sidecar. Complete private
  CLI builds passed.
- Cell reuse retains dependency diagnostics and refusal through new and resident
  intermediate roots. Units collected beside malformed source but never parsed
  remain unadmitted until demand. The existing cell check observes source repair
  restoring the original operation.
- The memory failure reproduced on HEAD: GC sized private headroom from roughly
  9.7 million shared nodes that its loops never scan. Headroom now follows the
  actually scanned private pool. The unchanged 500-call assertion passes: growth
  fell from about 12.5 MB to 0.25–0.30 MB; heap capacity fell from 4,194,304 to
  65,536 pairs.

Native SHA compression and demand-owned image/constants reduce admission parsing
after resource admission from 430 to 125 microseconds in the isolated probe
(126,171 to 23,938 dispatches). The complete restarted request remains about
4.5 ms, including first-use resource emission: this is **not** a restart speedup
claim. First cell parsing, closure discovery, image decoding, immutable pure
initialization and transparent native-page persistence still violate the complete
storage-only target. Their owning boundaries are the program-image loader and
native compiler; a separate SHA cache would duplicate authority.

Observed existing checks: demand JIT 511; SHA/model seal 2047; cell run 16777215;
GC cost 15; closure reclamation 1023; import carry 255; CLI build 262143; source
grammar bridge 32767; runtime image model 16777215; tensor lifecycle 1023; Metal
door 15; canonical conformance exit 0. Exact commands, original failures, fixtures
and re-observations remain in the local research directory, especially
`jit-final-event-result-guard.txt`, `metal-archive-final-generated-restart.txt`,
`empty-cache-repair.txt`, `admission-*-before.txt`, `admission-*-after.txt`,
`cell-memory-baseline.txt`, `cell-run-all-admissions-final.txt` and
`final-cli-build-band-repaired.log`. The remaining seed image transport/relocation
mechanics shrink toward Form's source compiler and program-image owner.

## Resident source-content repair

The next actual failure was resident reuse after a same-size source edit whose
old timestamp was restored. A binary built from published commit `e850e18b4`
returned 111 after both Form and BML source changed to 222. The strengthened
existing cell check returned 16769023, missing its source-freshness bit. The
repaired runtime returns 222 with residency 0, then 222 with residency 1 on the
unchanged next call; the complete check returns 16777215.

Each demand now reads source content. One demand shares successful complete
readings by exact path across its raw dependencies and BML observation packets.
A later demand or filesystem mutation invalidates that reuse. BML still checks
its owner, working directory, home, every source and every recorded absence;
read failures and absence are not memoized. A failed read or close cannot become
a partial successful reading, and an inaccessible path cannot certify absence.
The small seed transport shrinks toward the native compiler's observation map.

This repairs correctness; it is not a speedup against the invalid shortcut.
An uncontended sequential pair used 8.40 CPU seconds for the published binary
and 11.46 for the repaired runtime, including the additional source repair that
the old runtime wrongly skipped. Earlier comparisons through the mutable
`./fkwu` path are not a reliable before/after baseline: it had been renewed.
Sharing readings within a demand avoids the naive repeated-read implementation,
which had used 30.59 CPU seconds in its earlier cell-check run. These are single
runs, not a statistical performance claim. Exact source, commands and output
remain in `resident-content.fk`, `resident-observation-e850-band.txt`,
`resident-observation-fixed-timed.txt` and `resident-content-cell-band.txt` in
the local research directory.

Private immutable-result experiments also establish that binary string bytes
`[65,0,255]` survive ordinary program-image serialization and separate-process
reuse without re-running their producers. Effectful operations are refused by
the existing restricted program-image walker, including when an effect branch
is selected. An unselected effect branch does not run. This is evidence for the
next Form-owned implementation, not a persistent SHA initialization cache or a
whole-program cold/warm result.

## Demand-owned immutable image results

The existing restricted program-image walker now reads the actual admitted
definitions, evaluates only selected pure operations and retains the definitions
it reached. It supports ordered multiargument calls and explicit caller fuel;
`nothing()` selects uncapped work. Effects, ambient reads, unknown operations,
captures and writes to occupied local slots are refused. This is a selected-path
evaluation, not a general termination or purity theorem. The first production
publication policy accepts zero-argument immutable strings.

The existing byte codec writes those results into the owning ordinary v8 image.
It preserves source identity, diagnostics, function roots, aliases, constant and
unit ownership, and untouched metadata. A grant binds the completed value and
reached definitions to the exact native image descriptor; a matching name or
guessed sibling is insufficient. Multiple requested initializers compose into
one publication. No process handles or unrelated runtime state are serialized.

The C seed exports immutable admission coordinates and carries the filesystem
operation. Form owns evaluation, image transformation, events and continuation.
The source writer binds its descriptor to its own emitted length and checksum,
so a competing writer cannot attach a different snapshot to the executing graph.
An imported graph whose constant holds were rewired has no affine image grant.
These admission mechanics shrink toward the native program loader; the guarded
rename remains a host leaf, not a second compiler or cache policy.

Conditional publication compares exact destination bytes and renames under an
anchored directory lock. The C image writer shares that boundary; arbitrary
direct writes and ordinary renames do not. Competing 16 MiB publications produced
one success and one `destination-changed`. A FIFO destination originally blocked
the reader; it now returns `destination-not-regular` without blocking. Known-name
renames still work in search-only directories. Every descriptor close contributes
to release status; an unconfirmed release cannot authorize another attempt.

Publication failure retains the completed proof, candidate, original bytes and
effects in a private closure. Mutable public fields cannot change that authority.
Correlated care resumes the same operation and never repeats a completed rename.
Observed tampering of candidate bytes, owner, completed flags or values was
refused; absent and noncallable continuations were refused too. Successful care
links the prior event and reports health 1 with surprise 0. Local evidence includes
`image-patch-resume-v3.txt`, `image-patch-cases-v4.txt`,
`pure-image-continuation-unrestricted-v3.txt`, `guarded-race-a.txt`,
`guarded-race-b.txt`, `guarded-fifo-after.txt` and
`guarded-publication-host-fs-final-v3.txt` under the research directory.

Automatic SHA use remains **unactivated**. The private candidate produced the
correct SHA-256 result across separate processes: one same-binary isolated pair
measured 1,444 CPU microseconds for the prior initializer and 83 for the retained
literal. First restricted evaluation and publication cost about 2.47 seconds.
These single observations establish neither a statistical speedup nor whole
startup performance. Its blocked-storage care witness retained both proof and
scalar challenge, consumed one resource event and resumed native answer 7.

The candidate exposed a real raw-cell gap: no owning disk image exists there,
and the hash consequently returned `nothing` with `image-owner-unavailable`.
It is preserved as `sha-initializer-activation.patch`; default SHA remains the
verified existing implementation, with no alternate branch added. Raw-cell image
admission still needs exact per-source symbol ownership, per-unit sequences and
preservation of completed library effects. A standalone compilation or whole
process snapshot cannot supply those by inference. Later separate publication
requests against an already changed image also require verified publication
lineage; the current owner refuses stale descriptors. The full storage-only
cold/warm objective remains open at these boundaries and at loader reconstruction.

The existing image capability check returned 16777215, including all 14 care
and 8 publisher assertions; its separate raw-cell call also returned 16777215
with zero errors and explicit owner-unavailable evidence. Release-failure cases
model carrier observations; they do not claim an induced OS close failure.
The final bootstrap/freshness, filesystem, cell, model-seal and demand-JIT checks
returned 42/31, 32767, 16777215, 2047 and 511. The failed in-place binary install
was killed at launch while the private candidate ran; atomic replacement restored
execution. The first model-seal command named the wrong `.fk` extension; its
retained missing-source event led to the actual `.bml` check above.

## Source and scope

Mykel J. Kochenderfer and Tim A. Wheeler, *Algorithms for Optimization*, second
edition preview, revision 2026-09-05, 634 PDF pages, 18,967,454 bytes.
The [authors' download page](https://algorithmsbook.com/optimization/) identifies
the preview and January 5, 2027 publication. Its
[PDF](https://algorithmsbook.com/optimization/files/optimization.pdf) redirects
to Google Drive file `1Rx7MAekKZbyNVc5B2MNfR0HXXIEJCksp`.

The original PDF and extracted reading are local inputs under
`.hearth/research/algorithms-for-optimization/optimization-2e.pdf` and
`optimization-2e.txt`. The PDF remains unchanged; it carries CC-BY-NC-ND 4.0.
Relevant sections were studied, not every page. Arriving Codex supplied the
analysis and patch; native Form supplied emission, execution and observations.
The hearth enquiry returned `signal=nothing reason=no-standing-hearth`.

## What the book changes in our approach

Page references below use printed book pages. The kernel applications are our
inferences, not claims that the book discusses this implementation.

| Reading | Application |
| --- | --- |
| Section 1.2, pp. 4–5: optimization process | Name the objective, legal changes, constraints and baseline before experimenting. Distinguish startup, first useful answer, compilation, warm throughput and memory. |
| Sections 7.3–7.4, pp. 116–119: direct search | Evaluate small legal neighborhoods of inline settings or batch sizes. Timing is discontinuous and noisy; a gradient is not assumed. |
| Sections 15.1–15.3, pp. 325–333: multiple objectives | Retain latency/memory/throughput trade-offs. Correctness and source freshness are requirements, not soft penalties that a fast wrong result can offset. |
| Sections 16.1, 16.3–16.5, pp. 349–356: sampling | Enumerate a small configuration space; use stratified or space-filling plans for larger spaces. Include boundary configurations and interactions. |
| Sections 17.4–17.6, pp. 372–384: noise, model selection, multifidelity | Repeat observations, hold out workloads, and use cheap isolated experiments to screen candidates before expensive end-to-end measurements. |
| Sections 18.5 and 19.3–19.5, pp. 399–400, 413–416 | If experiment cost warrants a surrogate, model uncertainty and use it to select another real measurement. Predicted improvement is not acceptance evidence. |
| Section 22.5, pp. 474–477: dynamic programming | Reuse overlapping immutable compilation subproblems within the owning compilation. Preserve identity and invalidation. |
| Appendix C.1, pp. 571–572: asymptotic notation | Check scaling with input size. A constant-size example cannot establish removal of repeated prefix copying. |

Chapter 8's noisy descent adds deliberate search noise; it is not the benchmark
noise model. Smooth optimization guarantees do not transfer automatically to
integer compiler settings and contention-sensitive timings.

## Implemented JIT change

Owner: `form/form-stdlib/bml/jit-page.bml`, `jp-args` / `jp-args-at`.
All four production callers retain the same six-argument entry point.
Previously each argument appended its emitted words to the entire accumulated
prefix. Core's `append` traverses its first argument twice. The replacement
emits children left to right, prepends their reversed words and store instruction,
then reverses once at completion. Empty input returns the original accumulator.
There is no C-seed change and no new runtime or band.

The emission contract remains `[words, slots-used, native-node-count]`. A prefix
`A`, child words `R`, and store word `S` become
`S ++ reverse(R) ++ reverse(A)` internally and `A ++ R ++ S` on return.
Slot high-water marks, native counts, ABI, frame ownership and helper calls stay
unchanged. The old production helper implementation is removed.

Native experiment:
`form-run ./fkwu .hearth/research/algorithms-for-optimization/jit-args-experiment.bml`.
It retains the old algorithm only as a local comparison specimen, calls the
actual new entry, and compares the complete emission at arities
0, 1, 2, 4, 16, 64 and 256. The equality case includes a nonempty prefix,
nonzero initial argument index and slot offset. All emissions match.

Timing uses 80 emissions per sample, five alternating old/new rounds, with the
same AST for `x + 7`. `host_monotonic_ms` measures emission inside the process,
excluding process startup and source admission. The timings include warm-up;
small cases have visible outliers and insufficient resolution for a gain claim.

| Arguments | Old milliseconds, five samples | New milliseconds, five samples | Median old → new |
| --- | --- | --- | --- |
| 16 | 5, 5, 4, 4, 5 | 5, 3, 4, 3, 3 | 5 → 3 |
| 64 | 26, 23, 22, 23, 23 | 13, 14, 14, 14, 14 | 23 → 14 |
| 256 | 192, 189, 190, 188, 189 | 55, 56, 57, 55, 54 | 189 → 55 |

This demonstrates reduced compilation work for wide synthetic calls. It does
not establish a warm execution or whole-program startup gain. The generated
instruction words are identical. Raw observations are retained locally in
`jit-args-results.txt` beside the experiment.

The existing `form/form-stdlib/tests/jit-page-band.bml` now also exercises an
eight-argument call with separate position and ordered-effect expectations:
`[87654321, 12345678]`. The order function must have a bound native page.
The extended family still returns **63**, with no differing cold/hot answers,
matching tail loops, divide-by-zero refusal and all six requested pages bound.
Independent source review found no blocking issue.

## Startup observations and next experiments

The native `startup-experiment.bml` in the same local research directory uses
`cell_run`, retaining result, residency, elapsed milliseconds and CPU microseconds.
This measures cell admission plus execution, not operating-system process launch.

For `observe/form-cli-enrich-run.bml` with `optimization` on stdin, one initial
admission was 495 ms / 490,314 CPU us. A later fresh process using the now-warm
disk caches admitted it in 30 ms / 30,341 CPU us. The five resident calls in
that later process used 350, 874, 331, 312 and 320 CPU us. The second run's raw
rows are in `startup-results.txt`. The initial run's exact rows remain in this
session's execution transcript. This contrast motivates distinct cache-state
experiments; it is not an A/B result for the JIT patch.

The next useful choices are:

1. **Startup dependency work.** `form-source-compile-packet` in
   `source-compiler.fk` rebuilds the compiler closure for each packet while
   `bml-floor-compile.fk` already owns an exact-byte compiler pin. Measure reuse
   tied to that pin, home bindings and absent dependency candidates. Never
   replace exact source checks with mtimes. `fsc-bml-reading-counts` counts
   parsed-fact reuse, not saved physical reads: `fsc-bml-read-source` still reads.
2. **JIT amortization.** Measure compiler cost against expected remaining call
   savings before changing heat or inline choices. Our break-even model is
   `remaining_calls * (walk_cost - native_cost) > compilation_cost`.
   Current transparent thresholds are `FK_JIT_HEAT=4096` and
   `FK_JIT_TIME_SAMPLES=32`; the older `jit-decision.fk` registry is a different
   route. Inline depth 2 and the 768-word limit belong to `jit-page.bml`.
3. **Admission latency versus memory.** The seed batches up to 16 cold lowerings.
   Compare legal batch sizes on equivalent isolated inputs, retaining completed
   memos and diagnostics. The historical memory failures in
   `docs/in-process-cells.md` are leads, not current measurements.

Existing source already lowers BML in-process. Do not propose that completed
migration again. Repeated warm memo hashing remains seed-owned; its replacement
should move admission meaning into Form rather than add another global C cache.
These three changes remain hypotheses, not implemented speedups.

## Local care and retained limitations

The temporary experiment initially failed on `reps-1` being read as a name;
the exact diagnostic was `[unbound-name] 'reps-1' in value position matched no
binding/const/fn`. Spacing the subtraction fixed that input. `jp-fn` requires a
numeric function index, not a callable value; the experiment now resolves the
definition's row. Reading the AST header under the sandbox returned absence,
and the dependent comparison stopped with `only numbers have an order -- in
obe-find`. The same experiment completed with approved shared-memory access.
The startup probe's untyped equality lacked `bml-value-equal`; declaring its
numeric comparison `(Comparable)` resolved the binding. These were experiment
construction/access findings, not kernel correctness failures.

The activity publisher initially retained its row but could not publish the
shared-memory dashboard (`glass-publish-v1, failed`). Re-execution with approved
access published it. The PDF network download likewise needed the authorized
network boundary. Original command outputs remain in the session transcript.

## Cold/warm contract and timestamp repair

Urs clarified the target: cold and warm times may differ at most by storage
bandwidth. For unchanged meaning and target, repeated dependency discovery,
parsing, lowering, pure initialization and JIT specialization are unfinished
work against that contract. The compiler north star now carries it explicitly.
The separated startup observations above diagnose this gap; their differences
are not an accepted steady-state design.

Source inspection found that ordinary `.fkb` images retain the evaluator graph,
but admission recollects the source closure and decodes/rebuilds its tables.
First `cell_run` admission still parses source. Constant-hold memo values and
transparent JIT pages do not survive process exit. A persisted native image
must retain relocatable function/node/literal references and any mirror nodes
the emitter creates. Copying current machine words alone would be incorrect.
The existing program-image owner and native compiler's retained closure/load
plan are the owners of this remaining work.

One false invalidation is repaired now. `fk_src_unit_hash_range` formerly
included every dependency's mtime; both image readers also compared a stored
mtime, and root/import admission checked timestamp ordering. The v4 closure
identity keeps ordered canonical paths, sizes and content digests while
removing timestamp authority. Builder identity, source path, seal, diagnostic
record and content checks remain. The v7 binary format still consumes the
timestamp as provenance. Old v3 identities rebuild once. The dependency
planner no longer allocates a timestamp array; the C seed shrinks by 15 lines.
The scalar demand-JIT identity consumer moves to v4 with its existing guard.

The experiment used `.hearth/research/algorithms-for-optimization/identity-*.fk`
and `identity-bml.bml`, with a separate candidate built directly from the seed.
Fresh processes reported `[answer, kernel_stat(15), kernel_stat(16),
kernel_stat(18)]`: lane 1 is import compilation, lane 2 is cached-image replay.

| Observation | Result |
| --- | --- |
| Original binary: repeat unchanged source | `[42, 2, 0, 0]` |
| Original binary: `touch -t 202701010101 identity-dep.fk` | `stale .fkb ignored`, then `[42, 1, 1, 0]` |
| Candidate: touch root and dependency to `202701020101` | `[42, 2, 0, 0]`; root and dependency `.fkb` files byte-identical by `cmp` |
| Change only caller text; dependency remains future-dated | `[42, 1, 1, 0]`; dependency image still byte-identical |
| Change dependency `42` to `43`, same size and restored timestamp | `[43, 1, 1, 0]`, then `[43, 2, 0, 0]` |
| Touch unchanged BML source | `[42, 2]`; its `.fkb` remains byte-identical |
| Change BML `42` to `43`, same size and restored timestamp | `[43, 0]`; changed content rebuilds |
| Unbound value, then touch unchanged refused source | Exit 1 both times; cached execution retains the refusal |
| Feed actual generated v4 `.sym` identity to demand-JIT consumer | Nonempty identity, readiness 1 |

The transparent JIT band returns 63; the extended demand-JIT guard returns 511.
Bootstrap witnesses return 31, 42, 55, `[1, 2.5, [3, 4]]` and 11111.
Canonical conformance passes all 13 expressions, pinned FORMBIN2 roundtrips and
12 malformed-artifact refusals. The demand guard first stopped under the
sandbox with `fkwu: form_error: native owner process birth unavailable`; its
approved retry completed with 511. The separate outputs are retained as
`identity-demand-guard.txt` and `identity-demand-guard-allowed.txt` in the local
research directory. JIT and conformance outputs are retained there too.

This establishes removal of unnecessary recompilation after timestamp-only
changes. It does not establish the full storage-only cold/warm bound or a new
wall-time speedup. Dependency discovery, graph reconstruction, restart JIT
persistence and the separate timestamp-gated `.dylib` route remain open.

## On-demand, adaptive, self-contained caching

Urs then specified: "All catching shall be on demand, smart and fully self
contained." In this caching discussion, "catching" is read as "caching". The
compiler contract now applies this to all caches. Actual demand creates only
missing reachable work. Reuse benefit, transfer cost and resource pressure guide
retention and specialization. Each owning organ carries its validity evidence
and payload and handles publication, invalidation, recovery and release. Shared
unchanged dependencies reuse their retained results. Callers need no separate
warm-up, metadata preparation or cache maintenance.

This corrects the earlier contract's instruction to create executable state when
meaning changes: a change invalidates affected results; demand creates their
replacements. First-ever computation remains necessary when no result exists.
The storage-only cold/warm requirement applies to reuse of work already done.

Read-only source review identified another concrete open boundary. The scalar
demand-JIT authority obtains its default identity from the fixed compatibility
wrapper `.fk.sym`, which ordinary imports do not guarantee. Explicit-identity
guard tests do not establish this default path's self-containment. Its first
request also ignores disk images until the owner progresses pending work.
`kernel_ast` currently exposes only the last noted image's folded observation,
not a complete admitted-program/compiler/target identity. The default scalar
fingerprint also omits the builder identity that `.fkb` admission holds separately.
Replacing the fixed
path with another sidecar, a process ID or a hash of possibly changed source
would not repair that ownership contract. This finding stays with the existing
demand-JIT and program-image owners.

Two smaller violations are repaired in Form. The shared JIT step now asks an
identity resolver only on specialization/probe branches; idle, held faults and
publication care retain their owned state. Production owner progress uses that
same path. The disk-cache inspector no longer rejects an otherwise valid image
because its source mtime is newer. Its existing runtime identity, epoch, exact
program wire and image-fold checks decide admission, followed by the existing
native/Form point challenge. Source presence and publication ownership remain.

The extended existing guard returns 511. Its pending path invokes the resolver;
idle and publication-care paths use a resolver that raises if called and finish
without calling it. A separate native experiment,
`.hearth/research/algorithms-for-optimization/on-demand-cache.bml`, writes a
400-byte scalar image under explicit fixture identities. After
`form-run touch -t 202701030101 .hearth/research/algorithms-for-optimization/on-demand-source.txt`,
a fresh process reports:

```text
[explicit-fixture-identities, cache-older-than-source, 1,
 status, disk-installed, materialization, disk-image, answer, 7, route, native,
 refusals, [stale-epoch, stale-program, cache-runtime, cache-image-fold]]
1
```

These observations establish avoided identity lookup and unnecessary scalar
recompilation, not complete cache self-containment or a whole-startup speedup.
The default admitted-identity seam, first-demand disk selection and persisted
native validation evidence remain open. The guard needed approved process-birth
access; its original refusal and successful output are retained separately as
`on-demand-guard.txt` and `on-demand-guard-allowed.txt`.
Independent source review found no blocking issue. Step total/unattributed time
now includes identity resolution, which previously occurred before its timer;
compare future timings with that attribution change in view. The synthetic
`source-artifact-cache.fk` policy model still uses timestamp freshness and is
another caller contract to migrate alongside its descriptor chain; it does not
implement actual program-image admission.

## Raw-cell source ownership, 2026-11-04

Deleting a constant already changes the source hash. The observed defect was
name lookup retaining the removed declaration from an older load of that path.
Lookup now admits declarations from the current dependency only. A refreshed
declaration gets its own row; resetting the current root's holds leaves older
callers' holds intact. The existing cell check observes old mutable value 42,
new value 100, an unbound-name failure after deletion, then restored value 1
with the old value still 42. Its complete verdict is 16777215.

The existing image capability check also returns 16777215. It now exercises
inert projection of real captured functions and library state, including
restored identical source with distinct retained values. Four actual AST layouts
were corrected: empty indirect/method argument chains retain -1; packed record
and print nodes traverse only their one child. Independent native execution
returns 7 and 11 and projection reads no unrelated node zero.

Projection is verified; raw-cell disk reconstruction and the storage-only
cold/warm target remain open. No provenance tree or additional image format was
introduced. The next loader change must follow an actual failing reload and
reuse the existing unit, function and constant tables.

## Native image reconstruction, 2026-11-04

Actual reloads exposed three missing contracts. A hidden function lost its
arity and capture metadata in the image; a same-named constant in another unit
replaced the retained unit's binding; and a saved function ceased to equal its
export after another import created a duplicate function. Native FKB version 9
carries the existing function metadata and export owner. Import selects existing
constant and function rows by their unit, and matches repeated export names in
declaration order. No new provenance or invalidation layer is introduced.

The metadata check also exposed a prescan row for a definition inside a lexical
`do`. Prescan now follows the parser's distinction between top-level definitions
and lexical values. The existing image capability band covers this case and
returns 16777215. The existing import-carry band returns 255, including constants
`[11,22,11]`, saved-function equality `[1,1]`, and duplicate declarations with
results 3 and 7 in observed import mode 1 followed by cached mode 2. The full cell
band returns 16777215. Exact commands and output remain under
`.hearth/research/algorithms-for-optimization/`.

An earlier cell run returned 16744447 because RSS rose 3.2 MiB in its resident
window. A later instrumented window showed a 3.5 MiB step despite unchanged
endpoint table capacities. The final band window grew 311,296 bytes; that pass
does not explain the earlier step. An observer-only probe accounts for its own
802,816-byte growth by pair-heap capacity plus one page. GC allocation/residency
is still under observation; the threshold has not changed.

Raw cells publish the admitted closure through Form into `path.cell.fkb`.
The next process reconstructs the existing unit, function, constant and source
map tables from that image. The duplicate single-unit loading path is removed.
Fresh-process and retained-dependency reloads preserve mutable library holds,
function equality, deleted-constant refusal, and once-only library effects with
zero source compilation. Source-admitted image owners carry detached relocation
maps; disk-admitted owners use the existing offsets. A blocked publication keeps
its exact candidate and continuation, executes no user effects, and resumes
through the existing care response. All seven failure/resume checks pass.

Corrupt checksums and late malformed payloads emit failure and successful repair
events. Rejected imports restore the existing table boundaries before source
repair; they no longer poison subsequent resident calls. Native diagnostics
remain returned values, including repair events. Compiler-work counters include
the publisher's work. The existing cell band now observes fresh process reuse,
corruption and publication care and returns 16777215. Import-carry returns 255,
the demand guard 511, and image capability 16777215.

The byte encoder now uses the existing balanced string join: the larger cell
check fell from an interrupted run near 90 GB RSS to a completed run near
415 MB. Released JIT nodes use their existing slot column as a touched-node
chain; resident dispatch fell from about 2.5 ms to 32–50 microseconds. A private
timing candidate attributed about 950 microseconds of a 1.3 ms disk load to
seven repeated canonical-path resolutions. Capturing that observation in the
existing dependency table reduced the small cached fresh-process observation
to 671 microseconds, versus 42 resident; another root retaining its library took
285 microseconds. These seed fields and append rollback are mechanical loader
state, to leave with source admission moving into Form. They add no independent
cache authority. Exact commands and traces remain under the private research
directory in the `perf/raw-cell-disk-reconstruction` checkout.

On-demand SHA initialization now uses the admitted raw-cell owner. The source
case proves its two immutable producers and atomically publishes their literal
bytes; readback and publication release both verify. A later process loads
literal tags `[24,24]`, performs zero proofs and zero source compilation, and
does not load the proof helper. Both calls produce the correct SHA-256 of `abc`
using the native path. The measured hash operation is about 81 microseconds on
first disk demand and 36 resident; the first uncached proof took about 2.57
seconds. Observer scans are separate from that operation. Whole-program
storage-only cold/warm behavior remains unestablished; measured admission and
first native resource setup still differ.

A further loader pass reads size and modification time from the already-open
descriptor, checks its release, and removes the separate image-existence stat.
Image admission reuses the existing symbol-to-dependency table, skips an
unchanged function-reference map, and leaves source balance checking in source
compilation. The retained-context hash keeps the same bytes when its binding
scan is empty. Cell and image capability checks return 16777215; import-carry
returns 255 and binary freshness 31.

The 2,048-function observation (65,410 source bytes, 687,946 image bytes) returns
42 with zero source work on every disk reuse. Three fresh-process CPU readings
were 2320/2417/2309 microseconds before and 2385/2267/2247 after, with resident
calls at 87–96 microseconds. The small median change is not a reliable speedup
claim. A private phase observation attributes 727 microseconds to collection,
1 to identity, 788 to image reading/validation, 587 to import and 88 to catalog
capture. Source collection and image validation still perform CPU work; they
are not storage bandwidth.

A faster descriptor-path proposal was rejected after native hardlink probes.
Opening alias B changed `F_GETPATH` on an already-open A; removing B left one
link yet made that query fail while `realpath(A)` succeeded. Calling `realpath`
first also changed the result, invalidating that ordering as a shadow test.
Pathname ownership therefore keeps its existing resolver. This pass introduces
no alternate loader, format or cache. Its descriptor reads and table operations
remain mechanical seed transport to leave with the native Form loader. Exact
failures, alias lifecycle observations and timing samples remain in the private
research and image-review directories of the isolated checkout.

The next measured change keeps the FNV-1a checksum exactly: eight zero bytes
contribute multiplication by the eighth power of its existing prime modulo
2^64. Other bytes retain the ordered recurrence. Form's independent two-limb
fingerprint agrees with the loader's full 64-bit source digest in 49 cases,
including block boundaries, every byte value and long zero runs. Bulk copies
also replace scalar source/symbol loops and a duplicate newline scan, removing
nine seed lines overall; no further timing gain is attributed to those copies.
The final 2,048-function disk readings are 1982/2015/2025 microseconds versus
2320/2417/2309 before these measured rounds, about 13% lower median in this
fixture. Resident readings are 88/89/89 microseconds. Cell and image capability
still return 16777215 and import-carry 255. SHA disk demand loads literal tags,
does zero proofs and source work, and produces the expected digest; its hash
operation takes 80 microseconds on first demand and 35 resident. Storage-only
parity is still not established.

A fresh process reloading that same 2,048-function image returned 42 with zero
source compilation. Two disk readings were 2045 and 2382 microseconds; the
resident calls were 86 and 111. One clocked pass split the disk reload into a
source read of 5 microseconds, canonical identity 249, prelude and home scan
288, source append 153, image open and checksum 643, table import 645, and
catalog capture 152. The file bytes are not the cost. A home-name hash and an
unrolled dense checksum kept the same identity and the same reload, so both
left with the measurement. What remains is the path resolution, the source
scans identity still requires, the dense checksum chain, and the table import.

The subsequent shared-field probe observed all 110 GC generations across three
500-call windows. Heap capacity and growth count stayed fixed; RSS reads caused
no collection. RSS grew 480 KiB, 240 KiB and 128 KiB. The earlier large step was
not reproduced, so neither GC nor its threshold was changed.

Failure observation itself had one concrete defect: `form-run` counted stderr
bytes but hid them because its stdin-preserving `exec` also permanently redirected
stderr to `/dev/null`. Removing that redirection preserves the channel. A command
writing `failure-event` to stderr and exiting 7 previously displayed zero stderr
bytes while claiming 13 shown; afterward its stderr is exactly those 13 bytes and
its exit remains 7. Both the source carrier and installed local copy are repaired.

The next reconstruction pass reuses the admitted collection lane for source-map
labels, checks the existing dependency table before classifying an already held
unit, and lets the existing prelude scan identify whether import-line removal is
needed. Text without those lines copies once; carried text is already filtered.
The existing home registry supplies a local name-length range. Import restores
node arity once per node rather than once per field, reads the last source-map
entry directly when it owns the offset, and skips hold discovery when there are
no constants. No table, cache, image format or alternate loader was added.

The intermediate collection-only measurement did not establish a gain. Adding
the home filter and shared node-arity lookup produced fresh-process CPU readings
1699/1751/1909 microseconds on the same 2,048-function image, against the previous
1982/2015/2025. Resident calls took 85–97 microseconds. This combined improvement
is about 13% at the median; it does not establish storage-only parity. A reader
rewrite with fewer byte-bound checks showed no reliable gain and was discarded.

Cell and image capability witnesses returned 16777215, import-carry 255, and all
12 ownership/invalidation checks passed across separate processes. Actual CRLF
imports and directive-shaped multiline string contents preserved exact bytes;
the diagnostic after them remained on line 7. Corrupt checksum and valid-seal
trailing bytes both voiced image-admission-failed followed by image-admitted;
the repaired result and retained result were both 42.

The landing witness exposed a cold bootstrap defect: JIT warm-up occurred before
the floor compiler's existing ownership guard. It attempted image publication
through another compiler before the first floor call owned that work. A retained
scratch-checkout trace showed the nested publisher stop and a silent outer exit
2. Warm-up now occurs inside the same guard; a new, uncached scratch BML unit
checks successfully. This is an ordering repair at the existing owner, with no
new recovery path. The native publication task witness also retains its actual
value, status, errors, stop reason and individual source-check results in its
checkpoint and emits them through organ health. An intentionally wrong pin
reported 42 against 41; the corrected pin reported healthy with surprise 0.

The remaining source-discovery work cannot be dropped on the strength of old
member hashes: new home mappings and newly preferred import paths can change
the selected closure. Current v9 unit rows hold IDs and sequences, while exact
source observations and selected windows live in the existing RAM catalog.
Validation and reconstruction still consume CPU; that gap remains measured and
open. The byte-copy, row lookup and ordering changes remain seed transport that
leaves with native Form collection and image admission.

The final ordering candidate measured 1762/1739/1729 microseconds from disk and
85/85/87 resident, with zero source work. The cold scratch observation also
exposed publisher ownership: resolving its imports through a logical symlink in
the caller's scratch checkout could miss the publisher body's own compiler
sources. Admission now resolves only that selected internal publisher to its
own source location, refusing if it cannot be resolved. User source and image
paths retain their existing meaning. Cell and image capability witnesses again
returned 16777215; the complete landing observer returned 524287.

Reunion had ignored redraw failure and could push unfinished work. It now
retains the affected paths and owning branch with the existing native checkpoint
writer before drawing, returns the failure through its result and organ health,
and resumes that obligation at the public entry even after the rebase is already
complete. A different branch cannot consume it. In two separate processes a
deliberate redraw exit 1 left the commit unpublished and the checkpoint present;
the next public reunion completed the redraw, published and released the
checkpoint. Healthy observation has surprise 0. No fallback replaced the failed
operation.

Fresh working directories exposed two more ownership seams. Lowering memo slots
now include the exact source spelling, working directory and home registry that
the existing packet already validates. Primary-slot ownership uses that same
packet reader without redoing source observations. The selected floor compiler,
like the image publisher, now resolves its physical owning source before entry;
its dependencies no longer inherit a scratch checkout's logical symlink root.
No user source identity or packet format changed.

The existing atomic checkpoint writer moved from coding memory to the existing
host-filesystem module, shared by coding and reunion. Reunion retains redraw,
amend and release phases and resumes before another rebase. An actual rejecting
commit hook left phase amend and the remote unchanged; resumption drew exactly
once. An empty checkpoint was refused. A directory permission failure retained
phase release and resumed without repeating drawing or amendment. Failure and
completion were both observed through organ health. These private observations
retain the exact commands and output under raw-redraw-phases in the research
directory. The complete landing witness now returns 524287; current cell and
image capability return 16777215, import-carry 255 and freshness 31.

After these ownership repairs, fresh-process disk CPU measured 2013/1942/1825
microseconds, with resident 95/100/92 and zero source compilation. This noisier
round does not establish another speedup or storage-only parity. The remaining
raw-source classification read and collection read are separate work; the next
observation targets that duplicate while preserving read and ownership checks.

The collector now classifies the bytes from its existing successful read. Raw
classification, identity and imports therefore describe one observation; the
same bytes are copied before recursive imports reuse the buffer. The existing
lowering owner handles `.bml` and embedded-section sources. Cell admission,
dependency collection and bootstrap share that entry, removing 24 seed lines
without a new cache or table. Descriptor metadata and release checks remain.

The first single-read samples were 1702/1706/1853 microseconds. A control reversal
measured 1704/1647/1696 on the previous binary and 1710/1886/1734 on the candidate,
so this is a correctness simplification, not an established speedup. Cell and
image capability returned 16777215, import-carry and home linking 255; cyclic
lowering, retained values, malformed-image repair and exact source-byte/line
behavior held. The private cell observer initially mixed candidate and installed
builders; giving both parent and oracle the same candidate restored its complete
pin. That failed command and output are retained with the corrected observation.

Raw v9 images no longer serialize the node-symbol inspection rows that both
native loaders discarded. AST rows, function roots and ordered exports already
carry those relationships. The existing v9 empty section is used; no image
version or runtime selector changes. The Form patcher admits either an empty
section or a complete ordered section and preserves its form during literal
replacement. Partial counts still fail. The producer's two derivative maps and
per-node metadata writer were removed.

The same 2,048-function image shrank from 687,946 to 466,582 bytes (32%). The
adopted Form path measured 1413/1553/1497 microseconds from disk, with resident
85/85/89 and zero source compilation, against the preceding roughly 1.7 ms
readings. The existing image witness now also checks partial-count refusal and
empty-section replacement with nested literals, checksum and ownership-tail
preservation. Cell and image witnesses return 16777215, import-carry 255 and
static projection 127. Actual SHA demand publishes its literals into a compact
image; the next process reads tags 24/24, executes zero proofs and compiles zero
source units/functions, without admitting the helper. Both calls hash abc
correctly; hash CPU is 103 then 47 microseconds. Storage-only parity remains open.

A private prefix filter over the existing home names gave no reliable timing
gain and was not adopted. Further work follows the remaining measured stages.

A node of that image stays in the sealed bytes until the unit sequence, a
constant initializer, a call, or a later read names it. The row is written at
the index the image already gave it, so the tables stay the ones already
published. Two fresh processes reloading the 2,048-function image returned 42
with zero source compilation, in 1433 and 1382 microseconds; the resident
calls were 79 and 83. Unreached function bodies stay bytes. A sibling that
only reads a shared function wakes that body. These readings sit with the
compact-image readings above and do not establish a further speedup.
Storage-only parity remains open.

A sealed raw image with a negative node-symbol count was silently admitted:
answer 42, zero source work and no diagnostic. Both native readers now share
the section validation already expressed by Form: the count is zero or the
node count, rows are ordered, and dependency counts are nonnegative and fit
the remaining bytes. This removes 12 seed lines. Execution relationships still
come from the existing AST, roots and export tables.

Negative count, partial count, unordered rows and negative dependency count
were each exercised with a freshly sealed malformed image. Direct execution
refused the image with its specific compile-error event. Raw-cell admission
reported image-admission-failed, rebuilt and re-admitted the image through the
existing repair flow, and returned 42. The next process reused the repaired
image with zero source work and no diagnostic. Commands, malformed producers
and outputs are retained under raw-symbol-shape and raw-symbol-* in the private
research directory. Cell and image witnesses return 16777215, import-carry 255;
the native authoring guide completed successfully.

The current compact-image samples were 1541/1893/1663 microseconds from disk
and 100/108/86 resident, with zero source compilation. The validation repair
establishes correct refusal, not another timing gain. A private stage profile
measured collection 537, image opening 363, import 467 and catalog capture 131
microseconds; storage-only parity remains open.

After the on-demand node decoder arrived, a fresh-process observation exposed
literal ownership: an untouched function's string was reclaimed after two
melts. Its later call returned GO-BNF-SHORT-VAR instead of its original literal,
with no compile error. Successful raw admission now pins its existing string
table range, including strings whose nodes still sleep. It no longer scans
every node to find those strings. This uses the existing pin table and leaves
with native image admission. The demand owner also takes the already prepared
function map directly instead of allocating, copying and freeing another one.

The same observation now survives two melts and returns the exact original
literal from disk with zero source compilation in both admissions. A malformed
section after the map transfer still reports image-admission-failed and repairs
successfully. Cell and image witnesses return 16777215, import-carry 255; native
authoring guidance completed. The private raw-demand-string observations retain
the failing and repaired outputs. Latest 2,048-function disk samples are
1784/1696/1516 microseconds versus resident 98/96/90. These noisy readings establish
no additional speedup, and storage-only parity remains open.

Admission was naming every export as the image came in, and resolving the
image path again. The name joins the live roster when the body is reached.
The location stays the absolute path collection already resolved, when the
artifact is that path with the cell suffix. Ten fresh processes reloading
the 2,048-function image returned 42 with zero source compilation, in 1272,
1313, 1358, 1392, 1444, 1462, 1514, 1526, 1645 and 1739 microseconds. One
resident call in that series was 98. These sit with the readings above and
do not establish a further speedup. Cell and image witnesses return
16777215, import-carry 255, static projection 127 and the demand guard 511.
Storage-only parity remains open.

The 2,048-function file wrote a fixed crowd so a reload could be timed.
The call that ran was one function. An unreached row stays sealed until a
call or a read names it. That file, its image, and the generator that
wrote the crowd are retired. A reached call is the witness.
