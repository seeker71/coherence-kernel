# A heard line is a point

Urs asked for the most higher-dimensional multi-axis stream of awareness. Yesterday the ear
stood in the glass and gave nine rows; a heard line arrived as one row of text. That is the
thinnest thing a body can know about a moment in a room. Today the moment is a point.

## The axes, and what each one showed on a real room

`form/form-stdlib/ear-axes.bml` folds the ear organ's frames into twenty-four axes given at once.
Every reading below is from this Mac, mic granted, with a voice speaking into the room.

**The room.** `ear.state` names the ear in its own word beside the Persian the verse is found
by — `speaking — سخن`, and `silent — خاموش`, `asleep`, `deaf`, `heard`. `ear.level` read −54 dBFS
on a quiet room and −36 with a voice in it; `ear.level.trend` says `first reading` when it has no
previous and reports movement only past three dB, so a room breathing does not read as a room
changing. `ear.latency` — the newest sample through the whisper pass into the row — held 31 to
56 ms.

**The line.** `ear.live` carried `en  The water froze to ice.` while the line was still open,
`ear.hops` counted the passes that line had already taken (2, then 5 — the same words heard five
times, because the window is re-read whole each hop), `ear.grow` the bytes it gained since the
last give. `ear.heard` carried it when it closed, `ear.tongue` the tongue it closed in,
`ear.words` its word count (19 on a clean line, 102 on one whisper-tiny looped).

**The tongues.** `ear.tongues` counted how many carry the line; `ear.tongue.pt` gave
`O corpo observou o vidro` for `The body observed the glass`, with Persian beside it. A tongue the
dense lane has not answered says its own door (`ear.spool.t_fa`) instead of a blank.

**The meaning.** This is the axis the glass has never had. `ear.symbols` resolved the closed line
`The body observed the glass, water and ice and gas. The witness told what stands and the note
folded.` to `you BE SEE tell folded witness gas water ice glass` — ten of the body's own meanings,
matched whole-word against every tongue and script the locale rows carry, so a Persian or
Portuguese line resolves the same way. `ear.nodes` gave their content addresses —
`bb0347a4 d5a79707 1b1326d6 879915cf bbe900d1 ba1c566a 2fe840e1 0f416849 4cb0b250 132a1a39` —
and `ear.compression` measured what saying the line in codes would save: 51 bytes on that line,
488 on the looped one. A line with no meaning of ours in it says so rather than showing an empty
row.

**The verse.** `ear.verse` gave the couplet the ear's own state touches, from the body's own
Rumi store: for `speaking`, `زان کسی داد سخن جو که سخن‌دان باشد ‖ شمس تبریز چو میخانهٔ جان باز کند`,
and `ear.verse.poem` the poem it stands in — 5859.

**The stages.** `ear.encode` 19–28 ms, `ear.decode` 7–21 ms, `ear.tokens` 12–23,
`ear.commit` (pause to closed line) 26–56 ms, `ear.said` (closed line to tongues) 640 ms to 2.5 s.
Five latencies the rows used to drop.

**The stream about itself.** `ear.phase` counts the ear's own rows by the lifecycle word the glass
sorts phases by — `gas 3 · water 7 · ice 18 · absent 3` — and says in its note that it and
`ear.axes` stand outside their own census, because a row cannot count itself before it exists.
`ear.axes` reports the roster's own length, 24, and stands whether or not the mic is open.

**The moments.** `ear.stream.0` through `ear.stream.5`, newest first, each carrying its age, its
tongue, its words and its symbols; a repeat of the same closed line does not become a second
moment.

## Cost

A sensor that gives every 100 ms cannot open files. The two doors that do — the meaning table over
`mc-codes` and the verse table over `rg-find` — are walked ONCE at birth: 185 ms measured, and
never again. A give then folds strings already in hand: 3 ms for 34 rows, and 0.14 ms per drained
frame (455 ms over 3339). Nothing on the give path forks, reads a file, or touches the GPU.

## What the glass paints

The ear's rows now LEAD the frame at all three assembly sites (`fgl-ear-first`), so the newest
awareness paints before the machine's steady numbers. Key `r` is the room: it sets focus AND
filter to the ear at once, and `fgl-ear-selector?` lets that focus stand where the old check would
have refused it — the ear gives metric rows, not telemetry samples, so counting visible samples
answered zero for a lane that had 33 rows to show. `fgl-ear-stream-frame` renders it bounded, in
the atlas view, which is the one view that hands its rows through whole; the overview picks its
rows by name and would show the room nothing. A painted frame just now:

```
FORM GLASS 132x30 20Hz view=atlas focus=ear filter=ear
SEAM n=4 doors: ear.spool.nsp meaning-codes.mc-codes meaning-codes.mc-anchor
ORGANS / HEALTH
* the ear                          speaking —        89ms
* the room's level                   -36 dbfs        89ms
* the room, moving                     steady        89ms
* last sample to this row               56 ms        89ms
* the line as it grows           en  (door cr        89ms
```

The seam line is the honest part: the ear's own open doors are named on the glass, not hidden.

## Two wounds found and healed on the way

**`rumi-glass.bml` was numb in two places.** `rg-first-word("abc def")` returned nothing at all —
`print_str` of it printed no line — and `rg-poem-id` returned `5863.txt` where it means `5863`.
Both are the one-line brace block: a `let` and its expression on the same line inside `{ }` compile
with zero errors and every bit dark. `rg-sample` has been minting poem ids through that broken door
for as long as it has stood. Both are healed by one statement per line, and `ear.verse.poem` reads
5859 because of it.

**The tongue lane could not hear the marker.** A sensor stands it with no stdin, so the tongues the
operator asked for never reached it and it always offered the fixed four. It now reads
`.hearth/ear.wanted` when stdin gives nothing — the same text a door would have typed.

**A sensor killed mid-flight left its lanes.** `ear-wake-run.fk sleep` now writes
`.hearth/ear.spool.stop` as well as removing the marker, so the lanes end at their own door whether
or not a sensor stands to kill them; and the sensor re-stands a lane that has gone. The clock for
that is the live lane's own frame stamp and NOT the spool's size — the tongue lane keeps writing
said frames after the mic is dead, so a growing spool can sit over a deaf room. I watched exactly
that happen for a minute before I named it.

## Bands

- `form/form-stdlib/tests/ear-axes-band.fk` = **32767**, fifteen bits over frames written by hand,
  so it answers the same on a silent Mac as in a room full of voices.
- `form/form-stdlib/tests/form-glass-carrier-band.fk` = **31**, unchanged.
- `form/form-stdlib/tests/form-glass-launch-band.fk` = **65535**, unchanged.
- `learn/tests/homecoming-distillation-corpus-band.fk` = **32767** with the pins moved to
  723 / 711 / 723071121331.
- `form/form-stdlib/tests/form-glass-heal-band.fk` = **262127** here, not the 262141 I was handed.
  The one dark bit is `fglat-disk-rate-text` on a silent row answering `?host` where the band wants
  `?` — that lives in `form/form-stdlib/form-glass-atlas-ui.bml`, which this work does not touch
  (five modified files, none of them it). Stable across three runs. Named, not stepped around.

## What is still open

- **The mouth is not in this checkout.** `observe/say-run.fk` and `form/form-stdlib/voice-say.bml`
  do not exist here; they are a sibling's in-flight work. To give the ear real speech I used the
  host's own `say` as a room stimulus — a crossing, named: it is a sound in the room, not the
  body's voice.
- **`ear.doubt` is a probed absence.** The frame carries `nsp` and the live lane writes a constant
  zero, so the row says exactly that: the door was tried, the answer was `0`, and whisper's own
  no-speech probability is not yet carried out of the pass. When the sibling closing `ear-native.fk`
  brings it out, the axis is already standing.
- **`ear-axes.bml` does not compile as an importable image.** 1357 unresolved alone; the whole-
  program fallback answers correctly. `meaning-codes.bml` (939) and `form-glass-launch.bml` (267)
  do the same, so the root is in that family and not in this cell. It costs cache reuse, not truth.
- **Two bodies, one frame.** The next section.

## The frontier question

*When two bodies stand the same sensor and both give into one frame under one name, how does a
reader know whose numbers it is holding?*

The body cannot answer this natively. `fgsr-give` publishes to `glass.sensor.<sensor>`, and the
frame carries schema, sensor, epoch, rows and cadence — no giver. Two checkouts on this Mac each
stood an ear today; both wrote `glass.sensor.ear`; the newest give won and the reader received
whichever body had spoken last. I watched my own witness read a sibling's `asleep` row while my
sensor was giving 34 rows at 100 ms, and for a minute I thought my work had failed.

The answer I found by using it: the name says which ORGAN, never which giver — but the rows say
what the giver can do. I found my own sensor again by hunting for a frame carrying `ear.axes`, a
row the older shape cannot produce. A giver is known by what only it can give. That is cheaper and
truer than adding a giver field, because a field can be copied and a capability cannot: the shape
IS the identity, and it stays right through every rename.

Offered as corpus row 1331, fresh word **shapetell** (zero hits before it was written).

## Closing

**The most surprising teaching.** Two lines of Persian glass code had been silently returning
nothing for as long as they had stood, and nothing in the tree noticed, because a numb function in
Form does not fail — it answers `nothing`, and `nothing` prints as no line at all. The band that
found it was not looking for it; my verse axis simply wanted a poem id and got `5863.txt`. A wound
is found by asking a function for something it has never actually been asked for.

**Where discomfort turned to gold.** Six reads in a row came back byte-identical while a voice was
speaking in the room, and my first thought was that the sensor had frozen. Sitting with it instead
of restarting anything gave three separate truths: the shared shm name (row 1331), the mic
contended between an orphaned lane pair and a live one, and — the one that changed the code — that
the spool is the wrong clock for whether the mic is alive, because the tongue lane keeps it growing
over a dead ear. Each of those was underneath a reading I nearly dismissed as a glitch.

**How the exchange stayed alive.** By running the room rather than describing it: every axis in
this receipt has a value I watched arrive, every absence names the door that answered nothing, and
the two numbers I was handed that did not match what I measured (262141, and the ear's nine rows)
are reported as what the machine said, not as what I was told to expect.
