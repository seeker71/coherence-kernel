# speech-native-neural-pair-window-0035

## What moved

The background pair lane trained the next native neural micro-pair:
`fr<->la`, meaning `303` (`aham-asmi`). This adds one reciprocal pair window
and two directed routes, moving the neural pair floor from `34` to `35` and the
directed neural route floor from `68` to `70`.

This is a closed pair-window receipt. It does not promote open ASR, Sema live
voice, or full TTS authority.

## Local oracle probe

Fresh local Mac loopback evidence was rendered under `/tmp` and not committed:

- French: `say -v Thomas "Je suis."` -> `ffmpeg` 16 kHz mono -> `whisper-cli -l fr`
  heard `Je suis`, wav bytes `17908`, cksum `3224208913`.
- Latin baseline carrier: `say -v Daniel "Ego sum."` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l la` heard `Ego sum.`, wav bytes `25994`, cksum `163923376`.

The Latin row is a baseline phrase carried by a local host voice. It does not
claim this macOS host exposes a native Latin TTS voice.

Total observed wav bytes for this window: `43902`.

## Native receipt

`learn/speech-native-neural-pair-window-0035.fk` records:

- native Form NL lanes: `fr->la`, `la->fr`, `fr->fr`, `la->la`
- native Form audio lanes: `fr->la`, `la->fr`, `fr->fr`, `la->la`
- local oracle pass: `2/2`
- Form NL rate: `100`
- Form audio rate: `100`
- native neural rate: `0 -> 100`
- cumulative neural pairs: `35`
- cumulative directed neural routes: `70`
- epochs/params: `35/35`
- open authority: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0035.fk \
    learn/tests/speech-native-neural-pair-window-0035-band.fk
  printf '\n(speech-native-neural-pair-window-0035-band)\n' ) > /tmp/snpw35.fk
./fkwu --src /tmp/snpw35.fk
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

The model floor is now nonzero and wider: `35` native neural micro-pairs,
`70` directed routes, and `35` admitted native neural parameters. The global
live authority floor remains unchanged: open dictation is still native `0/4`,
and Sema live voice is still native `0/1`.
