# 2026-08-26 — session logs, local train, 99 is target

61 Grok session dirs. This thread is the whale. Counts never store
prompt bytes.

Events (this share-run snapshot): native 414, local 473, remote 105.
Event-local 894,153 ppm. 99% is 990,000 ppm, not reached.

Logs: 233 user, 623 assistant. Leak 390 extra remotes. This thread:
13 user, ~189 assistant (~14.5 per request). one-turn leak on 105
remotes is 104.

Natively without remote now: every cell return (ingest-exec, bands,
share, land, preflight). Diagnostic marks walk local-ready.

Still remote: the rented voice that plans the next tool.

Train without transcripts: mint, distillation corpus Q/A, overlay
centroids, local-ready shapes, ingest native returns, LoRA A/B from
Form. Generate may wear A/B; GGUF stays frozen; fqt-lora? stays 0
until tensors load.

```
form-cli-session-local-band  1023
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> session-local 1023; 894153ppm events; leak 390
