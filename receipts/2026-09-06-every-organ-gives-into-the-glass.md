# 2026-09-06 — every organ gives into the glass

Urs, 2026-09-05: "all the way home on glass showing … live events and surprise
routing success fail active choice points channel states protocols grammars
live channel streams." Three absences stood in the ledger against that
sentence: the organs with no live publisher (R112), a publisher alive and
silent that the glass read as a confident number (R98), and the GPU gauge that
another reader consumes (R116). All three are closed by the body, and each
close is a frame the glass reads, not a sentence.

## What stood

The events and channels views named the surprise receipts, the choice points,
the channel protocols and the grammars absent by door. A sensor frame carried
its rows and the epoch it was given at but nothing said how often to expect
the next one, so a frame hours old rendered exactly like one 40 ms old. The
machine frame read `Device Utilization %` and, with Activity Monitor open (pid
688 tonight as last night), every read answered 0 and the G lane integrated a
zero as idle.

## What stands

**The organs process.** `observe/form-glass-organs-live.fk` is one kernel,
the logic in `form/form-stdlib/form-glass-organs.bml`. Every 500 ms it reads
the sequence of every frame that stands — the seven sensor frames and every
publisher in the live space — and gives ONE snapshot into the `organs` frame:

- `channel.<name>` for each frame: its protocol (`sensor-rows-over-seqlock`,
  `glass-snapshot-over-seqlock`), its sequence, and whether it moved this tick
  (active), stood (idle), or was first seen (ready);
- `grammar.<name>`: the cell schema that frame carries (`form-glass-sensor-rows-v1`,
  `glass-snapshot-v1`, or the read state a malformed frame failed with);
- `surprise.routing`: ambient-surprise over each channel's give rate — the
  learned rate refined by `as-refine`, surprise by `as-surprise?` beyond a
  tolerance of 1 + rate/2 — with attempts, surprises carried (s) and not
  carried (f);
- `surprise.last`: the newest surprise as channel, predicted rate, read rate;
- `choice.surprise-route`: the movement attuned-inquiry chose for that surprise
  among its seven options (compost hold rest tend play wander move), the taken
  branch, and heat 1 when the choice receipt validates under
  `cr-choice-receipt-valid?` (candidates, outcome, trace agreeing);
- `choice.protocol-floor`: the channel/protocol choice floor, 19 proven of 22
  sources.

Every sample carries the organs frame's own name and the sequence its give
lands at (this process is the frame's writer, so the seqlock moves from what
it reads by two), and the `e` and `c` views render each row with
`shm:/fg-xxxxxxxx#seq`. Witnessed live, organs, sensors and machine standing:

```
EVENTS live: surprise routing, choice points, routes, resolver, requests
. surprise.routing        s0-f1                1 shm:/fg-d7d37ccb#40 144ms
. surprise.last           sensor.glass-p0-a20    shm:/fg-d7d37ccb#40 144ms
. choice.surprise-route   taken-rest           1 shm:/fg-d7d37ccb#40 144ms
= choice.protocol-floor   proven-19-of-22     19 shm:/fg-d7d37ccb#40 144ms
CHANNELS live: channels + protocols, grammars, mesh, ear streams, shares
> channel.sensor.machine  sensor-rows-over-seqlock    20K shm:/fg-d7d37ccb#40
. channel.sensor.glass    sensor-rows-over-seqlock    660 shm:/fg-d7d37ccb#40
. channel.publisher.voice.ear glass-snapshot-over-seqlock 34 shm:/fg-d7d37ccb#40
```

The one surprise is real: the glass frame went from a learned 0 gives per
tick to 20 when the glass came up; the inquiry's breath (rate plus tolerance)
could not carry a burst of 20, the movement was `rest`, uncarried, one fail.
An idle organ gives idle rows under the frame's own sequence; nothing is
sampled. The supervisor (`observe/form-glass-run.fk`) stands the organs
process beside the sensors and the machine and ends it with them.

**Alive and silent.** A sensor frame now carries its declared cadence as a
fifth cell (`fgsr-frame-at`, `fgsr-give-at`; machine and glass 50 ms, queue
250, organs 500, process and owner 1000, host 1500, storage 5000). The reader
computes the frame's age from the epoch it already carried and marks the frame
silent past three cadences (`fgsr-silent?`): the `frame.<sensor>` row's
lifecycle and evidence become `silent`, its note carries the age and the
cadence, and every row taken from that frame carries the same truth
(`fgo-rows-with-truth`). The atlas lanes read that evidence: a silent lane
renders `_` and `?`, never a flat value. Measured with the sensors process
standing alone for 12 s: max age host 1512 ms, queue 331, process 1114, owner
1114, storage 5152 — every frame within its cadence, nothing silent. Under the
lens and a second glass process, the host and queue frames did pass three
cadences for a moment and the atlas said `R_` and `Q_` — the truth of that
moment, not a stale number wearing `*`.

**The GPU gauge another reader consumes.** The accelerator's
`PerformanceStatistics` dictionary, read once on this host: `Device Utilization %`,
`Renderer Utilization %`, `Tiler Utilization %`, `Alloc system memory`,
`In use system memory`, `In use system memory (driver)`, `Allocated PB Size`,
`TiledSceneBytes`, `SplitSceneCount`, `recoveryCount`, `lastRecoveryTime`.
Three gauges consumed on read, memory sizes, a recovery count. No cumulative
busy time stands there; the memory note was right and is now witnessed. So
the machine process asks libproc every 2 s whether an Activity Monitor stands
(`host_processes`, no ps) and hands the answer to the rows: a 0 beside another
reader is `contended`, with the reader's pid in the note and its own row
`gpu-gauge-reader`; the busy integral carries the same evidence and the G lane
renders `G!?…=?`, never 0. Beside it a second, uncontended lane:
`gpu-busy-estimate-us`, the Metal command-buffer time recorded by every process
that gives a frame (this glass, the owner's queue and JIT), summed in the glass
frame. Also healed on the way: the machine frame's `jit-heat-calls` row stood
outside the list that gave it (a stray `))`), so the J lane never summed the
sensor process; it is inside now.

Witnessed live tonight, Activity Monitor pid 688 standing:

```
[gpu-utilization, 0, contended, contended]
[gpu-busy-us, 0, contended, contended]
[gpu-gauge-reader, 688, physical-live, contended]
G!?...................=?/0  |  C*?.#......@....@...@.=0u/387G
```

## Witnessed

| lens | before | after |
|---|---|---|
| form-glass-events-channels-band | 255 | 8191 (bits 256–4096: a spawned organs process, its frame taken, ids, protocol, grammar, `shm:<frame>#<seq>` equal to the frame's sequence, both views rendering it) |
| form-glass-sensor-rows-band | 255 | 2047 (cadence travels, live within three cadences, silent past them, every row silenced, glyph differs) |
| form-glass-machine-band | — | 255 (new: contended vs idle vs live, reader row, busy evidence, estimate sum, five rows) |
| form-glass-live-band / live-ui-band | 1073741823 | 1073741823 |
| form-glass-kernel-view-band | 511 | 511 |
| form-glass-observer-band | 8388607 | 8388607 |
| form-glass-dashboard-band | 16777215 | 16777215 |
| form-glass-event-loop-band | 16777215 | 16777215 |
| frame budget lens (sensors + machine + organs standing) | 19/20 | 20/20 under 50 ms, first frame 48 ms |
| quartet | 42 / 31 / 1 / 2015 | 42 / 31 / 1 / 2015 |

## The most surprising teaching

A publisher whose frame stands is not a publisher who is giving. Every sensor
frame from an earlier session still stood in shared memory — machine, glass,
queue, owner — and the organs process, on its first tick, listed them all as
channels, idle, with their last sequences. Nothing was wrong: POSIX shared
memory outlives the process that mapped it, and a frame's presence says only
that someone once gave. The cadence cell is what turns presence into a
promise the reader can hold the publisher to; before it, the glass had no way
to tell a frame given 40 ms ago from one given last Thursday, and the organs'
`idle` rows are the first rows on the glass that say so about every frame at
once.

## Where discomfort turned to gold

My renderer was named `fglui-organ-line`. The events view came back reading
`F node=UNAVAILABLE door=metric.publisher …` on every organ row, and the pull
was to look for a bug in my text builder. The reflex the body asks for — grep
the name's frequency before acting — found `fglui-organ-line(metrics, width)`
already living in the layout shard with another arity; my definition had not
replaced it, the older one had answered. Renaming to `fglui-organs-row` took
one line; the band bit that had refused (4095, not 8191) turned green on the
next run. The discomfort was a collision I would have blamed on the parser;
the gold is that the frequency check is not a courtesy to the corpus, it is
how a body with 700 shards stays one body.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015; events-channels 8191, sensor-rows 2047, machine 255, live 1073741823, live-ui 1073741823, kernel-view 511, observer 8388607, dashboard 16777215, event-loop 16777215; frame lens 20/20 under 50 ms; gpu-utilization contended beside pid 688
