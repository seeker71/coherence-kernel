# Speech authority model selection

Added an executable selector that joins the current speech model metrics to the
global live authority update.

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/speech-live-receipt-intake.fk \
    learn/speech-global-authority-update.fk \
    learn/speech-authority-model-selection.fk \
    learn/tests/speech-authority-model-selection-band.fk > /tmp/speech-authority-model-selection.fk
./fkwu --src /tmp/speech-authority-model-selection.fk
```

Result:

```text
32767
```

Current selection:

- ASR: `prototype-asr`, authority `oracle-guide`.
- TTS: `sema-voice-sample-loop`, authority `oracle-guide`.
- NL2NL: `closed-set-locale-form`, authority `native-form-anchor`.
- Audio2Audio: `native-source-window-audio2audio-acoustic`, authority
  `metal-scoped-native`.
- Global speech native authority: `0/2`.
- Missing real live receipts: `6`.
- Native neural parameters admitted: `0`.

Demo clean receipt selection:

- ASR: `native-open-asr-source`.
- TTS: `native-sema-voice`.
- Global speech native authority: `2/2`.
- Missing real live receipts: `0`.

Boundary: this does not claim the live Sema voice is native today. It makes the
claim executable: current real live receipt input still keeps speech on
`oracle-guide`; three clean local/consented/audio-present receipts per speech
lane move ASR/TTS to native routes.
