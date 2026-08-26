# 2026-08-26 — Claude and Codex logs, form-cli + local Qwen

Counted without prompt bytes. Kernel Claude mains 48. Codex August
slice is mostly wait/send_message (session bus), not the kernel loop.

Claude kernel tools 14,358. Web 80. Coverable by form-cli/host
**994,428 ppm**. Bash 11,549; 1,502 mention fkwu. Assistant/user
**1.94** — already near one-turn. This Grok thread is 14.5.

The remaining rented slice is the speaker that picks the next tool.
That speaker can be the local Qwen form-cli already generates
(LOCAL FORM ALIVE, 6.41 tok/s). Overlay is live. LoRA A/B is emitted;
`fqt-lora?` stays 0 until generate loads tensors. GGUF frozen.

```
form-cli-session-local-band  1023
claude-tool-coverable-ppm    994428
```

99% of *tools* are already a local shape. 99% of *sessions* waits on
that one local generate turn, not on more Claude.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> claude 994428ppm tools; asst/user 1.94; lora flag 0
