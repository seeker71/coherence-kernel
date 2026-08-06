# Speech open-ASR trial window 0003

This receipt adds a third scoped open-ASR challenger window on top of
`learn/speech-open-asr-trial-window-0002.fk`.

The global backlog is unchanged: live open dictation is still local-oracle
`4/4`, native `0/4`, route `oracle-guide`. The new cell widens the scoped
native segmented source learner with Arabic and Chinese locale lanes using
ASCII transliterations so the direct-source runner stays stable.

Measured trial-window result:

- Added rows: Arabic `ana`, Chinese `wo shi`.
- Baseline meaning: `303`, Sanskrit key `aham-asmi`.
- Window 0003 rows: local oracle `2/2`, native `2/2`.
- Cumulative scoped open-ASR trial: local oracle `6/6`, native `6/6`.
- Native scoped rate: `100`, over the `50` floor.
- Action: `cut-challenger-for-trial-window`.
- Boundary: `trial-window-native-not-global-authority`.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/speech-open-asr-trial-window-0003.fk \
    learn/tests/speech-open-asr-trial-window-0003-band.fk > /tmp/speech-open-asr-trial-window-0003.fk
./fkwu --src /tmp/speech-open-asr-trial-window-0003.fk
```

Result: `32767`.

The Sema voice lane remains nonzero through `sema-voice-authority-floor`: live
authority is `0/1`, scoped training is `1/1`, combined training floor is `1/2`.
