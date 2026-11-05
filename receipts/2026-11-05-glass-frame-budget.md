# Glass frame work returns inside its budget

The existing physical check remains unchanged:

```sh
form-run ./fkwu form/form-stdlib/tests/form-glass-frame-work-band.fk
```

Its full verdict is 32767: work below 30 ms, nonzero rest, fewer than
1,500,000 dispatches per tick, bounded framebuffer roots, retained gift handles,
and the existing rendering/index assertions. Active presentation targets 40 ms.

The first warm reading failed with verdict 32751:

```text
frame-work-ms=26 frame-wait-ms=0 budget=40 ticks=310
dispatches-per-tick=831729 rest-ticks=98 index-agrees=1
```

Temporary timing in the actual loop showed alternating ordinary 18–22 ms work
and roughly 37–44 ms work. The extra time moved between collection, control and
dashboard stages. A physical stack sample located repeated native-generation
membership scans during heap marking, plus repeated process-lifetime reads and
shared-memory mappings during ownership sweeps. The temporary loop timing was
removed after diagnosis; its raw trace and sample remain in the local `.hearth`.

The repair stays at that ownership boundary in `runtime/fkwu-uni.c`:

- Share the eligibility predicate between membership and marking. Marking skips
  already marked generations and stops after the first containing range. Live
  ranges are disjoint; marking still retains the full transitive hold graph.
- Observe each exact `(pid, birth)` once per locked sweep. Generations and reader
  leases share that reading. The scratch is released and the next sweep reads
  liveness again; no liveness result survives a sweep.

These are physical allocation/collection operations. Their shrink path remains
the native field allocator and heap that replace the seed carrier. No new
primitive, external runtime, policy allowance or performance threshold was added.

A fleet started in the main checkout during diagnosis using the older runtime.
Its contention changed the measurement conditions: a partial profile under that
fleet observed 103–374 ms prior work. After the ownership checks passed, the same
runtime patch was applied to that checkout, its seed rebuilt and verified at 31,
and the existing source watcher refreshed all four sensor children and the live
renderer. The terminal and supervisor stayed owned by their existing processes.

With that live fleet running, the unchanged physical check returned **32767**:

```text
frame-work-ms=18 frame-wait-ms=21 budget=40 ticks=305
dispatches-per-tick=909853 rest-ticks=104 index-agrees=1
framebuffer-events=0 gift-frames=117
fgd-cat=0 crc32-fold8=0 fgd-find-metric=0
```

The main checkout's `glass-watch-run.bml` then observed 51 published frames in
two seconds, averaging 39 ms between gives. Work observations ranged from
18 to 54 ms during that watch, which included observer admission and concurrent
source reads. The check and live cadence establish recovery in these windows;
they do not promise that every frame remains below 30 ms under arbitrary load.

Ownership verification: `node-gift-band.fk` 4095, `field-band.fk` 255,
`cell-store-band.fk` 255 and `form-glass-telemetry-membrane-band.fk` 2097151,
all exit zero. The existing node-gift band now also keeps an owned outer root
whose payload is another owned root across replacement of both frames and a
heap collection, then verifies the nested string, float and list values.
