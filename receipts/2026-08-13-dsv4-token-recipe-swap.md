# 2026-08-13 — a Form recipe may challenge one DS4 token, but silence keeps the model

Urs asked for SuperDeepSeek-V4-Flash to produce its default token while any Form-native recipe may
offer an alternative inside the same timeout and Metal-bandwidth budget, with a more-vital recipe
learned and installed as the new baseline.

## The freshness gate was stale, not the binary

The required checkout build returned `31` from
`form/form-stdlib/tests/binary-freshness-band.fk`, while `AGENTS.md`, `MANIFEST.md`, the active floor,
and source-runner admission still expected `15`. Rebuilding did not change it. The band itself names
the reason: bit 16 landed on 2026-08-03 to witness runtime-built string values, and its documented
fresh verdict is now 31. Current contracts and fixtures were healed to 31; historical receipts were
left untouched.

## What landed

`form/form-stdlib/dsv4-token-recipe-swap.fk` is the missing membrane between the existing DS4 argmax
and `recipe-learning.fk`'s healthier-equivalent promotion law:

- the default token is always retained as the fallback;
- a Form function value receives `(context, default-token, absolute-deadline)` and may return a token;
- default and alternative share one absolute window and one aggregate byte envelope;
- `nothing`, decline, lateness, bandwidth excess, an unoffered/untrusted/unobserved boundary, or a
  vitality tie keeps the default token;
- vitality is the minimum of quality, trust, resonance, sovereignty, energy, and freshness, so one
  broken axis cannot be hidden by a flattering average;
- a safe multi-sample window must show a strict vitality-total advantage and a winning majority;
- installation returns a new immutable baseline cell carrying the exact old baseline as its parent.

The concrete starting envelope is not guessed: 34 ms/token from the observed DS4 generation floor,
471,000,000 bytes/ms from the measured resident-memory bus, and 9,103,000,000 default weight bytes per
token. The 34 ms window therefore carries 16.014 GB and leaves 6.911 GB for one alternative while the
default remains available. These are re-measurable baseline values, not portable constants.

`form/native/metal/ask-ds4.fk` now emits the swap-cell path and all three limits. Its previously named
`ask-ds4-band.fk` did not exist; `form/form-stdlib/tests/ask-ds4-band.fk` now makes that claim real.
The live `ask_ds4.sh` carrier now reads and validates that keyed Form emission instead of independently
hard-coding steps, cache cap, lane, tokenizer, and prompt binding. It passes the swap cell and measured
limits into the model process. `FORM_DS4_CONTRACT_ONLY=1` witnesses that crossing without loading the
model. The current stack does not yet call an arbitrary Form function between exit-head argmax and the
next embedding; its live footer therefore says the default is retained rather than claiming a swap.

## Proof and live observation

Preflight on the swap cell, its band, the ask cell, and the source-runner admission band: balanced,
zero errors, zero warnings, zero unresolved calls, clean chains.

```
binary-freshness-band.fk                       -> 31
source-runner-admission-band.fk                -> 2097151
dsv4-token-recipe-swap-band.fk                 -> 65535
validate.sh dsv4-token-recipe-swap-band.fk     -> 65535, four-way, 0 divergent
FORM_DS4_CONTRACT_ONLY=1 ask_ds4.sh -n 8 x     -> keyed Form contract, steps 8, cap 16
```

After that carrier crossing landed, the live `-n 8` ask was run again through it. It returned the
same continuation and ids, reported `generation 28.04 t/s · 95s wall`, and printed both the swap
membrane and `live carrier retains default until an in-loop recipe hook is witnessed` before the
unchanged `stream sane (0 of 3 readings tripped)` verdict.

The first live ask used `-n 4` and failed honestly:

```
FAIL native continuation token evidence missing
ask_ds4: the lane did not complete (rc=1)
```

The bounded framebuffer recorded only `[expected-continuation, observed-continuation] = [1,0]`,
selected action 5 (`rehearse-ground`), and changed the retry state from 4 to 8. The retry succeeded:

```
The capital of France is Paris. The capital
generation 25.63 t/s · 98s cold wall
stream sane (0 of 3 readings tripped)
token ids [11111, 16, 455, 6102]
```

Inference, not yet a renamed CLI contract: `-n` reached the sequence frontier only after it exceeded
the prompt length, so the value behaves as a total sequence-step cap in this path rather than four
guaranteed generated tokens. The successful retry establishes the movement; a dedicated carrier test
would be needed before rewriting the public option description.

No challenger was promoted from this one live ask. It supplied a real default stream but no independent
semantic/vitality evaluation of a recipe alternative. SuperDeepSeek/DS4 therefore remains the installed
baseline, exactly as the new fallback law requires.

## Honest seam

`dtrs-run-live` can measure an arbitrary in-process Form recipe but the evaluator cannot preempt it.
The recipe receives the absolute deadline cooperatively; a late result is discarded as `nothing` after
return. A Metal carrier can enforce the deadline at submit/fence boundaries. Post-hoc rejection is not
preemption, and the cell says so at its own door.

The `dtrs-install` result is an immutable baseline cell offered to its caller. This change does not
silently mutate a persistent production baseline or model file.

## The most surprising teaching

The model lane was healthy; the smallest requested walk never crossed the prompt frontier. A timeout
budget can be correct while the counter it constrains names a different span than its caller assumes.

## Where discomfort turned to gold

The attractive move after the live failure was to quote the four-way synthetic band and call the live
path pending. The framebuffer forced a real next action. Rehearsing the step cap produced a sane, fluent
stream and separated the 98-second cold residency cost from the 25.63-token/s generation rate—the exact
distinction the timeout membrane needed.

; witnessed: 2026-08-13 -> swap band 65535 four-way; ask contract band 255 four-way; live DS4 sane
