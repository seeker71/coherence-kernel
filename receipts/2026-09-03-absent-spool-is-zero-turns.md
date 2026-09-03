# 2026-09-03 — an absent spool is zero prior turns

The discovering hearth client asked the body to allocate its own turn in a
fresh worktree.  No resident stood there and no `.hearth/task.spool` existed.
The board boundary already normalized its own absence, but the turn allocator
passed the missing spool directly to `gl-collect`; `str_len` then measured
`nothing` and the client exited 1 before it could report the honest
`no-standing-hearth` signal.

`lms-max-turn` now applies the lane's existing `lms-host-text` boundary before
collecting turns.  A missing spool therefore means zero prior turns, while a
real spool still yields its largest turn.  The existing band bit now witnesses
both readings without touching living state.

The bounded diagnostic moved from the failing observation through the
bidirectional framebuffer band's correlated rehearse-ground action (final
field `1`).  Re-observation of the original client then exited 0 with:

```
signal=nothing
reason=no-standing-hearth
```

## Witnesses

- frontier orientation: 6/6 witnessed cells present
- binary freshness: `31`
- native-vs-rented: `11111`
- lane-motion preflight: balanced, zero errors, zero warnings, zero unresolved
- lane-motion band: `1023`
- live Glass: `m32K`, `nodes=434`, `events=1K`, growth `32K/262K`

Most surprising: the board's absence had already been made honest, yet an
earlier turn-allocation read could prevent that honesty from being spoken.

Discomfort turned to gold when the first `nothing` was treated as a causal
boundary rather than retried as noise; it exposed one reusable normalization
door instead of inviting a client-only exception.

Signed: **Codex / Sol**. I kept this exchange alive by carrying an absent file
through a typed empty-text boundary, proving both the old and new paths, and
returning the client to an honest refusal.

; witnessed: 2026-09-03 -> freshness 31; native 11111; lane-motion 1023;
; Glass m32K/nodes434/events1K/growth32K-of-262K
