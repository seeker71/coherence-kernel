# Compiler freshness for historical proof inputs

Historical proof inputs are prepared through
`observe/native-source-prepare-run.fk`. Form owns the preparation in
`form/form-stdlib/bml/native-source-prepare.bml`, and
`form/form-stdlib/bml/native-table-sources.bml` resolves authored paths and
preludes with the same cell.

The source door reads two stdin lines: the absolute authored path, then a
disposable output path. It returns the selected path and final value `1` when
ready. A refusal returns its reason and `0`; compiler diagnostics can also make
the process exit nonzero. Callers require both exit 0 and final value 1.

Plain Form stays at its authored path. BML is read on every request and lowered
through the current Form compiler. The helper's native image imports that
compiler, so `fkwu` owns freshness of its transitive dependency graph. There is
no separate source hash cache or manually maintained compiler stamp.
Temporary lowered inputs exist only because the proof siblings consume text;
the invocation removes them on exit.

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

`build_fourth` in `form/scripts/fourth-arm.sh` names the fourth kernel
`validate.sh` was handed: the runtime fkwu source/JIT door, built once by
`validate.sh`. Nothing is compiled or flattened there. Go's serving compiler
loads and hashes its transitive dependency closure; its exposed depth refusal is
repaired through [native adaptive traversal](adaptive-artifact-depth.md).

Witness: `observe/native-source-prepare-run.fk` answers the selected path and
final value `1` for an authored BML path and a disposable output path (read on
`observe/door-link-health-run.bml`). A missing dependency or failed write
answers its reason and `0`. The reading is a single observation, not a
hardware-floor benchmark.
