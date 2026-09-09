# Native JIT routing

Run Form source with `./fkwu path.fk` or `./fkwu path.bml` from the repository
root. Form owns lowering, emission, demand and reuse. Source is authoritative;
native images are local cache. The executable route lives in
[`native-jit-route.bml`](../form/form-stdlib/bml/native-jit-route.bml).

`./fkwu observe/native-jit-guide-run.fk` reads the current route's source and
witness paths. It reports readable and unreadable references with a timestamp.
File presence establishes availability; executable witnesses establish behavior.

`./fkwu observe/native-jit-witness-run.fk` emits an ARM64 image in Form, executes
two inputs through the physical carrier, and checks malformed-image refusal.
`./fkwu form/form-stdlib/tests/direct-source-jit-discovery-band.fk` checks heat,
choice, birth, reuse, timeout and undo. `form-cli-jit.bml` carries demand and
CPU and native Metal routes. Tensor programs run through
`form/form-stdlib/native-tensor.bml`. Device drivers admit Form-emitted programs.

The Go, Rust and TypeScript kernels are proof interpreters. Physical MAP_JIT
and library loaders carry Form-emitted images. `register_jit` binds an existing
native alias; its witness is the alias registry sample. Compilation uses fkwu.

Observe native JIT demand and reuse from the resident Form panel in its own
process. API observation routes retain framebuffer events and route timings.
Each process reports its own measurements.

For recovery, search local Form cells, RAG, git history and receipts. Make the
native attempt, retain behavior checks and per-stage streams, exit status and
timing, update callers, then re-observe. The shared native authoring guide links
this door so the next attempt can start from current local evidence.

`./fkwu observe/native-jit-retirement-witness-run.fk` runs eleven native checks
after preflight and retains each case's stdout, stderr, status and timing under
`.hearth/`. Recursion, lists, helper calls and IEEE/string/vector bands prove
results. The independent leaf witness proves Form emission and execution;
each function's machine-code entry requires its own observation.

The proof validator retains each carrier's streams and status under
`.hearth/validation-*`. It compares stdout and requires successful exits.
