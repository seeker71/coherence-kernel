# Speech Token Training Source Receipt

Date: 2026-06-30

Speech-token training now has a Form-native provenance policy. Training rows
separate local oracle labels, consentful corpus labels, and internal-state
inference so the body can learn special tokens without confusing where truth
came from.

## Sources

- Local oracle: local ASR over measured audio can label transcript words,
  confidence, WER grade, audio provenance, and receipt tokens.
- Consentful corpus: translated text rows can label words, locale scope,
  source, and route tokens.
- Internal state: runtime state can infer confidence, warmth, cadence,
  hesitation, excitement, attunement, controls, evidence, memory, and scope.
  It is metadata-only and cannot claim transcript truth.

## Witness

```sh
cat observe/speech-token-stream.fk \
    learn/speech-token-training-source.fk \
    learn/tests/speech-token-training-source-band.fk \
  > /tmp/speech-token-training-source.fk
./fkwu --src /tmp/speech-token-training-source.fk
```

Observed:

```text
32767
```
