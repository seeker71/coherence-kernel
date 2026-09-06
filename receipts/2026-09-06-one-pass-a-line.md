# 2026-09-06 — one pass a line

The frames-stay-open sibling left the live glass loop at 2.36M dispatches a tick and named what
remained: the line patch (every changed line's bytes copied through five nested `append`s), the
atlas render, the per-kernel rows, the ages, the flow point's three phase walks. Since then the
seed's `nil?`-against-`len` compare walks one cell and the string table melts. Re-measured first,
on this tree at e3fb33f0, the loop's own page read from another kernel: 2.58M dispatches a tick
(438M over 170 ticks), `frame-work-ms` 12–20, CPU 45–54% of a core, `append` 21K calls a tick.

## What stood

A probe with the loop's own rows (100x30, atlas view, 226 rows, a 128-point history), dispatches
counted between lets with `kernel_stat 0`: one frame 972K; the changed-line patch 271K for 1105
bytes; a whole frame 5.74M for 11119 bytes — 516 dispatches a byte, each byte consed and copied
through style, segment, line, position and frame; the structural line compare 4.8K a line, 145K
a frame before any changed line was built. Inside the frame: the atlas's metric tiles 323K for
ten rows (1.7K a cell: a byte-list token built through `fgdb-cat`, four state chains for one
background, two style constructions), the flow lanes 92K (every lane walking all 128 points
three times for a 36-wide spark), and 156K of map and typed-observation lines that
`fglui-left-budget` built eagerly for every view and the atlas never drew. Elsewhere in the tick:
the inventory decoded every tick because its cache test read `fgl-cache-empty()` instead of the
cache; the phase counts three full walks with three `fgl-phase?` chains each; every kernel's
eleven rows rendering the pid and the page source eleven times.

The string lane was not the cut. The string projection of one atlas line costs 15.5K — a style
renders through three `int_to_str` at 85 dispatches each (628 a style) — more than the compare it
would replace; and the write doors decide the lane before the numbers do: tag 61 walks a byte
list and appends, tag 104 opens `/dev/stdout` with `O_TRUNC`, `print_str` adds a newline. The
lane stays bytes.

## What stands

**One pass a line** (`form-terminal-canvas.bml`, `ftcb-onto-*`). Every byte is consed once onto
the bytes that follow it: `ftcb-onto-style` (weight, foreground, background, `m`),
`ftcb-onto-text` (a string walked from its last byte, tail-recursive), `ftcb-onto-list` (a byte
text prepended as it stands), `ftcb-onto-int` (digits from the least significant, so the result
reads most-significant first), `ftcb-onto-segment-line`, `ftcb-onto-position`,
`ftcb-onto-lines-diff` (the changed lines walked from the last index down, one accumulator),
`ftcb-full-frame`. No `append`, no `reverse`. Byte-equal to the chain on the diff patch and on
the whole frame before it replaced it. The compare reads each style field once, alpha and the
background first, and checks lengths once (`ftcb-style-same?`, `ftcb-bytes-same?`,
`ftcb-segment-list-equal?`). The chain builders (`ftcb-cat`, `ftcb-segment`, `ftcb-segments`,
`ftcb-segment-line(s)`, `ftcb-position`, `ftcb-style/fg/bg/weight`) had no caller left and are
gone.

**The atlas cell** (`form-glass-atlas-ui.bml`). Tokens are strings (`fglat-metric-token`,
`-sample-token`, `-observation-token` through `str_concat`; the byte twins are gone); a state is
classified once (`fglat-state-class` → background and weight off the class, `fglat-state-style-alpha`
takes the age alpha into the one construction); the kind chain reads the kinds tonight's rows
carry first; the legend's six evidence walks are one (`fglat-ev-triple`); the flow lane walks only
the points its spark shows (`fglat-flow-visible-values`, the standing value off the last point);
the phase line the same; the omitted count is arithmetic over what the budget already counted
(`fglat-omitted-of`); `fglat-moving-metric?` reads the zero-state chain only on a zero.

**Built only where drawn.** `fglui-left-budget` dispatches on the view first; the map and typed
lines live in `fglui-left-typed` for the views that draw them. The header, footer and TICK lines
are string segments (`fgd-segment-line`, `fglui-value-short`); their byte twins are gone.

**The loop** (`form-glass-live.bml`). The inventory is decoded only when the next let will read
it (the cache test reads the cache). The three phase counts are one walk (`fgl-phase-triple` over
`fgl-phase-index`, the one home of the phase sets; `fgl-phase?` reads off it). Each kernel's pid
text and page source render once for its eleven rows; the ids and labels join through
`str_concat`. The hot rows' id is the rank (`k.<pid>.hot.0..2`), the name in the label: an id
carrying the name gave every name a kernel ever had in its top three a permanent field node and
a framebuffer root (21, 9 and 3 in three rounds of a probe, 0 after).

**The band** `form-glass-frame-work-band.fk` = 32767 (8191 before): under 1,500,000 dispatches a
tick over the rest (8192); `fgd-cat` off the child's eight hottest defns (16384).
`form-glass-kernel-view-band.fk` reads a source pointer in `.fk` or `.bml` on the hottest defn
(511): once the append chains fell away a BML defn stood first in that process.

## Witnessed

Same host, the loop run with `</dev/null`, its page read from another kernel at 8 s and 28 s of
uptime, 13 live kernels, no sensor organs.

| | before (e3fb33f0) | after |
|---|---|---|
| dispatches per tick | 2.58M (438M / 170 ticks) | 1.263M (789.6M / 625 ticks); 1.198M over a 6 s window, 1.261M over another 20 s |
| frame-work-ms / frame-wait-ms | 12–20 / 13–26 | 6–8 / 21–33 |
| CPU | 45–54% | 30–33% (6.50 cpu-s in 20 s) |
| one frame (probe) | 972K | 545K |
| changed-line patch, 1105 bytes | 271K | 86K |
| whole frame, 11119 bytes | 5.74M | 203K |
| atlas metric tiles, 10 rows | 323K | 238K |
| flow lanes / phase line (60 points) | 92K / 23K | 63K / 18K |
| map + typed lines built for the atlas | 156K | 0 |
| flow point | 81K | 47K |
| top-12 hot rows (calls) | nil? 39.2M, append 39.2M, fstr-digit-char 9.2M, fstr-int-to-str-pos 9.2M, fgo-present? 9.2M, fgo-id 5.4M, fgo-count-attributed 5.3M, fgo-event-attributed? 5.3M, fgd-flow-value 5.2M, fgd-cat 4.5M, ftcb-text-go 4.4M, fgo-int-value 4.2M (1870 ticks) | fstr-digit-char 2.64M, fstr-int-to-str-pos 2.64M, fgo-present? 2.32M, fgo-id 2.16M, nil? 2.13M, append 2.09M, fgo-int-value 1.60M, int_to_str 1.58M, fgo-domain 1.46M, fgo-evidence 1.21M, fgd-drop 1.17M, fgo-count-attributed 1.16M (899 ticks) |
| `append` calls a tick | 21K | 2.3K |
| `fgd-cat` calls a tick | 2.4K | off the top 24 |
| frame lens (three organs standing) | 20 of 20 under 50 ms, warm max 10 | 20 of 20, first 45 ms, warm max 7 |

The drawn frame: before and after captured back to back from the same live rows (HEAD's files
restored, then mine), escapes stripped, digits masked — identical, 29 lines; the raw bytes differ
only at the TICK clock. Bands: observer 8388607, live 1073741823, live-ui 1073741823, kernel-view
511, sensor-rows 2047, dashboard 16777215, event-loop 16777215, events-channels 8191, gift-frame
4095, meaning-ui 8191, staged-startup 65535, launch 65535, wait 255, machine 255,
telemetry-membrane 2097151, hearth-glass 16777215, metal-deadline 127, host-pressure 4095,
jit-lens 16383, terminal-canvas 524287, frame-work 32767. Quartet 42 / 31 / 1 / 2047 (the
TypeScript witness built with `npm ci`).

## What does not stand yet

The target was 1.2M; a 6 s window met it and the 20 s windows read 1.26M. The spread is the
host: sibling publishers moving (the inventory lane) and a climb the receipts before this one
did not see because their runs were short — the loop's framebuffer roots grow 2.6–3.0 a tick
(824 at tick 274, 2500 at tick 899; 15164 at tick 5143 on the old tree), strings with them, and
`fgo-count-attributed` walks every root every tick (1.16M calls in 899 ticks, climbing). Every
door of the loop, called alone across rounds with the root count read between — field-node
repeats, `node_gift_write` of ints, strings, points and the glass rows, `record_new`, `bp`, the
fast, heat, kernel, growth, route and gpu rows, the sensor and glass takes, the inventory decode,
jit samples, dedupe, control poll, poke, the dashboard frame, schedule, handles, wait, wake, plan,
present, kept-for, manifest, dependency check, surface, hz, roster, the selfmolt check — mints
nothing past first sight. The hot-row names did (closed above); the live climb stands, and its
site is not one the doors show when called alone. It is the loop's selfmolt clock now.

`int_to_str` is 85 dispatches for "255" and 177 for seven digits; a style's three colours, a hot
row's three counters and every atlas value pay it. `append` is 2.3K calls a tick (the history
ring push, the row-list joins, the bound rows' 23rd field) and still fifth on the page; both are
the seed's tonight, not this lane's. The frame-level omitted count still recomputes its five
walks (35K); the ages still rebuild every row (76K); the roots climb.

## The most surprising teaching

The lane I reached for first cost more than the thing it would replace, and the doors had
already decided against it. Rendering a line to a string and comparing with one `str_eq` looked
like the cut; a style string costs 628 dispatches because `int_to_str` costs 85, so a line costs
15.5K against a 4.8K compare. And there is no door that writes a string to the terminal without
truncating or adding a newline. The cut was in the shape the body already had — bytes — consed
once from the end.

## Where discomfort turned to gold

Twice, and the second was the harder. The kernel-view band went red on a bit I had not touched;
the fix I wrote cut a path through `str_find` and `substring`, and the next measurement was
worse than the one before it — 1.334M — with `fstr-find-loop` at 2.3M calls on the page. The
memory said those are recipes that walk per byte. I had read it and reached anyway; the page
said it in one read, and the walk went back the way it was. Then the roots: ten probes, every
door of the loop called alone, and the climb unnamed. The pull was to stop measuring at eight
seconds where the receipts before had stopped, where the number looks like 1.2M. The number in
the table is the twenty-second one, with the climb in it and named.

— a sibling in Sema's worktree, 2026-09-06

; witnessed: live loop 8-28 s: dispatches/tick 2.58M -> 1.263M (789.6M/625 ticks), work 6-8 wait 21-33 cpu 32%; probe frame 972K -> 545K, patch 271K -> 86K, full 5.74M -> 203K; masked frame identical; frame-work band 32767 (perTick < 1.5M, fgd-cat off top 8); kernel-view 511; all glass bands green; quartet 42/31/1/2047; lens 20/20 warm max 7; roots +2.7/tick unclosed
