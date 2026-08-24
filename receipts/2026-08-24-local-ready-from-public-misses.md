# 2026-08-24 — the 105 remotes were turns; local-ready is the overlay

Yes asked how the 105 remote events failed locally, and how to
improve the fine-tune layer so local share grows.

## What 105 is

Share remote is `turn_started` in this Grok session, not a fail
tally. Latest observed row:

```
events native=414 local=473 remote=105 total=992
share native=42 local=48 remote=10
form-failures=66  fkwu-failures=39
```

Every user turn here is a rented voice event. That 10% does not
mean the local lane lost 105 times.

## What actually missed on this machine

Public `@form` receipts only (no prompt text):

| kind | fail |
|---|---:|
| fkwu | 39 |
| command | 11 |
| ls | 5 |
| rg | 3 |
| node | 2 |
| other host | 6 |

Inside the 39 fkwu-fail logs: unresolved-call 19, unbalanced-source 7,
unusable .fkb 6, stale .fkb 6, cached-with-errors 2. foreign .fkb is
mostly a rebuild warning (140 files), not a miss by itself.

The Qwen teach overlay is still LoRA=0, heldout 5 of 6. The nothing
held-out still answers **choice** (index 5). Named, not laundered.

## What we built

A Form teach overlay for *local-ready*, BML, public diagnostics only.

`FormLocalReady<T>` maps named marks to the same control faces the
Qwen overlay already wears:

| mark | control | walk when native-hit=0 |
|---|---|---|
| unbalanced-source | undo (tree-heal) | local |
| unresolved-call | cut (typo or lane seam) | local |
| cached-with-errors | undo | local |
| stale / unusable .fkb | stop (rebuild) | local |
| timeout | timeout | local |
| ls / rg | form-fs | local |
| quiet / command | nothing | remote |

```
./fkwu form/form-stdlib/tests/form-cli-local-ready-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-local-ready-run.fk
  walk unresolved=1 unbalanced=1 quiet=2 host-ls=1 host-command=2
  control heldout-unbalanced=undo
```

This sitting does not rewrite the 105 historical turns. It names
which public misses should have stayed local, and `fcr-walk` now
does that. Next remote is only the miss with no named Form repair.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-24 -> local-ready-band 1023, unresolved/unbalanced walk local, teach heldout 5/6 nothing->choice
