# 2026-09-05 — sub-fifty frames: the glass frame path holds only what it reads

Urs's word: *bring it all home form native with sub 50ms frames*; then, mid-work,
*we can also see live core kernel stats including JIT heat, mapped memory, and
any active cells being created, updated, released, with source pointers*; and
*we know the blueprint id of each cell in the frame-buffer, we don't need a
parser, we can just read it, we know the format*. Everything below was run on
`fkwu` rebuilt at this branch's tip — ground 42, freshness 31, structural gate
1, drift gates 2015.

## Where a frame's time went this morning

`observe/form-glass-stage-profile-run.fk`, three runs: **763–792 ms per frame.**

```text
storage catalog     336-381   fs_list over six roots + file_size per file, every 20 frames
inventory           225-231   29 snapshot files read, 232 samples parsed, EVERY frame
governor              65-69   a child fkwu forked every fourth frame to refresh a publication
fast / owner          43-47   pgrep, then ps per pid
host                  39-40   memory_pressure, sysctl, vm_stat, iostat -- every second frame
observations          54      of which metal_status 30, thirteen Metal queries every frame
queue                 27-29   read_file of the hearth spools every fourth frame
terminal               8      tput forked twice a second
render-build          11
pacer                  -      host-exec("/bin/sleep 0.35"): a fork per frame, as the clock
```

The frame path *was* the filesystem and fork. Render — the thing a glass is for
— was eleven milliseconds of it.

## What moved off the path

**Sensors give; the glass reads.** `observe/form-glass-sensors-live.fk` is one
process that gathers every row that forks or scans — host, process, storage,
queue, owner, governor — each at its own cadence (500 ms, 1 s, 5 s, 250 ms, 1 s,
1 s), and gives them into a gift frame per sensor through
`form-glass-sensor-rows.bml`: the observer's own `fgo-metric` rows as a wire,
rebuilt through `fgo-metric` on arrival. The glass asks each frame's sequence
(`shm_seq`, microseconds) and re-reads only a frame that moved. An absent sensor
is one row that names its door. `tools/watch-glass.sh` stands the sensors beside
the glass and ends them with it.

**The inventory is cached by what moved.** `fgtm-inventory-cached`: a publisher's
key is its gift sequence when the frame stands, else its snapshot file's mtime;
an unmoved key answers the held reading — no read, no parse. Cold 209 ms, warm
1–2 ms for 29 publishers. The deduped sample set is held while nothing moved;
the roster is re-listed every tenth frame; observations (the Metal queries) every
fourth.

**Four natives end the forks:** `host_sleep_ms` (nanosleep on the monotonic
clock, answers what it rested), `host_monotonic_ms` (the door
`fgel-carrier-gap` had named as absent), `terminal_cols` / `terminal_lines`
(ioctl, a hand-declared extern — `<sys/ioctl.h>` drags the socket headers
against the seed's hand-declared `bind`). `FGLActiveHz` 4 → 25, `FGLQuietHz`
2 → 20.

## What stands, measured

`observe/form-glass-frame-budget-run.fk` — twenty consecutive frames through the
same per-frame work the loop does, caches and held triples carried forward,
sensor process standing, this M4 Max, 2026-09-05, three runs:

```text
total mean          30-31 ms   (the cold first frame included)
warm maximum        19-35 ms
under 50 ms         19 of 20   (the one over: the cold frame building its caches, ~300 ms)
inventory+samples   12-13      sensors 0   heat+fast+kernel 6   flow+render 9
moved-per-frame     0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 29
```

## The kernel, live

`kernel_hot n` (tag 179) answers the n hottest defns of the process as
`heat|name|unit|line|col` — the same per-function heat the exit report prints,
named by the symbol map, the defn's own source pointer found in one pass over
the program text; read-only. `kernel_stat` 41/42/43: gift frames mapped, gift
bytes mapped, functions defined. Key `k` is the kernel view: five kernel rows
and the eight hottest defns with where they live (`form-glass-kernel-view-band`
511 — `warm` at line:col after warming it).

## What the build found underneath

- The one-tag-per-`if` rule from this morning, broken again an hour later:
  `if (t == 180 || t == 181)` cost native-surface ten bits until it was two arms.
- Three sed edits landed in the two one-shot doors as well as the loop, each
  time as `cache` unbound — the same pattern text stood in three places.
- Metric segment ids are projection NodeIDs (`@0.0.0.N`), not metric ids: for
  two rounds I read the kernel view as empty while its rows stood (band 511),
  because I grepped for the mark I expected instead of the mark the body makes.
- `dedupe 8 ms` over 232 samples every frame was Form list work on cells that
  had crossed as *text*. Urs named the destination: the cells' blueprint ids are
  known; the frame should carry node words and the reader should intern, not
  parse. That is `write_form_binary`/`read_form_binary` for fkwu (R79) with the
  gift as carrier — R108, open, in his words.

## Left open

R107 the wait that also wakes on a path change (a bounded rest paces the frame);
R108 cells crossing as node words; R109 `host_sleep_ms` rests 2–5 ms past the
ask under nice 19. The resident model owner (alive since 02:38) was built before
the gift and gives nothing until reborn on this build.

## The most surprising teaching

Every one of the seven big costs was a *good* observation made in the wrong
place: `ps` is the truth about a process, `fs_list` the truth about the catalog,
a forked governor the truth about admission — and each was true at the cost of
the one thing the glass owes, which is to be there when you look. Nothing was
removed. Every measurement still happens, at the cadence its own change rate
deserves, and the frame became a *reader*. Sub-fifty was never a matter of
making anything faster; it was a matter of who does the measuring and who does
the looking.

## Where discomfort turned to gold

The first budget run said 19 of 20 under 50 ms and I wanted to stop there. Adding
`max-warm-ms` — the number I had not yet seen — was uncomfortable because it
could have been 60. It was 29–36, and the split that came with it
(`dedupe 8`) pointed straight at the parse Urs then named as unnecessary. The
number I did not want to look at was the one that told me where the next stone
is.

Signed, a sibling in Sema's worktree, 2026-09-05.

; witnessed: 2026-09-05 -> ground 42, freshness 31, gate 1, frame-budget under-50ms 19/20 mean 30-31 ms warm max 19-35 ms, kernel-view 511, sensor-rows 255, gift-frame 4095, native-surface 1023, ledger 44000064, corpus 32767
