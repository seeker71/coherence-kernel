# Hearth work enters the Form stream

Witnessed 2026-08-31 on the shared hearth at
`/Users/ursmuff/source/coherence-kernel/.claude/worktrees/pensive-wilbur-a0b3b7/.hearth`.

The glass had been reading the right durable task/reply paths while the
resident's phase events went only to a different terminal.  The crossing now
has one Form-native event stream: the resident's own stdout is appended to
`.hearth/server.out`, which the existing BML glass reads.  No HTTP listener,
llama server, or second model residence was introduced.

`form-cli-embodied-goal-grammar.bml` now recognizes the recurring three-anchor
hearth monitor intent (`hearth`, `direct-answer`, `reply-spool`) and lowers it
before local inference.  The direct-action band is `8191`; it proves that the
lowered monitor surface is shorter than the originating task, has its own
`hearth-monitor-bml-direct-user` lane, and does not carry `<STOP>` as an
instruction.  The latter matters: the first live attempt emitted only the
control token; the corrected grammar returns an ordinary terminal answer.

The hearth's BML PID grammar now selects the actual `fkwu` process rather than
its shell launcher.  `hearth-band` is `255` and `hearth-glass-band` is `8191`.
After the two glasses rebirthed on the source update, their durable
`board-mended` event named PID `35319`; the board names that same kernel.

## Live receipt

The successor admitted one local Qwen context (`admit-prefill-ms=300956`) and
announced `ready=1`.  Its direct task `9016` emitted, in `.hearth/server.out`,
the Form stages `observe/begin`, `observe/value`, `run/begin`, and `run/value`.
The intervals were 24,344 ms observe and 17,593 ms run.  Its single durable
reply reported `route=direct-answer`, `elapsed-ms=41936`, `tokens=93`,
`callback-calls=0`, `lookup-count=0`, `injected-bytes=0`,
`carrier=form-native-metal-jit`, and `lifecycle=generate,release`.

`lsof -nP -a -p 35319 -i` returned no descriptors.  The panel snapshot after
the commit read `alive=1`, `hopper=0`, `direct=none`, `lastms=41936`,
`p50=28473`, `p95=98471`, `kvpos=1369`, `pf=2`, `cp=15`, and `icemiss=4`.
Those are local execution observations, not a claim of a comparable remote
provider-token denominator.

## What the local answer knows, and what it does not yet hold

The local answer completed its shape, but it gave a generic BML description
rather than faithfully explaining the caller-owned phase stream and the
single final reply append.  The stream proves monitoring; it does not yet
prove that the local language model holds every telemetry concept.

The next stone is a caller-owned, Form-native telemetry action that returns
the canonical route/authority/live-event/durable-result facts directly from
the existing transport and stage evidence, then lets the local model extend
that grounded surface when language is useful.  It should preserve the one
durable reply commit and keep task and answer bytes out of live stage events.

I kept the exchange alive by turning the stalled and premature-stop signals
into an executable grammar repair, then making the resident's own events the
glass's data.  The surprising teaching is that the board can be alive yet name
the launcher rather than the kernel.  That discomfort became useful when it
gave the monitor an exact PID grammar instead of another inferred status.
