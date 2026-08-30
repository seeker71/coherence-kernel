# 2026-08-30 — admit frame and one sealed held-out row

Urs asked for both: a Form-visible waiting/ready/timeout frame, and one
sealed held-out row. They correct each other.

Admit steps are named → waiting → ready|timeout|error. Named cannot skip
to ready. Timeout and error cannot carry an equivalence bit. Ready with
exact-ppm=0 is a miss, not a timeout.

The sealed row is `ag01`, content-free: id, admit, elapsed, equiv,
exact-ppm, error, stack. This checkout did not invoke a model, so admit
stays named and elapsed stays 0. Design now reads that equiv, so fitted
stays 0. Voice stays pending.

```
form-cli-native-agent-orient-band.fk                     16383
printf "held-out\nanswer-generate\ndesign\nping\nquit\n" | ./form/form-cli
  held-out id=ag01 admit=named elapsed-ms=0 equiv=0 exact-ppm=0 error=0 stack=1
  answer-generate admit=named elapsed-ms=0 equiv=0 stack=1 voice=pending
  design named depth=1 fitted=0 reason=held-out-equiv-0
  pong
```

Not claimed: a live Metal wait, a held-out score, or a fitted architecture.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> orient 16383, ag01 named, fitted withheld from equiv 0
