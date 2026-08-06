# Speech model metrics report

Date: 2026-06-30

This receipt adds an executable aggregate report for the current native speech
stack. It records model size/composition, success rates, voice quality, and
native-vs-local-oracle rates without loading every speech component into one
source-runner file.

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/tests/speech-model-metrics-report-band.fk > /tmp/speech-model-metrics-report.fk
./fkwu --src /tmp/speech-model-metrics-report.fk
# 32767
```

Current report:

- Native neural weight parameters admitted: `0`.
- Native Sema voice organs/components present: `6`.
- Selected arms: ASR `prototype-asr`, TTS `sema-voice-sample-loop`, NL2NL
  `closed-set-locale-form`, audio2audio
  `native-source-window-audio2audio-acoustic`.
- Closed-prompt Metal anchors: `7/7 = 100%`.
- Live `en<->zh` anchor: `10/12 = 83%`.
- Live `en<->ar` anchor: `12/12 = 100%`.
- Live open dictation: local oracle `4/4 = 100%`, native `0/4 = 0%`.
- Sema live voice: live-native pass `0/1 = 0%`, WER `100`, route `oracle-guide`; native voice machinery is present but the live rendered sample has not passed.
- Voice target: f0 `165`, warmth `82`, cadence `64`, steadiness `76`,
  breath `18`; current live formant voice is not intelligible enough yet.
