# 2026-07-01 -- Sema voice authority floor

## What changed

`learn/sema-voice-authority-floor.fk` now prevents Sema voice from being read as
native/oracle `0/0`.

The cell keeps two lanes separate:

- Live authority: local oracle `0/1`, native `0/1`, WER `100`, route `oracle-guide`.
- Scoped training floor: the TCAV trial rows contribute local oracle `2/2` and native
  `2/2`, WER `0`, but only inside the trial window.

The combined floor is therefore local oracle `2/3 = 66%` and native `2/3 = 66%`, while
global live authority remains held. A zero-denominator row now resolves to
`block-zero-denominator-voice-gate`.

## Witness

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/sema-voice-trial-window.fk \
    learn/sema-voice-trial-window-0002.fk \
    learn/sema-voice-authority-floor.fk \
    learn/tests/sema-voice-authority-floor-band.fk > /tmp/sema-voice-authority-floor.fk
./fkwu --src /tmp/sema-voice-authority-floor.fk
```

Result: `32767`.

Also re-witnessed:

- `speech-open-asr-tts-target-model`: `32767`
- `speech-model-metrics-report`: `32767`

## Boundary

This is not a native live Sema voice pass. It is the first nonzero Sema voice training
floor wired into the aggregate reports, with the next action set to
`render-next-live-sema-sample-and-oracle`.
