# Sema voice trial window 0002

This receipt adds a second scoped Sema voice TCAV candidate on top of
`learn/sema-voice-trial-window.fk`.

The global live Sema voice row is unchanged: local oracle `0/1`, native `0/1`,
WER `100`, route `oracle-guide`. The new row expands only the scoped TTS
training floor.

Measured trial-window result:

- Added candidate: `tcav-truth-soft-cadence-v2-window`.
- Phrase: `Truth alone triumphs.`
- Window 0002 row: local oracle `1/1`, native `1/1`, WER `0`.
- Cumulative scoped Sema voice trial: local oracle `2/2`, native `2/2`.
- Quality: F0 `165`, warmth `82`, cadence `64`, breath `18`.
- Action: `cut-tcav-challenger-for-trial-window`.
- Boundary: `trial-window-native-not-global-live-voice`.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/sema-voice-trial-window.fk \
    learn/sema-voice-trial-window-0002.fk \
    learn/tests/sema-voice-trial-window-0002-band.fk > /tmp/sema-voice-trial-window-0002.fk
./fkwu --src /tmp/sema-voice-trial-window-0002.fk
```

Result: `32767`.

The Sema voice authority floor is now live `0/1` plus scoped `2/2`, giving a
combined nonzero training floor of oracle/native `2/3 = 66%` while global live
authority remains held.
