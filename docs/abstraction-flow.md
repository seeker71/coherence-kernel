# Abstraction and observed use

The native authoring guide and `observe/authoring-altitude-run.fk` refresh a
working-tree census of tracked and unignored `.fk`, `.bml`, `.c` and `.h` files.
The BMF cursor reads named definitions, skipping quoted specimens and comments.
Mixed modules resume their enclosing syntax after each section; executable BML
sections contribute definitions while other dialects remain grammar data.
Each file has its content fingerprint, physical line count and definition spans.
Exact cached source bytes reuse the parsed spans; changed bytes are fingerprinted
and scanned again. Deleted files leave the census. These disposable local caches
are not another source authority.

The disjoint function levels describe authored syntax, not engineering quality:

| Level | Meaning |
| --- | --- |
| host-c | C function body following a top-level parameter group |
| form | Raw Form `defn` |
| bml-primitive-spelling | BML body still calling an arithmetic, comparison, string or list operation with an available higher-level spelling, including `empty()` and `value_eq()` |
| bml-expression | BML body without those spellings or the constructs below |
| bml-composed | BML using a lambda, choice, match, try/catch, repeat/retry, iteration or intrinsic control, without those primitive spellings |

Body, verification and specimens have separate totals. Physical lines include
comments and blanks. Definition lines cover each named definition's span, including
nested definitions (whose enclosing spans overlap); they are not a second count
of all physical lines. Anonymous lambdas, generated
fields, declarations without bodies, macros and foreign dialects are not authored
function definitions. C preprocessing is not evaluated: platform alternatives
are source, not an assertion that both execute. Objective-C methods, other host
languages and prose are outside this census.

Run a real improvement workload through the existing source compiler and kernel:

```sh
form-run ./fkwu observe/abstraction-flow-run.bml <<'ROUND'
form/form-stdlib/tests/form-bml-cursor-reader-band.bml
ROUND
```

The optional second line supplies a file for the workload's stdin; otherwise it
reads `/dev/null`. The selected source runs its real effects. Compilation records
the source compiler's work. Execution wraps the lowered top-level expression in
the same `fkwu` process and preserves its answer. Imported dependencies retain
their native loader path. An enclosing Form operation can use
`fo-measure(lane, owner, () => operation())` directly.
The wrapper has its own source path. Workloads whose behavior depends on
`self_source()` should use that direct window in their original entry point.

Each window retains its start before working, captures the complete available
hot-function ledger before and after, and publishes its completed delta. An
interruption leaves an open window. Completed records include elapsed and CPU
time, kernel entry counts, RAM-JIT entry counts, source fingerprints and retained
`bml-discovery-ms` framebuffer events. The frame count exposes ring truncation.
The framebuffer also receives the completed window's summary.

These are same-process entry counters. They do not time individual functions or
count C calls, inlined native calls, native loop iterations or child kernels.
The execution window starts after dependency admission; the outer run also
retains admission-plus-execution wall time and process exit. Instrumentation
entry overhead remains visible. Calls omitted by the runtime ledger stay outside
its coverage rather than becoming zero-cost work.

The guide joins names to authored definitions, prefers a unique matching source
path and checks its retained fingerprint. Flattened line numbers never establish
source identity. Ambiguous, missing and changed definitions stay separate from
attributed use. Discovery times guide cost reductions; entry counts identify
frequently exercised code to inspect, not expensive code by themselves.

Current source, the preceding source census, and current/previous windows per
lane and workload live under `.hearth/abstraction-flow/` as native binary data.
`guide.txt` is the small human reading. Every measured round refreshes the source
view afterward, so the next choice uses current code and explicit observations.
Ordinary unwrapped runs are not claimed as measured. The direction is complete
native stage attribution through cached admission, inlining and child execution,
with observation cost visible alongside the work it helps improve.
