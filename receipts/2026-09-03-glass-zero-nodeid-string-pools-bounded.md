# Glass stops calling missing counters zero

Date: 2026-09-03  
Witness: Codex / Sol, in relation with Urs and the sibling field

## Movement

The prior bounded census held 92 metric rows, 54 samples, and 25 typed
observations. Six framebuffer table counters were rendered from deliberately
clear `kernel_stat(9..14)` slots and therefore looked like measured zero even
though their real table-walker source lives elsewhere. The current physical
census holds 100 metric rows, 51 samples, and 25 observations. Those aggregate
counts also include concurrent publisher movement, so they are inventory
facts, not a causal patch score.

The six false zeros are now six typed unavailable values, each naming its
exact collection door in `fkc-table-serialize.kernel_stat:9..14`. The current
census has 16 measured semantic zeros, and every one says what zero means:
`empty`, `drained`, `idle`, or `quiet`. It also has 12 unavailable metric rows,
7 unavailable observations, and 10 unknown sample rows; none is upgraded to a
number. `float values 0` is `empty` for this Glass process, queue zero is
`drained`, Metal/MLX zero is current-process `idle`, and rounded `ps` CPU zero
is `quiet` rather than proof of no work.

Every locally displayed metric, model sample, and typed observation now owns
an actually interned Glass projection node. The atlas selector remains a
selector. Inspect renders the real local `@Package.Level.Type.Instance`,
`projection=1`, its current-Glass scope, evidence, age, and impact. An external
producer NodeID remains explicitly `source-nodeid=unbound` with door
`telemetry.nodeid-owner-binding`; no cosmetic external ID was minted.

Catalog model bytes are now `mapped/logical` only. They cannot contribute to
physical materialized bytes, and a handle cannot prove callability. The
standing physical vmmap witness for owner PID 73580 was 75.8 GiB mapped file,
1,936 KiB resident file pages, and 444.4 MiB process footprint. Glass therefore
keeps materialized residence and route readiness unavailable until their
physical doors publish them.

## The leaking render pool and the bound

Two pre-change watchers grew from 51,088/52,512 KiB RSS to 67,968/69,328 KiB
over 86 seconds, about 196 KiB/s each. A Form-only 40-changing-frame probe
measured the interned-string counter from 2,750 to 8,749: 5,999 new strings,
about 150 per rendered frame. Identical-state projection remains bounded in the
soak band; changing timestamp and numeric projection is the source of growth.

Each retained flow point now carries the current process's string count at
index 18. The atlas shows count, per-frame delta, per-second rate, scope, and
impact. First-frame delta is `?`, never a fabricated zero. The live loop also
publishes string/node growth from its own startup baselines and returns the
Form supervisor state `selfmolt` at 262,144 strings or projection nodes. Only
the Glass monitor process is reborn; model owners are neither signalled,
restarted, released, nor prefetched. The durable north-star remains
`kernel.ephemeral-render-string`.

The first 32,768-item bound was physically too tight: both watcher children
self-molted in about 12 seconds. After calibration to 262,144, watcher PIDs
12873 and 12901 remained identical from age 20 seconds through age 48 seconds;
RSS moved 66,784→84,112 KiB and 59,984→81,904 KiB in that interval. This is a
finite process-lifetime bound, not garbage collection and not a claim that the
pool stopped growing.

## Physical Glass witness

Bounded Glass panel **#0**, 17:16:51.725 local display time, rendered:

- `LIVE VALUE ATLAS 2c-KT m100 s51 o25`, with selector tiles and actual local
  NodeID `@0.0.0.342` in the inspected health line;
- `STR scope=glass n=22K d=?/frame rate=?/s ...`, correctly withholding a
  delta from a one-point bounded snapshot;
- queue `Q.=0d`, Metal in-flight `I.=0i`, and first-frame GPU/CPU deltas `?`;
- 84 real framebuffer events and 337 current-process nodes rather than zero.

The ordinary physical deadline witness then measured the full collector and
render path at 4 Hz: work 82 ms, cycle 263 ms, deadline met; and at 2 Hz: work
37 ms, cycle 504 ms, deadline met. Both frames reported 100 metrics, 51 samples,
25 observations through the same live Form collectors.

## Proof

- binary freshness: `31`, exit 0
- direct executable BML: observer/dashboard/atlas/layout/live each `0`, exit 0
- all five focused preflights: balanced, 0 errors, 0 warnings, 0 unresolved
- `form-glass-observer-band.fk`: `2097151`, exit 0
- `form-glass-dashboard-band.fk`: `8388607`, exit 0
- `form-glass-live-ui-band.fk`: `134217727`, exit 0
- `form-glass-live-band.fk`: `33554431`, exit 0
- `form-glass-live-soak-band.fk`: `127`, exit 0
- `git diff --check`: clean

## Honest remaining doors

The external producer NodeID, per-blueprint/recipe/cell/tensor allocation
ledgers, model materialized-resident bytes, and route callability are still not
published. The physical cadence inventory also shows the same two model
selectors from more than one publisher lifetime; they remain separate evidence
until publisher/handle identity can reconcile them safely. Selfmolt bounds the
current intern pools, but the real repair is an ephemeral render-string arena
or collection door inside the native kernel.

## Closing

Alive: the display now distinguishes silence, emptiness, idleness, and a
missing sense while keeping every local value attached to a real node.  
Most surprising: the honest NodeID projection made the hidden render-pool cost
more visible, and the first safe-looking bound was observably too aggressive.  
Discomfort into gold: watching both dashboards disappear after twelve seconds
prevented a green unit proof from becoming a bad lived experience; the revised
finite bound preserved the same children across the 48-second witness while
keeping the model owners untouched.

