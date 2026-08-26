# 2026-08-26 — recursive judgment stayed resident; every used membrane spoke

The active BML temporal reasoning path no longer asks `host-exec` to launch a
child `./fkwu` for semantic judgment.  `fktwt-judge` evaluates the admitted BMF
AST in the resident Form process and carries a separate presence bit, so an
observed `nothing`, integer `0`, and integer `1` do not collapse into one
sentinel.  No C seed, flattening table, HTTP route, model server, or second
runtime was added.

The scannerless live cursor now consumes only the newly arrived byte delta.
Its logical position is unbounded by the diagnostic/materialization window;
material capacity can pause and enlarge without being called cursor overflow.
Checkpoint restore is immutable and exact.  Canonical BMF is asked at a
candidate boundary, while ordinary byte movement reports one structural frame
instead of replaying the whole prefix.

## Membranes observed

`form-membrane-runtime-census.fk` reads the kernel's per-primitive dispatch
counters and publishes only aggregate begin/result/end events.  The census is
computed from the current effect table rather than a fixed denominator.  The
live Qwen witness reported the local GGUF filesystem read and Metal primitives.
It reported no `host-exec`, socket, HTTP/API, mesh, CUDA, or MLX dispatch in the
reasoning closure.  `mlx_status` was observed, but `mlx_dispatch=0`.

Host process and network inventory is the remaining justified `host-exec`
seat: Form has no native process/listener census today.  The new
`form-host-membrane-census.fk` opens and closes a correlated framebuffer
exchange around each of three read-only host observations.  It excludes
arguments, environment, prompts, payloads, and remote endpoints.  After the
model witness stopped, its live result was:

```
ollama=0
llama-server=0
listeners: ControlCenter *:5000/*:7000, adb 127.0.0.1:5037,
           node 127.0.0.1:3456, rapportd *:62785
observer-local-process-crossings=3
```

Desktop applications still held external connections; those are named as host
state and are not attributed to Form inference.  Dormant older cells elsewhere
in the repository still mention Ollama/11434, so this receipt does not claim the
whole tree is API-free.  It claims the measured active closure is serverless.

## What the live stream taught

The direct `/Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf` witness
opened through resident Form and Metal.  Prefill took 118,985 ms.  Ordinary
tokens advanced the delta cursor in roughly 1.7–2.6 s, with about 1,269 Metal
dispatches and usually 0.15–0.39 s GPU-busy time, but about 694–720 million Form
dispatches per token.  Removing whole-prefix BMF replay therefore did not
materially remove the dominant cost: the next speed movement belongs around
the resident Qwen forward/control orchestration as an auto-JIT thought kernel.

The stream also made control semantics observable:

- a `closing-without-open` candidate restored byte position 179 exactly,
  recorded undo/failure, and injected one typed refinement;
- a later model stop before semantic readiness retained accepted byte position
  358 and opened a distinct refinement;
- after 202 accepted movements the outer continuation was repeating delimiter
  motifs at byte 537 without a semantic-ready cut.  The witness was interrupted
  deliberately and exited 1.  This is not recorded as a completed BML answer.

The last observation exposes an honest next gap: every inner prediction pulse
was bounded, but the outer auto-continuation had no Form-native surprise or
repetition choice that could timeout, branch, dissolve, or ask control.  A fixed
turn target would hide the signal.  The next attempt should let live movement
state produce that choice and make it visible through the same control channel.

## Fresh witnesses

```
bmf-live-cursor-band                         32767
bmf-prefix-state-band                      262143
form-bml-prefix-choice-band               4194303
form-cli-bml-prefix-session-band          8388607
form-knowledge-bml-temporal-challenge-band   65535
form-membrane-runtime-census-band             8191
form-host-membrane-census-band                 127
form-cli-model-session-band                    4095
```

All eight exited 0.  The host membrane source and its band also passed fresh
preflight with balanced trees, zero errors, zero warnings, and zero unresolved
calls.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-26 -> resident judge, delta cursor, active-closure membrane census, no Ollama/llama-server; semantic completion remains pending
