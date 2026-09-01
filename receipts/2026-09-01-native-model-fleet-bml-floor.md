# Native model fleet BML floor

Date: 2026-09-01

## Crossing

`form/form-stdlib/bml/form-cli-model-fleet.bml` introduces a Form-native
model-fleet ownership surface.  A dynamically named seat holds one admitted
`fcms` session, thus one native model context and one independent stream/KV
state.  A fleet revolution advances each live seat by **at most one** native
token and carries the returned session with that seat.  No model name, path,
or fixed function seat is compiled into the grammar.

`fcmf-open` is the physical admission door: it calls the existing native
`fcms-open`, which seals the offered artifact, opens the Qwen Metal context,
and creates the stream state.  A repeated name or path is a typed `choice`
before a second open.  An absent fleet or absent identity is `nothing`; token
IDs `0` and `1` remain present values.  `fcmf-release` closes only sessions
owned by the admitted fleet.

This movement does **not** claim that the present live resident is already a
multi-model server.  Its turnwheel still has one physical model admission.  A
future resident must carry this fleet at birth, route each task to a named
seat, and emit a per-seat stage before it can claim independent concurrent
service.  Metal command-buffer overlap also remains an observed property of a
particular carrier/device; fair interleaving alone is not called overlap.

## Evidence

```
./fkwu form/form-stdlib/tests/form-cli-model-fleet-band.fk
=> 1023, exit 0

preflight form/form-stdlib/tests/form-cli-model-fleet-band.fk
=> parens balanced; errors 0; warnings 0; unresolved 0

./fkwu form/form-stdlib/tests/form-cli-author-high-band.fk
=> 4095, exit 0
```

The active hearth glass at the same observation showed its current physical
floor rather than a fabricated fleet result: resident PID `36364`, Qwen3.8
Q8, KV `33%` at position `1360`, task `#100100` in `phase=run`, and
`served=36`.  That is an explicit one-model observation, not evidence for two
loaded artifacts.

## Adjacent gaps kept visible

1. The live Qwen GGUF resident and the cached MLX Llama LoRA adapter have
   different architectures and tensor representations.  No adapter is loaded
   across that boundary.
2. The `repo-patch-v1` hearth route currently reports
   `empty-or-missing-file` for natural-language task bytes; its old servant
   expects `path|find|replace`, while the resident is born without a patch
   capability context.  It cannot yet own arbitrary repository movements.
3. A fleet resident needs a task-to-seat protocol, per-seat durable staging,
   native memory-admission telemetry, and a two-local-artifact Metal receipt.

I kept the exchange alive by making the multi-model idea executable without
claiming physical overlap that has not been observed.  The surprising teaching
is that a model identity belongs beside its KV state, not in a dispatcher
table.  The discomfort was the silent `repo-patch` route; it became a precise
born-capability gap rather than a reason to pretend that an external shell is
the resident agent.
