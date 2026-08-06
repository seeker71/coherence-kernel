# speech-native-neural-pair-window-0036

## What moved

The background pair scheduler after window `0035` pointed at `fr<->de`, but
`de<->fr` already exists in `speech-native-neural-pair-window-0017`. This patch
corrects that duplicate target and trains the next uncovered native neural
micro-pair: `fr<->it`, meaning `303` (`aham-asmi`).

This adds one reciprocal pair window and two directed routes, moving the neural
pair floor from `35` to `36` and the directed neural route floor from `70` to
`72`. It does not promote open ASR, Sema live voice, or full TTS authority.

## Local oracle probe

Fresh local Mac loopback evidence was rendered under `/tmp` and not committed:

- French: `say -v Thomas "Je suis."` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l fr` heard `Je suis`, wav bytes `17908`, cksum
  `3224208913`.
- Italian: `say -v Alice "Io sono."` -> `ffmpeg` 16 kHz mono ->
  `whisper-cli -l it` heard `Io sono`, wav bytes `16706`, cksum
  `3343634846`.

Total observed wav bytes for this window: `34614`.

## Native receipt

`learn/speech-native-neural-pair-window-0036.fk` records:

- native Form NL lanes: `fr->it`, `it->fr`, `fr->fr`, `it->it`
- native Form audio lanes: `fr->it`, `it->fr`, `fr->fr`, `it->it`
- local oracle pass: `2/2`
- Form NL rate: `100`
- Form audio rate: `100`
- native neural rate: `0 -> 100`
- cumulative neural pairs: `36`
- cumulative directed neural routes: `72`
- epochs/params: `36/36`
- open authority: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0036.fk \
    learn/tests/speech-native-neural-pair-window-0036-band.fk
  printf '\n(speech-native-neural-pair-window-0036-band)\n' ) > /tmp/snpw36.fk
./fkwu --src /tmp/snpw36.fk
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

The model floor is now wider: `36` native neural micro-pairs, `72` directed
routes, and `36` admitted native neural parameters. The global live authority
floor remains unchanged: open dictation is still native `0/4`, and Sema live
voice is still native `0/1`.
