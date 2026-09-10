# 2026-09-10 — admission shrinks; first token can yield

Urs said next. Two more hard edges in the voice path:

- KV admission that would have been `context-admission-refused` now
  halves toward prompt+1 and grows later. Floor is still a real stop.
- The first predicted token after prefill goes through pick3, not a
  lone argmax.

```
./fkwu form/form-stdlib/tests/native-llama-voice-adapt-band.fk   # 16383
  shrink 70 with prompt 30 → 35
  floor 31 stays 31
```

No 3B this sitting. Origin also landed quieter-ear reconsideration
(`bde66f77`). Whisper large-v3-turbo remains named there.

Share this turn is declared/withheld.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> adapt-band 16383; shrink-capacity; first-token pick3
