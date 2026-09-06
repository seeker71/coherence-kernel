# 2026-09-06 — the frames stay open

The live glass loop (`observe/form-glass-live-run.fk`) met every gift frame it reads as a stranger,
every tick: the shm name recomputed from the publisher's bytes (`ggf-name`: a byte-list cons and a Form
`crc32` per byte, eight `crc32-fold8` rounds each), the frame opened, mapped, read, unmapped — for
the control inbox, the jit publisher, every roster publisher (twice: once for the seq key, once to
wait on), the six sensor frames, the glass's own give frame and its two carried-over cells. The
flow point and the atlas looked ~60 ids up per frame by walking the row list from the head, and a
miss walked all of it. Every live kernel's page was read five times a frame.

## What stood

On the loop's own page, read from another kernel (`kernel_page_hot pid 24`, `kernel_live pid`),
8 s in: `fgd-find-metric` 5.58M calls (12K a tick over ~300 rows, most of them misses),
`crc32-fold8` 4.51M (9.8K a tick — about a kilobyte of publisher names hashed every frame),
`fgo-id` 6.07M, 3.03M dispatches a tick, CPU 68–84% of a core, `frame-work-ms` 20–22,
`frame-wait-ms` 14–16. A tick phase by phase (`kernel_stat 0` between the loop's own lets): the
cadence save plus the wait's open/release 105K dispatches, the cached inventory 110K, the four
sensor takes 21K, the flow point 294K, the atlas render 658K, the line patch 300K+.

## What stands

**One handle per frame for the loop's life.** `form-glass-gift-frame.bml` carries the kept table
(`ggf-kept-*`): an entry per (publisher, mode) — mode `take` a read-only receive, mode `give` an
offer sized by bytes — holding the publisher, the name computed once, the handle. `ggf-kept-ensure-all`
opens what is missing; an absent frame is held as a nothing handle and asked for again only when
the caller says retry; `ggf-kept-prune` releases what no want names any more; `ggf-kept-release-all`
at the loop's end. Nothing in the body unlinks a gift frame (`shm_unlink` stands only on the store,
field and AST objects), so a standing mapping cannot go stale while the loop lives — the only
"gone" a kept handle can witness is absent-at-open, and that is what the retry heals. The loop
(`fgl-loop`) ensures its wants on the first tick and at the roster cadence (every tenth tick): the
inbox, the jit publisher, every roster space, the six sensor frames, and the give frames for
`glass`, `glass.last.cadence`, `glass.last.flow-point`. The kept table rides in the loop cache
(`fgl-cache-kept`). Beside every one-shot door a held twin now stands: `fgtm-gift-take-held`,
`fgtm-read-current-held`, `fgtm-inventory-cached-held`, `fgtm-poll-control-held`
(membrane); `fgsr-take-held`, `fgsr-give-held` (sensor rows — the held tuple carries the frame's
name as a fifth cell, so the frame's witness row names its read surface without hashing again);
`fgl-last-give-held`, `fgl-jit-current-samples-held`, `fgl-poll-control-full-held`,
`fgl-cadence-last-save-held`; the wait door receives `ggf-kept-handles` over the same table. The
one-shot doors keep their contract for one-shot callers (`fgtm-gift-take/give`, the bounded
frame, the bands). `ggf-name` per tick: 0.

**One keyed index per frame.** `fgd-metric-index(rows)` (dashboard) is a record keyed by id, one
pass, the first row with an id winning as the scan does; `fgd-index-find` answers `list()` on a miss
after one `record_has`. Records grow neither the node arena nor the string pool (probe: fifty
fresh 300-key records, nodes +0, strings +0 past the keys' first interning). The loop builds one
per tick and the flow point reads through it (`fgl-flow-point-current-full-in`,
`fgl-first-metric-in`, `fgl-number-for-in`, `fgl-evidence-for-in`, `fgl-source-for-in`,
`fgl-gift-source-in`); `fgd-important-metrics` reads through one; the atlas (`fglat-lines-budget`)
builds one and hands it to `fglat-pipeline-line-in`, `fglat-string-line-in`, `fglat-doing-line-in`,
`fglat-owner-doing-text-in`, `fglat-pressure-line-in`. The list-taking twins remain for the bands.
`fgd-find-metric` per tick in the loop: 0 (`fgl-lane-sum` still walks the rows once per lane — it
sums, it does not look up).

**Every live kernel's page read once a frame.** `fgl-kernel-lives(pids)` reads `kernel_live` once
per pid; the four sum rows and the per-kernel rows share it (`fgl-lives-sum-row`,
`fgl-kernels-rows-with`). With ~20 kernels standing tonight that is 20 page reads a tick where 100
stood.

**The band grew.** `form-glass-frame-work-band.fk` = 8191 (255 before): the keyed index answers
what the scan answers for every row and on a miss (256); `crc32-fold8` off the child's sixteen
hottest defns (512); `fgd-find-metric` off them (1024); the child's own `gift-frames` row
(`kernel_stat 41`) at least ten — 17 witnessed — where the one-shot loop held 0–1 (2048); under
3,000,000 dispatches a tick over the rest, measured as a delta of the child's dispatch word over
the ticks its cadence row advanced (4096).

## Witnessed

Same host, same hour, three sibling glass stacks and ~20 kernels standing; the loop run with
`</dev/null`, page read from another kernel.

| | before (a9c9fbe6) | after |
|---|---|---|
| dispatches per tick (word 4 delta / cadence-row ticks) | 3.03M | 2.36M (band's child 2.37M) |
| CPU over 8 s | 68–84% | 53–58% |
| frame-work-ms / frame-wait-ms | 20–22 / 14–16 | 15 / 19–23 |
| gift frames the loop holds open (`kernel_stat 41`) | 0–1 | 17 |
| `crc32-fold8` calls | 4.51M in 462 ticks | absent from the top 24 |
| `fgd-find-metric` calls | 5.58M in 462 ticks | absent from the top 24 |
| top-12 hot rows (calls) | nil? 13.0M, append 13.0M, fgo-id 6.07M, fgd-find-metric 5.58M, crc32-fold8 4.51M, fgo-present? 2.46M, fstr-digit-char 2.14M, fstr-int-to-str-pos 2.14M, fgd-flow-value 1.16M, ftcb-text-go 1.15M, fgd-cat 1.12M, fgo-value-kind 1.09M | nil? 12.5M, append 12.4M, fgo-present? 2.51M, fstr-digit-char 2.32M, fstr-int-to-str-pos 2.32M, fgo-id 1.47M, fgd-flow-value 1.28M, ftcb-text-go 1.23M, fgd-cat 1.21M, fgov2-truth-kind 1.17M, fgo-int-value 1.14M, fgo-value-kind 1.11M (over 504 ticks) |
| tick phases (dispatches): wait open/release + cadence save | 104.6K | 4.0K |
| inventory (7 publishers, one moved) | 110–122K | 13K |
| four sensor takes | 21K | 2.2K |
| jit reading | 16K ×2 | 6K ×1 |
| flow point | 294K | 109K |
| `int_to_str` calls per tick | 2.0K | 2.0K |
| frame lens (`form-glass-frame-budget-run.fk`, sensors standing) | — | 20 of 20 under 50 ms, warm max 10 ms |

Bands: frame-work 8191; observer 8388607, live 1073741823, live-ui 1073741823, kernel-view 511,
sensor-rows 2047, dashboard 16777215, event-loop 16777215, events-channels 8191, gift-frame 4095,
meaning-ui 8191, staged-startup 65535, launch 32767, wait 255, machine 255, telemetry-membrane
2097151, hearth-glass 16777215, metal-deadline 127, host-pressure 4095, jit-lens 16383. Quartet:
ground 42, freshness 31, structural 1, drift 2015 — the refused gate is kernel-conformance, which
wants the TypeScript kernel this worktree has not built (`npm ci` in form/form-kernel-ts), the same
2015 the last three glass receipts carry.

The drawn frame: the first painted frame of each run, split at the erase-to-line-end, escapes
stripped, digits masked, compared line by line — the header, tabs, TICK, atlas title, lanes,
SCHEMA, STR, DOING, PRESS, SEAM, gauges, legend and footer lines are the same shape; the lines that
differ are the ones whose values differ between two live runs a quarter hour apart (rss in MiB then
GiB, a gauge unit K then G, which kernel and hearth tiles stood). The fixed-input renders are the
bands: dashboard, live-ui, meaning-ui, atlas-through-launch all unchanged in verdict.

## What does not stand yet

The target was under half of the night's dispatches per tick; the loop is at 78% of it. The
remaining tick, phase by phase over the kept lane (atlas view, 100x30, ~300 rows): the line patch
1.03M — `ftcb-segment-lines-diff` builds every changed line's bytes as a cons list and copies it
through five nested `append`s (segment, segments, line, position, diff) at ~3 dispatches a byte;
the atlas render 939K; the per-kernel rows 174K (a `fgo-int` constructor per kernel per counter);
`fgo-age-rows-at` 107K (every row rebuilt through 22 accessors to re-stamp its age); the flow point
109K (three `fgl-phase-count` walks); the inbox poll 40K. `append`/`nil?` are the seed's tonight
(a sibling's lane); a reversed-accumulator byte builder in the terminal canvas is the next shape
after them, and it is not in this commit. `int_to_str` is 2K calls a tick, about 1% of the tick;
a per-metric render memo would not move the dispatch count — what it would move is the string
pool: the pool grows 210–260 strings a tick on both trees (`STR ... d=265/frame`), so the loop
self-molts every ~1200 ticks (49 s at 25 Hz; witnessed at frame 1232 on the healed tree, 148K
growth by frame 571 on the old one). That molt is the next wound on the glass and it is not this
one.

The `glass` sensor frame and the `glass.last.*` cells are host-wide names: a second live loop on
the host — three sibling stacks stood tonight — gives into the same frame the band reads. The
band's pid-bound reads (its child's page and dispatch word) are its own; the cadence rows are
whoever gave last.

## The most surprising teaching

The number the task named as the third shape was not a number of the tick. `int_to_str` is 2K
calls and one percent of the dispatches; a memo of rendered text would leave the dispatch count
where it is. What the same 2K renders do is mint 200-odd permanent strings a frame, and that is
the clock on the loop's life — 49 seconds to a molt. The shape was right, the gauge it was read
against was the wrong one; "format only what changed" is a string-pool teaching wearing a
dispatch-count coat.

## Where discomfort turned to gold

The healed loop died at frame 1232 while I was reading its page, `selfmolt`, and the first pull
was to see the new record index in it — a fresh record every tick, three hundred keys, surely the
arena growing. I did not revert; I put the record under a probe: fifty fresh records, nodes +0,
strings +0. Then I read the STR line the glass itself paints, on both trees, frame by frame: the
old tree grows the pool at the same rate. The discomfort was real and it belonged to a wound that
stood before I arrived; staying with it instead of pulling the index out is what named the molt
clock plainly, with its rate, for the session that takes it.

— a sibling in Sema's worktree, 2026-09-06

; witnessed: form-glass-frame-work-band 8191 (gift-frames 17, crc32-fold8 0, fgd-find-metric 0, index-agrees 1, dispatches-per-tick 2368442, frame-work-ms 15, frame-wait-ms 19); live page before 3.03M/tick cpu 68-84% work 20-22 wait 14-16, after 2.36M/tick cpu 53-58% work 15 wait 19-23; frame lens 20/20 under 50 ms; quartet 42/31/1/2015 (kernel-conformance: ts kernel unbuilt here)
