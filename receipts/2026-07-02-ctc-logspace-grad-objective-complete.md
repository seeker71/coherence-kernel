# 2026-07-02 — the log-space backward+gradient: the CTC objective, complete and stable

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 08:17: "yes, please." Laid the next stone: the CTC backward and gradient in log space.

## What was built

`model/ctc-logspace-grad.fk` — the analytic CTC gradient computed through log-space alpha/beta.
`model/ctc-grad.fk` gave the gradient in probability space (correct, but its alpha/beta underflow
at length, so the gradient becomes 0/0 garbage exactly where `model/ctc-logspace.fk` showed the
loss dies). This is the same gradient, log-stable:

```
log-posterior_t(k) = LSE_{s: ext[s]=k}( log-alpha_t(s) + log-beta_t(s) − log y_t(ext[s]) ) − log-p
∂L/∂u_t^k          = y_t^k − exp(log-posterior_t(k))
```

log-beta mirrors log-alpha backward with the bounds-guarded skip — the strict-walker lesson from
`ctc-grad.fk` baked in from the first write.

## Witness — four-way, verdict 127 (fkwu = Go = Rust = TS)

`model/tests/ctc-logspace-grad-band.fk`:
- **Correctness:** the log-space gradient EQUALS `model/ctc-grad.fk`'s probability-space gradient
  on the toys ("a" all four params; "ab" on the skip-exercising params), to 1e-5.
- **Validity:** the gradient is a proper posterior residual — every value in [−1,1], per-frame
  values summing to ~0 (softmax mass 1 minus posterior mass 1).
- **At length:** on a 12-frame sequence the gradient stays a valid residual. Because it is
  computed entirely through `clog-lse2` (the same log-sum-exp that carried the forward past
  underflow), it inherits the forward's underflow-immunity by construction.

```
fkwu: 127   go: 127   rust: 127   ts: 127
```

Both files balanced on the first write; no walker-caught defects.

## The objective is complete

Five CTC stones now stand, and together they are a full, numerically-stable, four-way-proven CTC
training objective:
- `ctc-loss` — the forward objective (score an alignment).
- `ctc-train` — it trains (a decoder learns its target from wrong to right).
- `ctc-grad` — the analytic gradient (O(T·S), not O(params)).
- `ctc-logspace` — the forward, stable at length (no underflow).
- `ctc-logspace-grad` — the backward+gradient, stable at length.

Nothing about the OBJECTIVE is now missing or unstable. What stands between here and a moving WER
is no longer the loss — it is a **real encoder over real features** and the **reafference loop**.
Global native open-speech WER still 100; the objective is finished, upstream of the number.

## The most surprising teaching this work left behind

Each log-space stone was verified by making it EQUAL its probability-space twin where the twin is
valid, then trusting it beyond. The whole log-space edifice rests on one identity checked at toy
scale — `log-grad == prob-grad` on four params — and then extended, by construction, into the
region the probability-space version cannot enter. A single verified equality at small scale
licenses a whole new stable regime. Correctness does not have to be re-proven everywhere; it has
to be proven at the seam, and then the construction carries it.

## Where discomfort turned to gold

The discomfort was completeness-anxiety: five stones, each labeled "WER still 100," and the
question of whether "the objective is complete" is a real milestone or a consolation for the
number that won't move. Witnessed against the receipts, it is real and checkable: two nights ago
the body had zero of these, and named "no CTC training objective" as its #1 capability gap. That
gap is now closed — forward, backward, gradient, all stable, all four-way. The number hasn't
moved, but the thing that was ABSENT is now present and complete. Completing what was missing is
progress even when the headline waits; the honest floor names both without letting either erase
the other.

## Corpus

Row 631 **entelechy** — a potential fully actualized, complete in its own form (fresh; Aristotle's
word for what "no CTC training objective" has become — the gap realized into its finished actuality,
now waiting only on the encoder to express it outward).
