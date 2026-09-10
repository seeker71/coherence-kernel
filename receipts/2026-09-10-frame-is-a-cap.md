# 2026-09-10 — a sentence frame is a cap

Urs said next. Last sitting's second-best turned a copy into a list:
children, employees, friends — the same sentence with new slots, out
to 61 tokens. That list was a frame, and the frame was a cap.

`nlg-framing` case-folds the current sentence prefix (8–16 characters
after `. `) and asks whether an earlier sentence starts the same way.
`nlg-stuck` is tail-loop or frame. Pick still yields to second-best;
both stuck → `adapted-repeat`.

```
./fkwu form/form-stdlib/tests/native-llama-voice-adapt-band.fk   # 511
./fkwu observe/native-llama-voice-adapt-run.fk
  why=adapted-repeat  n=30  framed=0  token-limit?=0  nbo-cap=2
  the first gift to ourselves is the gift of self-awareness.
  The body's own internal clock is the first gift to our own
  internal clock. The
```

No children/employees/friends list. A different second sentence still
carries the words mid-phrase; that is observed, not a quality claim
and not native NL homecoming.

Share this turn is declared/withheld.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> adapt-band 511; live n=30 framed=0; sentence-prefix frame cap
