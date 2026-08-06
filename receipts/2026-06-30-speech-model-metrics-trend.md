# Speech model metrics trend

Date: 2026-06-30

This receipt adds the before/after movement that the snapshot metrics report did
not show explicitly.

Witness:

```sh
cat learn/speech-model-metrics-trend.fk \
    learn/tests/speech-model-metrics-trend-band.fk > /tmp/speech-model-metrics-trend.fk
./fkwu --src /tmp/speech-model-metrics-trend.fk
# 32767
```

Trend report:

- Mac Metal reciprocal audio: native `0 -> 83`, route `oracle-guide -> native`.
- Multiseed NL/audio: native `0 -> 100`, route `oracle-guide -> native`.
- Live open dictation: local oracle `100`, native `0`, route `oracle-guide`.
- Sema live voice: native `0`, WER `100`, route `oracle-guide`.
- Aggregate: `4` rows, `2` shifted native, `2` oracle-held, average native rate `45`.
