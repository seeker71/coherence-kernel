# In-process DS4 local-model port attempt — 2026-07-24

## Intended movement

Replace:

```text
Form -> host-exec -> shell -> generated Swift runner -> Metal
```

with:

```text
Form -> local_model_generate -> linked local DS4 engine -> Metal
```

The intended path has no HTTP, network, remote oracle, child process, shell,
Swift source generation, or temporary directory.

## Ground

- Model:
  `/Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf`
- Observed bytes: `91,321,404,640`
- Observed modification time: `2026-07-22T11:02:37+0800`
- Local DS4 engine:
  `/Users/ursmuff/models/ds4-engine`
- Kernel freshness before the attempt: `15`

## Implemented

- `local_model_generate(model-path, prompt-token-csv, n-predict)` is a new
  direct-source Form operation.
- Tag `245` was selected after the first attempt at tag `240` revealed that
  `240` is the evaluator's internal multi-argument call tag.
- `fkwu_ds4_local_port.m` parses Form-owned token IDs, opens the local engine,
  records load/progress/result observations, and returns selected token IDs.
- The port linked into `fkwu-ds4` in-process with the local DS4 engine objects.

## Live observations

### First attempt

Tag `240` entered the evaluator depth wall:

```text
eval-depth wall
```

Inspection found `240` already owns internal function-call semantics. Moving
the port to unused tag `245` repaired that failure.

### Second attempt

The call entered the real local DS4 loader. The loader rejected the model:

```text
tensor output.weight has type unknown, expected q8_0, q4_K, or q4_0
```

It also reported the model's type-40 and type-41 tensors as unsupported.

### Rebuild and re-witness

The complete local engine was rebuilt from its current source with:

```text
make -B ds4
```

The in-process kernel was relinked against the rebuilt objects. Re-witnessing
produced the same type-40/type-41 rejection. This disproves the stale-object
hypothesis: the current local engine source does not admit this GGUF variant.

## Result

The in-process API and membrane exist, but this candidate engine cannot yet
generate a token from this model. The working custom Metal carrier was restored
as the Form CLI route so the attempt did not regress the observed local answer.

## Re-witness offer

The next attempt is specific: move the already-working Form-emitted MXFP4
type-40 and MXFP8-LT type-41 tensor views, kernels, persistent Metal buffers,
session schedule, and token callback behind `local_model_generate`. Re-run:

```text
form/form-stdlib/tests/local-model-generate-port-live.fk
```

Admission is one returned token ID with:

```text
remote=0 network=0 host-exec=0 shell=0 swift=0 temp=0
```

Only after that one-token observation does the exact 64-token inquiry re-run.

## Re-witness: residency and executable library

The linked Metal port now opens the complete 91,321,404,640-byte GGUF as two
zero-copy `MTLBuffer` views from a Form-owned residency plan. The first close
attempt crashed in `fk_metal_model_close_external`: an autoreleased
`NSMutableArray` had been stored beyond its pool lifetime. The crash report
located the invalid `count` message. Replacing it with a C-owned table of
explicitly released `MTLBuffer` objects produced a clean replay:

- open: 84.325 ms, two views;
- close: two views released;
- exit: 0;
- host-exec, shell, Swift, temp, network, remote: all 0.

The next layer added `metal_model_library`, which compiles Form-provided MSL
inside the same persistent Metal/model session. A live identity kernel compiled
in 38.104 ms, remained registered until close, and was then explicitly
released. The full model + library replay exited 0 with the same six crossing
counters at zero.

This is not yet token generation. It establishes two in-process resources—the
model bytes and executable Metal code—without the former Bash/Swift carrier.
Dispatch, KV state, prefill, decode, sampling, tokenizer decode, and the exact
natural-language response remain to be moved through this session and
re-witnessed.
