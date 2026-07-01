# Speech learning data sufficiency

This receipt corrects the tiny-data claim. The current live speech rows are
useful because they prove local observation, oracle comparison, Form-native
prototype insertion, held-out repeat decode, and receipt plumbing. They are not
enough data for real model training.

Current data:

```text
live wav rows: 191
observed wav bytes: 6806882
live locales: 6
held-out repeat rows: 7
cross-phrase held-out rows: 6
cross-voice held-out rows: 6
native neural parameters: 0
```

Training floor before calling the speech learner data-sufficient:

```text
live teacher wavs: 12000
live teacher locales: 6
held-out rows: 1200
cross-phrase held-out rows: 1000
cross-voice held-out rows: 300
```

Scale gap:

```text
wav deficit: 11809
wav floor coverage: 159 basis points
held-out deficit: 1193
cross-phrase deficit: 994
cross-voice deficit: 294
promotion: false
meaning: larger instrumentation corpus, still not model learning
```

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/tests/speech-learning-data-sufficiency-band.fk > /tmp/speech-learning-data-sufficiency.fk
./fkwu --src /tmp/speech-learning-data-sufficiency.fk
```

```text
65535
```

Meaning: the band passes by proving the current corpus is under the floor and
the training status is `tiny-corpus-not-data-sufficient-training`. At 159 basis
points of the wav floor, this is too small to train a real native speech model.
The next movement is consentful corpus/audio expansion toward the 12000-row
floor and cross-voice diversity, not treating 191 wavs as training sufficiency.
