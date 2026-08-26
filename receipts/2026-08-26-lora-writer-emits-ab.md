# 2026-08-26 — the LoRA writer emits A/B; Qwen stays frozen

Yes asked what was missing, then asked to please do it.
`fqt-lora?` is still 0: that flag means the 27B GGUF carries a
loaded adapter. It does not. This sitting wrote the **emitter**.

`lora-adapter.fk` proved the identity at d=3. The named next stone
was general-d. `FormLoraWriter<T>` now:

- identity W, cheap apply `x + B*(A·x)` at any d
- one-line emit `lora-v1|d|r|scale|A|B`
- write/read round-trip
- A/B seeded from a Form-knowledge sample string (heedmark gloss)

```
./fkwu form/form-stdlib/tests/form-cli-lora-writer-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-lora-writer-run.fk
  written=1 ready=1 dim=8 rank=1
  qwen-lora-flag=0
  emit=lora-v1|8|1|1000|432,460,...|448,468,...
```

The file is gitignored (`.form-lora-adapter`). Generate does not load
it. That seam is still owed. Flipping `fqt-lora?` before that load
would fail the teach-layer band on purpose.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> lora-writer-band 1023, ready=1, fqt-lora?=0
