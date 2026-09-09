# Native JIT routing

Run Form source with `./fkwu path.fk` or `./fkwu path.bml`. Form owns lowering,
emission, demand and reuse. The source is authoritative; native images are local
cache. The executable route lives in `form/form-stdlib/bml/native-jit-route.bml`.

`./fkwu observe/native-jit-witness-run.fk` emits an ARM64 image in Form, executes
two inputs through the physical carrier, and checks malformed-image refusal.
`./fkwu form/form-stdlib/tests/direct-source-jit-discovery-band.fk` checks heat,
choice, birth, reuse, timeout and undo. `form-cli-jit.bml` carries demand and
CPU/Metal/MLX routes. Device drivers still admit Form-emitted device programs.

The Go, Rust and TypeScript kernels serve as proof interpreters. They contain
no recipe-to-Go-plugin, recipe-to-Rust-cdylib or recipe-to-JavaScript JIT.
The Go physical MAP_JIT and library loaders remain available for Form-emitted
images. `register_jit` is a compatibility name for binding an existing native
alias; it neither compiles nor invokes a toolchain. Compiler requests use fkwu.

The former TypeScript `--compiled`, `--bench` and `--numeric-bench` options
report the native door and exit 2. Numeric proof execution uses the existing
arithmetic interpreter. Backend source-format proof emitters are separate from
runtime JIT dispatch. Historical measurements stay in git and receipts.

The API observation routes retain actual framebuffer events and route timings.
They no longer publish the deleted Go plugin counters as JIT measurements.
Observe native JIT demand and reuse from the resident Form panel in its own
process; a proof interpreter cannot supply another process's counters.

Run `./fkwu observe/native-jit-guide-run.fk` to inspect current source candidates,
file coverage and unread paths. This bounded static scan covers the three proof
kernel trees and names its signatures in executable Form. A candidate asks for
inspection and a native replacement, never a hidden fallback. Search local Form
cells, RAG, git history and receipts; retain the behavior checks, witness the
native route, update callers, and remove the obsolete emitter. The shared native
authoring guide links this door so the next agent can repeat the recovery.

Result bands for recursion, lists, helper calls and IEEE/string/vector behavior
run on fkwu. Their separate leaf witness proves Form emission and execution;
it does not assert that every tested function acquired a machine-code entry.

`./fkwu observe/native-jit-retirement-witness-run.fk` runs all eleven native
checks after preflight, retaining per-case stdout, stderr, status and timing.
The proof validator also retains each physical carrier's two streams and exit
status under `.hearth/validation-*`. It compares result stdout and requires
successful exits; per-carrier diagnostic receipts are kept separately.
