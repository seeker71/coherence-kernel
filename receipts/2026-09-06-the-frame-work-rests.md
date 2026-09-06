# The frame work rests

The live glass loop (`observe/form-glass-live-run.fk`) held a core at 100% and its TICK line read
`dt=69..88/40ms` five seconds in. I read `dt` as work, as the task did. It is not: `dt` is
`fglui-frame-delta(history)`, the epoch between the last two flow points — the frame *period*. While
the work overran the budget the wait was zero and the period equalled the work, so the number told the
truth by accident; once the loop rests, `dt` settles at the budget and says nothing about work. The
honest gauge is the loop's own cadence row, `frame-work-ms`, given into the "glass" gift frame every
tick beside `frame-wait-ms`.

## What stood

The work was not flat; it climbed. Same door, unedited tree, one run: `frame-work-ms` 69 at tick 47,
144 at tick 92, 207 at tick 119, `frame-wait-ms` 0 throughout, CPU 99%. Dispatches per tick did not
climb (5.2M in both halves of a 14 s run), so it was not more work — it was one native walking a
longer list each frame.

The list is the kernel's framebuffer root list. `intern_node_at` lowers to `fb_record`, and
`fb_record` appends a root on every call — in all four arms (`form-kernel-go/main.go`,
`form-kernel-ts/src/kernel.ts`, `form-kernel-rust/src/main.rs`, and tag 128 in
`runtime/fkwu-uni.c`); parity, not a divergence, and the kernel's own contract in
`fkc-table-serialize.fk` hands windowing to the reader. The glass minted its projection node for every
metric row every frame through `intern_node_at` (`fgo-metric`, `fgd-sample-projection-node`,
`fglat-observation-projection-node`). Witnessed in a probe: an identical re-intern leaves the node
arena untouched (258541 both times) and appends one more root each call. So the root list grew by
about 150 roots a tick, `framebuffer-events()` rebuilt it as a fresh cons list every tick, and
`fgo-count-attributed` walked it with `eq(len(events), 0)` at every step — `len` is a C cons walk, so
the count was quadratic in the roots. At tick 120 that is ~18K roots and ~160M C steps a tick. The
constructor's own comment says the NodeID names the field, never the reading, "so a changing counter
cannot grow the permanent NodeID arena" — the arena kept that promise; the root list did not.

`fgd-cat` at 7.8M calls was a second, smaller shape: `fglui-unique-metrics` deduped rows by building a
key string for every held row on every step (n²/2 keys, two callers a frame).

## What stands

- `fgo-field-node` in `form-glass-observer.bml`: intern, and record attribution only when the node
  carries no source yet. All three projection sites use it. The reading path `fgo-frame-row` (one
  root per written snapshot row, asserted by the observer band's bit 9) is untouched.
- `fglui-unique-metrics` builds each row's key once, holds keys beside rows, conses and reverses
  (first occurrence wins, order kept).
- Band `form/form-stdlib/tests/form-glass-frame-work-band.fk` = 255: spawns the loop quietly, waits
  on its own frame for twenty ticks, rests four seconds, reads `frame-work-ms` (< 30), `frame-wait-ms`
  (> 0), `framebuffer/events` (< 2000), the child's hot page (`fgd-cat` under a quarter of the
  hottest) and its dispatch word, and kills the child by pid.
- Drawn text unchanged except the `framebuffer` count rows, which now count distinct attributed
  fields (45 in the band's run) instead of re-registrations; their labels already said "bounded".

## Witnessed

| | before (tip d4a3f511) | after |
|---|---|---|
| frame-work-ms, ticks 47 / 92 / 119 | 69 / 144 / 207, climbing | 18–20, flat over 441 ticks |
| frame-wait-ms | 0 | 13–18 |
| period (`dt`) | 41 → 275 ms | 33–43 ms against a 40 ms budget |
| CPU over 6 s | 99–100% | 61–62% |
| framebuffer events at ~130 ticks | ~18K (150/tick) | 45 |
| hot rows per tick (top 8) | fgd-cat 117K, nil? 80K, append 80K, fgo-id 36K, fgo-domain 30K, fglui-metric-key(-held?) 29K, fgo-count-attributed 6.9K→17K climbing | nil? 43K, append 43K, fglui-key-held? 29K, fgdb-repeat-byte 10.7K, ftcb-text-go 9.8K, crc32-fold8 8.7K, fgo-id 7.4K, fgd-find-metric 6.5K; fgd-cat 2.2K |
| dispatches per tick, steady | 5.2M | 4.26M |
| frame lens (three organs live) | — | 20 of 20 frames under 50 ms, warm max 8 ms |

Quartet 42 / 31 / 1 / 2015. Keep-green: observer 8388607, live 1073741823, live-ui 1073741823,
kernel-view 511, sensor-rows 2047, dashboard 16777215, event-loop 16777215, events-channels 8191,
gift-frame 4095, meaning-ui 8191, staged-startup 65535, launch 32767, wait 255, machine 255,
hearth-glass 16777215, metal-deadline 127, host-pressure 4095, jit-lens 16383. No band named
`form-glass-layout-ui-band.fk` exists in `form/form-stdlib/tests/`; the layout shard is covered by
live-ui and launch.

## What does not stand yet

The work rests at 18–20 ms of a 40 ms budget — at the line, not under it with margin. A tick is
4.26M dispatches; the named shapes together are about a quarter of it: byte-list concatenation
(`append`/`nil?` 44K steps, every line's bytes copied through nested `fgdb-cat`/pad), the
key dedupe (29K `str_eq`, still n²/2), `crc32` in `ggf-name` for ~30 publisher names a tick, padding
one byte at a time, `fgd-find-metric` linear scans (~40 a tick). The rest is spread thin across row
and segment builders. Byte lists cannot become strings here: every `str_concat` is a permanent
pool string and the loop self-molts at 262144 of them (the pool grows 41 a tick as it stands, a
rebirth every ~4.5 min). A doubling recipe does not shorten a byte-list pad — one cons per byte is
its floor. A keyed structure for rows and a reversed-accumulator line builder are the next two
shapes; neither is in this commit.

The prelude image warning (`unit is not importable standalone ... falling back`) on
`form-glass-live.bml` and `form-glass-sensor-rows.bml` stands before and after this work.

## The most surprising teaching

The number I was sent to shrink was not measuring the thing I was sent to shrink. `dt` is a period.
Reading it as work fit the story exactly while the work overran, and would have read "40 — no
change" after the fix while the work had dropped tenfold. The body already had the honest gauge in a
row it gives every tick; the receipt of the fix is that row, not the line on the glass.

## Where discomfort turned to gold

I had a list of five render shapes to close and a timing probe that said the render was 3 ms. That
was uncomfortable: either the probe lied or the task did. I sat with the probe instead of the list,
put a clock between every let of the real loop body, and the 40 ms fell into the doors around the
render — then into one native `len` on a list nobody meant to keep. The dispatch count that refused
to climb while the work climbed was the discomfort that pointed there. The five shapes are still
real; they are a quarter of a tick, named above with their weights, and the task's own target for
them (`dt` under 20) is not a number that door can show.

— a sibling in Sema's worktree, 2026-09-06

; witnessed: form-glass-frame-work-band 255 (frame-work-ms 19, frame-wait-ms 13, ticks 134, events 45, hottest nil? 7.6M, fgd-cat 0); frame-work-ms before 69/144/207 wait 0 cpu 99%; after 18–20 wait 13–18 cpu 61.6%; quartet 42/31/1/2015; frame lens 20/20 under 50 ms
