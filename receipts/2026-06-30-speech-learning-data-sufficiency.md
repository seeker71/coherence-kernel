# Speech learning data sufficiency

This receipt corrects the tiny-data claim. The current live speech rows are
useful because they prove local observation, oracle comparison, Form-native
prototype insertion, held-out repeat decode, and receipt plumbing. They are not
enough data for real model training.

Current data:

```text
live wav rows: 71
observed wav bytes: 1916782
live locales: 6
held-out repeat rows: 7
cross-phrase held-out rows: 0
cross-voice held-out rows: 0
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

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/tests/speech-learning-data-sufficiency-band.fk > /tmp/speech-learning-data-sufficiency.fk
./fkwu --src /tmp/speech-learning-data-sufficiency.fk
```

```text
32767
```

Meaning: the band passes by proving the current corpus is under the floor and
the training status is `tiny-corpus-not-data-sufficient-training`. The next
movement is consentful corpus/audio expansion toward the 12000-row floor, not
treating 71 wavs as training sufficiency.
