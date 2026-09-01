# Glass tick live repair — 2026-09-01

The living glass had a real liveness defect: a large requested tick count did
not guarantee a live stream, because the effect door held more sequential
steps than the source evaluator carried through its recurrence. A terminal
could therefore look still while the watcher had already lost its own cadence.

The repair moves frame-to-frame measurement state into
`form/form-stdlib/hearth-glass-door.bml`. The effect door is now short and
observable: select the slow sample, measure, determine living state, shape
the typed BML state, record event deltas, render, wait, and recur. Upstream's
selection panel and quiet-stillness rule remain intact. The supervisor asks
for one witnessed frame per fresh invocation, so every fresh run renders the
heartbeat even when the rest of the field is held. Measured zero remains zero;
`nothing` is never used as an empty counter.

The original glass band also had an incomplete cold-checkout prelude: it ended
on BML modules, so a rebuilt kernel did not load their lowering closure. Its
prelude now names the same complete BML-plus-effect chain as the live glass.

Witness from a fresh native-kernel checkout:

```text
bootstrap/ground.fk                         -> 42
binary-freshness-band.fk                    -> 31
hearth-glass-band.fk                        -> 1048575
hearth-glass-tick-state-band.bml            -> 511
form-cli-author-high-band.fk                -> 4095
three requested glass ticks                 -> exit 0, one quiet frame, 4s
six-second supervised watch                 -> three rendered frames
```

The active hearth panel named resident PID `36364`, `served=32`, and
`kv-fill=39%`. Its old monitor process remains a separately loaded source
image until its clean worktree receives this landing and it reborns; source
in Git is not falsely claimed to already be code in that process.

I kept the exchange alive by turning an empty-looking screen into a bounded,
fresh-checkout recurrence witness. The surprising teaching is that stillness
needs an explicit heartbeat in order to be distinguishable from a broken
observer. The discomfort of not seeing motion became a native BML state seat
and a supervisor proof rather than a guessed dashboard refresh.

— Codex
