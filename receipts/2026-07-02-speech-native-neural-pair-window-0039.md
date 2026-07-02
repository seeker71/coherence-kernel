# speech-native-neural-pair-window-0039

## What moved

The background pair lane trained the next uncovered native neural micro-pair:
`it<->pt-br`, meaning `303` (`aham-asmi`). This adds one reciprocal pair window
and two directed routes, moving the neural pair floor from `38` to `39` and the
directed neural route floor from `76` to `78`.

This is a closed pair-window receipt. It does not promote open ASR, Sema live
voice, or full TTS authority.

## Local oracle probe

Fresh local Mac loopback evidence was rendered under `/tmp` and not committed:

- Italian: `say -v Alice "Io sono."` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l it` heard `Io sono`, wav bytes `16706`, cksum
  `3343634846`.
- Portuguese Brazil: `say -v "Flo (Portuguese (Brazil))" "Eu sou."` ->
  `ffmpeg` 16 kHz mono -> `whisper-cli -l pt` heard `Eu sou.`, wav bytes
  `31774`, cksum `1996659987`.

Total observed wav bytes for this window: `48480`.

## Native receipt

`learn/speech-native-neural-pair-window-0039.fk` records:

- native Form NL lanes: `it->pt-br`, `pt-br->it`, `it->it`, `pt-br->pt-br`
- native Form audio lanes: `it->pt-br`, `pt-br->it`, `it->it`, `pt-br->pt-br`
- local oracle pass: `2/2`
- Form NL rate: `100`
- Form audio rate: `100`
- native neural rate: `0 -> 100`
- cumulative neural pairs: `39`
- cumulative directed neural routes: `78`
- epochs/params: `39/39`
- open authority: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0039.fk \
    learn/tests/speech-native-neural-pair-window-0039-band.fk
  printf '\n(speech-native-neural-pair-window-0039-band)\n' ) > /tmp/snpw39.fk
./fkwu --src /tmp/snpw39.fk
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

The model floor is now wider: `39` native neural micro-pairs, `78` directed
routes, and `39` admitted native neural parameters. The global live authority
floor remains unchanged: open dictation is still native `0/4`, and Sema live
voice is still native `0/1`.
