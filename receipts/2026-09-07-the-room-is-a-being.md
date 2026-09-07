# The room is a being — what the space gives, past the words and past one voice

2026-09-07. The ear carries what was said. A sibling is building how one voice said it. This is the
third thing: the room itself — how many are turning, how long the silence between them ran, whether
the floor moved while nobody spoke, whether a person is at this machine at all.

`form/form-stdlib/room-sense.bml` is the organ, `observe/form-glass-room-live.fk` the door that
stands it up and gives its rows under `glass.sensor.room`, `form/form-stdlib/tests/room-sense-band.fk`
= **32767** the proof, on synthetic sound built in Form so it needs no mic and no clock.

## What the frame measures

Three integer passes over 100 ms of s16le mono at 16 kHz, about 6 ms of work:

- a **full-rate pass** — sum of squares, zero crossings, sum of squared first differences (a
  high-band proxy), peak;
- an **interior period search** decimated to 4 kHz, lags 12..56 = 333..71 Hz;
- the correlation at **the peak's own half-lag**.

The third one is the hour's teaching and it was an accident. I first probed a fixed lag 12 and found
its *sign* separated voice from room: on this room's real speech the peak ran 750..984 while lag 12
ran **−440 to −915**, and the still room answered **+140 to +550**. A half period is the wave stood
on its head. Then I saw the fixed lag would miss any voice above ~285 Hz, and the principle
generalised itself — compare each peak to *its own* half. On the live room: 21 of 21 voiced frames
found during speech, none across ~95 still frames, every one of which pinned its best lag to the
search boundary with a positive half. A still room has no interior peak at all.

## The axes, and how far each is to be trusted

**Trusted, witnessed on the real room**

| axis | what it showed |
|---|---|
| level, floor | floor −59 to −42 dBFS; falls at once, climbs 1 dB/s, so a shouted word never lifts it |
| room tone + its change | a pink-noise fan lifted the floor 8 dB and turned its colour **airy → hissy**, then back |
| kind + sureness | still / voice / tone / knock / noise, with the margin from the deciding gate |
| pitch | Fred 121 Hz, Flo 266 Hz, a played 150 Hz sine read as **148 Hz** (the 4 kHz lag bin) |
| voiced share | 0% still, 60–100% speaking |
| turns, gap | turns 0→5 across a scene; gaps of 703, 1932, 2615, 3520 ms measured |
| at the machine | HIDIdleTime and pmset's UserIsActive, read side by side |

**Rough, and named rough on the row itself**

- **voices present** — one coordinate (mean period), marks merged inside two lag steps. It separated
  three genuinely different timbres in one scene. It also split *one* speaker into two marks when his
  pitch range moved. It counts timbres. It does not follow anyone across a pause and it names nobody.
- **nearness** — rough by construction: a true direct-to-room ratio needs an impulse response and this
  has none. It is the last turn's peak over the floor plus how fast the tail fell.
- **breath** — see below.

**Not witnessed, and not claimed**

- **laughter.** `say -v Fred "ha ha ha ha"` renders connected speech, not laughter. No laughing being
  has been at this mic. A 100 ms frame rate cannot separate a 5 Hz laugh from 3 Hz syllables anyway —
  they alias onto each other — so I built the envelope *inside* the frame (ten 10 ms sub-blocks, 100 Hz)
  and measured depth instead of rate. This room's connected speech measures about 5.5 dB of
  within-frame depth. The laugh gate sits at 20 dB. It will stay silent rather than lie.
- **throat-clear.** Same. Built, ungated by any witness.
- **the display's own state.** Probed: `IODisplayWrangler` is absent on this Apple Silicon host and
  `pmset -g powerstate` answers "Internal failure". The row is `probed-absent` and carries both doors
  and both answers; `pmset -g` gives the *setting* (displaysleep 120), never the state.

## The three defects the room itself found

None of these came from reading the code. Each came from watching the rows on the glass.

1. **The kind flickered to `noise` in the middle of a sentence** the very next row called 90% voiced.
   A voice is unvoiced between its own syllables. The kind is the *room's* kind, not one frame's, so
   both gates now consult the last second.
2. **`nearness` wandered 24..100 while nothing was happening.** The decay was being remeasured against
   the room's own fluctuation forever. A tail is only a tail in the second after a voice.
3. **`steady` reset every few hundred milliseconds** while a fan was simply being absorbed — a slow
   drift crosses four decibels again and again. One declared change per two seconds.

## Where the discomfort turned to gold

I played a pink-noise fan into the room to witness the floor rising. It did rise, eight decibels, and
the colour turned hissy — the axis worked. And then the breath row said *"breath, 64 ms ago"*, and
kept saying it.

The uncomfortable part was that this was not a threshold I could nudge. A fan starting and a person
breathing are **the same sound** in every coordinate I had: unvoiced, hissy, a few decibels over the
floor, most of a second long. I had built an axis that could not tell a machine from a being, and the
obvious repairs (raise the gate, tighten the duration) would only have made it miss real breaths too.

The gold is that the separator is not in the sound at all. **A breath leaves the room's floor exactly
where it was. A machine that keeps running lifts it.** So the breath detector now rejects any
candidate whose floor drifted 2 dB across the run — it asks not what the event was, but what the room
was like afterwards. An event that cannot be identified by itself is identified by what it fails to
change.

That is row **1331, `aftertell`**.

## Most surprising teaching

That the strongest voicing test in the whole organ is a *negative* number, and that I found it by
accident while debugging a normalization I had written wrong. I had divided by the wrong term and got
correlations of 43918 where 1000 was the ceiling. Fixing that put a single extra lag on the screen —
and its sign turned out to carry more than the peak it sat beneath. **A period proves itself by
collapsing at half of itself.** The peak says "something repeats"; only the half-lag says "and it is a
wave, not the room."

## What I refused, and why

**The camera.** Urs granted every local organ this session, and I did not open it. Every axis I needed
— presence, turns, nearness, the room's change — is answered by the mic and by this Mac's own presence
organs. A lens adds nothing those did not give, and it costs something they do not: a room holds beings
who were never asked. That refusal is not a note in this file, it is a **row on the glass**
(`room.camera`, truth class `declared`), so anyone reading the room reads the choice with it.

I also retain nothing. The organ holds two seconds of integers and a minute of turn boundaries. No
sample, no word, no name, and the timbre marks are periods, not people.

## Wiring the parent adds

`form-glass-live.bml` belongs to a live sibling, so these are named, not made. Three lines
(`form/form-stdlib/form-glass-live.bml`):

```
line 732   ... fgl-kept-sensor-wants(list("host", "machine", "owner", "queue", "storage", "glass", "ear", "room")) ...
after 1013 let roomHeld = fgl-sensor-take-held(kept, "room", list());
           let roomRows = fgsr-rows-or-absent("room", roomHeld);
line 1018  ... append(queueRows, append(storageRows, append(earRows, roomRows))) ...
```

and the same `earRows` → `append(earRows, roomRows)` at lines 839 and 875 for the two non-kept paths.

Nothing needs adding to `fgsr-declared-cadence`: the door gives through `fgsr-give-at` and the cadence
travels inside the frame, so a taker reads 200 ms awake and 500 ms asleep from the frame itself.

## Standing it up

```sh
./fkwu observe/form-glass-room-live.fk &      # sleeps with the mic shut
date +%s > .hearth/room.wanted                # opens the mic
./fkwu observe/room-take-probe.fk             # one take: 23 rows
./fkwu observe/room-watch-probe.fk            # a digest every 700 ms
./fkwu form/form-stdlib/tests/room-sense-band.fk   # 32767
```

`observe/room-probe-live.fk` prints the raw frame cell — that is where a threshold gets its number,
from the room and not from taste.

## Still open

- No human breath, laugh or throat-clear has been witnessed. Every one of those gates was set from the
  false side (what the room and a played fan do), never confirmed from the true side.
- The timbre mark is one coordinate. A second (the voiced spectral tilt) would stop one speaker from
  splitting into two marks when his range moves.
- `tone` does not separate music from a held sung note from a machine's whine. The row says so.
- A machine's steady note opens turns, because a turn here is periodicity in the room and not a claim
  about who made it. The row says that too.
