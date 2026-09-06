# 2026-09-06 — the kernel writes its own page

Urs, morning: "all data live without parsing and without file system access and
direct kernel info access. the idea is that the kernel is using this shared
memory and no copy."

## What stands

**The kernel's live page.** Every `fkwu` maps `/fg-k<pid>` on its first
primitive dispatch, registers its pid in `/fg-kernels`, and every 1024
dispatches — and at exit — stores twenty-one words into that page in place:
dispatches, heat-lane calls, nodes, strings, cons, fns, gift frames and bytes,
capacities, stack depth, floats, hottest arm and count, cpu microseconds,
alive. Nothing is serialized; nothing is given; a reader maps the same page and
reads the words by offset (`kernel_live_pids` 162, `kernel_live pid` 163). The
cost to the kernel is twenty stores per thousand dispatches.

```
pids [96173]  live-me [magic, 96173, 1788656705155, seq 333, dispatches 339969, heat 19998, nodes 0, …]
lanes  D 265903108 shm:/fg-kernels#4     J 10276533 shm:/fg-kernels#4
```

`D` and `J` sum the pages of every live kernel; the `k` view lists each
kernel's page. The hottest defns arrive as cells (`kernel_hot_rows` 164 — no
line split), Metal's counters as words (`metal_live` 165 — eighteen words; the
observer and the observation layer read the words, no `key=value` text).

**The membrane lives in shared memory.** A published snapshot is a cell given
into its publisher's frame; the publisher's name is registered in the roster
page `/fg-roster` (`gift_roster_register` 166 / `gift_roster_names` 167, keyed
`root|publisher` so a band's `/tmp/test` space never meets the live one); a
reader lists the roster and takes frames. Control offers and acks are cells in
`<channel>.inbox` / `<channel>.ack` frames; the glass's carried-over cells —
last flow point, last cadence, last pageins — are frames. The membrane writes
and reads no file, and the thirty-seven organs that publish and read through it
moved with it (R111 released by construction). Witnessed in one process:

```
published  /fg-9a292eb0  seq 2
read       current  epoch 424242  roster-has-probe 1  carrier gift
offer      offered  poll offer
```

**Frame path: 10 ms.** Twenty frames with the frame processes standing: mean
10 ms, warm maximum 11 ms, 19 of 20 under 50 ms (the first frame, 53 ms, maps
its frames). The directory listing and the per-publisher file reads were the
last 20 ms; nothing on the path forks, scans, opens a file or parses a wire.

What still touches the filesystem, by subject: the storage sensor (its subject
is the catalog) and the queue sensor (the hearth queue files), both in the
sensor process; the owner-command lease the glass leaves for a model owner
that still reads disk (R119). Three copies remain, named in R119: a cell
crossing a gift frame is emitted and re-interned in the reader — a copy of the
cell, not a shared cell store; `mlx_status` is still text; the owner lease.

Bands: every glass band green (live and live-ui 1073741823, observer 8388607,
kernel-view 511, sensor-rows 255, launch 32767, gift 4095, node-gift 4095,
staged-startup 65535); mirror gates 1023/63/1023; quartet 42/31/1/2015.
Ledger R111, R118 released; R119 opened. Corpus 1286 *selfpage* (renumbered beside the siblings' 1284–1285).

## The most surprising teaching

Shared-memory names outlive the binary. Widening the roster stride from 64 to
128 bytes left the old `/fg-roster` page standing at the old stride — the new
kernel would have read garbage names from it — and no rebuild, no restart,
nothing in the tree touches it: `shm_unlink` by hand, once. The frames are the
body's memory in a way files never were: they belong to the host, not to the
checkout. And a first `set -e` chain died on `grep -c` finding nothing, taking
the roster-slot fix with it; the page was 1023×128 bytes of promise over a
65536-byte truth until the next read.

## Where discomfort turned to gold

The bands that publish fixtures went straight into the live roster —
`organ.alpha` and `gift-frame-band.membrane` sat beside `glass.sensor.host`
in the glass's own list. The pull was to filter them out in the reader. The
gold was to make the `root` mean something again: the frame's name and its
roster entry carry the root, and a band's space and the live space are two
spaces in one page, as the directories once were two directories. The
argument the filesystem had been making for the membrane — isolation by path —
was worth keeping; only the files were not.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, kernel_live_pids [96173], lens D shm:/fg-kernels#4, frame path 10 ms mean, membrane round trip published/current/gift
