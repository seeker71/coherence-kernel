# A surprise is a choice point

Urs, 2026-09-28: *"issues and gaps are backtrack points that will choose a different option, all that is form
native and allows to choose without losing context and inline with the first choice … errors are not return
codes, they are form primitive backtrack points and we use choice to handle errors and unexpected issues."*

An issue met on the way — a failing check, a missing file, a band that reads short, a tool that stops, a
gap nobody named — is not an end and not a report to hand back. It is a point where the walk chooses again:
the intent of the first choice stands, what the failed option saw is carried forward, and the next option is
taken. The work continues in the same movement.

## In the work

- **Keep the first choice.** The intent that set the work going stays the intent. A different option serves the
  same end; it does not become a new task.
- **Carry what you saw.** The failed option's reading — the error line, the band's number, the diff that did not
  land — travels into the next option. Nothing learned is dropped, and nothing is retried blind.
- **Choose the next option and continue.** Another route, another tool, a smaller step, a repair at the cause.
  When the options at hand are spent, the walk answers nothing, honestly, with its memory: what was tried and
  what each attempt saw.
- **Continue around what only a person holds.** When an option needs what only Urs holds — his consent, his
  private world, his own configuration — the rest of the work goes on, and that one point is offered to him
  plainly, once, with what is known.

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

So in the code:

- **Failure is nothing, not a number.** A reading that did not arrive is `nothing()` (axiom-1), never `-1`, `0` or
  an exit code that a caller must remember to decode. A value that arrived is the value.
- **Alternatives are options offered to a choice.** Where code would test a code and branch, it offers its options
  to `oac-choice` or `oac-backtrack` and lets the ack decide.
- **An option that learned something says so.** It acks `oac-node(finding)`; the next option reads
  `oac-option-memory(offered)` next to `oac-option-args(offered)`.
- **Recovery is a handler, not a catch.** `oac-try(cell, args, handler)` — the receiving boundary decides.

BML offers its own defs as options (witnessed on fkwu, 2026-09-28):

```text
// preludes: control/offer-ack-core.fk control/choice-lane-core.fk
def first(offered) = oac-node("the band read 12 of 15; the candidate left sw-tokens unchanged");
def again(offered) = if nil?(oac-option-memory(offered)) then oac-nothing() else oac-one(15);
let walk = oac-backtrack-walk(list(first, again), list(gap), list());
```

The body's own loop walks this way: `observe/native-turn-run.bml` takes a gap as two options, and a turn that did
not turn its band green hands its band reading, the lane's status and its candidate's diff to the second option,
which asks the lane again in line with the first. Witness: `control/tests/backtrack-band.fk` (255).

Where the code still meets surprise with a code: `./fkwu observe/choice-reading-run.bml` counts the `-1` returns and
the control-invite uses outside `control/`, and names the files carrying the most returns — a mirror for the next
movement, not a gate.
