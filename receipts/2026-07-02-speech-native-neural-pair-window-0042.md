# speech-native-neural-pair-window-0042

## What moved

The background pair lane trained the next uncovered native neural micro-pair:
`sa<->it`, meaning `303` (`aham-asmi`). This adds one reciprocal pair window
and two directed routes, moving the neural pair floor from `41` to `42` and the
directed neural route floor from `82` to `84`.

This is a closed pair-window receipt. It does not promote open ASR, Sema live
voice, or full TTS authority.

## Local oracle probe

Fresh local Mac loopback evidence was rendered under `/tmp` and not committed:

- Sanskrit baseline, romanized carrier: `say -v Rishi "Aham asmi."` ->
  `ffmpeg` 16 kHz mono -> `whisper-cli -l en` heard `Aham Asmi.`, wav bytes
  `27510`, cksum `2808389783`.
- Italian: `say -v Alice "Io sono."` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l it` heard `Io sono`, wav bytes `16706`, cksum
  `3343634846`.

The Sanskrit row is deliberately named as a romanized baseline carrier. It is
not a claim that this macOS host has a native Sanskrit voice.

Total observed wav bytes for this window: `44216`.

## Native receipt

`learn/speech-native-neural-pair-window-0042.fk` records:

- native Form NL lanes: `sa->it`, `it->sa`, `sa->sa`, `it->it`
- native Form audio lanes: `sa->it`, `it->sa`, `sa->sa`, `it->it`
- local oracle pass: `2/2`
- Form NL rate: `100`
- Form audio rate: `100`
- native neural rate: `0 -> 100`
- cumulative neural pairs: `42`
- cumulative directed neural routes: `84`
- epochs/params: `42/42`
- open authority: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0042.fk \
    learn/tests/speech-native-neural-pair-window-0042-band.fk
  printf '\n(speech-native-neural-pair-window-0042-band)\n' ) > /tmp/snpw42.fk
./fkwu --src /tmp/snpw42.fk
# 32767
```

Aggregate witnesses after the update:

- `speech-neural-pair-coverage-band` -> `32767`
- `speech-pair-training-next-action-band` -> `32767`
- `speech-open-asr-tts-target-model-band` -> `32767`
- `speech-authority-learning-priority-band` -> `32767`
- `speech-model-metrics-report-band` -> `32767`
- `speech-learning-data-sufficiency-band` -> `65535`
- `speech-current-status-ledger-band` -> `32767`

## Honest boundary

The model floor is now wider: `42` native neural micro-pairs, `84` directed
routes, and `42` admitted native neural parameters. The global live authority
floor remains unchanged: open dictation is still native `0/4`, and Sema live
voice is still native `0/1`.
