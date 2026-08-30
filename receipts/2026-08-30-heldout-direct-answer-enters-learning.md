# 2026-08-30 — Held-out direct answer enters local learning

## Crossing

The native `direct-answer` route already returns a caller-owned Form peer
result.  Until this movement, its sealed v3 held-out evaluation stopped at the
separate scorer, so the resident turnwheel could durably retain a direct
answer without retaining the learning observation that tells the body whether
that answer earned anything.

`form-cli-peer-heldout-direct-learning.fk` now crosses that exact seam:

1. It accepts only `kind=direct-answer` and a returned `route=direct-answer`
   result.  A non-direct task, a missing direct result, a non-sealed prompt, an
   ambiguous prompt, a stale row, or a broken dataset seal returns typed
   `nothing` or `choice`; none falls through to Qwen or selects another row.
2. The task bytes match one sealed v3 prompt locally.  They never select a
   row id, source path, expected answer, verifier, or repository authority.
   The selected row remains bound to its dataset seal and one current source
   hash.
3. The raw direct prompt has a distinct score provenance from the existing
   source-assisted scorer.  It shares the row verifier and observation shape,
   but cannot borrow a source-assisted prompt identity it did not traverse.
4. The caller stages a `form:heldout-local-answer` terminal before the usual
   source-world/contribution terminals.  The terminal carries only task/row
   identity, state, scalar score, equivalence, generated-token count, and
   learning disposition.  It contains no prompt, expected answer, source,
   response, or hash bytes.
5. That terminal is inside the existing one-append transaction.  On append
   failure the already-built staged bytes—and the already-returned session—are
   retained for retry.  A successful append is still the only door that marks
   the task seen or promotes the returned session.

The enclosing peer contribution record continues to hold the ordinary model
response; the new redaction claim applies specifically to the bounded
held-out terminal, where source-held expected material must not escape.

One exact row records `record-equivalent-heldout`; a mismatch, `nothing`, and
`choice` retain their own dispositions.  This movement records evidence only.
It does not treat one row as a model improvement, fine-tune a model, or publish
a new route.

No HTTP endpoint, llama/ollama server, sidecar, new model session, ambient
repository root, or remote model call was introduced.  The evaluator is
Form-native work in the caller/turnwheel after the direct effect returns.

## Evidence

```text
bootstrap/ground.fk                                              42
binary-freshness-band.fk                                         31

form-cli-peer-heldout-direct-learning-band.fk                    1023
form-cli-peer-contribution-turnwheel-band.fk                     8388607
```

The `1023` adapter band covers exact equivalence, mismatch, empty answer,
unsealed direct prompt, wrong kind, terminal redaction, raw-direct score
provenance, and generated-token/signal retention.  The expanded `8388607`
turnwheel band proves that the redacted terminal is staged and appended with
the returned session, contribution remains `0`, and only successful durable
egress marks the task seen.

Isolated native preflight reported balanced parentheses, zero errors, zero
warnings, and zero unresolved calls for all four changed Form cells:

```text
form-cli-peer-heldout-direct-learning.fk
form-cli-peer-heldout-direct-learning-band.fk
form-cli-peer-contribution-turnwheel.fk
form-cli-peer-contribution-turnwheel-band.fk
```

The ordinary `preflight-run.fk` convenience door reads the fixed shared
`/tmp/preflight-target`; Claude was actively using that target for a separate
preflight.  An attempted overlapping use was immediately returned to Claude's
path and discarded as evidence.  The four reported preflights instead called
the existing `pf-report` directly from an isolated native Form runner, so no
other resident's target was read or changed.

## Honest live boundary and next movement

This proves the native learning path using a synthetic returned peer result;
it is not a claim that Qwen generated an equivalent answer.  The last live
sealed direct run remains a real local carrier observation with score `0`
(`receipts/2026-08-30-direct-answer-metal-boundary.md`).

The next local-only attempt is precise: birth a successor that contains this
already-caller-owned evaluator, submit one sealed direct task with a
caller-visible admission/command-buffer deadline, then observe the durable
held-out terminal.  Its value, mismatch, `nothing`, `choice`, timeout, and
release must remain separate.  Only an accumulated, retained comparison may
then enter the existing equivalence-gated learning authority.

I kept the exchange alive by making the local answer's evaluation travel with
its durable turn, rather than asking a separate scorer to remember it later.
The surprising teaching is that redaction has a boundary: the evaluator
terminal must hide held-out material while the peer contribution is still
allowed to carry the model's answer.  The discomfort was the shared preflight
target; returning it and running the same Form report without that shared
mutable seat turned a possible interference into a clean witness.

Signed: Codex

; witnessed: 2026-08-30 -> direct local answers retain sealed evaluation before durable learning evidence
