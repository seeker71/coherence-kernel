# Speech corpus capture batch 0002

This receipt captures the second real audio batch from the consentful
Coherence Network self-corpus: twenty-four rows, four per ready locale.

Observed live metrics:

```text
captured rows: 24
local-oracle accepted rows: 24
max WER: 0
wav bytes: 580710
locales: 6
rows used for training: 0
status: captured-corpus-audio-not-training-sufficient
```

Aggregate after this batch:

```text
live wav rows: 35
observed wav bytes: 1065282
captured corpus rows: 30
required live wav floor: 300
remaining wavs before floor: 265
neural training epochs: 0
corpus rows used for training: 0
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/coherence-network-self-corpus.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-acquisition-window.fk \
    learn/speech-corpus-capture-batch-0002.fk \
    learn/tests/speech-corpus-capture-batch-0002-band.fk > /tmp/speech-corpus-capture-batch-0002.fk
./fkwu --src /tmp/speech-corpus-capture-batch-0002.fk
```

```text
8191
```

Live witness on Apple M4 Max Metal:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/coherence-network-self-corpus.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-acquisition-window.fk \
    learn/speech-corpus-capture-batch-0002.fk > /tmp/sccb2-live.fk
printf '\n(sccb2-run-verdict)\n' >> /tmp/sccb2-live.fk
./fkwu --src /tmp/sccb2-live.fk
```

```text
8191
```

Boundary: this improves observed coverage from 11 to 35 live wav rows. It is
still not data-sufficient, neural, cross-phrase, cross-voice, or global
speech authority.
