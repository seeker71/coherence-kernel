# 2026-09-10 — adapter asks take the native LoRA door

Urs said next. The named remaining work was: connect the voice callers;
optimizer continuity; hear interned fuel, not only train it.

`lv-ask` wearing an adapter now goes through `nlg-ask` (llama32-3b LoRA
on Metal). The dense 1B lane stays adapterless. `lv-cross-ask` still
names the mlx generate membrane for side-by-side grades. `lv-train-cmd`
still names mlx_lm — that trainer crossing is kept, not deleted.

```
./fkwu form/form-stdlib/tests/native-ask-band.fk   # 4095
  lv-route "" = native
  lv-route adapter = native-lora
  train command still contains mlx_lm
./fkwu observe/form-cli-session-home-embody-run.fk
  loss 6.34177 → 6.06886  moved=1
  voice (token-limit 40): the first gift to ourselves is the gift of
    self-awareness. …
  nbo-voice=2
  adapters.safetensors 13905800 / 112
  optimizer.safetensors 27812024 / 224
  retained-buffers=0
```

The local answer repeated and hit the token cap. That is observed
speech from interned fuel through native LoRA, not native NL homecoming
and not a held-out quality claim.

Share this turn is declared/withheld.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> native-ask-band 4095; lv-route native-lora; interned fuel spoken locally; optimizer saved
