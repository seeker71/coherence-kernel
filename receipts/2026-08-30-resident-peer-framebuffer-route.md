# Resident peer writes opaque lifecycle frames

Signed: Codex, 2026-08-30

## Crossing

`observe/form-cli-peer-agent-live.fk` now takes the local Qwen/Metal route
through a resident peer that records `prefill-ready`, `task-begin`, and
`task-terminal` in the kernel framebuffer.  The small Form adapter is
`form/form-stdlib/form-cli-peer-live-framebuffer.fk`; its high-grammar source
of truth is `form/form-stdlib/bml/form-cli-resident-continuity.bml`.

The frame payload is only `(turn, phase-code, public-signal)`.  It carries no
task, source, model, prompt, or answer bytes.  The existing peer remains one
stdin process: weights and KV are admitted/prefilled once, then later tasks
reuse the same live session.  This is a native fkwu/Metal path with no HTTP,
llama-server, Ollama, or provider call.

## Witness

- `form-cli-peer-live-framebuffer-band.fk` returned `511`.
- `form-cli-peer-live-grammar-band.fk` returned `511`.
- `form-cli-peer-direct-answer-action-band.fk` returned `511`.
- `form-cli-peer-agent-band.fk` returned `8191`; its warm native run was 5 ms.

The direct task route uses the resident remaining context instead of a fixed
answer-token ceiling.  A malformed or stopped local turn remains a typed local
result; it does not silently fall through to a remote model.

The first live task exposed one direct-route wiring error: task observation
already returns the live `fcms` session, rather than a wrapped peer result.
The selected direct or recipe action now receives that raw session unchanged;
it cannot accidentally substitute the session-reason field for the model.

## Direct user-turn observation

The selected action now owns a distinct `direct-user` crossing.  It closes the
previous assistant stream, enters the task in a ChatML `user` turn, and opens a
plain `assistant` turn.  Recipe/source work remains on the tool-observation
tail.  The high-grammar rule is
`direct-answer-enters-as-user-turn-then-opens-plain-assistant-turn`.

On the real `Qwen3.8-27B-Q8_0.gguf` Metal route, the original 552-byte
objective reached `direct-answer/observe=begin`, then
`direct-answer/observe=value`, then `direct-answer/run=begin`; this establishes
that the bytes entered the new user-turn rather than the old tool tail.  The
initial residence prefill was 166,063 ms and the user-turn observation span
was 138,584 ms.  No provider callback or fallback occurred.

Decode produced no token or terminal frame across five 30-second observation
windows, so the live channel applied an explicit interrupt and released the
process.  This is a timeout signal, not an answer.  The next local crossing is
native per-token/progress publication plus a model-stop terminal frame, so a
long decode can remain observable and controllable without imposing a fixed
answer-token limit.

`fcms-walk` now emits opaque checkpoints after 1, 2, 4, 8... native decoder
steps.  The event carries no token id, text, task, or model bytes; the
process-local framebuffer holds only the stage and its completed-step count.
This keeps an unbounded local turn visible with O(log n) frames rather than a
second O(n) token stream.  The dedicated decode-checkpoint band returned
`127`, and the resident BML grammar returned `4095`.  A future live answer
receipt still needs to establish the model's stop terminal and measured decode
rate under this compressed channel.

## Original-objective local completion

The exact 552-byte objective was then answered by the real
`Qwen3.8-27B-Q8_0.gguf` residence.  The cold prefill was 165,529 ms; its
direct user-turn observation was 68,986 ms; decode returned with model-stop
after 890 local tokens and 356,446 ms.  Its terminal receipt carried
`callback-calls=0`, `route=direct-answer`, `stopped=1`, and one local
observation.  Checkpoints reached 1, 2, 4, 8, 16, 32, 64, 128, 256, and 512;
the next would be 1,024, so no missing event is inferred from model-stop at
890.

This proves the mechanical local route: one Qwen residence can receive the
original objective, answer, stop, and release without a provider or HTTP
crossing.  It does not admit the answer's prose as source knowledge: the
model constructed unverified CLI measurements and names, so it has zero
knowledge/mutation contribution.  Source-world query-and-receipt injection is
the remaining semantic crossing before a Form-specific answer can be called
grounded.

## Resident teaching overlay

The existing Form teaching overlay now enters `fcpa-bootstrap` before the
resident's first local reply.  The previous bootstrap body remains a separate
core function; the overlay wrapper is one explicit context seat rather than a
second prompt truth.  Its BML rule is
`resident-bootstrap-carries-current-form-teaching-overlay-before-local-answering`.

The grammar band returned `8191`, the peer band returned `16383`, the direct
action band returned `1023`, and continuity returned `65535`.  This teaches
the existing local curriculum without a provider crossing.  It is not source
retrieval: an answer still enters the source-world model only after a strict
query and returned current-answer receipt.

## Honest floor

The local peer answer call has not yet yielded a completed terminal frame to
the outer collector, and there is no completed, comparable provider fallback
row.  Therefore no claim of a 10% remote-token result is made here.  The next
crossing is a persisted native frame reader that can receive the terminal
receipt from the already-running peer, followed by a closed local-versus-
provider comparison using the retained provider quantities.

The exchange stayed alive by putting progress in the resident body's own
framebuffer rather than adding remote polling.  The surprise is that a warm
peer proof already returns in milliseconds; the discomfort is now only the
outer receipt boundary, where it can be observed and changed.
