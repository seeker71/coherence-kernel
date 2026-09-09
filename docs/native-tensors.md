# Form tensors on native Metal

`form/form-stdlib/native-tensor.bml` owns tensor parsing, shape inference,
broadcasting, integer and float types, emitted Metal source, dispatch, and
cleanup. `observe/native-tensor-run.fk` accepts one program on stdin:

```sh
printf 'v4 1 2 3 4 f32 r2 2 2 dup matmul sum i32\n' |
  form-run ./fkwu observe/native-tensor-run.fk
```

The result is 54. `nt-run` returns a result containing presence, value, reason,
counter snapshots before and after, captured carrier status, stage rows,
resident counters before cleanup, elapsed milliseconds, and final wait
milliseconds. `nt-eval` returns a successful integer or raises the retained
error. A valid zero has presence 1; a refusal has presence 0 and a named reason.
The result owns its diagnostic data, so a later call cannot overwrite it.

Programs use integer scalars, `vN` vectors, `rN` reshape (one inferred `-1`
dimension), `f32`/`i32` casts, `dup`/`swap`, arithmetic, comparisons,
transcendentals, reductions, gather, and matrix multiplication. Broadcasting
aligns trailing dimensions. Matmul accepts float vectors, matrices, and
broadcast batches. Reductions cover every axis. Large sums reduce through
parallel partial buffers; floating-point association can differ from a serial
sum. A program ends with one int32 scalar. Float programs explicitly scale and
cast their answer. Gather validates indices on the device and reports failure.

`tf32`, `q8`, `q4k`, and `q6k` take a path, byte offset, rows, and columns.
Files must contain the complete extent. Shapes must contain complete quant
blocks. File buffers use the Metal carrier's shared mapping door; its counters
report mapping reuse or a carrier copy fallback. Quant decoding uses the
existing Form Q8_0/Q4_K/Q6_K Metal emitters. `dup` and reshape share the same
buffer handle. Every allocated handle has one cleanup owner.

Pipelines are retained by exact emitted source and entry point. Warm calls
reuse them with fresh tensor data. The stage rows report operation kind,
selected Form/Metal route, elapsed milliseconds, pipeline births, dispatches,
buffer admissions, shared mappings, and error. They contain no tensor values
or file paths. GPU busy time is measured for the submitted command buffer;
the runtime does not invent per-operation device timings.

For repeated execution of an immutable literal program, `nt-image-open`
retains its buffers and command image; `nt-image-replay` executes the commands
again, and `nt-image-close` releases the buffers. Replay allocates no buffers
and emits no source. Closed images refuse execution. File-backed programs use
fresh `nt-run` admission; the image API refuses file replay until file identity
and shape-dependent inputs have an attested lifetime. The image band overwrites
the output before replay, proving that replay recomputes the answer.

An unknown token returns `unknown tensor token; offer NodeID JIT`. The
`form-cli-tensor-nodeid-fallback-live.fk` door can offer the caller's admitted
recipe to the existing NodeID JIT and retain both attempts. An invalid shape
does not trigger unrelated fallback execution.

Behavior witnesses live in `form/form-stdlib/tests/native-tensor-*-band.fk`.
`./fkwu observe/native-tensor-witness-run.fk` retains each affected band's
preflight, stdout, stderr, exit, answer and elapsed time beside the source tree
and binary identities. The JIT witness remains
`./fkwu observe/native-jit-retirement-witness-run.fk`.
The lifecycle band also checks full 32-bit JIT results, signed agreement
between tensor/Metal/CPU execution, warm pipeline reuse, and buffer release.
These are correctness and lifecycle witnesses. They do not establish a
hardware throughput floor or claim native execution of external model and
adapter-training programs.
