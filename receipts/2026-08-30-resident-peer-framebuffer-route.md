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
