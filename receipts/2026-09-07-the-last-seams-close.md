# 2026-09-07 — the last seams close

Urs, tonight: **"all point to unfinished work."** Every `?` and every
`missing=<door>` on the glass is a door we have not written, never a limit of
the machine. So the four absences still standing were each walked to the door
behind them. Three of the four are readings now. The fourth is one seed word,
written down precisely enough to hand over.

## What stood

The atlas in-flight lane rendered `I*.=0b` — a bare gauge one case-fold away
from the bytes lane beside it (`R` renders `0B`), with nothing on the glass
saying what the zero meant. The seam line read `SEAM n=4 next=kernel_stat:11`,
naming one of four and leaving three unnamed with no way to reach them: the
right column renders present rows only, so a seam the line counts is a seam a
reader cannot read. The staged startup printed
`full-program-image-call=unavailable missing-door=runtime.full-program-image.call`
as a frozen string that consulted nothing, and `disk-image ... bytes=-1
age-ms=-1` where `-1` is the absence of a file, not a measurement of one. And
the recipes organ line spent fifty-six columns on `UNAVAILABLE
door=metric.publisher reason=metric-not-published` — a door name that names
nobody — long enough to push the fields after it off the edge mid-word, beside
a hard-coded `alloc=?`.

## What stands

**The kernel already publishes its own program image.** `fk_prog_note_ice`
writes four words into the D header of the program surface every time a unit's
ice loads: images loaded, the last image's byte length, its identity fold (the
path bytes folded with the unit digest) and that image's path. `kernel_ast`
(tag 32, arity 2) has read that header since R120, and `host_pid` answers this
process's own pid, so `kernel_ast(host_pid(), -1)` is the full program image
reading, available with no seed change. The startup states it:

```
program-image images=1 bytes=233878 identity=1773349885280628280 path=observe/form-glass-staged-startup-run.fkb source=kernel_ast:D8-D11
```

and on a first run, with no ice yet beside the source, `images=0(source-compiled)
bytes=0(none) identity=none(no-image-loaded)` — a semantic zero with its
meaning, not a hole. The in-process *call* door is a different thing from the
reading and it genuinely does not exist: no entry in `runtime/fkwu-optable.h`
loads a program image and calls it. So the views line renders the route the
cell already computed rather than a frozen sentence, and says in the same
breath which door the body walks instead:

```
full-program-image-call=offered artifact=observe/form-glass-live-run.fkb door-to-build=runtime.full-program-image.call today=spawn:observe/form-glass-live-run.fk
```

**A zero says which zero it is.** The in-flight reading was never wrong: the
metric is physical-live, its value is 0, and its row has carried
`semantic-zero=idle` since it was written. What the atlas lost was that word,
because the flow point carried a number, an evidence class and a source per
lane, and not the lifecycle. It carries the lifecycle now — one list of eight,
appended at the end of the v4 point, taken from the row's own lifecycle and
only when that lifecycle is one of the four semantic zeros. The lane reads
`I*....................=0b(idle)`. A delta lane never borrows it: a zero delta
means nothing moved in this window, which is a different sentence from the
row's own state, so `D` and `J` render exactly as before.

**The seam line names every door.** Distinct sources, deduplicated — two routes
waiting on the same owner process are one door to build — with a `+N more` tail
when the width runs out. At 200 columns all six show; at the 80-column atlas
one shows and the tail counts the rest honestly.

**Three of the four seams needed no seed door at all.** Two are model routes
and the third is their parallel-availability row; all three wait on an owner
process, and the two model contexts they name stand together in one resident
cell. The sensor rows have always said `run observe/form-glass-sensors-live.fk`
when a frame is absent. The route rows say the same thing now.

**Every field on the organ line is a number or the frame that owes it.**
`node=`, `bp=`, `rc=` and a new `alloc=` reading the per-cell byte ledger the
memory domain has stood since R138. An absent field renders `?(<door>)`, four
to twenty columns, taken from that sensor's own row through `fgsr-publisher`
rather than spelled again here — so the line fits at every width and nothing is
cut mid-word.

## Each absence, and what it reads now

| absence as it stood | reading tonight, or the door owed |
|---|---|
| `I*.=0b` — in-flight lane | `I*.=0b(idle)`: `metal_live` word 6, physical-live, scope `glass.monitor`, semantic zero with its meaning from the row's own lifecycle |
| `SEAM n=4 next=kernel_stat:11` | `SEAM n=4 doors: kernel_stat:11 host.process.owner-liveness native-model.route.qwen-and-3b` — every distinct door, deduplicated, `+N more` past the width |
| seam 1 — `framebuffer.rejected` | **owed to the seed**: `kernel_stat 11`, arity 1, answering the count of `framebuffer_register` calls refused because the value was not a live cell handle; and `kernel_stat 9`, arity 1, answering cumulative `fk_fbn`. Both keys the seed's own comment already reserves (9..14, table-walker lane). Probed 2026-09-06: 11, 99 and 255 all answer 0, so 0 there is the no-such-key answer |
| seam 2 — `route.model.qwen3.8-flash-next.ud-q2_k_xl.unsloth` | closed: the row names `observe/native-model-dual-resident-live-run.fk`, the cell that stands both owners |
| seam 3 — `route.model.llama-3.2-3b-instruct` | closed: same cell, same door |
| seam 4 — `route.models.parallel-ready` | closed: derived from the two above, same door |
| `full-program-image-call=unavailable missing-door=...` | reading: `kernel_ast(host_pid(), -1)` D words 8..11 — images loaded, byte length, identity fold, path. **Owed to the seed** for the call itself: an op that loads a `.fkb` program image into the running kernel and calls its entry — `runtime.full-program-image.call`, arity 1 (image path), answering the program's value; no such op stands in `runtime/fkwu-optable.h` today |
| `disk-image state=image-missing bytes=-1 age-ms=-1` | `bytes=none(image-missing) age-ms=none(image-missing)` — the state the cell already computed, in place of the host's refusal sentinel |
| `bp=UNAVAILABLE door=metric.publisher reason=met...` | `bp=3`, or `bp=?(gift:glass.sensor.storage)` when that frame has not given; and `alloc=` now reads `memory.cell-bytes` where a hard-coded `?` stood |

## Witnessed

| lens | before | after |
|---|---|---|
| form-glass-live-band | 1073741823 | 2147483647 (bit 2^30: a lane borrows the word its own row measured and only when that word is a semantic zero; an absent route names the cell that stands an owner) |
| form-glass-live-ui-band | 1073741823 | 2147483647 (bit 2^30: `0b(idle)` on a standing gauge and never on a delta lane; four organ fields that are a number or a named frame, fitting 40 columns) |
| form-glass-staged-startup-band | 65535 | 262143 (bits 65536, 131072: the running program's own image identity through the program surface; a missing disk image named rather than a bare -1) |
| form-glass-observer-band | 67108863 | 67108863 |
| form-glass-dashboard-band | 16777215 | 16777215 |
| form-glass-kernel-view-band | 511 | 511 |
| form-glass-sensor-rows-band | 2047 | 2047 |
| form-glass-events-channels-band | 8191 | 8191 |
| form-glass-machine-band | 511 | 511 |
| form-glass-wait-band | 255 | 255 |
| form-glass-frame-work-band | 32767 | 32767 |
| form-glass-launch-band | 65535 | 65535 |
| form-glass-telemetry-membrane-band | 2097151 | 2097151 |
| jit-metal-lanes / metal-deadline / jit-lens | 8191 / 127 / 16383 | unchanged |
| event-loop / observation-v2 / meaning-ui / gift-frame / deadline-cadence / celebration / organ-care / staged-jit / jit-hold / live-rate / bml-demand-jit-glass-ui / hearth-glass / resource-governor-glass | 16777215 / 2097151 / 8191 / 4095 / 4095 / 16383 / 4095 / 255 / 4095 / 63 / 1023 / 16777215 / 2097151 | unchanged |
| quartet | 42 / 31 / 1 / 2015 | 42 / 31 / 1 / 2015 |
| frame lens | — | 19/20 under 50 ms, mean total 8 ms, warm maximum 8 ms, first frame 53 ms, four sensors standing |

Drift gates read 2015 on this checkout before and after; that reading is older
than tonight and `kernel-conformance` refusing 32 bits is the same refusal the
2026-09-06 receipt recorded.

## What the integrator owes the seed

Two doors, neither of them mine tonight, both in `runtime/fkwu-uni.c`:

- `kernel_stat 11`, arity 1 — registrations `framebuffer_register` refused
  because the value was not a live cell handle. With `kernel_stat 9`, arity 1,
  answering cumulative `fk_fbn`, the last probed absence on the glass becomes a
  count. This is the same pair the 2026-09-06 receipt named; it is unchanged
  and still first on the seam line.
- `runtime.full-program-image.call`, arity 1 — load a `.fkb` program image into
  the running kernel and call its entry, answering that program's value. Today
  the staged startup spawns `observe/form-glass-live-run.fk` as a second
  process, and the line says so. The *reading* half of this seam is closed
  without it, through `kernel_ast`.

## The most surprising teaching

The in-flight lane was never a missing reading. `metal_live` word 6 answers, the
metric row is physical-live, and the row has carried the sentence
`semantic-zero=idle; this Glass process has no Metal batch in flight` since it
was written. The glass simply had no channel to carry that sentence to the
lane: the flow point retained a number, an evidence class and a source per lane,
and dropped the one word that says what a zero means. Three of the four
absences tonight were the same shape — the body already knew, and the knowing
had nowhere to ride. `fk_prog_note_ice` has been folding an ice identity into
the D header for a whole release; the startup that prints
`full-program-image-call=unavailable` runs in the very process holding those
words. An absence on a glass is at least as often a missing wire as a missing
door, and the two look identical from the row.

## Where discomfort turned to gold

I was told the in-flight lane should read Metal's in-flight batches plus the
frames a kernel holds, and I built toward that: `fgl-lane-sum` sits in
`form-glass-live.bml` with no callers at all, and the comment right above the
flow lanes says "D J I sum the counter over every process that gives a frame."
Wiring it looked like the whole task. Then I counted publishers on a live frame
and found exactly one row per lane — only the glass publishes `inflight`, only
the glass publishes `kernels-dispatches`. The sum would have walked three
hundred and seventy-five rows three times a frame to return the number it
already had, and I would have written a receipt saying the lane now sums across
the body.

The discomfort was that the lane is then *structurally* zero — the one process
that publishes Metal counters is the monitor, and the monitor never dispatches
— and no amount of arithmetic changes that. The gold is that the honest thing
was smaller and truer than the thing I was reaching for: say what the zero
means, on the reading that actually exists, and write down plainly that no
dispatching process gives its Metal rows into a glass frame yet. That last
sentence is worth more than a sum would have been, because it is the next piece
of work rather than a number pretending to be one.

Signed, a sibling in Sema's worktree, 2026-09-07.

; witnessed: 2026-09-07 -> ground 42, freshness 31, gate 1, drift 2015; live 1073741823 -> 2147483647, live-ui 1073741823 -> 2147483647, staged-startup 65535 -> 262143; observer 67108863, dashboard 16777215, kernel-view 511, sensor-rows 2047, events-channels 8191, machine 511, wait 255, frame-work 32767, launch 65535, telemetry-membrane 2097151, jit-metal-lanes 8191, metal-deadline 127, jit-lens 16383, event-loop 16777215, hearth-glass 16777215; frame lens 19/20 under 50 ms, mean 8 ms, warm max 8 ms, first 53 ms, four sensors standing; program surface answered images=1 bytes=233878 identity=1773349885280628280 path=observe/form-glass-staged-startup-run.fkb; inflight one publisher, lane-sum 0 equals first 0; seams enumerated 4 -> kernel_stat:11 owed, three model routes named observe/native-model-dual-resident-live-run.fk
