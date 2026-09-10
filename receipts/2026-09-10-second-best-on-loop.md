# 2026-09-10 — a loop cap picks second-best

Urs said next. Last sitting stopped at the first echo (n=18) and named
what remained: the model can still start a loop. Stopping hosted the
cut; it did not adapt the choice.

`nll-select-except` now reads the runner-up on the same logits.
`nlg-pick` takes second-best when first-best would repeat, and only
returns `adapted-repeat` when both would. Decode logs `adapt-choice`.

```
./fkwu form/form-stdlib/tests/native-llama-voice-adapt-band.fk   # 255
./fkwu observe/native-llama-voice-adapt-run.fk
  why=adapted-repeat  n=61  echo=0  token-limit?=0  nbo-cap=2
  the first gift to ourselves is the gift of self-awareness.
  The first gift to our children is the gift of self-discipline.
  The first gift to our employees is the gift of self-improvement.
  The first gift to our friends is the gift of self-acceptance.
  The first gift to
```

Was three copies of one sentence to 40. Then a cut at 18. Now a varied
run to 61, grown past the caller cap, stopped when the new frame itself
looped. Not native NL homecoming; not a held-out quality claim.

Share this turn is declared/withheld.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> adapt-band 255; live n=61 echo=0; second-best on loop
