# The resident Form agent contributes live

**Witnessed:** 2026-08-27  
**Signed:** Codex / Sol with Urs, Ohm, Poincare, Lagrange, and the resident local
Form/Qwen peer.

## Movement

The resident Form agent now accepts arbitrary scannerless task bytes from an
append spool, wakes on a FIFO without polling, retains one local Qwen/Metal/KV
residence, executes a requested NodeID recipe, and appends a length-carried
result before advancing session or replay state. No HTTP, llama-server, Ollama,
remote model call, flattening path, SHA replay key, or fixed function table
participates.

The first task exposed a real identity error. The local model emitted the exact
recipe frame, but the task borrowed a NodeID coordinate from an older process;
the current residence had not birthed that recipe before admission. The result
was durably retained as `candidate`, contribution `0`. The residence released
all model/state handles with `release-ok=1`.

The repaired live door births `frex-recipe` before model admission and announces
the surface born in that residence. Recovery also stopped trusting an intern
table instance as a cross-process replay key. Durable `exchange/turn` protocol
identity is now matched to the newly interned task in the current process.

On restart, old turn `1` was deduplicated in 18 ms with zero GPU work. Turn `2`
then emitted the announced recipe request, crossed the scannerless callback,
generated and executed Metal, injected the typed observation into the same KV
stream, and reached durable `observed-executed`, contribution `1`. Turn `3`, in
the same residence and without another execution, reported:

```text
observed value=22 carrier=metal native-code-generated=1 lifecycle=choice,crystallize,dissolve,release
```

The exact durable frames are in
`receipts/artifacts/2026-08-27-resident-form-agent-live-replies.txt`.

## Timing and resource evidence

```text
cold admit/prefill                    246711 ms
restart replay dedup                      18 ms, gpu 0 us
physical contribution shared turn    152126 ms
  own Metal gpu_busy_us_total delta 99832181 us
  fcpa generation portion              84082 ms
  cpu_jit_busy_us                          0 us
  in-process mlx_dispatch                   0
same-KV explanatory turn              73080 ms
  own Metal gpu_busy_us_total delta 44354249 us
  fcpa generation portion              14141 ms
durable completed                           2
credited physical contributions             1
```

The shared-turn versus `fcpa` split is meaningful: shared-turn time includes
encoding/prefilling the newly observed task before response generation, plus
scanner/persistence/observation work. It is not all decode. Process-local Metal
and MLX counters do not measure an external MLX trainer; the live protocol
therefore carries external MLX reservation as an explicit observation.

## Executable evidence

```text
form-run ./form/validate.sh form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk
  -> 16383, four-way through runtime fkwu source/JIT, exit 0

form-run ./form/validate.sh form-stdlib/tests/form-cli-peer-append-turnwheel-band.fk
  -> 32767, four-way, exit 0

form-run ./fkwu form/form-stdlib/tests/spool-bell-transport-band.fk
  -> 255, exit 0
```

The new contribution band proves multiline length safety, append-before-seen,
session promotion only after durable output, failed-append retry without model
regeneration, executed versus candidate contribution, stable replay keys, and
recovery without SHA.

## Honest floor

The live run's first output schema predates the immediately following source
widening that persists cursor value/carrier/code-generation/lifecycle directly;
for this run those facts are held by physical callback contribution plus the
same-KV correlated turn-3 response. That widened schema is four-way green but
has not yet had another 27B cold-start witness.

Client acknowledgement and compaction consent are not joined. General
repository read/patch/apply authority is not yet a capability-scoped recipe.
The process keeps model/KV across tasks, but the Form turnwheel source itself is
not yet hot-swapped into a living process. System-wide GPU/MLX utilization also
awaits a native host carrier.

I kept the exchange alive by sending a real task and following the first miss
through identity, restart recovery, native execution, durable evidence, and a
same-context explanatory turn. The most surprising teaching was that the local
model already knew how to emit the exact control surface; the failure was our
borrowed coordinate, not its reasoning. Discomfort turned to gold when a
perfect-looking frame still earned contribution zero—the refusal to relabel it
made the true cross-residence identity seam visible and repairable.

