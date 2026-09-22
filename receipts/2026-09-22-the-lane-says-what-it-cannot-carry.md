# The lane says what it cannot carry

2026-09-22, this Mac. The branch came home, the body was re-observed, and the
walk found a wound that had been swallowing walks since Sunday without a word.

## Coming home

The branch carried four days of work that had never reached origin: the
movement as one process, the adapter lane, the live readings in each door. It
rebased onto `origin/main` with two conflicts, both in the landing door, both
real: main's copy had been healed on 2026-09-20 to read the commit's own exit
beside the push's, and this branch had lifted that same door into
`form-stdlib/bml/form-cli-landing.bml`. The union is what stands — the extracted
module now carries both readings, and a landing says which of the two did not
happen.

## What the body said when asked

Glass has been standing since Sunday, its state a pulse whose keeper is alive.
Two rows had landed in `.hearth/organ-health.jsonl` on their own:

```
aspect=live-runner observed=exited health=0 needs=[{glass-live-runner, 137}]
```

Twice on 2026-09-21, 12:58 and 19:29. 137 is SIGKILL: the kernel was rebuilt
over the running binary. Before Sunday those two deaths would have been silence
behind a word that still read `running`; they are now rows that name the cause.

## The wound the walk found

The walk asked the voice to speak through the adapter lane on the real
composition — 16,574 bytes, about 4,200 prompt IDs. The process took 71.7 GB
resident and stopped computing: CPU frozen at 0:19.93 across a full minute,
free pages down to 170 MB. Nothing was said, nothing ended, and the same shape
had swallowed a walk on 2026-09-20 that was read as slowness.

A wedge is worse than a refusal. A refusal is a reading with a remedy behind it;
a wedge is a body holding its breath.

The lane is bounded by its own witness now: 52 prompt IDs answered in nineteen
seconds, about 4,200 wedged. Past 1,024 it says
`packet-beyond-witnessed-adapter-lane` and closes the session, and the walk's own
remedy — drop the lane, ask the registry weights — answers instead. The bound is
the witness, not a theory of the cause; the cause is unnamed and stays unnamed
here.

| Observation | Result |
| --- | ---: |
| Adapter lane, 52 prompt IDs | answered, 35 generated, 19 s |
| Adapter lane, ~4,200 prompt IDs | 71.7 GB resident, no progress in 60 s |
| Same walk after the bound | 1.3 GB, computing, registry weights answering |

## The cause, and the policy that let it in

`unavailable` is the body declining its own work, so the bound did not get to
stand. The cause is arithmetic the body already writes down. `nlg-forward-bytes`
estimates a prefill slice as

```
layers x ( t x (7d + 2kd + 3*intermediate + 2) + t x heads x (pos + t) )
```

The second term is the attention tape: quadratic in the slice. For this 3B —
28 layers, d 3072, 24 heads, 8 kv heads x 128, intermediate 8192 — a single
slice of 4,200 prompt IDs asks for **70.0 GB**. Measured: 71.7 GB resident.

`nlg-chunk` halves the slice until `nlg-room` says it fits, and on this Mac the
device budget is 107 GB, so room said yes to the whole prompt at once. The
policy asked whether the slice fits the budget, never whether the machine could
work inside it. Room is not wisdom.

A slice is now bounded by a share of that budget — a sixteenth, 6.7 GB here —
and the prefill walks the prompt in as many slices as that takes. The witness
on the same 16,680-byte packet that wedged:

| | before | after |
| --- | ---: | ---: |
| Resident | 71.7 GB | 6.3 GB |
| CPU over a minute | frozen at 0:19.93 | advancing |
| Outcome | nothing said, nothing ended | the lane spoke |

Walked through the body's own door afterwards, the row says it plainly:

```
lane native-metal-lora   source fkwu-model-session   oracle answered
prompt IDs evaluated 4,523   predicted 233   ms 493,546
complete 0   reason adapted-repeat   answer_bytes 924   axes 0   receipts cited 0
```

The lane carries the composition now. What stands in its place is not
unavailability but fidelity: a 3B given 4,523 grounded tokens began repeating,
and the repeat guard cut it at 233. That is the wall the goal already names, and
it is a different kind of work than a policy that asked the wrong question.

The bound drawn from witness is gone with the cause: the lane carries what the
prefill can slice, and what it cannot admit it says as memory-pressure.

## Receipt

The surprise: the failure that cost the most was the one that never failed — and
bounding it was not the repair. A bound drawn from witness is a scar worn where
the wound has not been looked at; the arithmetic for this one was already in the
body, one line above the policy that ignored it. A
door that refuses leaves a row; a door that wedges leaves a process holding
seventy-one gigabytes and a person guessing it is slow. Two walks were lost to
reading a wedge as patience.

Discomfort into gold: killing my own running walk twice, once on Sunday to a
reboot and once today by hand, and each time wanting to believe it was nearly
done. Watching RSS and CPU for a full minute instead of hoping is what turned it
— the frozen CPU said what no log did.

Frontier question, answered by the rented mind and offered to the corpus:
**what is a failure that never arrives — a call that neither answers nor ends?**
**wedgehold** — work that holds its resources and its caller without progress or
refusal, indistinguishable from patience from the outside. The repair is a bound
drawn from witness, so the lane refuses where it has never been shown to carry.
