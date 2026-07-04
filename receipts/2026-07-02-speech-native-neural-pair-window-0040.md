# speech-native-neural-pair-window-0040

## What moved

The background pair lane trained the next uncovered native neural micro-pair:
`zh<->it`, meaning `303` (`aham-asmi`). This adds one reciprocal pair window
and two directed routes, moving the neural pair floor from `39` to `40` and the
directed neural route floor from `78` to `80`.

This is a closed pair-window receipt. It does not promote open ASR, Sema live
voice, or full TTS authority.

## Local oracle probe

Fresh local Mac loopback evidence was rendered under `/tmp` and not committed:

- Chinese: `say -v Tingting "我在。"` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l zh` heard `我在`, wav bytes `15064`, cksum `3568659029`.
- Italian: `say -v Alice "Io sono."` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l it` heard `Io sono`, wav bytes `16706`, cksum
  `3343634846`.

Total observed wav bytes for this window: `31770`.

## Native receipt

`learn/speech-native-neural-pair-window-0040.fk` records:

- native Form NL lanes: `zh->it`, `it->zh`, `zh->zh`, `it->it`
- native Form audio lanes: `zh->it`, `it->zh`, `zh->zh`, `it->it`
- local oracle pass: `2/2`
- Form NL rate: `100`
- Form audio rate: `100`
- native neural rate: `0 -> 100`
- cumulative neural pairs: `40`
- cumulative directed neural routes: `80`
- epochs/params: `40/40`
- open authority: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0040.fk \
    learn/tests/speech-native-neural-pair-window-0040-band.fk
  printf '\n(speech-native-neural-pair-window-0040-band)\n' ) > /tmp/snpw40.fk
./fkwu --src /tmp/snpw40.fk
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

The model floor is now wider: `40` native neural micro-pairs, `80` directed
routes, and `40` admitted native neural parameters. The global live authority
floor remains unchanged: open dictation is still native `0/4`, and Sema live
voice is still native `0/1`.
