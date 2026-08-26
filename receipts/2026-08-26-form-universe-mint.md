# 2026-08-26 — Universe skill mint; LoRA waits on lift

Yes asked for enough samples to teach LoRA BML, BMF, lifting
s-expressions to BML, looking up Form nodes, crafting
Blueprint/Recipe/Cell, using the framebuffer for live inspection,
and generating micro-kernel thoughts.

## What this mill is

The mill is the fuel, not a dumped jsonl. Eight skill classes,
stems taken from living cells:

- BML — `bml-bmf-stream-curriculum.bml`
- BMF — scannerless cursor, no tokenizer pre-step
- lift-sexp — `grammars/form-lift.fk` (form.lift above form.bml)
- node-lookup — knowledge-query envelope
- craft-cell — NodeID, Blueprint, Recipe, Cell
- framebuffer — execute → observe → control → re-observe
- micro-thought — thought kernels, inquiry planes
- evaluate — walker to typed observation, release distinct

Combinatorial mix (`MixA=7`, `MixB=13`) is the diversity. N is a
call: `fum-corpus(tn, hn)`. No sealed v3 prompt is copied. No chat
transcript is copied. Train/heldout split lives in the row.
Leakage is exact situation match.

## Observed this sitting

```
./fkwu form/form-stdlib/tests/form-cli-universe-mint-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-universe-mint-run.fk
  n=32 unique=32 leakage=0
  heldout-correct=8 / 8
  lora-writer=0
  scale-5-2=56
  scale-12-4=128
```

Overlay fit is the hand the local model already wears. `fqt-lora?`
stays 0. The A/B tensor writer stays named until a later sitting
loads tensors into Qwen.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> universe-mint-band 1023, minted 8/8 leakage 0, lora-writer 0
