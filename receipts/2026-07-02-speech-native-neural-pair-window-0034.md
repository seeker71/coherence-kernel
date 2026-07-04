# speech-native-neural-pair-window-0034

## What moved

The background pair lane trained the next native neural micro-pair:
`fr<->sa`, meaning `303` (`aham-asmi`). This adds one reciprocal pair window
and two directed routes, moving the neural pair floor from `33` to `34` and the
directed neural route floor from `66` to `68`.

This is a closed pair-window receipt. It does not promote open ASR, Sema live
voice, or full TTS authority.

## Local oracle probe

Fresh local Mac loopback evidence was rendered under `/tmp` and not committed:

- French: `say -v Thomas "Je suis."` -> `ffmpeg` 16 kHz mono -> `whisper-cli -l fr`
  heard `Je suis`, wav bytes `17908`, cksum `3224208913`.
- Sanskrit baseline, romanized carrier: `say -v Rishi "Aham asmi."` -> `ffmpeg`
  16 kHz mono -> `whisper-cli -l en` heard `Aham Asmi.`, wav bytes `27510`,
  cksum `2808389783`.

The Sanskrit row is deliberately named as a romanized baseline carrier. It is
not a claim that this macOS host has a native Sanskrit voice.

Total observed wav bytes for this window: `45418`.

## Native receipt

`learn/speech-native-neural-pair-window-0034.fk` records:

- native Form NL lanes: `fr->sa`, `sa->fr`, `fr->fr`, `sa->sa`
- native Form audio lanes: `fr->sa`, `sa->fr`, `fr->fr`, `sa->sa`
- local oracle pass: `2/2`
- Form NL rate: `100`
- Form audio rate: `100`
- native neural rate: `0 -> 100`
- cumulative neural pairs: `34`
- cumulative directed neural routes: `68`
- epochs/params: `34/34`
- open authority: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0034.fk \
    learn/tests/speech-native-neural-pair-window-0034-band.fk
  printf '\n(speech-native-neural-pair-window-0034-band)\n' ) > /tmp/snpw34.fk
./fkwu --src /tmp/snpw34.fk
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

The model floor is now nonzero and wider: `34` native neural micro-pairs,
`68` directed routes, and `34` admitted native neural parameters. The global
live authority floor remains unchanged: open dictation is still native `0/4`,
and Sema live voice is still native `0/1`.
