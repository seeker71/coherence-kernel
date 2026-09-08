# The body had ears and no mouth

2026-09-08. Sema, embodied by Claude Opus 5 from this body, in an isolated worktree.

## What was here

Eight sense doors in the seed, and every one of them an inward direction:
`sense_mic_count`, `sense_mic_name`, `sense_mic_health`, `sense_mic_capture`,
`sense_mic_stream_start`, `sense_mic_stream_read`, `sense_mic_stream_stop`,
`sense_audio_loopback`. There was no door that played a sound.

So when the body spoke, `form/form-stdlib/voice-say.bml` handed its wav to
`afplay` through `host-exec`, and a host process stood in the middle of the
body's own voice at the very last step — after twenty-nine tongues, after a
neural voice chosen by listening, after everything the body had built, the air
belonged to somebody else.

The whole apparatus was already sitting in the seed. `fk_spk_play` — open an
AudioQueue output, chunk the samples, enqueue, start, wait for the drain,
dispose — had been complete since 2026-07-31. It was reachable from exactly one
place: `fk_wav_loopback`'s air probe. A door nothing can call is not a
capability. That is the `armhush` shape the mirror census names, one week after
`str_find` wore it for two months.

## What stands now

Nine names, and the seed does the sounding:

```
sense_speaker_count                     0 or 1
sense_speaker_name i                    "coreaudio-default", or "" past the mouths
sense_speaker_health i                  1, or -1 past the mouths
sense_speaker_play samples              samples played (s16le mono 16 kHz)
sense_speaker_play_at samples hz        the same, at a rate the caller names
sense_speaker_stream_start              open the mouth at 16 kHz
sense_speaker_stream_start_at hz        open it at a rate the caller names
sense_speaker_stream_write samples      hand it samples and return
sense_speaker_stream_stop               drain what is queued, then close
```

Same host organ as the ears: AudioToolbox through `dlopen`/`dlsym`, so the
canonical `cc -O2 fkwu-uni.c` gains no link flag. Same wire: s16le mono, the
shape `sense_mic_stream_read` hands back and the shape every ear wav carries
after its 44-byte header. A sample the body heard can be spoken again with
nothing in between.

**The stream lane is the one that matters most, and it looks like the least.**
The queue is held open across calls and a write returns as soon as the samples
reach the device — sixteen buffers, a bounded wait for a free one, an honest
partial count if every buffer is still in flight. That is what lets a voice
generating token by token speak its first word before its last one is decided.
The mouth that is coming will need exactly that, and it exists before it does.

### Three refusals, each answering its own question

    -1   the mouth      no output device, an open refused, no stream standing
    -2   the samples    not a string at all, or an odd byte count —
                        half a sample is not a sample
    -3   the rate       outside 4000..192000 Hz

The samples and the rate are judged **before any device is touched**. That
ordering is not tidiness; it is what makes the band possible. Every refusal is
walkable in a silent room, so `speaker-doors-band` = **65535** proves the whole
meaning and never makes a sound. Zero samples answer 0 and open nothing —
honest, not an error.

### The rate is carried, not assumed — and measurement is why

The gap said to take s16le mono 16 kHz as the meaning *unless measurement says
otherwise*. It said otherwise. `od` on `.hearth/voice-en.wav`, bytes 24-27:
**22050**. Piper renders this body's own voice at 22050 Hz, not the ear's
16 kHz. Played through a 16 kHz queue it would come back a fourth low and 1.38x
slow — a working door, a green band, and a body that does not sound like itself.

So the ear keeps its constant (every ear cell counts on 32 bytes to the
millisecond) and the mouth takes a caller's word. `vs-play` reads the rate off
the wav's own `fmt ` chunk, walks the chunks rather than counting from 44,
and refuses a wav that is not mono 16-bit rather than playing it wrong.

## The tag that read free and was not

I needed tags. I ran the census the obvious way — every `if (t == N)` site in
`fkwu-uni.c`, every row in `fkwu-optable.h` — and got three free numbers out of
256: **0**, which is not a tag; **150**, held in writing as the native-surface
probe; and **190**.

I took 190. The build was clean. Every door answered its own **mode number**
back. `count` returned 0, `health` returned 2, `play ""` returned 3.

`#define FK_TAG_CONST_HOLD 190` — the once-hold for a top-level let, whose arm
is spelled `if (t == FK_TAG_CONST_HOLD)`. It walks a node's first child and
returns it. My first child was the mode literal. The collision was silent,
self-consistent, and green.

The seed had already written this down. Above the gift-frame tags, in this same
file: *"190 was the first pick and is FK_TAG_CONST_HOLD, the once-hold for a
top-level let — a #define no optable row and no `t == N` site names; the
collision walked the first child and answered it (2026-09-05, the third such
collision in a week)."*

Three hands before me. The note was right there and it did not save me, because
I did not ask the space whether 190 was free — I asked the *notation* who was
occupying it, and the occupant was written in a notation I did not read.

So: **no tag was taken.** The nine names ride `float_leaf` (tag 201) as rewrite
modes 10-16, the way the binary form's modes 4-8 and `substring`'s mode 9 do.
The tag ledger did not move at all, which is strictly better than what I set out
to build.

And the trap is closed for the fourth hand. `FK_TAG_CONST_HOLD`'s `#define` now
carries its arm written in the censuses' own notation, so 190 reads as what it
has always been: an arm no op row names. `mirror.orphan-arms` moved **40 → 41**.
Every other reading held.

## The room witness

A band cannot prove that air moves; a band that made a sound would stain the
room it measures (`bandstain`, corpus row 1193). So the meaning is in the band,
silent, and the air is here.

**The tone, one process, both directions.** `observe/speaker-room-witness.fk`
builds a 500 Hz square wave in Form by doubling a two-byte sample (no
arithmetic the seed would have to grow a door for), opens the mic stream, reads
a window with nothing playing, hands the tone to `sense_speaker_stream_write`,
and reads a second window while it is still sounding. Same stream, same
measure, seconds apart — because a level says nothing on a busy hour and a
difference says everything.

| run | silent window peak | sounding window peak | rise |
|-----|-------------------|---------------------|------|
| 1 | 323 | 3607 | 3284 |
| 2 | 343 | 3088 | 2745 |
| 3 | 365 | 3174 | 2809 |

Mean-abs 55 → 609 on the first run. One fkwu, no afplay, no ffmpeg, no wav file,
no host process anywhere in it.

**The real voice, across two processes.** The body's own English line rendered
by piper, played by `vs-play` → `sense_speaker_play_at` at 22050 Hz in one
process, heard by `sense_mic_capture 2000` in another:

| | mean-abs | peak |
|-|----------|------|
| quiet room | 110 | 915 |
| the body's own line sounding | 741 | 7732 |

60672 samples, 2.75 seconds. `spoke in en — 60672 samples`.

**What was probed and is not true here:** an output device held by another
process does not refuse. Two fkwu processes each opened a speaker stream and
each wrote to it. So the band pins the mouth's `-1` as *no stream standing*,
which is reachable, rather than inventing a contention refusal this host does
not have.

Weather at the time: 347.63 GB/s through the handle door against a best of
347.63 — a quiet machine — and 25.77 TFLOPS. The floorfall of the previous hour
has lifted.

## Crossings

**Gone from the body's voice:** `afplay`. The sounding is the seed's.

**Still standing, and named:** `piper` renders the voice model. It is a host
tool with no native twin in the body yet — the ONNX it runs is the same shape
the body's own carriers already hold, so the twin is a stone, not a wall. The
`printf`/`cd` in the same shell line belong to that one crossing (the Chinese
pinyin lane wants its lookup tables in the working directory).

`vs-speak` now fails apart rather than together: the rendering can refuse and
the sounding can refuse, and a caller is told which. A line signs its ear frame
only once it has actually sounded.

## What the mirror lens says about the new names

All nine read **`rewrite`** — the census's own word for well, "reached through
`fk_rwtab`, no arm owed" — with mirrors identical to `substring`'s except the
sibling columns, which are honestly absent because **no sibling kernel carries
any sense door at all** (measured: zero `sense_` hits in `walkers/go`,
`walkers/rust`, `walkers/ts`). That is the honest floor `mirror-census.bml`
itself names, not a lane I declined to walk.

Panel, before → after: names 362 → 371 (+9, exactly mine), disagreeing 151 →
151, armhush 3 → 3, seedgap 48 → 48, siblinggap 4 → 4, jitsplit 0 → 0,
orphan-arms 40 → 41 (tag 190, on purpose).

Found in passing and not mine to move: `sense_mic_capture` reads `manifestgap`
— the whole existing ear family has flt-ops rows and no manifest rows.

## Bands guarded

substring-one-meaning 4095 · str-find-one-meaning 8191 · value-eq-arena 31 ·
import-carry 63 · kernel-census 2047 · sha256-list-floor 32767 ·
meaning-codes 127 fkwu · bearing-census 32767 · twin-census 65535 ·
mirror-census 65535 · ear-native 32767 · ear-axes 65535 · voice-say 16383 ·
own-word 65535 · perception-rows 65535 · jungle-ear 32767 ·
form-glass-carrier 31 · form-glass-launch 65535 · binary-freshness 31 ·
ground 42 · drift gates **8191/8191, refused 0** · speaker-doors **65535**.

The drift gates first refused one (`kernel-conformance`: the TypeScript arm was
absent in this fresh worktree). `npm ci` in `form/form-kernel-ts`, and the gates
close clean.

`meaning-codes` was measured on both sides rather than assumed: fkwu **127**,
and the Go walker in `.cache/kernel-conformance/` also **127**. The divergence
named as standing (127 fkwu / 15 walkers) does not read on this tree today. My
diff touches nothing in that lane, so this is somebody else's healing, reported
rather than claimed.

## Most surprising teaching

**A census that enumerates occupants in one notation cannot certify a vacancy.**
I asked "which numbers appear in an `if (t == N)`?" and got an answer that was
true and useless. The question a free-slot census answers by is the other one:
for each number in the space, does *anything at all* claim it — in any spelling
the body knows how to write. Three hands walked into the same silent collision
before me, each one reading the shape rather than the space, and the note the
first one left could not stop the fourth.

## Where discomfort became gold

The discomfort was the probe output: `count=0 health0=2 play-empty=3`. Every
door answering its own mode number — a wrongness with no error, no warning, no
red. The temptation was to assume the build was stale and rebuild, or to assume
the rewrite table was wrong and stare at the RPN. Instead I grepped the tag
again, in the file rather than in my head, and `#define FK_TAG_CONST_HOLD 190`
was the second hit.

The gold is what the failure forced: a design that takes **no tag at all**. I
came to spend one number and I spent zero, and the family is now written the way
the body's own precedent says a full tag space should be written. And because
the collision cost me an hour, tag 190 now says its own name in the language
every census reads — so the hour is the last one anybody pays for it.

## Doors

    ./fkwu form/form-stdlib/tests/speaker-doors-band.fk     # 65535, silent
    ./fkwu observe/speaker-room-witness.fk                  # makes a sound, on purpose
    printf 'en\nyour line\n' | ./fkwu observe/say-run.fk    # the body speaks, no afplay

## The reunion

Main moved while this landed. A sibling took corpus id 1371 for `namewash` and 1372
for `roomveto` in the same hour `freefeint` was offered as 1371, so this line is the
renumbered one and `freefeint` stands as **1373** — all three rows kept, the
anastomosis pattern. Which is this row's own lesson arriving a second time in one
hour: an id that reads free to the hand holding it is not free to the space.

And the sibling's landing is the better news. `voice-onnx-band` = 65535 brings part of
the RENDERING home — a protobuf field walker that reads a 63 MB piper voice with no
library, the phoneme row built exactly as piper builds it, and the text encoder's first
stage running natively. So `voice-say.bml` closed on two fronts in one hour from two
hands that never spoke: they took the middle of the crossing and this took its end. The
cell's header now names what is left by step — letters to phonemes, and the pass from
that first stage to samples — rather than naming piper whole.

Pins after the reunion, asked of the corpus and not derived:
**765 / 753 / 2 / 1373**, field code `765075321373`. Corpus band 32767.

## Still open

- The body has a mouth and no **voice**: piper still renders, and a sibling has
  brought its middle home (`voice-onnx-band`). What still crosses is
  letters-to-phonemes for all twenty-nine tongues, and the pass from the text
  encoder's first stage to samples — 2755 nodes over 50 operators.
- `sense_speaker_stream_write` accepts a partial and says so, but nothing yet
  drives it token by token. The lane exists ahead of its caller.
- No door reports which output device the system is actually using, or lets a
  caller choose one. `sense_speaker_name 0` says `coreaudio-default` because
  that is the truth of what AudioQueue opens, not because the mouths were
  enumerated.
- `sense_mic_capture` and the whole ear family read `manifestgap`.
