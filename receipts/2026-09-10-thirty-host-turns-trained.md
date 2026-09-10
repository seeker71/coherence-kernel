# 2026-09-10 — 30 unique host turns trained

Urs: so we have 32+34 events to train and embody.

Those were two measurements of the same growing session, not 66
examples. Unique completed user/assistant pairs were 30. Remote
turn_started is now 35.

All 30 were retained as grok-host caller-teaching. The learner drained
them. 27 learned-and-promoted. 3 refused (training-failed-exit-1).
Optimizer step 27. Serving equals candidate. Worker exit 0. About 80
minutes. Stdout carried counts, not prose.

```
./fkwu form/form-stdlib/tests/grok-host-learn-band.fk   # 31
./fkwu observe/grok-host-learn-all-run.fk
  pairs=30 pending-before=30 pending-after=0 worker=completed
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-09-10 -> 30 retained; 27 promoted; 3 refused; band 31
