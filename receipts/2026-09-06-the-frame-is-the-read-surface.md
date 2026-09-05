# 2026-09-06 — the frame is the read surface

Urs, past midnight: "remove fallback and show me that you are using the shared
memory frame buffer as read surface." Both halves are done and the second is
printed by the body itself, not by me.

## What was removed

The flow lanes read a layered fallback — the owner's observation, then host or
process rows, then the glass's own Metal queue — and nothing on the glass said
which surface a number came from. `fgl-lane-number`, `fgl-lane-obs-number`,
`fgl-lane-evidence`, `fgl-lane-source` and `fgl-first-live-observation-id` are
gone. One id per lane; `D` `J` `I` sum their counter over every process that
gives a frame; the observation layer stays for the observation view and touches
no lane.

## What was added so the surface names itself

Every row set the glass takes from a frame now carries the frame's own witness
row, `frame.<sensor>`: the shared-memory name and the sequence the rows were
taken at. Each lane's source is that name and sequence. The glass gives its own
kernel, metal, framebuffer and heat rows into a `glass` frame every frame and
reads them back like any other frame, so a second glass — or a remote — would
read the same surface. The `machine` frame is its own fork-free process
(`observe/form-glass-machine-live.fk`, every 50 ms): the host GPU level and its
integral, host CPU busy over every core (`host_cpu_busy_us`, tag 173, from the
Mach load info), and its own counters. The slow sensors stay at their cadences
and never delay it. The one-shot doors read the same frames; nothing on any
glass path forks or scans. The `k` view lists the frames read.

`observe/form-glass-frame-budget-run.fk`, twentieth frame, sensors and machine
standing, printed by the body:

```
frames-read  frame.glass 126 shm:/fg-3db9b282 | frame.owner 7230 shm:/fg-ea12eba1
             frame.machine 4838 shm:/fg-5f223d19 | frame.host 13646 shm:/fg-d2f86e4f
             frame.queue 19862 shm:/fg-5a8f72be | frame.storage 1492 shm:/fg-1e5df9a9
lanes        G 0            shm:/fg-5f223d19#4838
             C 357843770000 shm:/fg-5f223d19#4838
             D 139163578    shm:/fg-3db9b282#126 + shm:/fg-5f223d19#4838 + shm:/fg-ea12eba1#7230
             Q 47           shm:/fg-5a8f72be#19862
             J 6859958      shm:/fg-3db9b282#126 + shm:/fg-5f223d19#4838 + shm:/fg-ea12eba1#7230
             c 457          shm:/fg-3db9b282#126
             R 53           shm:/fg-d2f86e4f#13646
             I 0            shm:/fg-3db9b282#126 + shm:/fg-5f223d19#4838 + shm:/fg-ea12eba1#7230
```

Twenty frames: 19 under 50 ms, warm maximum 34 ms, mean 29 ms; the give of the
glass's own rows and the take of six frames sit inside that. Bands: live and
live-ui 1073741823, kernel-view 511, sensor-rows 255, observer 8388607, gift
4095, node-gift 4095; mirror gates 1023/63/1023; quartet 42/31/1/2015.

Two facts about the sources, witnessed and named rather than smoothed: the Mach
CPU ticks move in bursts (24 reads at 50 ms: 3 moves, 21 zeros), so `C`'s
per-frame delta spikes; and the accelerator's `Device Utilization %` is
consumed on read — with Activity Monitor open on this Mac tonight (pid 688)
every read by anyone else answers 0, `ioreg` included, and `G` stands at 0
(R116).

Ledger: R115 released, R116 opened. Field 47000068. Corpus 671 rows.

## The most surprising teaching

A statistic the reading consumes. The accelerator produces its utilization
about once a second and hands it to whoever asks first; the next asker gets 0.
Yesterday's "window" was half the truth — it explained back-to-back reads, not
why the value vanished for an hour. `pgrep` explained that: Activity Monitor
was open, reading the same dictionary. There is no cumulative counter in it to
fall back on, and the body now says so instead of integrating a zero.
*consumeread*, row 1279.

## Where discomfort turned to gold

"Show me" is not a request for a sentence. The pull was to answer with the
architecture — gift frames, seqlock, node words — and it would all have been
true. What the frame budget lens prints instead is the surface itself: six
shm names, six sequences, and every lane pointing at the frame it read. Making
the glass give its own rows to a frame and read them back felt circular for a
moment; it is what makes the surface one surface — the same one a second
reader would hold — and the sequence numbers on the page are the proof no
sentence could be.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, live-ui 1073741823, kernel-view 511, sensor-rows 255, frames-read 6 with seqs, 19/20 frames under 50 ms
