# 2026-09-10 — more caps dissolved

Urs: more caps to dissolve.

Two remaining hard edges in the voice path:

- A hanging last word after `adapted-repeat` (`The`). `nlg-complete`
  now drops an unfinished last sentence, keeping the last period.
- A two-choice cap. `nll-select-except2` reads a third id on the same
  logits; `nlg-pick3` yields to it when the first two would stick.

```
./fkwu form/form-stdlib/tests/native-llama-voice-adapt-band.fk   # 8191
  complete(live stub) ends at ourselves.
  pick3 yields id 3 as adapted-choice
```

No 3B this sitting. Whisper large-v3-turbo remains the fusion
movement's named geometry.

Share this turn is declared/withheld.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> adapt-band 8191; stub complete; third-best
