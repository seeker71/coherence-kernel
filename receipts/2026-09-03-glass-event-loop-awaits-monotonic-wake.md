# 2026-09-03 — Glass event-loop state is Form; the timed wake is still a door

Glass now carries its deadline and presentation state in executable BML:

- deadlines advance from the prior deadline rather than from completion time;
- a late observation skips every elapsed slot with one arithmetic step;
- collection stages a back frame while the terminal retains the front frame;
- commit swaps the buffers without overwriting the prior front;
- telemetry, control, deadline, and JIT-refusal are distinct typed wakes;
- JIT refusal has highest priority, retains its exact evidence, and stops
  ordinary presentation. `jit-hold-evidenced` preserves that stop. Only
  `jit-healed` resumes.

The live Glass renderer uses that front/back buffer now. The launcher includes
the event-loop executable in its content identity, refresh set, and consumer
set, so a changed scheduler cannot leave an old live image admitted.

## The physical reading

The bounded read-only witness saw two current telemetry inventories with 18
publishers and 117 samples each. They crossed the buffer as generations 1 and
2, ending with presented generation 2 and staged generation 2. The new event
module contained zero `host-exec(` call sites.

The same breath's bounded Flow panel reported `2Hz`, `ev=93`, `nodes=218`, and
`cons=14K`. It showed the existing resident Qwen owner and parallel Qwen + 3B
availability without restarting or releasing either model or the user's Glass.

Witnesses:

```
./fkwu form/form-stdlib/tests/form-glass-event-loop-band.fk  # 4194303
./fkwu form/form-stdlib/tests/form-glass-launch-band.fk      # 32767
./fkwu form/form-stdlib/tests/form-glass-live-band.fk        # 1073741823
./fkwu observe/form-glass-event-loop-current-run.fk          # 18 publishers, 117 samples, generation 2
./fkwu observe/form-glass-flow-current-run.fk                # 2Hz, ev=93, nodes=218, cons=14K
```

All three changed targets passed the isolated stdin preflight with zero errors,
warnings, and unresolved calls after the sibling flow-UI repair landed.

## Honest boundary

This does **not** claim that the live wait is event-driven yet. The physical
witness reports `live-compat-sleep-present=1`, and the authority names
`idle-polling-remains-unreplaced`.

The current kernel exposes wall `now_unix_ms`, indefinitely blocking FIFO/file
reads, blocking TCP accept/connect/recv, and Metal fence wait. None can join a
monotonic deadline with telemetry/control path change while also guaranteeing
that an ordinary regular-file producer never blocks when Glass is absent.
Using the existing FIFO bell would reintroduce the already observed ringlock;
using a loopback socket would turn a blocking connect into an unearned
nonblocking claim; spinning would spend the idle core.

The exact remaining carrier is therefore two linked doors:

```
kernel.monotonic-ms
kernel.monotonic-wait-until-path-change
```

The second must accept a monotonic deadline plus the telemetry-root and control
inbox paths, wake on either path movement or deadline, skip missed deadlines,
and require no producer-side FIFO/socket action. Until it exists, the existing
per-frame compatibility sleep and control acknowledgement polling remain
visible rather than being renamed as the requested event loop.

Signed, Codex — the loop's state came home to Form; the absent carrier stayed
absent instead of becoming a simulated clock.

; witnessed: 2026-09-03 -> event state 4194303; physical 18 publishers / 117 samples; timed wake unavailable
