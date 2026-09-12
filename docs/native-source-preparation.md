# Compiler freshness for historical proof inputs

The survey at `form/scripts/fourth-arm-survey.sh` prepares BML through
`observe/native-source-prepare-run.fk`. Form owns the preparation in
`form/form-stdlib/bml/native-source-prepare.bml`.

The source door reads two stdin lines: the absolute authored path, then a
disposable output path. It returns the selected path and final value `1` when
ready. A refusal returns its reason and `0`; compiler diagnostics can also make
the process exit nonzero. Callers require both exit 0 and final value 1.

Plain Form stays at its authored path. BML is read on every request and lowered
through the current Form compiler. The helper's native image imports that
compiler, so `fkwu` owns freshness of its transitive dependency graph. There is
no separate source hash cache or manually maintained compiler stamp in the
survey. Temporary lowered inputs exist only because this historical table
proof interface consumes text; the invocation removes them on exit.

Prelude and import paths are resolved from the authored file before transport.
Missing dependencies and failed writes remain refusals. The Go proof carrier's
BML prelude syntax cannot represent whitespace in a path; that case is refused.
The native execution interface remains `./fkwu source.bml`.

The Go, Rust and TypeScript proof siblings keep each BML lowering under
`.cache/kernel-bml-lowered/<kernel>-<path>-<key>.fk`. The key covers the BML's
path and bytes, every source in the loaded compiler closure, and the sibling's
own executable, so a change to any of them lowers again through the current
Form compiler. Each sibling keeps one lowering per path: the new entry replaces
the old one. An entry named by source bytes alone is never read.

Survey workers select the content-qualified table carrier through `build_fourth`.
They no longer choose an arbitrary cached executable. The committed table seed
still has its own emitter stamp and maintenance regeneration door. The survey
is a historical Go/table comparison, not a measurement of native runtime coverage.
The separate caches in `validate.sh`, the optional `fourth-arm.sh` text adapter,
remain outside this change. Go's serving compiler now loads and hashes its
transitive dependency closure; its exposed depth refusal is repaired through
[native adaptive traversal](adaptive-artifact-depth.md). The cache receipt
retains the original failure and the subsequent repair.

The survey retains Go, flattening and table stages' output, stderr and exit
status under its output directory. Nonzero stage exits become explicit rows
and a nonzero survey-worker exit. An empty successful Go output is `go-empty`;
it is not called a timeout. Table serialization emits one value without a
trailing `null`, which the current table parser correctly rejects.

`heal guide|bash form/scripts/fourth-arm-survey.sh` names the native preparation
door, its witness, and this reference. It guides the next repair without executing
the supplied command.

Witnesses:

- `form/form-stdlib/tests/native-source-prepare-band.fk`: pure/read-only checks.
- `observe/native-source-cache-witness-run.fk`: copies the native dependency
  manifest into an isolated body, checks cold/warm/source/compiler changes,
  executes relocated FK/BML dependencies, observes missing inputs and failed
  writes, and plants a source-only cache entry containing the wrong answer for
  the real Go proof carrier. Each child retains exit status, stdout, stderr and
  elapsed milliseconds in a separate JSON file. Only the owned poison is removed.

The witness reports its private evidence directory. A correlated framebuffer
exchange records the choice from missing source to an owned dependency fixture,
applies that source selection and re-observes execution. These fixture timings
are single observations, not a hardware-floor benchmark or a whole-tree survey.
