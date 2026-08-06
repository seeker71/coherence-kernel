# speech-native-neural-pair-window-0041

## What moved

The background pair lane trained the next uncovered native neural micro-pair:
`ar<->it`, meaning `303` (`aham-asmi`). This adds one reciprocal pair window
and two directed routes, moving the neural pair floor from `40` to `41` and the
directed neural route floor from `80` to `82`.

This is a closed pair-window receipt. It does not promote open ASR, Sema live
voice, or full TTS authority.

## Local oracle probe

Fresh local Mac loopback evidence was rendered under `/tmp` and not committed:

- Arabic: `say -v Majed "أنا"` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l ar` heard `أنا.`, wav bytes `10906`, cksum `3840888175`.
- Italian: `say -v Alice "Io sono."` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l it` heard `Io sono`, wav bytes `16706`, cksum
  `3343634846`.

Total observed wav bytes for this window: `27612`.

## Native receipt

`learn/speech-native-neural-pair-window-0041.fk` records:

- native Form NL lanes: `ar->it`, `it->ar`, `ar->ar`, `it->it`
- native Form audio lanes: `ar->it`, `it->ar`, `ar->ar`, `it->it`
- local oracle pass: `2/2`
- Form NL rate: `100`
- Form audio rate: `100`
- native neural rate: `0 -> 100`
- cumulative neural pairs: `41`
- cumulative directed neural routes: `82`
- epochs/params: `41/41`
- open authority: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0041.fk \
    learn/tests/speech-native-neural-pair-window-0041-band.fk
  printf '\n(speech-native-neural-pair-window-0041-band)\n' ) > /tmp/snpw41.fk
./fkwu --src /tmp/snpw41.fk
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

The model floor is now wider: `41` native neural micro-pairs, `82` directed
routes, and `41` admitted native neural parameters. The global live authority
floor remains unchanged: open dictation is still native `0/4`, and Sema live
voice is still native `0/1`.
