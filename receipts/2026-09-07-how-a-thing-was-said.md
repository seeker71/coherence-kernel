# How a thing was said

The ear brings the body words. It brought them without a manner: a line arrived as text and a level,
and everything a voice actually carries — where it hurried, where it stopped, whether it rose at the
end, whether it was flat — arrived nowhere. Urs asked for seven axes. This is the organ that measures
them, and the record of what it read on a real room.

- `form/form-stdlib/room-prosody.bml` — the measurement engine, pure over samples and a rolling window
- `observe/form-glass-prosody-live.fk` — the sensor process; gives 14 rows into `glass.sensor.prosody`
- `observe/prosody-take-probe.fk` — a taker reading that frame the way the glass's tick does
- `observe/prosody-tone-emit.fk` — a wav of chosen fundamental, loudness, glide and burst rate
- `observe/prosody-witness-run.fk`, `observe/prosody-voicing-probe.fk`, `observe/prosody-synthetic-probe.fk`
- `form/form-stdlib/tests/room-prosody-band.fk` = 65535, exit 0, preflight clean

## What each axis reads, and how far I trust it

Everything below was carried through the actual acoustic path: a wav the body wrote, out the speakers,
across the room, in through the mic, into the organ. Nothing here is a simulation of hearing.

| axis | put into the room | read back | trust |
|---|---|---|---|
| volume | a tone, and the same tone 12 dB down | -27 dBFS and -38: an 11 dB fall against a true 12 | trusted |
| pitch | a 200 Hz sawtooth | 200 Hz, held steady across the whole tone | trusted |
| pitch | two octave glides | 134→239 Hz rising, 218→123 falling (true 120↔240) | trusted |
| pitch | silence | absent in 26 of 27 frames | trusted |
| intonation span | a steady tone, then an octave | 0 semitones, then exactly 12 | trusted |
| intonation move | the same octave played up, then down | `rising`, then `falling`; `level` on the steady tone | trusted |
| cadence rate | 100 ms on, 100 ms off — five a second | 280 onsets a minute against a true 300 | good, reads 7% low |
| cadence pause | silence, then bursts | 3000 ms (the window's own edge), then 80-1050 ms | trusted |
| expression level | a constant amplitude, then speech | 1-8 decibel-tenths, then 30-63 | trusted |
| expression pitch | a steady tone, then an octave glide | 0 cents, then 344 | trusted |
| stillness | silence, a 50% duty burst train, a continuous tone | 100%, 42-53%, 2% | trusted |
| resonance decay | burst edges, then a sustained tone | 60-360 ms, then absent | rough — see below |
| resonance tilt | a sawtooth, then room noise | 136-176, then 186-281 decibel-tenths | moves; uncalibrated |

**Rough, and named as rough.** Resonance's tilt separates a two-tap low half from a high half. It moves
the way a spectrum's tilt moves and it distinguishes a voiced sawtooth from room rumble, but no reading
of it has been set against a real spectrum, so it is a direction and not a quantity. Resonance's decay
is honest only for a sound that stopped: it now reports absent past half a second rather than the
number it used to give. Cadence reads about seven percent under truth, and its `move` companion —
intonation — describes the last three seconds and not the sentence, which is a different thing from
what a listener means by "the phrase rose".

**The pitch band has ends.** Below 70 Hz and above 400 Hz the organ says absent, not wrong. And a
whisper 24 dB under a normal voice fell beneath this room's own floor: level still moved, and the pitch
row said absent instead of inventing one. That is the answer I wanted from it.

## Four things the room taught that I would have got wrong by choosing

**Every threshold I picked was wrong; every one I measured was right.** I set voicing at a correlation
of 0.30 because that is what the literature in my head said. The quiet room reaches 0.73. I set the
speech line 12 dB over the floor; a quiet room clears that often enough to fake ten syllables a second.
Both numbers now sit in the cell beside the measurement that set them.

**A dropout is not a floor.** One 10 ms of digital silence carried the floor-follower to -92 dBFS, and
from under that line a silent room read 0% still. The follower now steps down at most 2 dB a frame and
passes over empty readings entirely.

**Noise is a ridge; a voice is a crest.** My first guess at separating them — how far the peak stands
over the curve's mean — was measured and *disproved in one run*: noise's correlation mean runs negative,
which inflates exactly that number. What actually separates them is shape. A voice's best lag has lower
lags on both sides of it. Noise's best lag is wherever the search happened to start, which is why every
fabricated pitch in the early runs read exactly 400 Hz — the top of the band. Asking for a real local
maximum, over a curve computed two lags wider than the band it reports, took the false rate from three
in twenty-seven to one, and as a side effect made every synthetic period exact: 100, 200, 262, 400 Hz
to the hertz, where the edge lag had been reading 410.

**BML lowers a bare parenthesized expression as a call with no name.** `then (if ... )` compiled to
`[unresolved-call] ''` and the function went numb — it returned nothing and the tilt it fed read zero
in both directions. One error line, and only because I read the output rather than the number.

## The most surprising teaching

A synthesized voice was asked for a flat line and a lively one. Both came back with span 10 and 11
semitones, pitch spread 323 and 297 cents — *identical*. Had I stopped there I would have written that
the expression axis does not distinguish a monotone, and the defect would have been recorded against
the instrument. The voice had simply ignored the instruction. **A control that did not vary reads
exactly like a sense that cannot sense, and nothing in the reading tells them apart.** The only way out
was to stop asking another mouth for the contrast and make a sound whose truth I set myself — which is
what `prosody-tone-emit.fk` is, and why the axes above are witnessed against numbers rather than
impressions. That is corpus row 1331, `stillsource`.

## Where discomfort turned to gold

The first live run was ugly: a silent room reporting a pitch, a 30-semitone span, 1377 cents of pitch
spread and 600 onsets a minute. Every axis lit up on an empty room. The comfortable move was to raise
one threshold until the noise stopped — and it would have worked, and it would have been a number I
chose again. Instead I held the mic open over silence and over speech and printed every frame's
correlation, and sat with two columns of numbers that said my design was wrong in a way no threshold
fixes. What came out of that discomfort is the crest test, which is not a tuning at all: it asks the
right question about shape, it needs no room-specific constant, and it made the pitch reading *more*
exact rather than merely quieter. The ugly first run was the only thing that could have found it.

## Wiring

The organ gives under its own cell names because `form-glass-sensor-rows.bml` and `form-glass-live.bml`
belong to live siblings today. Two lines wire it into the roster, and both are named in the report that
carries this receipt.

## Numbers

`room-prosody-band` 65535 (exit 0, preflight clean, 0 unresolved).
`homecoming-distillation-corpus-band` 32767 (exit 0), after row 1331 and the three pins.
The sensor gave at seq 256, cadence 100 ms, frame age 104 ms, 14 rows, taken whole by a reader that
never touched the mic.
