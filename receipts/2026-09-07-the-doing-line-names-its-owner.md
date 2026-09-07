# 2026-09-07 — the DOING line names its owner

Urs, reading a live frame: **"owner=absent? ... PRESS rss=?, cpu=?, gov=? ...
all point to unfinished work."** So the two densest rows of the atlas were read
field by field, and each `?` was followed back to whoever owes it.

## What stood

```
DOING n=0 cpu=0ms rss=? owner=absent T=none pin=? dsk=? ?
PRESS free=83% used=17% ok file=717K cmp=0 rss=? cpu=? gov=2 g.w.i.
```

Eleven fields with no name on them. `owner=absent` was returned for four
different situations by one branch. The trailing field was a bare `?` with no
label at all. And `gov` was not what it looked like.

## What stands

**Every field carries a reading or the door that owes it.** `fglat-door` in
`form/form-stdlib/form-glass-atlas-ui.bml` renders an absent field as `?` plus
the name of who would give it: `host`, `process`, `owner`, `machine` are the
sensor names `observe/form-glass-sensors-live.fk` gives under; `dual-liveness`
and `owner-pid` are the two steps of the owner-liveness publication; `token`,
`metal_live` and `governor` name the sample kind, the word door and the
publisher a reading would come from. Nothing on these two rows is a bare `?`
any more.

**The four owner states are told apart.** `fglat-owner-absent-text` reads the
`model-owner-present` row and answers `?owner` when the owner sensor has given
no row at all, `?dual-liveness` when it gave one and no `models.dual-liveness.*`
publisher stands, `?owner-pid` when a liveness row stands with no pid handle,
and `dead` when the pid is gone. The old branch answered `absent` for the first
three and `dead` only for the fourth, so a reader could not tell a missing
publisher from a missing sensor. Tonight's host says `?owner`: `nmdoc-discover`
refuses because `nmdoc-newest-model-publisher` finds no `models.dual-resident.i*`
in the roster — no native model owner stands here, and that is now the visible
sentence rather than an inference.

**`gov` is a reading again, not a constant.** The old field rendered
`fgtm-sample-capacity` of the governor's pressure row. That slot is
`frgg-policy-pressure-critical` — the policy's critical *level*, the fixed
number 2. So a healthy machine displayed `gov=2` and looked pinned at maximum
pressure. `fglat-governor-text` now renders the measured level over that
critical level with the door that answered: `gov=0/2@governor` from the
published frame, `gov=0@vm_stat` when the frame is not current and the level is
computed here from the same free-percent the publisher samples, and
`gov=?governor` when neither stands.

**The device bytes are scoped by name.** The DOING line's trailing field is now
`gpu=`: the owner's `memory.owner.metal-current-allocated` sample as
`<bytes>@owner` when an owner stands, otherwise this Glass process's own
`MTLDevice.currentAllocatedSize` row as `<bytes>@glass`, then `?machine` when
the machine sensor has given no such row and `?metal_live` when the row stands
and the carrier refused. `fglat-domain-metric` reads it by domain, because
`fgd-metric-index` is keyed by id alone and both `metal` and `mlx` publish a row
called `allocated-bytes`; the index returns whichever was written last.

**`fglat-sample-bytes-text` is gone.** It had one caller and that caller now
names its scope, so the door was removed rather than left standing empty.

## Witnessed

Live frame on this host, `./fkwu observe/form-glass-run.fk` with its own
sensors, machine and organs standing, terminal width 100:

```
DOING n=23 cpu=346Kms rss=24GiB load=7.3 owner=?owner T=?token pin=+27 dsk=5.29MB/s gpu=?machine
PRESS free=61% used=39% ok file=2M cmp=0 rss=?owner cpu=?owner gov=0/2@governor g=w=i=
```

| field | reading tonight | the door it reads |
|---|---|---|
| `DOING n=` | 23 | count of live `process.*-rss` rows, `sensor.process` (pgrep + ps per fkwu) |
| `DOING cpu=` | 346Kms | max of live `process.*-cpu`; `?process` when the sensor has given none |
| `DOING rss=` | 24GiB | sum of live `process.*-rss`; `?process` when the sensor has given none |
| `DOING load=` | 7.3 | `load-1`, sysctl loadavg through `sensor.host` |
| `DOING owner=` | `?owner` | `model-owner-rss`/`-cpu` from `sensor.owner`; then `?dual-liveness`, `?owner-pid`, `dead` |
| `DOING T=` | `?token` | newest `token-position` sample, `fglui-owner-token-fallback-samples` |
| `DOING pin=` | +27 | `pageins-delta` then `pageins`, `mach.vm_statistics64.pageins` through `sensor.host` |
| `DOING dsk=` | 5.29MB/s | `disk-mbps` then `disk-tps`, iostat through `sensor.host` |
| `DOING gpu=` | `?machine` | owner sample `memory.owner.metal-current-allocated`, else `metal_live` word 18 through `sensor.machine` |
| `PRESS free=` / `used=` | 61% / 39% | `memory-free`, `mach.vm_statistics64` through `sensor.host` |
| `PRESS <level>` | ok | free-percent thresholds 10/5, the same scale `frghp-level` holds |
| `PRESS file=` | 2M | `file-backed-pages`, `mach.vm_statistics64.external_page_count` |
| `PRESS cmp=` | 0 | `compressor-pages`, `mach.vm_statistics64.compressor_page_count` |
| `PRESS rss=` / `cpu=` | `?owner` | the same four owner states as the DOING line |
| `PRESS gov=` | `0/2@governor` | `governor.pressure.memory` bytes over capacity, publisher `resource.governor` |

| lens | before | after |
|---|---|---|
| form-glass-live-ui-band | 1073741823 | 2147483647 (bit 1073741824: every DOING and PRESS field asserted — seven doors on an empty frame, seven on an empty pressure line, the four owner states told apart, the published `1/2@governor` and the local `0@vm_stat`, the `@glass` scope and the `?machine`/`?metal_live` split) |
| form-glass-observer / live / dashboard / kernel-view | 67108863 / 1073741823 / 16777215 / 511 | unchanged |
| sensor-rows / events-channels / machine / wait | 2047 / 8191 / 511 / 255 | unchanged |
| frame-work / telemetry-membrane | 32767 / 2097151 | 32767 / 2097151 (frame-work flaps 20479–32767 under this host's load; 32767 reached before and after) |
| resource-governor / host-pressure / observation-v2 | 1048575 / 4095 / 2097151 | unchanged |
| hearth-glass / jit-lens | 16777215 / 16383 | unchanged |
| quartet | 42 / 31 / 1 / 2015 | 42 / 31 / 1 / 2015 |
| frame budget lens | — | 19/20 under 50 ms, mean total 9 ms, warm maximum 10 ms, four sensors standing |

Drift gates read 2015, the same reading the 2026-09-06 receipt recorded: the
kernel-conformance 32 refuses on this checkout and that refusal is older than
tonight.

## What the integrator still owes the seed

One door, in `runtime/fkwu-uni.c`, and it is the reason half of tonight's `?`
exist at all.

`fk_gift_give` is a single-writer seqlock: it loads `s0`, stores `s0 + 1`,
copies the payload, stores `s0 + 2`. Two processes writing the same frame can
both load the same `s0`, and the sequence is then left **odd** with no writer
active — and it never repairs, because the next give also computes its parity
from the value it loaded. `fk_gift_take_str` retries 4096 times and answers
nothing, so the reader sees `unreadable` and the row goes `?`.

That is not a hypothesis. In a private root, one writer of the ten-row governor
frame reads back `current`, cross-process too. Three concurrent writers of the
same frame leave it `malformed / frame-unreadable` after every writer has
exited. On the live root tonight, `resource.governor` read `malformed` thirty
times out of thirty while three glass fleets stood on this host, and three of
the six sensor lanes — `machine`, `queue`, `owner` — answered zero rows to an
outside reader while `host`, `process` and `storage` answered 71, 54 and 20.
`owner=?owner` and `gpu=?machine` in the frame above are that wound seen from
the glass.

The seed change is small and is not mine tonight: a give that lands on an even
sequence whatever it loaded, and two writers that cannot claim the same `s0`.
An atomic fetch-add of 1 to open and a store of `(claimed | 1) + 1` to close
would give both. Until then, any second Glass on a host silently empties
lanes for the first one, including Urs's.

## The most surprising teaching

`gov=2` was never a measurement. It was `frgg-policy-pressure-critical` — a
constant in the governor's policy, read out of the pressure row's capacity slot
and printed where a level belongs. The machine was at level 0 the whole time.
A constant wearing a gauge's position on the glass is worse than a `?`, because
a `?` at least tells you not to trust it; the 2 told a reader the machine was
pinned at its critical threshold and gave them no way to notice. Every absence
on that line was honest about being an absence. The one field that carried a
number was the one that lied.

## Where discomfort turned to gold

I wanted the governor level to come from the governor's own `frghp-level`, so
the free-percent thresholds would have one home instead of being mirrored in
the atlas. I added `form-resource-host-pressure.bml` to the atlas shard's
preludes, the band went green, and then `form-glass-frame-work-band` dropped
from 32767 to 24559: the extra unit pulled the whole governor chain into the
render process's startup and the glass never reached a rest inside the band's
four-second window. The clean design cost the frame its budget.

The discomfort was that the honest fix and the honest cost pointed opposite
ways, and I had already written the comment claiming one home. Sitting with it,
the thing I actually owed was not the deduplication — it was the reading. So
the level now comes from `fglat-level-number` over the atlas's own level word,
which is the same 0/1/2 scale on the same free percent, and the mirror is
written down in the comment beside the thresholds instead of being resolved by
loading a chain the render path has no other use for. The gauge became
available, the frame kept its budget, and the duplication is now named where
the next reader will meet it rather than quietly paid for in startup.

Signed, a sibling in Sema's worktree, 2026-09-07.

; witnessed: 2026-09-07 -> ground 42, freshness 31, gate 1, drift 2015; live-ui 1073741823 -> 2147483647, observer 67108863, live 1073741823, dashboard 16777215, kernel-view 511, sensor-rows 2047, events-channels 8191, machine 511, wait 255, frame-work 32767, telemetry-membrane 2097151, resource-governor 1048575, host-pressure 4095, observation-v2 2097151, hearth-glass 16777215, jit-lens 16383; frame lens 19/20 under 50 ms, mean 9 ms, four sensors standing; live frame owner=?owner T=?token gpu=?machine gov=0/2@governor; governor frame malformed 30 of 30 on the live root, current 1-of-1 and cross-process in a private root, malformed after three concurrent writers; sensor lanes host 71 / process 54 / storage 20 rows, machine 0 / queue 0 / owner 0
