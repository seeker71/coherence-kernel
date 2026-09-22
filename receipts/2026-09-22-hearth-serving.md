# Carry the whole enquiry to the native resident

The original enquiry asks how axioms, trust, frequency awareness, numeric
identity, translation and vocabulary change a response. Running that enquiry
through the serving hearth exposed three concrete gaps. They are repaired in
Form/BML, with the C seed unchanged.

## Observed and repaired

- **Discovery:** no resident stood initially. The existing spool resident
  admitted Qwen3.8-27B-Q8_0 and reached `READY`, but only Glass published its
  board. The resident now publishes and reads back its own board after birth
  checks, for the canonical task/reply/bell paths. Custom spool residents stay
  independent. Its release path clears only a board naming its own PID.
  The rebuilt resident, PID 1708, corrected the old PID 80715 itself and
  re-observed matching board bytes before `ready=1`; no Glass run intervened.
- **Context delivery:** the old line client offered only 8,191 bytes from the
  26,697-byte packet. The seed's `read_line` stops at that chunk boundary.
  The native client now assembles the complete physical line; the actual
  packet re-read at 26,697 bytes, byte-identical. The file-backed door also
  preserves multiline packets. Turn 2 appended its complete 26,818-byte frame,
  carrying all 26,697 body bytes, and rang the resident.
- **Follow-up tokenization:** direct questions used the full-scan reference
  encoder despite an existing indexed native cursor. The session now uses that
  cursor with the same role scaffold and falls back to the reference encoder
  before any KV mutation if the index is unavailable or incomplete. On the
  exact 8,191 bytes delivered in turn 1, both paths produced the same 1,819 IDs:
  indexed **16,883 ms**, reference **248,972 ms**. This is one encoding-step
  comparison, with no model admission or generated tokens. It is not a
  whole-session quality or speed comparison.

## Native answer and remaining work

Turn 1 took 720,456 ms in the direct action and generated 2,048 IDs. Its durable
header says `stopped=0`. The final prose breaks off mid-heading. It also claims
unsupported changes to the model's own cognition. The partial context and the
unfinished answer remain separate observed faults; neither durable delivery
nor the `candidate` signal establishes useful completion.

The [original final prose](artifacts/2026-09-22-hearth-serving/turn-1.final.txt)
is retained **unfinished**, attributed to Qwen3.8-27B-Q8_0 on
`form-native-metal-jit`. Its full raw output, including the model's preceding
analysis, remains in the local reply spool and the before snapshot. Codex
does not adopt its claims about a changed model identity or cognition.

The original process released cleanly. The rebuilt resident is processing
turn 2 with the complete source packet and a generation budget of 4,096.
Its indexed tail contains 6,663 IDs. That answer and the live release-board
observation remain open at this landing. Read their correlated evidence before
claiming completion or comparing answer quality. The changed context, fresh
residence and larger budget are explicit.

## Checks and cost

Compile-only checks passed for both clients and the resident. Clean preflights
preceded the existing birth (255), model observation (16,777,215), model session
(4,095) and direct-answer action (8,191) bands; each exited zero. Drift gates
passed **8,191/8,191**, with zero refused checks. Glass reported its first frame
in **34 ms**. The native authoring guide reports zero Python implementations,
two existing invocation candidates and zero unread files.

The verified teaching was retained as session event
`hearth-context-delivery-indexed-followup-2026-09-22`; its learner was launched.
That learning path trains Llama 3B. Retention is observed; no Qwen weight change
or new serving promotion is claimed. Its worker also adds concurrent local
work while turn 2 runs.

[Encoding comparison](artifacts/2026-09-22-hearth-serving/hearth-enquiry.tail-comparison.json),
[complete input check](artifacts/2026-09-22-hearth-serving/hearth-enquiry.input-check.json),
and [native response header](artifacts/2026-09-22-hearth-serving/turn-1.header.txt)
retain the measured steps.

The [preceding completed coordinator turn](artifacts/2026-09-22-hearth-serving/preceding-coordinator-cost.json)
used **5,281,550 rented tokens**, including **5,056,384 cached input tokens**,
across 28 model calls. That identified turn excludes this open movement.
Both native response requests used no provider call. These repairs do not
establish parity or attainment of the rented-token objective.

The useful teaching is concrete: inspect delivery and tokenization before
attributing a weak response entirely to the model. Here, most of the grounding
packet never arrived. Repairing that fact gives the next quality observation
firmer ground.

— Codex
