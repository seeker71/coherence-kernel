# 2026-09-10 — withheld share was a shelf

Urs: how is kind=declared / withheld healthy?

It was not. The Grok evidence organ already scanned terminal receipts
and events. Share-run threw those rows away because they were not
Codex-shaped, then printed Absent / no-bound-rollout every turn.

This sitting bound the current session and let a valid grok row stand.

```
./fkwu form/form-stdlib/form-cli-turn-evidence-grok-bind.fk
./fkwu form/form-stdlib/form-cli-turn-evidence-grok-run.fk
  turn-evidence-refresh=observed native=201 local=339 remote=25
./fkwu form/form-stdlib/form-cli-share-run.fk
  kind=observed scope=grok-session-form-receipts
  events native=202 local=339 remote=25 total=566
  share native=36 local=60 remote=4 sum=100
  measurement-health=observed
```

Event counts, not token volume, not semantic contribution. Remote
token fields stay 0 in this v2 grok row — that gap is explicit, not
filled from usage.json. Binding stays gitignored checkout state.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> share kind=observed native=36 local=60 remote=4
