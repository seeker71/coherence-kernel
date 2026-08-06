# Non-toy concept sensing witnesses

Date: 2026-07-18

This receipt records ordinary-world inputs and executable outputs. The live
entrypoints remain the evidence; these rows are not substituted fixtures.

## Spoken family sentence enters the full 10,000-concept detector

`presence/concept-audio-text-10000-live.fk` generated speech-only PCM with a
held-out US Eddy voice:

```text
spoken input:  The camera recorded our family picnic.
Whisper output: The camera recorded our family picnic.
WAV:            pcm_s16le, mono, 16 kHz, 3.614688 s
PCM observed:   57,835 samples, mean absolute level 1,502, present=1
WAV SHA-256:    26ecae40d438bb1a25ae66e0fa0dfbf0c73f793708302648300cef82ab3fac66
address signal: none:speech-only-no-address-envelope
```

The complete Form sentence scanner returned six uncollapsed concept anchors:

```text
family 270, recorded 3865, picnic 5079, camera 959, the 2, our 94
expected camera: found as 959, source F
camera senses: 2, context-ranked-not-resolved
acceptance: 63/63
```

This directly closes the former three-candidate integration ceiling: the audio
fixture still has three acoustic distractors, but its clean transcript is now
consumed by the full 10,000-anchor detector. The sense result stays honest: the
sentence ranks both camera senses but does not claim final word-sense resolution.

```sh
SEMA_WHISPER_MODEL=/absolute/path/to/ggml-large-v3-turbo.bin \
  ./fkwu --src presence/concept-audio-text-10000-live.fk
SEMA_WHISPER_MODEL=/absolute/path/to/ggml-large-v3-turbo.bin \
  ./fkwu --src presence/tests/concept-audio-text-10000-live-band.fk
# 63
```

The larger speech matrix contains 20/20 recognized ordinary utterances across
all 13 locales, water/camera/music, and seven held-out voices. Its complete
transcripts and waveform hashes live in
`receipts/2026-07-18-concept-audio-speech-content-13.md`.

## Real video content, intervention, and abstention

Four held-out frames from each committed real trajectory entered Apple's
pretrained image classifier as pixels only. Form composed the raw labels,
without seeing filenames or requested IDs:

```text
office 493:       score 34, margin 34
university 1927:  score 10, margin 10
courthouse 9066:  score 12, margin 12
world persistence: 3/3 content-derived entities
```

A solid magenta band then covered the identity-envelope region. All three IDs
remained stable. Independent Oxford footage was the negative control and
abstained both raw and banded (best score 6) instead of being forced into a
known class. The older exemplar sensor is deliberately retained with its hard
failures: 10/18 early-to-late, 10/18 late-to-early, and 0/7 on Oxford.

```sh
./fkwu --src presence/tests/concept-video-content-sensing-live-band.fk
# 511
./fkwu --src presence/tests/concept-video-semantic-stress-live-band.fk
# 127
```

The corrected 3 × 13 real-footage generation matrix is 39/39 generated,
animated, and localized. `loop-24f.mkv` is aligned to what is visible—office
493—not lexical loop 6196; courthouse footage is exact anchor 9066, not broad
court 751. Both old lexical mappings now return no backdrop.

## Text ambiguity is data, not first-match loss

The live text organ scanned a real sentence in each of the 13 NL lenses and
returned `262143`. Its explicit WN3.1 data contains 34,244 senses over 7,371
mapped anchors; 5,618 anchors are polysemous. Indonesian `ya` preserves all 17
candidate concept IDs. Programming and eyelet contexts rank different senses
of `loop`, while both are marked `context-ranked-not-resolved`.

```sh
./fkwu --src cognition/tests/concept-text-detection-13-live-band.fk
# 262143
```

## Programs execute behavior, not just source recovery

Four materially different concepts—`you` 265, `go` 504, `water` 1133, and
`loop` 1110—were generated, compiled or interpreted, executed, and checked in
12 permitted languages: JavaScript, TypeScript, Java, C, C++, C#, Go, Rust,
Ruby, PHP, Swift, and Kotlin. That is 48/48 exact executions. Python source
remains generated and recovered across the 10,000 × 13 matrix but was not run,
following the user constraint.

```text
you   -> below-unmatched:265:0
go    -> below-exact:504:5
water -> below-exact:1133:4
loop  -> holds-exact:1110:5
```

```sh
eval "$(presence/carriers/concept-pl-toolchains.sh)"
./fkwu --src presence/tests/concept-pl-10000-13-live-execution-band.fk
# 1023
```

## Honest remaining floor

These are real examples, not parity claims over every modality cell. Audio
content uses pinned local Whisper weights; video content uses host-pretrained
Vision labels and a three-concept Form evidence vocabulary; generated video is
not learned open-vocabulary synthesis. Text context ranks but does not resolve
senses. Those remaining gaps stay work orders rather than being hidden behind
the 130,000-address generation counts.

No Python ran and `runtime/fkwu-uni.c` was not changed.
