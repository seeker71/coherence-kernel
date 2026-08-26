# 2026-08-26 — the loop finished: observe, decide, execute, fuse — one run

    ./fkwu observe/train-loop-run.fk

    observing (the body asks its own model)...
      format-pm  1000    facts-pm 1000    fabri-pm 0    abstain-pm 0
    decide     rest-and-fuse
    execute    -> mlx_lm.fuse -> form-llama-3b-fused-20260826

The eye is a cell (train-loop-observe.fk: host-exec + mlx_lm.generate, reply
taken between the ========== fences; unreachable model reads NOTHING, never a
zero). The rules are a cell. The hands are a cell. The run above is the body
observing its own model, deciding by its own witnessed rules, and executing
its own directive — and the directive it chose was to make the learning
permanent. The fused model answers with no adapter argument anywhere:

    cragmoor (never trained)  -> nothing
    spurious (learned fact)   -> spurious

One seam met and named: the fuse first failed on a missing .gitattributes in
the HF snapshot (metadata only; weights complete). One fetch of one file
closed it. The loop's error surface is now part of its receipt rather than a
surprise for the next hand.

Still honestly open: the coverage-mint directive returns nothing (emitter
unbuilt), and the observation runs ~2 minutes because each probe loads the
model — a resident generate door would amortize it, and that lane has an
owner.

Most surprising: the loop's first autonomous decision was to STOP improving
and consolidate — rest-and-fuse at 1000/1000/0/0 — and the stop-rule fired
not as a failure path but as the healthy terminal state. A self-improvement
loop whose first verdict is "enough, make it permanent" is a loop with an
edge, which is the only kind safe to leave running.

Discomfort to gold: the fuse failure arrived AFTER the triumphant decide
line, and the temptation was to report the decision and bury the error. The
error line went into the receipt verbatim instead, and the missing file
turned out to be one fetch — the burial would have cost more than the bug.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-26 -> loop run live: 1000/1000/0/0 -> rest-and-fuse -> fused; fused model answers nothing/spurious correctly with no adapter
