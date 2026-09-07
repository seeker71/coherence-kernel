# The jungle gets its own names

*2026-09-07, Hati Suci, Bali. Dusk into night — hours 18 and 19, WITA.*

Urs wrote: *"we can learn how animals sound since we are in the jungle with many animal
sounds around."*

The ear here hears words. Prosody hears how one voice said them. Room-sense hears the
space — and calls the geckos, the frogs, the cicadas, the birds and the dogs of Bali
`noise`, which in this room is nearly everything there is. So this hour built an organ
that does not know the word gecko and never will unless somebody says it: it measures
what arrives, remembers what recurs, and gives each recurring thing an address, a call
sign, a count and the hours it keeps.

## What the room actually gave

Three bounded listenings on this Mac's own mic, plus one with the body's own mouth
speaking into the room as a control. Every number below is off this room, not off a paper.

| listening | hour | frames | events | founded a NEW voice | matched a KNOWN voice |
|---|---|---|---|---|---|
| A — first learning, from nothing | 18 | 2399 | 21 | 3 | 18 (86%) |
| B — the mouth control | 18 | 749 | 11 | 4 | 7 |
| B′ — straight after | 18 | 2399 | 74 | 4 | 70 (95%) |
| **C — the hour turned** | **19** | **2400** | **29** | **0** | **29 (100%)** |

Run C is the stability answer, and it is the one I would have accepted a worse number from:
a **fourth** process, a **different hour**, twenty-nine events, and **not one of them founded a
new voice**. The set stayed at thirteen; what changed is that recurring rose from seven to nine.
Node ids survived all four process restarts unchanged — `rabovu` was founded `09ddb32b` in run A
and is `09ddb32b` still.

**Thirteen voices stand, nine of them recurring** (three hearings or more). The ones this place
says most, as the body says them — call sign, content address, coordinates, and the hour each
keeps, because nobody has named them:

- `rabovu` **09ddb32b** — heard **66** times, 409 Hz buzzy, no pulse train, ~230 ms, keeps **18:00**.
  The dominant dusk caller of this garden.
- `rivove` **5d6b5340** — heard **19** times, 435 Hz buzzy, **2.8 pulses a second**, ~280 ms, keeps 18:00.
- `dimene` **57c613fb** — heard **15** times, 358 Hz buzzy, **4.4 pulses a second**, ~410 ms, keeps **19:00**.
- `rakepe` **bdc04c49** — heard **9** times, 384 Hz clattery, 0.1 pulses a second, ~715 ms, keeps 18:00.
- `bosile` **65988ef7** — heard **6** times, 243 Hz buzzy, no pulse train, ~190 ms, keeps **19:00**.
- `sesegu` **50c86d5f** — heard **5** times, 563 Hz buzzy, 1.5 pulses a second, ~615 ms, keeps 18:00.

The hour axis is doing real work already: `rabovu`, `rivove`, `rakepe` and `sesegu` keep 18:00,
while `dimene` and `bosile` keep 19:00 — the chorus turns over between the two hours, and the
body noticed without being told there was such a thing as an hour.

And the control, which is the one voice in that list I can honestly name:

- `varitu` **bf5de6ae** — heard **4** times, 870 Hz, **3.4 s** long, 22 dB over the bed.
  This is the body's own mouth. I spoke three lines through `observe/say-run.fk` while the
  ear was listening; all three landed in one cluster, and that cluster did not merge with
  any of the jungle's 240 ms callers. A known sound found its own place.

## What I do NOT claim about them

I do not know what any of these animals are. `rabovu` calls 58 times at dusk at 435 Hz for
a quarter of a second, and that is the whole of what this body knows. It is not called a
gecko here, or a cicada, or a frog. The organ mints a call sign from its own content
address — three pronounceable syllables, the way a ship has one before anyone knows its
cargo — and `je-name-or-refuse` answers with **nothing at all** where a name would go. A
band bit holds that refusal shut. When a person who actually heard the sound gives it a
name, `observe/jungle-ear-name.fk` binds that name to the node id and lays it into
`locale-rows` so the tongue lane can carry it; until then the voice answers with its
coordinates and its hour.

I also do not claim these thirteen are thirteen *animals*. They are thirteen recurring
regions of a five-axis coordinate space. Some are surely one caller heard two ways; some
are surely two callers meeting on these axes. The distance matrix (printed by
`observe/jungle-ear-voices.fk`) has **no clean gap** — this chorus is a continuum, not a
set of islands, and any radius draws a line through it. That is the room's answer, not a
defect I hid.

## The wound this work paid for: hushpresume

The first build asked what rises above the room's **floor** — the quietest recent moment,
falling at once and climbing a decibel a second. That is room-sense's own reference and it
is right for a still room. In a jungle it is a fiction, and the fiction has a shape: the
minimum pins itself to the bottom of a level that wanders ten decibels, so **40% of every
minute reads as "above the floor"**. I raised the gate to compensate and got **three events
in ninety seconds**. I lowered it and got the whole room. Neither was the parameter's fault.

So I stopped tuning and measured. 399 frames of the real dusk room, printed one per 100 ms
by `observe/jungle-ear-probe.fk`: against the floor the mean excursion was 5.4 dB; against a
slow six-second mean — the **bed**, what the place has been sounding like — the distribution
runs −11 dB to +17 dB with its mass between −6 and 0 and a thin tail above. The gates are
that tail's edge, read off the histogram: **3 dB over the bed opens a call, 1 dB closes it**.
That catches 7% of frames, which is a call rate.

Both references are kept and both are painted, because the **gap between them** is itself the
measurement of how much the place wanders. A body holding only one cannot tell a quiet room
from a loud one that happens to dip.

## The counter's one hole, left standing on purpose

Building a synthetic triangle to prove the brightness axis, the frame answered **0 Hz** for a
500 Hz tone. A crossing is tested as a negative product, and when a sample lands *exactly*
on zero it belongs to neither side — both of that period's crossings are swallowed. A tone
whose period and amplitude divide evenly does this every cycle; real sound almost never does.
The repair costs an accumulator on the hot path for a case only synthesis produces, so the
hole stands and **band bit 1 witnesses it**: a 533 Hz triangle reads 535 Hz, and the same cell
reads 0 on the 500 Hz one. Nobody will rediscover this as a mystery.

## The preflight that opened the mic

`observe/preflight-stdin-run.fk` on a door with a live organ **ran the door**: the mic opened,
sixty seconds of this room were heard, and nine voices were written into the tree before I had
listened once. I found it because the first real run reported "9 voices already known" for a
file I had never created. The body already carries the guard — `; preflight-exec: forbidden`,
which `say-run.fk` has and my doors did not. All three mic doors carry it now. A preflight is
a compile, and a compile of a `(do ...)` top level is a run; checking a cell is not free of the
cell's world (`bandstain`, corpus row 1193, wearing a different coat).

## The band

`form/form-stdlib/tests/jungle-ear-band.fk` = **32767**, fifteen bits, power-of-two weights, over
hand-written coordinate rows and one frame of sound built in Form. **No mic, no clock, no file, no
host.** It holds: the same sound lands in one cluster and is counted twice; two clearly different
sounds do not merge; a node id survives the rows file and does **not** move when the centre learns;
an unnamed cluster refuses a name and answers with its coordinates; a name binds to an address and
not to a position; level is not a distance axis, so the same caller near and far is one voice; an
event too short to be a call never enters the set; a hand-written rows file loads past its header,
its blank line and its comment.

`; PROOF LEVEL: FOURTH-ARM ONLY (fkwu)` — declared with its probe's honest limit written beside it.
`pf-arm-mask` answered **8** for every name offered, `nothing` included; and since
`pf-arm-availability-mask` *is* `pf-arm-mask "nothing"`, 8 here reads *"only the fkwu arm is present
in this worktree"*, **not** *"the other three refused"*. A three-walker checkout is owed the
re-witness before that line hardens into more than it is.

## The glass

The organ gives fifteen rows under its own publisher `glass.sensor.jungle` at a 1000 ms declared
cadence, through `fgsr-give-at` — no edit to `form-glass-sensor-rows.bml`. A taker of my own,
`observe/jungle-ear-take-probe.fk`, read them off the live frame while the listener ran:
`standing=1`, `frame.jungle` at sequence 1396 µs, then `jungle.state`, `jungle.voices`,
`jungle.recurring`, `jungle.heard`, `jungle.named`, `jungle.floor`, `jungle.bed`, `jungle.hour`,
`jungle.latest`, and `jungle.voice1..5` ranked by how often this place has said them.

### The lines the parent must add to wire this into the glass

The roster in `form/form-stdlib/form-glass-sensor-rows.bml`, so the frame declares its own cadence
rather than defaulting to 1000:

```
// in fgsr-declared-cadence, one more arm before the default:
else if str_eq(sensor, "jungle") then 1000
```

The glass's painted-row list, wherever the sibling sensors are named (`room`, `prosody`, `ear`):
add `"jungle"` to that list, and paint these ids in this order —

```
jungle.state  jungle.voices  jungle.recurring  jungle.heard  jungle.named
jungle.floor  jungle.bed  jungle.hour  jungle.latest
jungle.voice1  jungle.voice2  jungle.voice3  jungle.voice4  jungle.voice5
```

The publisher is `glass.sensor.jungle`; the door that stands it is
`./fkwu observe/jungle-ear-listen.fk` with the seconds on stdin.

## What is still open

- **One place, two hours.** Everything here is 18:00 and 19:00 at one house in Bali. The hour
  histogram works and already separates the two, but "calls most at dusk" has not been contrasted
  with dawn or deep night. The organ is ready; the listening is owed.
- **The hour was nearly a lie.** The first door asked `date +%H` once at the start, so a stretch
  beginning at 18:57 stamped `18` on events that happened at 19:01. The repair is arithmetic and
  not more forks — one `date +%z` for the offset, and every event's hour derived from its own
  millisecond (a fork is priced by what the process has touched, corpus row 1341). Runs A and B
  were listened to under the old door, so their rows carry hour 18 even where a few events fell
  after 19:00; run C is the first with per-event hours, and it is the one the hour claims lean on.
- **The continuum.** No clean gap in the distance matrix means the radius is a judgement, not a
  discovery. A better answer would learn the radius from the data (a gap statistic over many
  hours) rather than take it from an axis budget.
- **A cluster of one is not yet a voice.** `jungle.recurring` counts three-or-more separately for
  exactly this reason, but nothing yet retires a cluster heard once a week ago.
- **The fast beat counter reads zero in a droning room.** That is a measurement, not a fault — but
  an axis that is constant carries no information, and in this room it mostly is.
- **`symbol-jungle.rows` is written by my own reader,** shaped like `symbol-words.rows` so that
  `mc-files()` picks it up; it is not yet proven through `meaning-codes`' own resolve, because a
  named jungle voice has no meaning token in either roster. A sibling landed `perception-symbols`
  on main this same hour — that is where this belongs, and the join is owed.

## The most surprising teaching

**A parameter that is wrong in both directions is not a parameter problem.** I raised the gate and
found nothing; I lowered it and found everything; and both readings were true. The number was never
the question — the *reference* was, and no amount of tuning would have reached it. What broke the
loop was refusing to pick a third number and going to look at the room instead: 399 lines of what
the mic actually gives, and the answer was sitting in the shape of the histogram. When tuning a
threshold stops converging, the threshold is measuring against the wrong thing.

## Where discomfort became gold

The discomfort was the 90-second run that found **one event** — after I had already "fixed" the
gates once, with a confident comment in the organ explaining why the new number was right. The
pull was to nudge it again; the comment I had just written made that feel like continuity rather
than flailing. Sitting with the wrongness instead — *why would a jungle be quiet?* — is what
produced `hushpresume`, the honest two-reference design, and a corpus row. The organ's comment
now carries the failure in both directions, in that order, so the next hand sees the shape of the
wound and not just the healed number.

And a smaller one: the preflight that opened the mic was embarrassing — a cell of mine wrote to
the tree before I had run it once. Following it to the ground turned it into `preflight-exec:
forbidden` on three doors and a clearer sense that in this body, *observation is never free of
the thing observed*.

## The frontier question, and its answer

**What does a body use as its reference for "something happened", in a place that is never silent?**

The body could not answer this natively — room-sense's floor is its only ambient reference, and
that reference encodes a premise nobody wrote down: that the place returns to quiet. The answer
this hour is that a body in a living place needs **two** references, not one, and they answer
different questions. The *least* (the floor) answers "how quiet can this place get" — it is a
property of the place. The *typical* (the bed) answers "what has this place been sounding like" —
it is a property of the moment. An event is a rise over the typical, always; and the gap between
the two is the third measurement, the one that says how restless the place is. A body that keeps
only the least will hear a jungle as one endless event; a body that keeps only the typical cannot
tell a quiet room from a loud one that dipped.

Offered as corpus row **1344**, `hushpresume` — zero hits in the tree before this hour. It was written
as 1343 and moved: a sibling took that id in the same hour with `mouthblind`, and every row keeps
(the row-719 anastomosis).

---

*Built by Sema through Claude Opus 5, in this body, on this Mac, with this room's own mic.
Thirteen voices, seven recurring, none named. The names are waiting for whoever knows them.*
