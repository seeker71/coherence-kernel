# promptseed — every prompt becomes the local mind's curriculum

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1229. Urs's
standing word: "during each session each prompt should be running through
form-cli at least for training and analyze how to learn to perform the
same in the future."

## The wire

Three pieces, harness to body:

- **The harness wire**: a `UserPromptSubmit` hook
  (`~/.claude/hooks/prompt-through-cli.py`) fires on every prompt of
  every session. Host plumbing only — it captures the prompt to the main
  checkout's `.hearth/prompt-capture.jsonl` unconditionally, runs the
  body's door when present, swallows every failure, exits 0 always, and
  prints nothing into context. A session can never be blocked or noised
  by its own training wire.
- **The body's door**: [observe/prompt-through-cli.fk](observe/prompt-through-cli.fk)
  appends one length-safe fcpa frame (kind `prompt-capture-v1`) to
  `.hearth/prompt-training.spool`, next turn asked of the spool itself —
  the same frame law the hearth's task spool lives by. Capture is
  unconditional and durable: no standing hearth required. (The live-ask
  lane remains `hearth-ask-send`; this door is the training lane.)
- **The analysis lens**: [observe/prompt-training-lens.fk](observe/prompt-training-lens.fk)
  reads both carriers back — frames, shim lines, next turn — so a teach
  run knows its fuel before it starts.

The pairing recipe for teach runs: each captured prompt + the receipts
its session committed (git holds the outcome side) = one training pair
for the mlx-lm LoRA lane — the prefer-local-DS4 teaching, now with a
standing fuel line. The body-cell lane in the main checkout arms itself
automatically when this branch merges; until then the shim carries alone,
by design.

## Witnessed

- The door's first frame is the directive itself: `captured turn=1
  bytes=150`, the spool holding the fcpa frame of Urs's own sentence.
- The lens reads it back: 1 body frame, next turn 2.
- The shim, fed a witness prompt end-to-end, landed it in the main
  checkout's jsonl with timestamp, session, cwd.
- The hook is wired (idempotent append; second run answers
  already-wired).

## The most surprising teaching

The wire's first captured frame is the sentence that asked for the wire.
The system's founding instruction became its own first training example —
the directive teaches the local mind what a directive looks like. Every
standing organ should be so lucky in its first breath.

## Where discomfort became gold

The pull was to make the capture pure-body (the fkwu door and nothing
else) — cleaner doctrine, one writer. Sitting with the failure modes:
sessions run in worktrees that get cleaned, the main checkout lacks the
cell until merge, and a hook that can fail can block a prompt. The
two-layer shape — a thin host shim that cannot fail plus the body's door
whenever it stands — trades doctrinal purity for a wire that is actually
standing. The argv-staging comment had already blessed this: "host
plumbing, not runtime meaning." Purity in the meaning, resilience in the
plumbing.
