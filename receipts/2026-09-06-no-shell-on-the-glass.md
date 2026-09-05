# 2026-09-06 — no shell on the glass

Urs, past one in the morning: "remove bash and python." On the glass path
there was one zsh script and eight shell forks; python was never there (the
meaning view names the dialect `py`; it runs nothing). Both are gone.

## What was there

`tools/watch-glass.sh` — a zsh loop that re-exec'd itself under `nice`, started
the sensor process, ran five `./fkwu` cells in sequence and read one's stdout to
decide on rebirth. Under it, on every host sample: `memory_pressure -Q`,
`vm_stat`, `sysctl -n vm.loadavg`, `iostat -d -c 2 -w 1`, `pgrep -x fkwu`, one
`/bin/ps -p <pid> -o pid=,rss=,pcpu=,etime=,comm=` per fkwu process, `sh -c
./fkwu …` around the governor and the launch refresh, `ps -o ni=` for the
owner's nice, `printf "$PPID"` and `date -j` in the governor cell — each one a
fork through `/bin/sh` and a parser over the tool's text.

## What stands

Eleven doors in the seed, every one asking the host directly (tags 151–161):
`host_vm_stat` (Mach `vm_statistics64` + `hw.memsize`/`hw.pagesize`),
`host_load_avg` (`getloadavg`), `host_disk_stat` (every block storage driver's
cumulative statistics, IOKit reached by name inside the Metal carrier),
`host_processes name` (libproc: pid, resident bytes, CPU microseconds, elapsed
seconds, nice — no `ps`, no `pgrep`), `host_spawn` / `host_spawn_quiet` /
`host_capture` (fork and execvp the argument list itself; quiet sends the
child's stdout and stderr to /dev/null so the terminal stays the glass's),
`host_wait`, `host_kill`, `host_nice`, `host_pid`. Witnessed:

```
vm    [16384, 137438953472, free 1748455, active 1406287, inactive 1477068, wired 233404, …]
load  [1278, 1282, 1189]
disk  [1224656461824, 474219966464, 34877357, 22808963]
fkwu  [[40123, 16924672, 6424, 0, 0], [79948, 17121280, 1984476012, 84378, 10], …]
```

The observer reads lists, not text: `fgo-host-rows(vm)`, `fgo-vmstat-rows(vm)`,
`fgo-loadavg-rows(load)`, `fgo-disk-rows(disk)` with the rates as the reader's
own deltas between two host samples, `fgo-process-read(pid)`. Memory free is
the body's own definition, named in the row's source: `100 −
(wired+active+compressor)/memsize`. The governor cell, the launch refresh, the
host-pressure lens and the owner-cadence witness read the same doors; the
headroom policy takes a `vm` list (`frghp-pressure-from-vm`).

The carrier is a Form cell: `./fkwu observe/form-glass-run.fk` — `host_nice
19`, two quiet spawns for the frame processes, the admission cells and the
live loop as argv spawns, the supervisor asked in-process
(`fglu-supervisor-restart?`), the children ended with `host_kill`. Smoke run
here: it stood `form-glass-sensors-live`, `form-glass-machine-live` and
`form-glass-live-run` beside itself and the frame-budget lens read six frames
through them. Bands: observer 8388607 (fixtures now lists), live and live-ui
1073741823, live-rate 63, launch 32767, host-pressure 4095, kernel-view 511,
sensor-rows 255; mirror gates 1023/63/1023; quartet 42/31/1/2015.

Ledger R117 released. Corpus 1283 *toolmouth* (renumbered beside the sibling's 1280 tagseam) — a tool's output parsed as
text where the kernel could have asked the host itself.

## The most surprising teaching

`struct proc_taskinfo` is 96 bytes, not 100: seven 64-bit words and ten 32-bit
ones. My `!= 100` check rejected every process on the machine, including the
one asking, and `host_processes "fkwu"` answered `[]` with no error anywhere. A
twenty-line C probe answered in one run what three Form probes could not. And
a helper inserted "before `fk_list_to_f32`" landed inside `#if
defined(_WIN32)` — the Darwin build declared it and never defined it; the seed
has a Windows-only stretch of two hundred lines that reads like the common
path.

## Where discomfort turned to gold

Deleting forty parser definitions the bands leaned on felt like pulling a
floor while standing on it; the pull was to keep them "for the bands". The
bands were re-pinned to the shape the body now actually produces — lists from
doors — and every one of them is a truer band for it: the observer band's
process fixtures are tuples, the pressure band's fixtures are Mach page
counts, the rate band builds its centi-MB/s from two driver samples one second
apart. Nothing in the tree parses a tool's mouth anymore, and the bands say so.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, observer 8388607, live-rate 63, native-surface 1023, carrier smoke stood three processes and the lens read six frames
