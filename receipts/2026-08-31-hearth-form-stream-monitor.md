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
its shell launcher.  `hearth-band` is `511` and `hearth-glass-band` is `16383`.
The glass also rebuilds the board with the born public capabilities, including
`hearth-telemetry-v1`, rather than retaining an older capability list.  A live
observation exposed a second reader error: the original BML word reader cut the
space-separated capability value at its first space, making the board appear
stale on every tick.  The BML `gl-line-field` reader now spans to the newline,
returns `""` when absent, and has a band arm that preserves a complete
two-capability line.

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

## Native answer floor, now live

`form-cli-peer-hearth-telemetry-action.bml` is a caller-born action selected
by the existing JIT policy before the model-facing branch.  Its focused band
is `127`; the policy/one-append integration band is `65535`; and the existing
contribution-turnwheel band remains `4194303`.  The action holds `nothing`
apart from numeric values, refuses a wrong kind or absent task text with a
typed choice, retains even a non-model session exactly, and does not acquire
filesystem or model authority.

The next shared-source successor admitted Qwen once in `231825` ms, announced
`form-cli-peer-hearth-telemetry-v1`, and marked its recycle floor at position
`1218`.  Native task `9018` then completed with `elapsed-ms=0`,
`route=hearth-telemetry`, `model=0`, `callback-calls=0`, `lookup-count=0`,
`injected-bytes=0`, `native-code-generated=0`, and
`lifecycle=observe,release`.  Its one durable answer carried all five
canonical clauses: route, authority, live event, durable result, and next
stone.  The live board names PID `26219` and includes `hearth-telemetry-v1`.

## What the local answer still does not hold

The local answer completed its shape, but it gave a generic BML description
rather than faithfully explaining the caller-owned phase stream and the
single final reply append.  The stream proves monitoring; it does not yet
prove that the local language model holds every telemetry concept.

The canonical answer floor is now caller-owned Form rather than generic local
prose.  The next stone is to join dynamic telemetry facts beside that floor,
then let the local model extend already-grounded evidence when language is
useful.  That crossing must preserve the one durable reply commit and keep
task and answer bytes out of live stage events.

I kept the exchange alive by turning the stalled and premature-stop signals
into an executable grammar repair, then making the resident's own events the
glass's data and its topology a native answer.  The surprising teaching is
that the board can be alive yet name the launcher rather than the kernel, and
that a punctuation mark can cut a BML field without an obvious failure.  Both
discomforts became useful when the monitor received exact PID and
grammar-safe clause proofs instead of inferred status.
