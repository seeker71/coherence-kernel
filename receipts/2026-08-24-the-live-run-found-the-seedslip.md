# 2026-08-24 — the live run found what three green bands could not

Yes named the frame: "minimal safe" is fear-shaped and incoherent here. Take
the largest step whose effects, alternatives and verdict are witnessable. And
if the live GPU witness is outside the repair, name the floor **after**
attempting it, not instead of.

Attempting it is the whole receipt.

## The repair asked for

Defect 7 was mine: an over-long query died silent. One rolling window clipped
from the left at frame width ate the open mark of its own query, the frame
never completed, and the model asked and heard nothing back — no observation,
no reason, no status.

The repair is two named caps in the law rather than a bigger buffer:
`fhm-detect-cap()` before an open mark is seen, `fhm-hold-cap()` = 280 from it
onward. Once an open mark arrives the window holds **from** it and is never
clipped away; outgrowing the hold cap is an observation, `nothing` with reason
`query-budget-exceeded`, and no IO runs at all. Bytes only — no pretokenizing,
no ops table, no flattening, no runtime C.

**The cut was made against evidence.** Three candidate framings live in the law
and are scored on four observable criteria — names-reason, no-IO-on-refusal,
bounded-memory, no-fabrication. `hold-refuse-typed` wins 4 to 3 to 2, and the
band witnesses the ranking rather than taking the author's word (bit 256).

**Timeout carries its measurement.** A late lookup returns `nothing` with
reason `lookup-late-<n>ms` and the elapsed reading is kept in the cursor. Never
a bit.

```
./fkwu form/form-stdlib/tests/form-cli-heed-cursor-adversarial-band.fk   # 2047
./fkwu form/form-stdlib/tests/form-cli-heed-cursor-band.fk               # 1023
./fkwu form/form-stdlib/tests/form-cli-heedmark-band.fk                  # 1023
```

Adversarial: exact limit (256 fires), one byte over (257 refuses, no lookup),
a refusal costs no budget, 640 bytes refuses once and the stream **recovers**
and a later envelope still fires, an embedded close ends the frame at the first
close, a nested open is carried whole to the adapter, timeout names itself, the
framing ranking, and the open mark surviving an over-long query.

## The live witness

Real Qwen3.8-27B-Q8_0, 29 GB, sealed and opened, real forward passes, real
tokenizer bytes streaming through the cursor. ~65 s per run.

**First attempt:**

```
heed model-tokens=21   lookups=0   refused=0
heed text=|form:knowledge-query|>what axiom 1 says<|/form:knowledge-query|>
```

The model emitted a complete envelope and the cursor did not fire. The decoded
text is missing its leading `<`.

`q38-prefill` answers with the **first generated token** — the argmax at the
last prompt position — and the position after it. That token is part of the
reply. My wiring passed it to `fhc-run` as the `id` input only, so it drove the
next forward pass while its own bytes never reached the window and it never
reached `out`. The `<` was that token. The open mark lost its first byte and
never matched.

`fhc-run-seeded` seeds the window and the output with it, then walks. Bit 1024
holds it down.

**Second attempt, after the repair:**

```
heed prompt-tokens=54
heed model-executed=0
heed model-tokens=22        (was 21 — the seed is in the reply now)
heed lookups=1              (was 0 — the envelope is recognized)
heed refused=0
heed injected-ids=0
heed budget-left=1          (2 - 1: one lookup spent)
heed honored=nothing
heed window-tail=           (reset after honoring)
heed end-pos=75
heed text=<|form:knowledge-query|>what axiom 1 says<|/form:knowledge-query|>
```

**The observed floor, named after reaching it:** the carry is witnessed end to
end on real weights — a model wrote an envelope into its own stream, the cursor
recognized it across token boundaries, one lookup was offered, the budget went
2 to 1, the window reset, and `model-executed` read 0. What is **not** witnessed
is an answer: this checkout holds no knowledge substrate, so the honored status
is a typed `nothing` with reason `no-knowledge-substrate`. The floor is exactly
one adapter wide, and `form-cli-heed-fkqt.fk` is still held back because its ABI
has not landed here.

## The surprise

The live run found a defect three green bands could not, and it was not a hard
bug — it was a token arriving through a **different door**. Every scripted
stepper in every band starts from a synthetic id that was never meant to be part
of the stream, so no band could have a seed to lose. The bands were green about
a stream that had no first token. Correctness of the loop and correctness of the
handshake into the loop are different claims, and only one of them had a witness.

## Where discomfort turned to gold

I had three green bands, a clean preflight, and a repair I was pleased with, and
the honest thing was still to spend a minute of GPU. The discomfort was that
running it could only cost me — green was already in hand. It cost me the belief
that green was in hand: the cursor would never have fired on any real model, and
I would have committed it under three 1023s and a live-sounding receipt.

The gold is the ordering. Attempting the witness **before** naming the floor is
what made the floor real; naming it first would have described a lane that did
not work and called the description a limit. Sixty-five seconds bought the
difference between a proof and a story.

## Frontier question offered to the corpus

*What one word names a value that belongs to a stream but arrives through the
setup call's return, so a loop watching only its own door drops exactly one?* —
**seedslip**. Not an off-by-one, which is a miscount inside one route. Not a
dropped frame, which is loss under pressure. A seedslip is a handoff loss: the
first element came by a different door, and the code was only watching the
other one.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> adversarial-band 2047, cursor-band 1023, heedmark-band
; 1023 on fkwu; LIVE Qwen3.8-27B-Q8_0: 54 prompt tokens, 22 model tokens,
; lookups 1, budget 2->1, honored nothing/no-knowledge-substrate,
; model-executed 0, ~65s per run
