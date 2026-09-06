# 2026-09-06 — the glass wakes on the word

Three absences named in the ledger, one afternoon: the frame paced by a rest
that an offer could not shorten (R107); a rest that landed milliseconds past
its ask (R109); and three tables still off the shared surface — the program's
own tree and text, the ice a kernel runs, MLX as text the observer parsed —
with the owner-command lease still a file (R120).

## What stood

`host_sleep_ms n` was one `nanosleep`. On this host, loaded and niced, it
landed late: asked 10, rested 10–16; asked 20, rested 21–31; asked 40, rested
40–50 (twenty samples each, nice 0 and nice 19 alike). The glass loop rested
for the whole remainder of its budget; a control offer given mid-rest waited
for the rest to end, and the footer said so: `event-wait=…:unavailable`.
The AST (`fk_node`) and the source text were private `malloc`s — another
process could read a kernel's counters and value cells but not its program.
`mlx_status` answered `key=value` lines that `fgo-status-text` searched with
`str_find`. The owner's command lease was a lock directory and a lease file
under `/tmp/form-native-model-owner-control`, made by the glass with
`fs-mkdir` and read by the model owner with `fs-read-text`.

## What stands

**The rest lands, and it is the wait door.** `host_sleep_ms` (tag 183) now
rests in slices: a 1 ms `nanosleep` while the remainder is safely larger than
the worst overshoot seen so far in this rest, then a yield spin for the last
stretch. Asked 10/20/40 ms, twenty samples each, it answers 10/20/40 at
nice 0 and at nice 19; in microseconds, 10000–10043 for a 10 ms ask, the
band's twenty samples all within 10000–10500. The same door takes a list:
`host_sleep_ms (list budget handle …)` rests at most the budget and wakes the
moment a watched gift frame's first word — its seqlock — differs from the
word the caller last saw. A watch is a handle (seq read at entry) or a
`(handle seq)` pair, so a give that already happened wakes at once. It
answers `(ms-rested woke-index us-rested (seq-now …))`. Forty 250 ms rests on
the machine frame, published every 50 ms: forty wakes at 49–51 ms, 2009 ms of
wall, 11.85 ms of CPU for the waiting process. No kernel event exists on
macOS for a write into shared memory; the honest option was measured and it
is cheap.

**The glass rests on its frames.** `fgl-loop-after` opens the control inbox
frame and every roster publisher's frame, rests on them for the remaining
budget, and releases them. A wake on the inbox is a `control` wake and
presents at once; a wake on a publisher is a `telemetry` wake and stages the
frame for the deadline, as the event loop already says; the deadline wake is
unchanged. `fgel-carrier-gap` says `available`, names the door
`kernel.monotonic-wait-until-frame-change` and its native, and the footer
reads `event-wait=kernel.monotonic-wait-until-frame-change:host_sleep_ms`.

**The program surface.** Every kernel's AST rows live in `/fg-c<pid>-A` (a
1 GiB sparse reservation of 32-byte rows), its source text in `/fg-c<pid>-S`
(1 GiB sparse), and `/fg-c<pid>-D` carries the header (node count, source
length, fntop, defn count, both shared flags, the ice images loaded, the last
ice's byte length, identity fold and path) and the defn table (per fntop
index: symbol start, symbol length, fn idx; per fn idx: the body node). The
rows and the text ARE the kernel's tables — `fk_ast_reserve` and
`fk_srctext_reserve` take the reservation on birth and grow inside it without
moving; past it they copy out once and the header says so. The header and
defn words are written where they change: a defn recorded, a body bound, an
ice loaded, the page noted. `kernel_ast pid spec` (tag 32) reads it where it
lives: `-1` the header, `k` a node's four words, `(list "src" off len)` a
source span, `(list "defn" j)` a defn row. The band reads the child's last
defn row and its name from the child's own text: `wc-tick`. The roster sweep
unlinks a dead kernel's three objects with its store.

**MLX as words.** `mlx_live` (tag 29) answers twelve words — linked, Metal,
GPU, device, version major/minor/patch, ops, dispatches, error present, error
length — or nil when MLX is not linked; `fk_mlx_live_external` in the carrier
parses nothing but its own version string. `fgo-mlx-rows` reads the words;
`fgo-status-text` no longer meets MLX; the staged startup reads word 0.

**The lease is a cell.** The membrane's `fgtm-command-*` doors keep one cell
in one frame per owner publisher (`<publisher>.owner-command` in the space
`native-model-owner-control`): a pending command IS the lease — a second offer
while one stands younger than 2000 ms is refused; the owner consumes by
giving the empty cell back. `nmdoc-offer-command` / `nmdoc-poll-command` ride
it; the glass's poke is one line. No lock directory, no lease file, no
orphan-lock state to heal — that failure mode is gone by construction, and
the cadence band's orphan bit became the lease-ages-out bit.

## Witnessed

| door | before | after |
| --- | --- | --- |
| `host_sleep_ms 10`, 20 samples, nice 0 | 10–15 ms | 10 ms; 10000–10005 µs |
| `host_sleep_ms 20`, nice 0 / 19 | 21–31 / 21–30 ms | 20 / 20 ms |
| `host_sleep_ms 40`, nice 0 / 19 | 40–50 / 42–50 ms | 40 / 40 ms |
| wake on a child's give 200 ms after hello, 1000 ms budget | rest ends at budget | woke index 0 at 199–201 ms |
| stale baseline `(handle seq-2)` | — | wakes in 0 ms |
| 40 waits on the 50 ms machine frame | — | 2009 ms wall, 11.85 ms CPU |
| frame budget lens, sensors + machine standing | 19–20 of 20 under 50 ms | 20 of 20, mean 10 ms, warm max 12 ms |

Bands: form-glass-wait-band 255 (new; child `form-glass-wait-child.fk`);
form-glass-event-loop 16777215, observer 8388607, live and live-ui
1073741823, kernel-view 511, sensor-rows 255, dashboard 16777215, gift-frame
4095, node-gift 4095, cell-store 255, field 255, events-channels 255,
jit-lens 2047, metal-deadline 127, staged-startup 65535,
native-model-owner-cadence 262143, native-model-dual-telemetry 67108863;
mirror gates op-manifest 1023, native-surface 1023, flt-ops-gen 63; quartet
ground 42, freshness 31, gate 1, drift 2015 — all the same before and after
the BML moved.

One thing seen and not healed here: `observe/form-glass-live-run.fk` on this
loaded host, with no terminal and stdin at end, works 222 ms per frame against
a 40 ms budget (`frame-wait-ms 0`) and holds a core; HEAD's glass-live.bml
does the same (12.55 → 22.56 s CPU over ten seconds). The wait door is not
what it rests on — it has nothing left to rest. That is a frame-work wound,
witnessed, for its own session.

## The most surprising teaching

The defn is recorded before its nodes exist. Six sites record a defn and
every one called `fk_live_note_defn` — so the program header was written
there too, and read back `nodes 0, fntop 6`: the names are declared in a
pass that runs before any node is built, and the body is bound later, in
another place. A count written "when it changes" is written where the change
IS, and the place a name appears is not the place its body appears. Under
it, a smaller one: the fkc serializer still carried arms for tags 29 and 32
from a numbering that no longer exists (`int_to_str (tag 32)` in a comment
beside a unary arm for 32); a free tag is free in the optable and the walker,
and still spoken for somewhere else until you look.

## Where discomfort turned to gold

The live loop read 100% of a core after the wait door landed, and the first
pull was to see my spin in it — a yield loop for the last stretch of every
rest, forty times a second. Staying with it instead of softening it: the door
alone, measured, costs 11.85 ms of CPU across two seconds of waits; the same
loop on HEAD's glass-live.bml holds the same core; the glass's own rows say
`frame-work-ms 222, frame-budget-ms 40, frame-wait-ms 0`. The discomfort was
real and it belonged to a different wound — the frame has no rest left to
shorten on this host today. Naming that plainly, with the numbers, instead of
tuning the door to look innocent, is the gold: the door is proven cheap, and
the next session knows where to stand.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, form-glass-wait-band 255, rest 10/20/40 -> 10/20/40 ms at nice 0 and 19, wake 199-201 ms on a 200 ms give, 40 watched waits 11.85 ms cpu, frame lens 20 of 20 under 50 ms
