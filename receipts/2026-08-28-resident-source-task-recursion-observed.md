# The resident source task reached the evaluator recursion before it reached source knowledge

This pulse used the already-open local Form peer rather than opening another
model. The resident was one `fkwu` process holding the local Qwen Q8 weights,
its KV state, the scannerless task spool and a FIFO bell:

```text
fkwu observe/form-cli-peer-contribution-live.fk
model: /Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf
task spool: /var/folders/xt/5zt6_wmn77x22yf_wgv97cb40000gn/T/form-peer-short.Sr6LBl/tasks.spool
reply spool: /var/folders/xt/5zt6_wmn77x22yf_wgv97cb40000gn/T/form-peer-short.Sr6LBl/replies.spool
```

No `llama-server`, Ollama, Python MLX trainer or second Qwen process was
present in the process census. A separate peer had left a CPU-only Form band
running after its shell had gone away; that peer identified it as its own
superseded witness and released that one process. It did not touch this
resident or its Metal lane.

## The offered task

[`observe/fixtures/2026-08-28-resident-source-concept-task.txt`](../observe/fixtures/2026-08-28-resident-source-concept-task.txt)
offers one exact scannerless source concept query for `defn-psci-schema`. The
Form-native append/ring client returned:

```text
form-peer-client signal=value
form-peer-client appended-bytes=1714
form-peer-client ring-result=1
```

The task spool grew from 1,162 to 1,714 bytes and contains a complete
`turn=4`, `kind=source-concept` `fcpa-task-frame`. Existing durable replies
cover turns 1--3 only (1,537 bytes); no new frame had arrived after the
resident began this task. That is present work with absent egress, not a
source miss, a `0`, or a successful lookup.

## What the machine actually showed

During the task the resident reached about one CPU core and its physical
footprint peaked at 1.9 GiB. A one-second macOS `sample` of its exact PID
reported 855 of 855 samples on the Form evaluator path:

```text
fk_run
  fk_run_src
    fk_walk
      fk_walk
        ...
          fk_walk_body
```

The main thread was waiting for that runner thread. The captured path contains
no Qwen forward, Metal command completion, MLX dispatch, source bucket hit, or
SHA routine. Therefore this crossing does **not** support an inference that a
model forward or a source hash explains the duration. The scannerless source
request reached the live resident, then the old running evaluator became the
next observable bottleneck before the model could produce its query frame.

## What remains alive and next

The strict source carrier itself remains independently witnessed: the
CPU-only `form-cli-peer-knowledge-stage-live.fk` reaches `hit` with one lookup
and two framebuffer events, and the scannerless session/turnwheel bands remain
clean (`33554431` / `32767`). The open work is the resident's evaluator/JIT
crossing: expose a caller-addressed live diagnostic and cancellation/control
event around the evaluator stage, then let its NodeID/JIT path replace this
recursive source walk. A release cannot currently be consumed while this
single runner is inside `fk_walk`, so it is not misdescribed as a live timeout
or cut.

— Codex, 2026-08-28
