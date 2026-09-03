# Glass keeps owner request windows and explicit flow categories

Witnessed 2026-09-03 by Codex/Sol in the existing worktree. No model was
opened, released, reloaded, or restarted. The standing owner remained PID
18249 (`etime=04:18:57` at the closing liveness check).

## The physical gap

Before this movement, the bounded current Flow witness rendered `GPU`,
`CPU-JIT`, `DISPATCH`, and `QUEUE` from the Glass monitor's own post-drain
values. All four were zero even while the retained Qwen request chain reported
an actual 7,592 ms owner-command position window. The same snapshot displayed
producer CELL ids such as `0.2.99.7`, but the `node_category` value
`1.2.99.1703`, family, explicit semantic role, and stage type had been dropped
at the membrane.

`/Users/ursmuff/source/NUMS.Go/nums/nums_nodes.go` grounds the category design:
`RecipeNodeID_t` and `BlueprintNodeID_t` are distinct wrappers over the same
`NodeID_t` p.l.t.i coordinate. Therefore role is carried as an explicit field;
it is never inferred from coordinate bits.

## What now crosses

- Flow counter lanes select the resident owner's cumulative
  `gpu.owner.busy-us-total`, `jit.owner.metal-busy-us`, and
  `metal.owner.dispatch-total`. Adjacent observed values form an exact
  request-window delta. The newest nonzero window, epoch, evidence, and source
  remain visible after the current delta returns to zero.
- Queue selects only a standing Hearth's physical task-minus-reply depth. Its
  newest positive gauge remains visible after drain. Missing owner counters or
  missing standing queue publish their exact door, never monitor-local zero.
- A failed physical JIT sample renders before normal Flow lanes with stage,
  cause, age, evidence, source, and preemption state.
- Token-flow v2 carries the original CELL id plus CATEGORY, FAMILY, explicit
  ROLE (`blueprint`, `recipe`, `cell`, or `value`), and stage TYPE. V1 remains
  readable and explicitly projects `category-unpublished`,
  `family-unpublished`, and `role-unpublished` rather than guessing.
- The active request is published before command offer. A completed eight-stage
  chain is then published once, atomically, with `completed` lifecycle. The six
  artificial 250 ms post-completion sleeps are gone; completed/failed retained
  request history cannot pin 4 Hz cadence.
- Only `content-digest` is an immutable layout identity. Inode plus size/mtime
  remains collision-prone and cannot promote exact allocation extents.

## Fresh gates

Every changed executable chain was preflighted through the isolated stdin
door: balanced, zero errors, zero warnings, zero unresolved calls.

- `form-node-category-band.fk` -> **255**, exit 0
- `native-model-token-flow-band.fk` -> **8388607**, exit 0
- `native-model-owner-request-flow-band.fk` -> **65535**, exit 0
- `native-model-tensor-ledger-band.fk` -> **65535**, exit 0
- `form-glass-live-band.fk` -> **1073741823**, exit 0
- `form-glass-live-ui-band.fk` -> **1073741823**, exit 0

The current safe Flow witness completed in 2.5 s without touching owner state.
It showed `nodes=236`, the retained Qwen chain, and the honest old-publication
boundary: `CAT=absent FAM=absent ROLE=absent TYPE=absent`; GPU and dispatch
named their exact missing `metal_status` owner doors, CPU-JIT was an
owner-scoped measured zero, and Hearth queue was a physical drained zero. A
single-frame current command correctly said the request window was not observed
after its baseline. The next real v2 model-flow publication carries the category
binding; no restart was used to rewrite history.

## Boundary kept

GPU busy, CPU-JIT busy, and Metal dispatch are physical owner cumulative
counters; their request windows are derived only from adjacent publications.
Hearth depth is a physical gauge. The 7,592 ms token-flow Metal row remains
explicitly an owner-command position window, not device-only GPU time. A
retained counter window proves work in that observed interval, not attribution
to an individual kernel, layer, or token. MLX remains typed as not on the
resident owner model path rather than becoming a numeric zero.

The surprising teaching was that the visible `0.2.99.*` values were already
honest CELL identities; the loss happened beside them, where category and role
were omitted. Discomfort turned to gold when the old snapshot could not be
upgraded without restarting the owner: keeping `absent` visible proved the new
membrane without fabricating a retroactive fact. I kept the exchange alive by
preserving that physical boundary and making the next event carry the missing
truth.
