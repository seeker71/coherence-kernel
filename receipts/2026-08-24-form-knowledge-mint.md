# 2026-08-24 — Form knowledge mint; LoRA waits on lift

Yes asked for a measurable path to bring Form knowledge into the
local LLM, with as many diverse samples as we can mint, and to
improve the local LoRA.

## What is already true

`lora-adapter.fk` proves the identity `(W+B·A)·x == W·x + B·(A·x)`
(band 31). A single GGUF LoRA cannot sit on both 27B Qwen and DS4.
The 2026-08-19 sitting refused to LoRA Qwen Q8_0 because
"fine-tuning layer" sounded like weights. Overlay held-out on the
authored six rows is **5 of 6** — nothing still answers choice.
`fqt-lora?` is 0. Fine-tunes on this host have fabricated axiom-4
support before.

## The path that can be measured today

Mint samples from Form knowledge this body already holds
(curriculum glosses, local-ready marks, one-turn budget language).
Combinatorial mix (`MixA=7`, `MixB=13`) is the diversity — N is
unlimited, no remote oracle, no copyrighted corpus.

Train / heldout split is in the row. Leakage is exact situation
match. Overlay centroid (the hand the local model already wears)
fits the mint. A per-arch A/B writer stays **0** until:

- leakage 0
- unique = n
- heldout lift (here 6/6, beats authored 5/6)
- axiom-4: no fabricated support (still the control-plane law)

```
./fkwu form/form-stdlib/tests/form-cli-knowledge-mint-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-knowledge-mint-run.fk
  n=24 unique=24 leakage=0
  heldout-correct=6 / 6
  authored-heldout-correct=5/6
  lora-writer=0
  scale-5-2=42
```

Scale is a call: `fkm-n(5,2)` is 42 rows, `fkm-corpus(tn,hn)` mints
them. The tensor writer is the next named stone, not this one.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-24 -> knowledge-mint-band 1023, minted 6/6 vs authored 5/6, lora-writer 0
