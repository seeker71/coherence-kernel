# Native direct-token quantum

Date: 2026-09-01

## Crossing

`form/form-stdlib/bml/form-cli-peer-direct-quantum.bml` gives direct-answer
generation a one-token native state transition.  It retains the existing
`fcms` session and therefore the existing Qwen stream/KV state.  A step emits
the current pending ID, calls native `q38-forward` once to obtain the next
prediction, and carries the updated position, pending ID, generated-ID count,
and stopped state into the next resident revolution.

The state explicitly distinguishes `run`, `done`, `timeout`, `choice`,
`nothing`, and `failure`.  A token ID of `0` is a present generated ID, not
absence.  A stop prediction becomes `done` without emitting the stop as an
answer token.  A token budget reaching zero becomes typed `timeout` before a
new native forward call.

This is the needed inner primitive for both fair multi-model service and
control actuation.  It is not yet connected to the old synchronous
`fcpdaa-run` path in the standing resident; that process remains an old image.

## Evidence

```
./fkwu form/form-stdlib/tests/form-cli-peer-direct-quantum-band.fk
=> 1023, exit 0
```

The active hearth glass showed why this primitive matters: at the observation
it retained Qwen3.8 Q8 with KV `33%` at position `1360`, while direct task
`#100100` remained in `phase=run` for seven minutes.  The previous direct
action invokes `fcms-generate` over its whole token budget inside one call;
the new state carries one native token between turns instead.

## Next integration

Make a successor resident carry this quantum state in its staged task record,
persist only terminal result frames, and expose `cut`/`timeout` through the
same framebuffer row that names each selected model seat.  Then the model
fleet can revolve one token per live seat and a two-artifact Metal admission
can be observed without starving the other seat.

I kept the exchange alive by turning a seven-minute silent turn into a
concrete state boundary rather than blaming the model or waiting blindly.  The
surprising teaching is that the pending token is already a complete native
continuation handle.  The discomfort was seeing `run` stay open; it became the
exact place to preserve choice, timeout, and a next turn.
