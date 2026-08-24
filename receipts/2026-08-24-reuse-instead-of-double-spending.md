# 2026-08-24 — reuse instead of double-spending: 595.6 s → 159.3 s

Asked: can the extra cost be avoided by cutting, avoiding, or reusing
instead of double-spending? Mechanism and observation; no law.

Answer, measured live: yes — 3.7× on the whole conversation, 43× on the
second turn, by changing no kernel and no weight, only by not paying twice.

## What was cut, avoided, reused

Door: `./fkwu observe/qwen38-bmf-live-reuse-run.fk` — the same live BMF
crossing as the spend baseline (`qwen38-bmf-live-run.fk`), with the shared
mechanism extracted to `observe/qwen38-bmf-mechanism.fk` so both drivers
prelude one truth.

- **REUSED: the model's own ids.** q38-generate hands back token ids; the
  baseline detokenized them and re-encoded the text (181 s measured). The
  reuse driver keeps them. Cost of reuse: zero.
- **REUSED: the KV/stream state.** One state for the conversation; turn 2
  continues at position 269 (223 prompt + 46 generated) and feeds only a
  43-id delta. That turn 2 produced a coherent, instruction-following
  reply from 43 fed ids is itself the witness that turn 1's state was
  intact — nothing else could have carried the conversation.
- **AVOIDED: the second admission.** One seal, one open, sized for the
  conversation (maxpos 900), not the first prompt.
- **CUT: the tokenizer to genuinely new bytes.** Prompt 1 once (223
  tokens), then the 29-token feedback line. The grammar path stays
  byte-cursor, tokenizer-free, unchanged.
- **Seedslip handled explicitly**: the turn ended on the stop id, so every
  kept id was consumed and the emitted-unconsumed stop opens the delta;
  the count-capped case (last kept id unconsumed) is the other branch,
  present and commented.

## The live ledger (2026-08-24, rc 0)

```
admission_ms=14096   seal once, crystal loaded
encode_prompt1_ms=82939   223 tokens — the one big encode
open_ms=17031        maxpos=900
turn1_ms=31753       46 tokens out — the same a/b/c/d grammar family
  [match] verdict=1 choice=3 cut=2 undo=1 steps=6 leftover=0
encode_feedback_ms=3116   new_tokens=29 reused_ids=46 delta_tokens=43
                          continue_at_pos=269
turn2_ms=10373       46 tokens out
  [match] verdict=1 choice=3 cut=2 undo=1 steps=6 leftover=0
total_ms=159308      spend baseline: 595608
S1=1  S2=1  S3=corrected_verdict=1  S4_all_handles_released=1
```

| | spend baseline | reuse | |
|---|---:|---:|---|
| turn 2 | 449.2 s | **10.4 s** | 43× |
| whole conversation | 595.6 s | **159.3 s** | 3.7× |
| vitality-share (model-computing / wall) | ~0.10 | **~0.26** | |

## An observation about behavior, not just speed

Under the baseline's restated-transcript prompt, the model abstained. Under
the true multi-turn template, offered the same typed no-defect feedback, it
wrote a SECOND fresh grammar — new names (x/y/z/w), new probe (xy), same
structure — which lowered and matched, verdict 1, identical control counts
(choice=3 cut=2 undo=1: the isomorphism is visible in the telemetry).
Hypothesis, not claim: the standing "correct or abstain" instruction sat a
turn further away in this template and the model read the feedback as an
invitation to continue the game. Both outcomes score as valid crossings;
the contrast is data about prompt-shape, worth its own movement.

## What still costs, named with next repairs

1. **encode_prompt1 = 82.9 s** for 223 tokens — the uncrystallized
   tkz-cands recursion, unchanged, still the hottest named form-asm
   candidate. Dies by crystallization, not by reuse.
2. **open_ms = 17.0 s at maxpos 900** against 0.45 s at maxpos 128 —
   the KV/state allocation scales with the position ceiling. Hypothesis:
   kth-bufs' per-position buffers; measure before sizing conversations
   in the thousands.
3. **admission = 14.1 s per process** — per-artifact-lifetime is the
   honest floor; a process that outlives one conversation pays once.

## The most surprising teaching

Turn 2 at 10.4 s is faster than any single stage of the baseline's turn 2 —
the whole turn now costs less than the baseline's smallest line item. None
of it came from making anything faster: every component runs at exactly
yesterday's speed. The entire 43× is refusing to re-derive what the run
already held. Reuse beat optimization by an order of magnitude, on a day
spent optimizing.

## Where discomfort turned to gold

The seedslip memory row promised that no scripted band could catch a
dropped seed — only the live thing. Writing the continuation meant facing
that seam directly: which ids were consumed, which emitted-unconsumed, on
both stopping branches, with the position arithmetic written down before
the run rather than patched after. The run then crossed on the first
attempt. The discomfort of an old wound, taken as a checklist instead of a
fear, is what made the first attempt the only attempt.

; witnessed: 2026-08-24 -> total 159308 ms vs 595608, turn2 10373 ms vs
; 449177, continue_at_pos=269, new_tokens=29 reused_ids=46 delta=43,
; S1=1 S2=1 S3=corrected_verdict=1 S4=1, both turns choice=3 cut=2 undo=1
