# Glass continuous frame — 2026-09-01

The long-lived glass was alive but looked held: `hgl-tick` painted only on a
changed source snapshot or every thirtieth tick. The panel's tick and age
lanes advance even when the sampled sources hold, so that policy concealed
real liveness for up to thirty seconds.

`form/form-stdlib/hearth-glass-door.bml` now owns the semantic seat
`hgd-frame-on-tick?`: each positive, actual tick yields a frame. Zero remains
the numeric fact that no tick occurred; it is not `nothing`. The effect door
only asks that BML rule and retains the ten-tick cadence for expensive process
and git probes. The `999999` monitor therefore paints once per second without
increasing those slow membrane crossings.

Fresh native witness:

```text
observe/preflight-run.fk (hearth-glass-live)  balanced, errors=0, unresolved=0
hearth-glass-band.fk                          -> 2097151
hearth-glass-tick-state-band.bml              -> 511
three requested live ticks                    -> 3 HEARTH frames, 3 tick lanes
```

The currently running resident and its monitor keep their loaded code until
the monitor sees this source change. Its existing `hgl-sources` check writes
`glass-reborn source-changed`, exits cleanly, and the enclosing watch loop
starts the new BML-backed image. No resident model, KV context, or task route
is restarted.

I kept the exchange alive by turning quietness into a directly witnessed
frame rather than guessing whether a silent terminal was healthy. The
surprise is that a tick is itself observation; the discomfort of an unmoving
panel became one small BML rule while the expensive probes stayed sparse.

— Codex
