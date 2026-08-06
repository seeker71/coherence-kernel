# speech-open-asr-trial-window-0010

## What moved

This patch adds the tenth scoped open-ASR source window. It follows the
authority-learning priority lane: measured global open ASR is still native
`0/4`, so new live source hearing evidence remains higher priority than
background pair drift for this step.

The window adds Finnish and Japanese local Mac loopback rows:

- `fi`, voice `Eddy (Finnish (Finland))`, truth `minä olen`, heard
  `Minä olen.`, wav bytes `30744`, cksum `2803940054`.
- `ja`, voice `Eddy (Japanese (Japan))`, truth `私はいます`, heard
  `私はいます。`, wav bytes `52762`, cksum `704718359`.

Total observed wav bytes for this window: `83506`.

The audio artifacts were temporary `/tmp` files only; no wav/aiff was committed.

## Local oracle commands

```sh
say -v "Eddy (Finnish (Finland))" -o /tmp/soat10-fi.aiff -- "Minä olen."
ffmpeg -hide_banner -loglevel error -y -i /tmp/soat10-fi.aiff -ar 16000 -ac 1 /tmp/soat10-fi.wav
cksum /tmp/soat10-fi.wav
wc -c /tmp/soat10-fi.wav
whisper-cli -m ~/.cache/whisper.cpp/ggml-large-v3-turbo.bin -f /tmp/soat10-fi.wav -l fi

say -v "Eddy (Japanese (Japan))" -o /tmp/soat10-ja.aiff -- "私はいます。"
ffmpeg -hide_banner -loglevel error -y -i /tmp/soat10-ja.aiff -ar 16000 -ac 1 /tmp/soat10-ja.wav
cksum /tmp/soat10-ja.wav
wc -c /tmp/soat10-ja.wav
whisper-cli -m ~/.cache/whisper.cpp/ggml-large-v3-turbo.bin -f /tmp/soat10-ja.wav -l ja
```

## Result

- scoped open-ASR rows: `18 -> 20`
- scoped open-ASR oracle: `20/20`
- scoped open-ASR native: `20/20`
- total scoped trials: `22 -> 24`
- live wav rows: `254 -> 256`
- observed wav bytes: `8213600 -> 8297106`
- global live open dictation remains native `0/4`
- global live Sema voice remains native `0/1`
- C seed growth: `0`

## Witnesses

```sh
( cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/speech-open-asr-trial-window-0003.fk \
    learn/speech-open-asr-trial-window-0004.fk \
    learn/speech-open-asr-trial-window-0005.fk \
    learn/speech-open-asr-trial-window-0006.fk \
    learn/speech-open-asr-trial-window-0007.fk \
    learn/speech-open-asr-trial-window-0008.fk \
    learn/speech-open-asr-trial-window-0009.fk \
    learn/speech-open-asr-trial-window-0010.fk \
    learn/tests/speech-open-asr-trial-window-0010-band.fk
  printf '\n(speech-open-asr-trial-window-0010-band)\n' ) > /tmp/soat10.fk
./fkwu --src /tmp/soat10.fk
# 32767
```

Aggregate witnesses after the update:

- `speech-pair-training-next-action-band` -> `32767`
- `speech-model-metrics-report-band` -> `32767`
- `speech-learning-data-sufficiency-band` -> `65535`
- `speech-current-status-ledger-band` -> `32767`

## Honest boundary

This is a scoped source-window success, not global promotion. Full open ASR/TTS
authority remains held by the local oracle until live native receipts pass the
global gate.
