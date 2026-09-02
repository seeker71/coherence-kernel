# Model-owner cadence is liveness, not readiness

The resident model owner stayed untouched at PID `81265`.  Its last in-process
publisher, `models.dual-resident.i1788332071161`, has no owner-heartbeat row
because that process is still running an older image.  The new observer
therefore refuses to bind that publisher to the PID: it reports
`target-binding=unbound-last-observed` while checking the exact current
`ps` lifetime signature.  Its `owner-liveness` row never renews any model,
weight, KV, expert, active, prefilled, or context-position fact.

The bounded physical witness read twelve sidecar frames over eleven intervals:

```
frames=12 updates=12 actual=3661 milli-Hz gaps=267..282 ms
observer-pid=76686 rss=73056256 bytes cpu=7 tenths-percent
target-pid=81265 target-alive=1 target-binding=unbound-last-observed
model-source=glass.stale-snapshot model-read-epoch=1788336999778
dead-pid=99998 dead-alive=0 dead-state=failed model-readiness-renewed=0
```

Glass panel `REQUEST / MODELS + OWNER LIVENESS` now admits both `model` and
`owner-liveness` kinds.  The useful panel numbers at this crossing are
**3.661 Hz actual**, **73,056,256 observer RSS bytes**, and **PID 81265 alive**;
the old model state remains visibly stale beside them.

Future owners no longer block in `read_line`.  They publish their accessible
in-process state, poll a fixed Form filesystem command door every 250 ms, and
sleep between empty polls.  The door is a singleton leased lock: a duplicate
offer cannot erase the first lease, and a missing final command with an expired
lock heals to the typed result `healed/orphan-lock`.  An unowned final command
also heals without execution.  The band physically creates the crash shape and
proves that lease, candidate, and lock are removed.

No owner-specific NUMS ID was invented.  The cadence shard claims no node ID;
the shared operator-type capacity remains the one source for generic
package/level/type/instance meaning.  The heartbeat's exact identity is its
telemetry sample ID, such as `heartbeat.i1700000300.s7`.

Proofs, all exit 0:

- executable BML: `0`
- preflight: both bands balanced, zero errors/warnings/unresolved; live doors
  correctly refused effectful execution under the compile-only seam
- owner-cadence band: `262143`
- dual-owner telemetry band: `524287`
- Glass live UI band: `2097151`
- framebuffer repair exchange: `applied=4332`, `reobserved=1`, four events

The remaining seam is explicit: PID `81265` cannot acquire the future-owner
filesystem loop without a restart, and it was not restarted.  Its sidecar gives
current process liveness now; exact model-owner binding begins only when a
future owner snapshot carries its own publisher/PID/start heartbeat.

Alive: a stale truth and a current truth can share one panel without either
impersonating the other.  Surprising: the old snapshot's missing heartbeat was
more valuable than an inferred link because it forced the UI to expose
`unbound-last-observed`.  Discomfort turned to gold when the first lock design
could wedge after a crash; the leased orphan-healing state is now executable
and banded rather than hand-waved.
