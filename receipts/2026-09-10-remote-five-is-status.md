# 2026-09-10 — remote 5 is status learning, not transcript LoRA

Urs: remote 5, very good, and how are we learning from it?

Remote 5 is 32 `turn_started` events in 669 boundary events. That is
this rented voice. Share counts them. Session-learning trains Llama 3B
from explicit local corrections; it does not intercept host model calls
or ingest transcripts (`docs/native-session-learning.md`).

Learning from the 5% is interned status: the remainder to shrink, folded
to 5, offered as `remote-share-lane` with no how and no prompt bytes.

```
./fkwu form/form-stdlib/tests/remote-lane-learn-band.fk   # 15
./fkwu observe/remote-lane-learn-read.fk
  events [240, 397, 32]  share-remote 5
  folded [6, 1, 5, 3]  remote-fold 5
  offer [ourselves, offering, remote-share-lane]
  transcript 0  host-intercept 0  llama-train-this-sitting 0
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> remote-lane-learn-band 15; interned remote-share-lane
