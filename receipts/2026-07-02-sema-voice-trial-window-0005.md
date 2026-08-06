# sema-voice-trial-window-0005

## What moved

This patch adds the fifth scoped Sema voice TCAV trial window. It strengthens
the TTS side of the loop without claiming global live Sema voice authority.

Fresh local loopback evidence was rendered under `/tmp` and not committed:

- `say -v Samantha "I am present."`
- `ffmpeg` normalized to 16 kHz mono wav
- `whisper-cli -l en` heard `I am present.`
- wav bytes `25976`
- cksum `1611993373`

I also tested `Sema is here.` first. Whisper heard `Seema is here.`, so that
candidate was not admitted as an exact Sema-name receipt.

## Result

- scoped Sema voice oracle: `4/4 -> 5/5`
- scoped Sema voice native: `4/4 -> 5/5`
- combined voice floor: oracle `6/6`, native `5/6`
- total scoped trials: `24/24 -> 25/25`
- live wav rows: `256 -> 257`
- observed wav bytes: `8297106 -> 8323082`
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
    learn/sema-voice-trial-window-0005.fk \
    learn/tests/sema-voice-trial-window-0005-band.fk
  printf '\n(sema-voice-trial-window-0005-band)\n' ) > /tmp/svtw5.fk
./fkwu --src /tmp/svtw5.fk
# 32767
```

Aggregate witnesses after the update:

- `sema-voice-authority-floor-band` -> `32767`
- `speech-open-asr-tts-target-model-band` -> `32767`
- `speech-pair-training-next-action-band` -> `32767`
- `speech-model-metrics-report-band` -> `32767`
- `speech-learning-data-sufficiency-band` -> `65535`
- `speech-current-status-ledger-band` -> `32767`
- `speech-authority-learning-priority-band` -> `32767`

## Honest boundary

The scoped voice floor improved, but live Sema voice did not yet promote:
global live authority remains oracle `1/1`, native `0/1`, WER `100`.
