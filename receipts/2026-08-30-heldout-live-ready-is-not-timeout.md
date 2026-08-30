# 2026-08-30 — live waiting→ready, not a timeout

Urs asked for a real waiting→ready or waiting→timeout on the resident,
written into ag01 without collapsing them.

`held-out-live` steps named→waiting, then reads this process's Metal
carrier through `metal_status` under a 2000ms deadline. Deadline wins over
a late status. Empty status is error, not timeout. Ready with exact-ppm=0
stays equiv 0.

Observed this breath, no 27B generate, no prompt or answer bytes:

```
held-out id=ag01 admit=ready elapsed-ms=29 equiv=0 exact-ppm=0 error=0 stack=1
held-out id=ag01 admit=named elapsed-ms=0 equiv=0 exact-ppm=0 error=0 stack=1
design named depth=1 fitted=0 reason=held-out-equiv-0
```

The named floor stayed named. The live row completed ready in 29ms. Equiv
stayed 0, so design still withholds fitted. Band `32767`.

Not claimed: a local language answer, a held-out score, or a fitted
architecture.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> held-out-live ready 29ms, equiv 0, named floor held
