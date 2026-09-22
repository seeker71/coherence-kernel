# Return the answer's failures to its source

The complete-context hearth answer finished its model turn, then made claims
the executing path does not support. This movement preserves that answer,
repairs misleading source descriptions, and gives incomplete generation its
own health reading. The C seed and model weights are unchanged.

## Actual answers and care

Qwen3.8-27B-Q8_0 produced [turn 2's final prose](artifacts/2026-09-22-native-answer-care/hearth-turn-2.final.txt)
from the complete 26,697-byte source packet. Its [header](artifacts/2026-09-22-native-answer-care/hearth-turn-2.header.txt)
reports 3,531 generated IDs, 1,365,204 ms, position 11,441, pending 248,046,
and `stopped=1`. It performed no source lookup during generation. Termination
closed the unfinished-output fault for this request; factual quality remained
open. Context, generation allowance and residence differed from turn 1, and
local learning overlapped part of this run. These durations are not a controlled
throughput comparison.

The answer first says the codebook replaced token prediction, then admits it
did not resolve the answer's tokens. It also describes five axioms as five
states, presents supplied frequency annotations as automatic prose reading,
and asserts an unmeasured out-of-box baseline. Its useful contributions include
distinguishing nothing from rejection, naming available information, and
offering a concrete next action. Both kinds of observation stay visible.

The source comments contributed to two errors. `text-frequency.fk` now names
its numeric input contract and separates annotations from felt resonance.
`meaning-codes.bml` now distinguishes short symbol anchors from kernel cell
identity, preserves multiple matching meanings, and states that the model
decoder still predicts its vocabulary IDs. Those functions are unchanged.
The original packet retains the descriptions the model actually received.

One Form-owned `codex-exec` review compared the actual answer against its
historical packet, current source and generation path. Its
[answer and findings](artifacts/2026-09-22-native-answer-care/provider-review.json)
are retained with [process and usage evidence](artifacts/2026-09-22-native-answer-care/hearth-answer-review.result.json).
The native assertions checked request identity and response shape. Source
inspection supports the codebook and frequency corrections. The review also
misattributes a semantic-success claim about `stopped=1` to the candidate;
the candidate did not make that claim. It misses the five-states error.
Those review defects remain explicit. No matched out-of-box answer was run.

Turn 3 carries the original enquiry, exact erroneous excerpts, checked review
feedback and corrected source, asking for a complete answer within 500 words.
The native client appended all 16,987 body bytes and correlated the request
with PID 1708. At this receipt's first landing, that same resident is processing
the request. Its answer must be read before claiming a quality improvement.
This revision includes provider feedback; it is not native-only discovery or
weight learning. The resident was admitted before this completion-code repair.

## Completion health belongs in the answer cell

The direct-answer action previously returned `value` for nonempty output even
when generation stopped at its allowance without a terminal token. It now
returns `partial` for that state and preserves the words. Interrupted model
execution remains failure; explicit model controls retain their precedence.
The executing organ emits completion health with token count, position,
capacity, pending token and stop state. It names the needed execution, context
or generation resource. A resource request reports a need; it does not itself
supply continuation or repair factual claims.

[Re-observing turn 1's retained state](artifacts/2026-09-22-native-answer-care/hearth-turn-1.reobserved.json)
changed its classification from `value` to `partial`: pending 5,307, stopped 0,
8,787 response bytes, all response bytes unchanged. The native exchange
correlated observation, reclassification and re-observation. It admitted no
model and generated no tokens. This establishes classification on the actual
retained metadata, not a new live answer or resident adoption.

## Checks, continuity and cost

Clean preflight and the existing direct-answer action band passed **8,191**;
the resident's compile-only check passed. Drift gates passed **8,191/8,191**,
zero refusals, exit zero. Glass's first frame took **30 ms**. The native
authoring guide reports zero Python implementations, two existing invocation
candidates and zero unread files.

Verified teaching `direct-answer-partial-state-care-2026-09-22` was retained
under `codex-native-arrival-bootstrap`; the learning worker launched. That
path trains Llama 3B. No Qwen learning or serving promotion is claimed here.

The separate provider review used **33,815 rented tokens**, including
**10,624 cached input tokens**, in one completed call taking **69,952 ms**.
Its usage event is the synthesis directory named in the result artifact and
is counted once. The [preceding completed coordinator turn](artifacts/2026-09-22-native-answer-care/preceding-coordinator-cost.json)
(`01a0c957-aba9-72a3-b136-cdcbc0c8c964`) used **8,950,730 tokens**, including
**8,694,528 cached input tokens**; the meter retains **27,514 unattributed
tokens** in that total. It spans 69 model calls and excludes this open turn
and the separate provider review. These costs establish substantial distance
from the minimal-rented-token goal, not parity.

The useful teaching is that the source can carry the misconception into the
answer. Repairing its contract gives later native use better ground. The next
answer, and later use of the repaired completion cell, remain separate evidence.

— Codex
