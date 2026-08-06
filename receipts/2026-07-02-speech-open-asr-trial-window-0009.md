# speech-open-asr-trial-window-0009

## What moved

This patch adds the ninth scoped open-ASR source window. It follows the
authority-learning priority lane: measured global open ASR is still native
`0/4`, so new live source hearing evidence outranks another background pair
increment for this step.

The window adds French and Brazilian Portuguese local Mac loopback rows:

- `fr`, voice `Thomas`, truth `je suis`, heard `Je suis`, wav bytes `17908`,
  cksum `3224208913`.
- `pt-br`, voice `Luciana`, truth `eu sou`, heard `Eu sou.`, wav bytes `19708`,
  cksum `2995895654`.

Total observed wav bytes for this window: `37616`.

## Result

- scoped open-ASR rows: `16 -> 18`
- scoped open-ASR oracle: `18/18`
- scoped open-ASR native: `18/18`
- total scoped trials: `19 -> 21`
- live wav rows: `251 -> 253`
- observed wav bytes: `8155588 -> 8193204`
- global live open dictation remains native `0/4`
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
    learn/tests/speech-open-asr-trial-window-0009-band.fk
  printf '\n(speech-open-asr-trial-window-0009-band)\n' ) > /tmp/soat9.fk
./fkwu --src /tmp/soat9.fk
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
