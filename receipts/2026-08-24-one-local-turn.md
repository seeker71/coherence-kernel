# 2026-08-24 — one local turn, not 105 remotes

Yes named the biggest gap: a request should resolve in a single
local turn, not a rented marathon.

## What this session actually spent

Public events only:

```
turn_started ~105..107
tools per turn  min 0  median 17  mean 24.9  p90 62  max 125
conversation_message_count 4 → 345
```

105 remotes are not 105 failures of one compile. They are 105
Grok turns. The leak is that **form-cli never took the first
local turn**. Overlay already said it: form-cli first; remote
reviews.

## The budget, now a cell

`FormOneTurn<T>` (BML):

- `OneTurn = 1` — native then local, one walk
- after that miss, **one** remote review
- a third remote is `nothing`, not another attempt
- leak for one request = `max(0, remote-turns - 1)`

```
./fkwu form/form-stdlib/tests/form-cli-one-turn-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-one-turn-run.fk
  next local-first=0  review=1  cap=3 (nothing)
  walk unbalanced=1 (local)  quiet=2 (remote)
  session-remote=105
  leak-one-request=104
```

A named Form repair (unbalanced-source, unresolved-call) finishes
locally in that one turn. Quiet still owes a review — once.

Native generative voice is still pending. This sitting does not
claim Qwen now authors the whole agent loop. It claims the
**budget**: one local walk, one review, then honest nothing.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-24 -> one-turn-band 1023, leak-one-request 104 on 105 remotes
