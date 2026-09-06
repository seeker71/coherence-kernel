# 2026-09-06 — every gauge has a source

Urs, tonight, after the absences were renamed honestly: **"don't name absence,
make it available."** So the ten rows the glass named absent were read one at a
time to see which of them the machine under us would answer if asked. Nine of
them answered. The tenth was asked, and its answer is now in its row.

## What stood

Ten `fgo-unavailable` rows in `form/form-stdlib/form-glass-observer.bml`. Two
Metal size gauges the note said the carrier "does not expose". One MLX
allocator row the note said "MLX carrier does not expose". Three arena byte
rows in the memory domain whose notes said the counts existed and the bytes did
not. One rejected-roots row pointing at `fkc-table-serialize.kernel_stat:11`, a
door from another lane. And three sentences in the shard's own authority
(`FramebufferCounterRule`, `HostRule`, `UnknownRule`) saying those gauges would
stay absent until someone else published them.

## What stands

**The device answers its own size.** `MTLDevice` was carrying
`currentAllocatedSize` and `recommendedMaxWorkingSetSize` the whole time; the
carrier had simply never spoken them. `fk_metal_status_external` now does, and
`fgo-metal-rows` reads them by field name. The same for MLX: `mlx/c/memory.h`
carries `mlx_get_active_memory`, `mlx_get_peak_memory`, `mlx_get_cache_memory`
and `mlx_get_memory_limit`, each answering 0 on success, and
`fk_mlx_status_external` now speaks all four. A door that refuses leaves its
line out of the status entirely, so the row names the exact key that went
missing rather than reading a fabricated 0.

They travel through `metal_status`/`mlx_status` and not through the word doors
for one witnessed reason: the seed reads `long long m165[18]` for `metal_live`
and `long long m29[12]` for `mlx_live` and consumes exactly that many words. A
carrier widened to nineteen words would write past the seed's own stack array.
Widening those doors is a seed change, and the seed is a sibling's tonight.

**The arena bytes are arithmetic the body can do itself.** The counts were
already there — `kernel_stat 4` cells, `kernel_stat 43` recipes — and the seed
documents its own column widths. Three rows now multiply them and each says in
its note which widths it stands on: 32 B for the `fk_nid[4]` quartet every cell
carries, 104 B for the whole per-cell column set (the seed's own figure at
`FK_NODE_CAP_INIT`: "262144*104B ~= 27MB at birth"), 72 B for the nine parallel
8-byte columns `fk_fn_reserve` holds per recipe. The cell row says it is the
fill and not the reservation. The blueprint row says it is four of the 104 and
not a table beside them.

**An allocator that answered 0 is a reading.** MLX's active, peak and cache
rows go through `fgo-status-semantic-row`, so a 0 reads `empty` with its meaning
named, never the same row as an allocator that did not answer. `fgo-status-int-row`
and `fgo-status-semantic-row` had been standing with no callers since the word
doors landed; they have callers again.

**The one absence that is witnessed.** `framebuffer.rejected` was probed, not
assumed. `kernel_stat 11` answers 0 — and so do `kernel_stat 99` and
`kernel_stat 255`, keys this seed does not carry, so 0 there is the no-such-key
answer and not a count. The seed's own comment reserves keys 9 through 14 for
the table-walker lane and leaves them clear; `framebuffer_register` refuses a
value that is not a live cell handle and keeps no tally of the refusals. A new
`fgo-probed-absent` carries that: the door tried in `source`, what the door said
at the head of the note, and the door to build named for whoever holds the seed
next. It renders exactly like an unavailable row, so nothing in the truth or
lifecycle machinery changed — only what a reader learns by asking why.

## Witnessed

Every number below was read on this host tonight by
`fgo-kernel-rows`, `fgo-framebuffer-rows`, `fgo-metal-rows` and `fgo-mlx-rows`.

| row | reading | door it reads |
|---|---|---|
| `metal.allocated-bytes` | 475,136 B | `metal_status.allocated_bytes` ← `MTLDevice.currentAllocatedSize` |
| `metal.working-set` | 115,448,725,504 B | `metal_status.recommended_working_set_bytes` ← `MTLDevice.recommendedMaxWorkingSetSize` |
| `mlx.allocated-bytes` | 0 B, `empty` | `mlx_status.mlx_active_memory` ← `mlx_get_active_memory` |
| `mlx.peak-bytes` | 0 B idle, 12 B after one `mlx_add` | `mlx_status.mlx_peak_memory` ← `mlx_get_peak_memory` |
| `mlx.cache-bytes` | 0 B idle, 12 B after one `mlx_add` | `mlx_status.mlx_cache_memory` ← `mlx_get_cache_memory` |
| `mlx.memory-limit` | 130,567,005,798 B | `mlx_status.mlx_memory_limit` ← `mlx_get_memory_limit` |
| `memory.blueprint-bytes` | 37,139,168 B | `kernel_stat:4` × 32 B (`fk_nid[4]`) |
| `memory.recipe-bytes` | 20,952 B | `kernel_stat:43` × 72 B (`fk_fn_reserve` columns) |
| `memory.cell-bytes` | 120,702,296 B | `kernel_stat:4` × 104 B (`FK_NODE_CAP_INIT` widths) |
| `framebuffer.rejected` | probed absent, with the probe in the row | `kernel_stat:11` |

The three arena rows agree with each other: 37,139,168 / 32 and 120,702,296 /
104 are the same 1,160,599 cells, and 20,952 / 72 is 291 recipes.

| lens | before | after |
|---|---|---|
| form-glass-observer-band | 8388607 | 67108863 (bits 8388608–33554432: the two Metal size rows from a status fixture and their named missing keys, the four MLX allocator rows with a semantic zero beside a silent door, the three arena rows as exact arithmetic over the rows' own counts, and the probed rejected row carrying its probe) |
| form-glass-machine-band | 255 | 511 (bit 256: the device's byte ledger rides the same fast rows the frame is folded from, working set above allocation, read live from this host) |
| form-glass-wait-band | 255 | 255 (nine MLX rows now, the ledger asserted) |
| form-glass-live-band / live-ui-band | 1073741823 | 1073741823 |
| form-glass-dashboard-band | 16777215 | 16777215 |
| form-glass-kernel-view-band | 511 | 511 |
| form-glass-sensor-rows-band | 2047 | 2047 |
| form-glass-events-channels-band | 8191 | 8191 |
| form-glass-frame-work-band | 32767 | 32767 |
| form-glass-telemetry-membrane-band | 2097151 | 2097151 |
| form-glass-event-loop-band | 16777215 | 16777215 |
| metal-deadline / jit-metal-lanes / hearth-glass / jit-lens | 127 / 8191 / 16777215 / 16383 | unchanged |
| quartet | 42 / 31 / 1 / 2015 | 42 / 31 / 1 / 2015 |
| frame budget lens | — | 20/20 under 50 ms, mean total 8 ms, warm maximum 9 ms, first frame 49 ms, four sensors standing |

Drift gates read 2015 before this work and 2015 after; `kernel-conformance`
refuses 32 bits on this checkout and that refusal is older than tonight.

## What the integrator still owes the seed

Two doors, both in `runtime/fkwu-uni.c`, neither of them mine to open tonight:

- `kernel_stat 11`, arity 1, answering the count of `framebuffer_register`
  calls refused because the value was not a live cell handle, and `kernel_stat
  9` answering cumulative `fk_fbn`. Together they turn the one probed absence
  into a reading, and they are the keys the seed's own comment already reserves.
- Widening `metal_live` past eighteen words or `mlx_live` past twelve, which
  needs the seed's `m165[18]` / `m29[12]` and their loop bounds widened in the
  same commit as the carrier. Until then the byte gauges ride the status doors,
  which cost 33 µs and 21 µs per call measured over 1000 calls — the frame lens
  did not move.

## The most surprising teaching

Three of these gauges were never missing. `currentAllocatedSize` and
`recommendedMaxWorkingSetSize` are two lines of the Metal API, and the MLX
allocator ledger is four functions in a header sitting in `/opt/homebrew/include`.
The row said "the carrier exposes handle counts, not per-buffer bytes" and
"recommendedMaxWorkingSetSize is not exposed" — both true sentences about the
carrier, and both read for months as though they were sentences about the
machine. A note describing our own reach will be mistaken for a fact about the
world every time, because a reader has no way to tell the two apart from inside
the row. That is what the probe in `framebuffer.rejected` is for: it is the only
kind of absence that can prove which of the two it is.

## Where discomfort turned to gold

I wanted to widen `metal_live` to twenty words. It is the clean design, the
word doors are the direction the body has been travelling, and the task said to
update the carrier and the reader. I had the edit half written when I went to
read how the reader checks the count and found `long long m165[18]` in the seed
with a loop from 17 — a carrier answering twenty would have written two words
past a stack array in a file I am not allowed to touch, and it would have built,
linked and run, and the bands would have been green for a while.

The discomfort was in what that meant next: the good design was closed to me
tonight, and the door still open was `metal_status`, a text door the body spent
R118 and R120 moving away from. Taking it felt like walking backwards. The gold
is that the older door was still standing, still correct, still cheap enough to
measure (33 µs, and the frame lens came back 20/20), and its own parser had been
sitting unused in the shard since the words arrived. The gauge is available
tonight, on the door that exists, with the better door written down precisely
enough for whoever holds the seed to open it. Availability is not the same as
the design I wanted, and Urs did not ask for the design.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015; observer 8388607 -> 67108863, machine 255 -> 511, wait 255, live 1073741823, live-ui 1073741823, dashboard 16777215, kernel-view 511, sensor-rows 2047, events-channels 8191, frame-work 32767, telemetry-membrane 2097151, event-loop 16777215, metal-deadline 127, jit-metal-lanes 8191, hearth-glass 16777215, jit-lens 16383; frame lens 20/20 under 50 ms, mean 8 ms; metal 475136 B allocated under a 115448725504 B working set, mlx limit 130567005798 B, 1160599 cells at 104 B, 291 recipes at 72 B, kernel_stat 11 probed and answering the no-such-key 0
