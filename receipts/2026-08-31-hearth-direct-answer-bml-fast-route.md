# Hearth direct-answer BML fast route — 2026-08-31

## Crossing

The running Form-cli hearth in the sibling checkout received the active goal
as scannerless task `turn=9008`, through `observe/hearth-ask-send.fk` and its
existing `.hearth/task.spool`/bell route. Its own framebuffer and durable reply
then carried the result. No HTTP, llama-server, Ollama, remote-provider call,
or second model residence entered that turn.

The reply's carrier receipt was:

```
route=direct-answer
callback-calls=0
carrier=form-native-metal-jit
native-code-generated=0
injected-bytes=0
lookup-count=0
tool-status=value;tokens=124;pos=3219;pending=248046;stopped=1;observations=21
elapsed-ms=98471
```

The framebuffer also made the motion legible. The old shared turnwheel first
ran `fcpa-observe-task` from `1788149230027` to `1788149334385`: 104,358 ms of
generic task observation. The direct effect then made its necessary compact
direct-user observation in 73,187 ms and generated in 25,284 ms. The first
crossing was duplicated framing, not answer formation.

## Body change

`form/form-stdlib/bml/form-cli-peer-direct-answer-dispatch.bml` now owns the
direct route and says exactly that a direct answer enters the caller-held
session before generic model task observation. Its allocation is the caller's
per-turn lease; the dispatcher supplies no separate fixed ceiling.

`form/form-stdlib/form-cli-peer-stream-ingress.fk` is the compatibility seam:
it invokes that BML action before the `fcpa-observe-task` branch. Source,
model, and patch actions retain their existing explicit boundaries. The
compact direct-user observation remains: the model must receive the new task
bytes once. Only the duplicate generic task wrapper was removed.

`form/form-stdlib/tests/form-cli-peer-direct-answer-dispatch-band.fk` uses an
unavailable sentinel session to prove the fast branch without taking a model
or Metal call. Its `255` mask witnesses:

- BML selection distinguishes `direct-answer` from `model`;
- same session, `nothing` signal, zero callbacks/injections/lookups, zero
  observations, and direct carrier fields survive the action;
- a candidate is staged without claiming evaluation;
- `nothing`, `0`, and `1` retain distinct meanings.

## Fresh evidence

```
./fkwu form/form-stdlib/bml/form-cli-peer-direct-answer-dispatch.bml -> 0
./fkwu form/form-stdlib/tests/form-cli-peer-direct-answer-dispatch-band.fk -> 255
./fkwu form/form-stdlib/tests/form-cli-peer-stream-ingress-band.fk -> 2097151
./fkwu form/form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk -> 4194303
./fkwu form/form-stdlib/form-cli-bml-cache-run.fk -> bml-cache state=ready bounded=1
./fkwu observe/preflight-run.fk (stream ingress) -> errors 0, unresolved 0
```

## Boundary and next stone

The monitored process is already loaded from the sibling checkout's older
program image. It cannot receive this new BML function retroactively. It did
receive and expose turn 9008; a successor birth from synchronized source is
what makes the fast route executable in that resident while preserving the
old resident's in-flight session honestly.

The next locally actionable gap is to make the direct-user observation itself
emit an incremental per-turn framebuffer checkpoint while its bytes enter the
held KV state. That is the remaining 73-second opaque span shown by turn 9008;
it is not a reason to add a shell watchdog or a second server.
