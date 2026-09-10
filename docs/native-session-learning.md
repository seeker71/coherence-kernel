# Native session learning

Session learning is enabled by default. Form owns collection, routing,
supervision, tokenization, full LoRA gradients, Adam continuation, assessment
and checkpoint publication. The private home is `.hearth/session-learning/`,
with directory access restricted to its owner.

The integration points are the real CLI doors:

- `learn` retains a caller's vocabulary teaching and offers it for LoRA.
- `code` records checked document targets, or the actual incomplete status.
- `heal` records every candidate/check outcome, including the final rented
  outcome when that existing fallback is used. An unsuccessful proposal is
  never trained as a correct answer.
- An interactive session closes with its observed completed-command count and
  elapsed time. This is status learning, not a claim to understand its transcript.
- `session learn|{"prompt":"...","completion":"..."}` accepts an explicit
  local correction. Optional `session` and `event` values provide stable replay
  identity. `session ask|question` uses the serving adapter or the disclosed seed.

Examples are immutable and keyed by session/event identity. Replaying the same
event does not add a gradient step. Reusing an identity for different evidence
is refused. Evaluation requests are excluded. No transcript, prompt or answer
is copied into Glass or the diagnostic framebuffer. Training text and generated
streams remain private in the hearth; telemetry carries identities and counts.

One supervisor serializes the learning queue and records the learner's actual
exit and stderr. Recording does not wait for GPU training during a session.
Normal session close and the one-shot embodiment door retain the parent process
until the supervisor returns; an external interruption keeps the immutable queue
for the next drain. Each new observation
gets one full-gradient round, including rehearsal of the last promoted example
when it has a different prompt. The next round restores the candidate's adapter,
Adam moments and optimizer step. Candidate generations and the serving selection
are separate: a failed promotion does not discard the learning attempt or
replace the working adapter.

`model/fixtures/session-learning/valid.jsonl` supplies two public held-out
sentinels. A private `config.json` may name a `validation` JSONL and a positive
`learning_rate`; the default rate is `0.00001`. The new example cannot share a
prompt with a held-out row. Rows are processed whole; the trainer reports
position or memory refusal instead of silently truncating a row. The one loaded
training model produces the before/after per-row scores as well as gradients.
When a deferred candidate differs from the serving adapter, that serving
adapter receives an additional comparison on the same rows.

Promotion requires improvement on the current example and no per-row regression
beyond `1e-6` against either the candidate or serving baseline. Every assessment
row's identity and supervised-token count are checked. The base, adapter,
optimizer, configuration and input data are content-bound. An interrupted round
with a complete generation can finish assessment without repeating its update.
All completed generations remain available; storage grows with experience.

After native execution or retrieval misses, `ask` can use the evaluated session
adapter when it has a promoted dialogue teaching. Coding and healing require a
promoted checked example in their respective scope before trying its proposal.
They rerun their original checks; an unsuccessful prediction is reconsidered
for that same prompt only after the serving adapter changes. An explicit coding
model is respected.
Generic generated prose is labelled an unverified proposal. All token streams
use the existing dynamic native generation path, including cancellation and
observed context/memory pressure. There is no fixed total generation budget.

Use `session status` for candidate and serving generations, optimizer step,
promotions, pending/refused examples, live-worker state, actual exits and paths
to evidence. `session pause` lets the current round finish and keeps examples;
`session resume` drains pending work. Stage events link actual per-row token,
gradient, GPU, checkpoint and assessment timings. Inference events identify
the model, adapter, LoRA pair count, token flow, streaming path and stop reason.

This integration trains the native **Llama 3B** adapter. It does not train the
Qwen base used by the coding continuation, the host Codex/Claude model, or
Whisper. It cannot intercept model calls made independently by those host apps.
Lower sentinel loss establishes that measured objective only; fewer rented
calls and broader coding quality must be measured on real subsequent requests.
The existing code and repair checks remain the decision at each such request.

Witnesses: `native-session-memory-band.fk`, `native-session-journal-band.fk`,
`native-session-routing-band.fk`, `native-session-learning-band.fk`,
`native-session-worker-band.fk` and `native-session-code-band.fk` under
`form/form-stdlib/tests/`. The public two-worker, real-3B witness is
`observe/native-session-homecoming-run.fk`; its input is a fresh evidence directory.
