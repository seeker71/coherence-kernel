# sema-voice-trial-window-0004

## What moved

This patch adds the fourth scoped Sema voice TCAV trial window. It strengthens
the TTS side of the loop without claiming global live Sema voice authority.

Fresh local loopback evidence was rendered under `/tmp` and not committed:

- `say -v Samantha "I am here."`
- `ffmpeg` normalized to 16 kHz mono wav
- `whisper-cli -l en` heard `I am here.`
- wav bytes `20396`
- cksum `3566916401`

## Result

- scoped Sema voice oracle: `3/3 -> 4/4`
- scoped Sema voice native: `3/3 -> 4/4`
- combined voice floor: oracle `5/5`, native `4/5`
- total scoped trials: `21/21 -> 22/22`
- live wav rows: `253 -> 254`
- observed wav bytes: `8193204 -> 8213600`
- global live Sema voice remains native `0/1`
- C seed growth: `0`

## Witnesses

```sh
( cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/sema-voice-trial-window.fk \
    learn/sema-voice-trial-window-0002.fk \
    learn/sema-voice-trial-window-0003.fk \
    learn/sema-voice-trial-window-0004.fk \
    learn/tests/sema-voice-trial-window-0004-band.fk
  printf '\n(sema-voice-trial-window-0004-band)\n' ) > /tmp/svtw4.fk
./fkwu --src /tmp/svtw4.fk
# 32767
```

Aggregate witnesses after the update:

- `sema-voice-authority-floor-band` -> `32767`
- `speech-open-asr-tts-target-model-band` -> `32767`
- `speech-pair-training-next-action-band` -> `32767`
- `speech-model-metrics-report-band` -> `32767`
- `speech-learning-data-sufficiency-band` -> `65535`
- `speech-current-status-ledger-band` -> `32767`

## Honest boundary

The scoped voice floor improved, but live Sema voice did not yet promote:
global live authority remains oracle `1/1`, native `0/1`, WER `100`.
