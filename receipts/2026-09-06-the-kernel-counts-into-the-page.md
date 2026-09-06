# 2026-09-06 — the kernel counts into the page

Urs, afternoon: "kernel stats on glass without any performance impact."

## What the seed had

Every kernel already mapped a live page, `/fg-k<pid>`, and the glass already
read it where it lay. But the words got there by a tick: every 1024 dispatches
the dispatch loop checked a counter, summed the opcode arms, copied heat and
box totals into the page and bumped the seqlock. A copy on the hot path, paid
by every kernel whether or not a glass was watching. And a defn's name reached
the page only through the compile path, so a kernel that loaded its program
from `.fkb` ice showed hot defns with no names at all.

## What stands

**The counters are the page words.** `fk_arms`, the heat, box, unbox and
native-call totals, and the three per-defn ledgers are pointers. When the page
opens (`fk_live_open`, at `fk_nodes_init`) they are repointed into the page; the
increment the dispatch loop already does is the write the glass reads. The tick
is gone. No publish cadence, no serialization, no second copy.

**Meta at define time.** `fk_live_note_defn(j)` runs at all six sites that
record a defn — compile path and `.fkb` ice alike — and writes name, unit, line
and column into the page meta, with an incremental newline index so the line is
found once. When `fk_fntop` falls to 0, the meta count falls with it.

**Any kernel's hot defns from its page alone.** `kernel_page_hot pid n` (192)
and `kernel_page_box pid n` (193) read heat, boxes, unboxes and source for the
n hottest defns of any live kernel; `kernel_hot_rows` and `kernel_box_rows` are
the same readers over the own pid. The `k` view lists every live kernel's three
hottest defns: `k46821 fstr-find-loop core.fk:220 box 0 unbox 0  1M calls`.

**Words that change at moments are written at the moment.** nodes, strings,
cpu, alive, store kind, melt generation: at open, at field open, at melt, at
exit, at self-read. The field-open note is new — see below.

**Measured.** 20 million iterations, three runs each, same numbers every run:

| loop  | tick + publish | page words |
| ----- | -------------- | ---------- |
| int   | 0.89 s         | 0.67 s     |
| float | 0.98 s         | 0.78 s     |

Bands on the new seed: jit-lens 255, cell-store 255, field 255, kernel-view
511, live / live-ui 1073741823, observer 8388607, dashboard 16777215,
sensor-rows 255, gift-frame 4095, node-gift 4095, events-channels 255,
meaning-ui 8191, staged-startup 65535, launch 32767, event-loop 16777215,
live-rate 63, hearth-glass 16777215, metal-deadline 127, host-pressure 4095,
corpus 32767, python-bmf-grammar 219; op-manifest 1023, flt-ops-gen 63,
native-surface 1023; ground 42, freshness 31, gate 1, drift 2015. Frame budget
lens with sensors and machine standing: 20 frames, 19 under 50 ms, mean 9 ms.

## The most surprising teaching

Removing the sampler did not merely cost nothing; the kernel got faster by a
quarter. The tick was one compare and one branch per dispatch, and it was the
compare that hurt: it sat in the loop every other counter lives in. The
stats the glass wanted were already being computed — the loop was paying twice,
once to count and once to notice it had counted.

## Where discomfort turned to gold

The first run of the new seed answered `meta count 0` for a kernel that had
two thousand defns. The reflex was to suspect the page; the fact was that
the six places a defn is recorded are not one place, and the compile path is
only one of them. Ice-loaded programs never touched it. Walking the six sites
one by one, rather than adding a "replay" after the fact, is why any kernel's
hot defns now carry names from the first dispatch. And `cell-store-band` fell
255 -> 223 for one bit: the child's store word said "private" because the
field opened after the page did, and the tick that used to refresh the word
was the thing I had just removed. The word is written where it changes now;
the tick had been hiding a missing note for a day.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, jit-lens-band 255, cell-store-band 255, form-glass-kernel-view-band 511, int20 0.89->0.67 s
