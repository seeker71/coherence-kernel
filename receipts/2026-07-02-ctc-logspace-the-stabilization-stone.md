# 2026-07-02 — the stabilization stone: CTC forward in log space, four-way

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 08:10: "yes, please" — lay the next stones (log-space, then a real encoder, then
reafference). Laid the first: log-space stabilization.

## What was built

`model/ctc-logspace.fk` — the CTC forward in log space. The product-form alpha
(`model/ctc-loss.fk`) underflows on real-length sequences: a probability that is a product of
hundreds of per-frame terms rounds to 0 in double precision, and the objective dies (log 0 = -inf,
gradient 0). Every real CTC runs in log space; now this body does too. The recursion becomes:

```
log-alpha_t(s) = LSE( log-alpha_{t-1}(s), log-alpha_{t-1}(s-1), [skip] log-alpha_{t-1}(s-2) )
                 + log y_t(ext[s])
LSE(a,b) = m + log(exp(a-m) + exp(b-m)),  m = max(a,b)      ; stable — largest term factored out
```

log(0) is carried as a large-negative sentinel; LSE of two sentinels stays a sentinel.

## Witness — four-way, verdict 127 (fkwu = Go = Rust = TS)

`model/tests/ctc-logspace-band.fk`:
- **Correctness:** `exp(log-p)` equals `model/ctc-loss.fk`'s probability-space `p` on the toys
  ("a" → 0.75, "ab" → 0.25, exercising the skip), to 1e-6.
- **The point (why it matters):** 10 frames of per-frame probability `1e-40` give probability-space
  `p = (1e-40)^10 = 1e-400 = EXACTLY 0.0` (underflowed) — while log-space gives a finite `~-921`
  (= `10 · ln(1e-40)`), and the log-space loss is a finite `+921` where the probability-space loss
  would be `-log(0) = +inf`. Same computation, usable answer.

```
fkwu: 127   go: 127   rust: 127   ts: 127
```

Both new files balanced on the first write (depth 0) — the paren lesson from the gradient stone
held; no walker-caught defects this time.

## Honest floor (named)

This is the FORWARD in log space. The backward (beta) and the analytic gradient
(`model/ctc-grad.fk`) in log space — so training itself is stable at length, not just the loss
value — is the next refinement. Still toy scale, still free logits (no encoder over real signal).
Global native open-speech WER still 100; this stone, like the gradient, is upstream of it. But the
underflow wall that made real-length CTC impossible is down.

## The stones, so far

Four now stand where two nights ago there were none: `ctc-loss` (the objective), `ctc-train` (it
trains), `ctc-grad` (the gradient that scales), `ctc-logspace` (stable at length). Remaining named
rungs to a real WER: log-space backward/gradient, a real encoder over real features, the
reafference loop.

## The most surprising teaching this work left behind

The stone proves itself by DISAGREEING with its predecessor exactly where the predecessor is
wrong. `exp(log-p) == prob-p` where probability-space works; `prob-p == 0 while log-p == -921`
where it doesn't. A stone that only agreed everywhere would be redundant; this one earns its place
by matching in the safe region and diverging — correctly — in the region the old stone couldn't
reach. Progress is often a new tool that agrees with the old one until precisely the point where
the old one fails.

## Where discomfort turned to gold

The discomfort was momentum-doubt: four CTC stones in two nights, each honestly labeled "WER still
100," and the quiet worry that laying stones no headline number rewards is just elaborate
deferral. Witnessed instead, the underflow demo answered it concretely — probability-space
returning 0 on a sequence log-space handles is not abstract; it is the exact wall that stops a
toy from becoming real. Each stone removes one specific impossibility. The WER hasn't moved
because the wall it's behind takes more than one stone to breach — and tonight one more came down.

## Corpus

Row 630 **homomorphism** — a map that turns multiplication into addition, preserving structure
across a change of space (fresh; log is exactly this, and it is *why* log-space survives the
underflow the product form cannot).
