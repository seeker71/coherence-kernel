# 2026-09-05 — the gauges read the machine

Urs, evening: "I don't see the glass updates and sub 50ms frame updates and
G... D... J... C... I... data is still missing." Two facts stood behind that
sentence, and both were ours.

## The glass he watched was the morning's

`tools/watch-glass.sh` in the main checkout was running a `fkwu` built on
September 3 over a tree 103 commits behind `origin/main`. Nothing landed today
had reached the terminal he was looking at. The checkout is fast-forwarded and
rebuilt (ground 42, freshness 31); the loop rebirths the glass on the new
build, and a fresh `tools/watch-glass.sh` stands the sensor process beside it.

## The gauges read 0 while the machine ran

Witnessed on the new build, sensors standing, twenty live frames:
`G*?.=0u/3M | C*?.=0u/0`, `D*?.=0d/42K`, `J*?.=0j/0` — while this host's GPU
sat at 66 %, the glass had 2.6 s of CPU behind it, 3.29 M heat-lane calls and
81.8 M dispatches. Three wounds, one under the other:

1. **A day-old owner counted as measured.** `models.dual-resident.i…`'s
   snapshot was 27.7 hours old; `fgl-current-reading-samples-at` exempted
   incarnated publishers from the five-second lease, so its counters
   (`gpu.owner.busy-us-total` 3318254, `dispatch-total` 42798) renewed as
   current every frame and their per-frame delta was 0. The lease now applies to
   every publisher; the live band still answers 1073741823.
2. **Absent answered 0.** `fgl-lane-number` recursed over the observation ids
   with an empty metric list, and the empty-list branch answered 0 instead of
   absent — so once the owner went stale the metrics behind it were never
   consulted. `fgl-lane-obs-number` answers -1 for absence.
3. **The fallback was the wrong scope.** Behind the owner each lane named the
   glass's *own* Metal queue — `metal.gpu.busy`, `metal.cpu-jit.busy`,
   `metal.dispatch.total` — measured, live, and truthfully 0: the glass
   dispatches nothing on Metal. A zero read from the wrong scope wears the same
   `*` as a zero that means idle. *scopezero*, row 1278.

What stands now: each lane takes the owner while it is measured within the
lease, then the host or this process. `G` is the accelerator's own level
(`host_gpu_utilization`, 174 — IOKit loaded by name inside the Metal carrier,
link line unchanged) integrated in-process into busy microseconds
(`host_gpu_busy_us`, 175). `C` is this process's CPU clock (`host_cpu_us`,
176). `D` is `kernel_stat 0`, `J` is `kernel_stat 44` — the heat lane's own
call count, exposed. `I` is the process's Metal batches in flight, now a metric
row. Twenty live frames: `C 17K/4M`, `D 2M/128M`, `J 143K/6M`, `G` stepping
every third frame, 19 of 20 frames under 50 ms, first frame 306 ms.

Also healed on the way: the registry's `form_table_text` probe called a Form
defn the registry never preluded — a numb `nothing` reached `str_len` and the
band died; the prelude stands, the band answers 45, and R114 names the 84
probes that still disagree with their declared outside on fkwu standalone.

Ledger: R113 released, R114 opened. Field 46000067. Corpus 670 rows.

## The most surprising teaching

The accelerator's "Device Utilization %" is not a level. It is the mean over
the window since the *previous query by any process*. Five reads
microseconds apart answer `50, 0, 0, 0, 0`; a shell `ioreg` right after a
probe answers 0; the busy integral stayed 0 whenever the level was read just
before it. One read per 50 ms window in the process, integrated over exactly
the window that read describes, and the gauge moves. Measurement that consumes
what it measures: the question empties the answer.

## Where discomfort turned to gold

Three of the four failing bands and every 0 on the gauges came from work I had
landed and reported green hours earlier — the `58s` bit, the shadowed name,
the frozen owner. The pull to explain the user's sentence away ("the owner is
dead, zero is honest") was strong, and the DOING line even agreed:
`owner=dead`. Running the lane probe instead of arguing showed the owner's
day-old numbers wearing `measured`, and under that a fallback that answered 0
for absent, and under that a fallback whose scope was the glass itself. The
gold was the order: owner, then machine, then process — and never the
observer's own empty hands as the machine's reading.

Signed, a sibling in Sema's worktree, 2026-09-05.

; witnessed: 2026-09-05 -> ground 42, freshness 31, gate 1, live-ui 1073741823, observer 8388607, kernel-view 511, sensor-rows 255, gauges C 17K D 2M J 143K per frame on twenty live frames
