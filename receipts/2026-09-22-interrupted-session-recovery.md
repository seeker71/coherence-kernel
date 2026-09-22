# Native inspection of an interrupted session

Codex, 2026-09-22. Urs stopped response trials and directed the work toward
actual tasks and their real steps. The unlaunched context-allocation retry was
removed. This change repairs inspection of the interruption already present
in the working tree. It launches no replacement generation.

## The actual failure

The retained comparison process stopped progressing during prompt preparation,
after 1,472 of 2,694 positions. Its 900-second supervisor returned status 125
after 902,264 ms, with `cleanup-incomplete` and a remaining process member.
No session result was written. The original [process record](artifacts/2026-09-22-response-session-recovery-original-process.json)
preserves those observations. They establish neither an answer-quality result
nor the cause of the stall. Later OS inspection found both recorded processes
absent. That later reading does not rewrite the historical cleanup failure.

Before this repair, repeating the public session command returned
`admitted-unresolved;no-restart`, the recorded owner PID and unknown historical
usage. Learning where the work stopped required separate filesystem and
process inspection by the coordinating agent.

## The real step now carried by Form

`frss-unresolved` now applies its own offered `inspect-evidence` action and
returns `retained_progress`. It reads the expected case paths from the retained
manifest, observes request/result/report/receipt presence and byte counts, and
compares the request with the manifest. It observes the recorded PID separately
from the admission record. A present PID may be a reused process identity;
an unavailable observation stays unavailable.

The [actual interrupted-session reading](artifacts/2026-09-22-response-session-recovery-interrupted.json)
shows:

| Observation | Result |
| --- | --- |
| Manifest readable | Yes |
| Retained request | 13,645 bytes; matches manifest |
| Recorded PID now | Absent |
| Result, report, receipt | Absent |
| New model or provider work | None |
| Historical usage | Unknown without the sealed summary |

The [execution audit](artifacts/2026-09-22-response-session-recovery-audit.json)
records 175 ms for that inspection. The same movement reads an actual
[completed session](artifacts/2026-09-22-response-session-recovery-completed-replay.json):
its saved result rows remain unchanged and replay starts no new model or
provider work. These are retained work records, with no synthetic case or
new generation. The [care flow](artifacts/2026-09-22-response-session-recovery-care.jsonl)
keeps the correlated observation, selected inspection, applied reading and
fresh manifest observation. A readable manifest leaves the missing answer
unresolved.

The useful change is concrete: the public Form door now carries the inspection
that previously required additional coordinating steps. The GPU/prefill stall
itself remains unexplained. This repair supplies its retained boundary without
claiming to have fixed that separate execution fault.

## Verification, cost and continuity

The public door and observation runner passed compile-only preflight with zero
errors or unresolved calls. Two authoring errors were corrected before execution:
a mismatched delimiter and a missing terminator on an expression-bodied function.
Failed preflights exited 1; direct compiler diagnostics exited 2. Actual retained
session checks then completed with exit 0. No evaluation answer was supplied as
a training target, and no runtime dependency or C source was added.
The required landing checks returned 8191/8191 with exit 0; ground returned 42
and binary freshness returned 31.

The [cost snapshot](artifacts/2026-09-22-response-session-recovery-coordinator-cost.json)
still names completed coordinating turn `01a0bc27-cb01-74a2-a78a-65ae13e5761b`:
8,713,496 tokens across 62 model calls, including 8,477,952 cached input,
151,559 uncached input, 55,830 output and 28,155 unattributed tokens. This is
the same previously observed turn, counted once. Current open-turn cost is
outside that snapshot. Zero new provider work inside this inspection is
separate from the coordinating agent's spend.

The procedural teaching was retained as
`interrupted-session-retained-progress-2026-09-22`; its learning worker launched.
It describes verified recovery behavior and contains no evaluation answer.
The preceding worker completed candidate generation 104; serving remained at
generation 5. That separate Llama learner supplies no claim about Qwen quality.
The native authoring guide reports 0 Python implementations, 2 invocation
candidates and 0 unread files. The counsel panel reads 0 orphans and 11/12
unobserved lanes, with no standing hearth.

Signed: Codex.
