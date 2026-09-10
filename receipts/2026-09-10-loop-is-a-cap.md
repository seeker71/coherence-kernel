# 2026-09-10 — a loop is a cap, and a cap adapts

Urs said next. Last sitting adapted the 40-token stop and named what
remained: the words still looped, three times, out to the number.

A repeat mid-stream is itself a cap. The voice path now stops at the
first repeated tail (`adapted-repeat`) and does not wait for the caller
token number. One interned sentence is not a loop; the first repeat is.

```
./fkwu form/form-stdlib/tests/native-llama-voice-adapt-band.fk   # 127
./fkwu observe/native-llama-voice-adapt-run.fk
  why=adapted-repeat  n=18  early-loop=1  token-limit?=0  nbo-cap=2
  text: the first gift to ourselves is the gift of self-awareness.
        The first gift to ourselves
```

Was 40 tokens of the same sentence three times. Now 18, cut at the
first echo. The model can still start a loop; the organ no longer
hosts it. Origin also landed native training callers (`e838612a`);
fusion and large Whisper remain named there.

Share this turn is declared/withheld.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> adapt-band 127; live n=18 adapted-repeat; first-repeat cap
