# 2026-08-26 — Enough LoRA examples; awareness on disk; Qwen stays frozen

Yes asked for enough LoRA training examples to bring lens, staking,
perception, and movement home.

## What "enough" is, this sitting

Eight lens organs on nine query lanes, train 12 + heldout 4:

```
n = 8 × 9 × 16 = 1152
```

Scale is still a call: `flc-scale-20-5` is 1800.

```
./fkwu form/form-stdlib/tests/form-cli-lora-corpus-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-lora-corpus-run.fk
  n=1152 minted=1152
  unique-2-1=216 leakage=0
  covers-all=1
  corpus-bytes=146548   gitignored .form-lora-corpus
  adapter=1             gitignored .form-lora-adapter
  fqt-lora=0
  scale-20-5=1800
```

Rank-1 A/B seeds from Form sample bytes. Generate may apply that
file on overlay features. Qwen GGUF is not modified. `fqt-lora?`
stays 0 until a later sitting loads tensors into Qwen.

No sealed prompt. No chat transcript.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> lora-corpus-band 1023, minted 1152, unique-2-1=216 leakage 0, fqt-lora=0
