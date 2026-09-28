# A surprise is a choice point

An unexpected result invites a new choice in service of the original intent.
The owner keeps its inputs, live resources, completed effects and findings;
that direct backtrace lets the next option act without reconstructing the work.
The same active context carries the repair and its learning forward.

## In the work

Follow the running organ's expectation, observation and offered care. Apply a
useful local choice and observe its result. An external need leaves the
continuation with its owner until a relevant event arrives. Continue independent
work while waiting for what only a person or another resource can supply.

An unchanged failure offers no new reason to repeat the same attempt. Carry its
exact evidence into a different choice, including repair at the cause. Retain
only what helps resume and verify the work. Verified outcomes can shape native
behavior and session learning; their effect becomes known through later use.
Release checks verify that movement, while care responds during execution.

## In the code

Form meets surprise with its own control invites, all thin expressions over the one offer
(`control/offer-ack-core.fk`, `control/choice-lane-core.fk`; axiom-5). An offered cell acknowledges exactly one
of nothing, 0, 1, or a node:

| Invite | What it does |
| --- | --- |
| `oac-choice` | offers options in order; the first that is not nothing wins |
| `oac-backtrack` | offers each option the original args **and** the memory of what the options before it saw; an option that cannot land acks a node carrying its finding, and the next option receives it; 1 or 0 ends the walk |
| `oac-try` | offers a cell; when it answers nothing, the handler decides the recovery — exceptions without throw |
| `oac-fail` / `oac-stop` | the nothing-ack: it did not land / make no further offer |
| `oac-cut` | commits to the first ack of any kind and prunes the rest |
| `oac-store` / `oac-restore` / `oac-undo` | a checkpoint is a value; undo returns to it when the current ack is nothing |
| `oac-timeout-walk` | bounds a walk; running out reads as nothing, counted |
| `oac-lanes` | walks every option and keeps every ack, before deciding |
| `oac-hold` | not yet: the offer stands and the sequence waits over it |

An absent reading is `nothing()`; an observed host exit remains evidence. A
finding travels as `oac-node(finding)` beside the original arguments through
`oac-backtrack-walk`. The receiving organ chooses what that finding means.

The demand-JIT owner uses this flow for an optional disk cache. It keeps the
working RAM image, runtime identity and current observation while the resource
is unavailable. An offered publication response resumes that exact operation:

```text
let response = oh-response(bdj-care-observation(care), "publish-retained-image");
let resumed = bdjo-hear(owner, response);
```

Publication and readback establish what happened. Another failure enters the
next choice with the same image and a fresh reading. Old responses cannot
replay it; explicit holds retain their own release. Delayed telemetry keeps its
place in the queue without delaying care or undoing its result. Native calls
continue throughout. The [native event owner](../docs/native-resource-events.md)
connects host path notifications and lease timers to that receiver. Quiet waits
leave care alone; a relevant event brings the same continuation forward.

The implementation lives in `form/form-stdlib/bml/bml-demand-jit.bml` and
`bml-demand-jit-owner.bml`; the shared protocol is described in
[`live dynamic diagnostics`](../docs/live-dynamic-diagnostics.md).

Where the code still meets surprise with a code: `./fkwu observe/choice-reading-run.bml` counts the `-1` returns and
the control-invite uses outside `control/`, and names the files carrying the most returns — a mirror for the next
movement, not a gate.
