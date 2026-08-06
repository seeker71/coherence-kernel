# Speech open-ASR trial window 0002

This receipt adds the second scoped open-ASR challenger window on top of
`learn/speech-open-asr-trial-window.fk`.

The global backlog is unchanged: live open dictation is still local-oracle
`4/4`, native `0/4`, route `oracle-guide`. The new cell only expands the scoped
native segmented source learner so the trial window has a larger nonzero
denominator before any global authority claim.

Measured trial-window result:

- Added rows: Spanish `soy`, French `je suis`.
- Baseline meaning: `303`, Sanskrit key `aham-asmi`.
- Window 0002 rows: local oracle `2/2`, native `2/2`.
- Cumulative scoped open-ASR trial: local oracle `4/4`, native `4/4`.
- Native scoped rate: `100`, over the `50` floor.
- Action: `cut-challenger-for-trial-window`.
- Boundary: `trial-window-native-not-global-authority`.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/tests/speech-open-asr-trial-window-0002-band.fk > /tmp/speech-open-asr-trial-window-0002.fk
./fkwu --src /tmp/speech-open-asr-trial-window-0002.fk
```

Result: `32767`.

Also re-witnessed:

- `speech-open-asr-tts-target-model`: `32767`
- `speech-current-status-ledger`: `32767`
- `speech-model-metrics-report`: `32767`

The Sema voice lane remains nonzero through `sema-voice-authority-floor`: live
authority is `0/1`, scoped training is `1/1`, combined training floor is `1/2`.
It is not `0/0`.
