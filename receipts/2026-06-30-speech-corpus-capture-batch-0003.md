# Speech corpus capture batch 0003

This receipt admits the first screened phrase batch from the translated
Coherence Network self-corpus. A 60-row candidate probe was rendered locally with
macOS voices and checked with local whisper.cpp/Metal. The local oracle accepted
34 rows under the WER floor; the other 26 rows were rejected instead of counted
as training data.

Screening:

```text
candidate phrase rows: 60
admitted rows: 34
rejected rows: 26
accepted admitted rows: 34/34
max admitted WER: 25
observed admitted wav bytes: 1272388
native neural parameters: 0
rows used for training: 0
status: screened-phrase-corpus-audio-not-training-sufficient
```

Aggregate after this receipt:

```text
live wav rows: 105
observed wav bytes: 3189170
captured corpus audio rows: 64
captured corpus wav bytes: 2065622
data-sufficient training: false
data floor: 12000 wavs, 1200 held-out rows, 1000 cross-phrase rows, 300 cross-voice rows
status: tiny-corpus-not-data-sufficient-training
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
    learn/speech-corpus-capture-batch-0003.fk \
    learn/tests/speech-corpus-capture-batch-0003-band.fk > /tmp/speech-corpus-capture-batch-0003.fk
./fkwu --src /tmp/speech-corpus-capture-batch-0003.fk
```

```text
8191
```

Meaning: the corpus is growing, but the learning claim remains blocked by data
scale and generalization coverage. The important movement is not that every
candidate passed; it is that the local oracle rejected weak rows and only clean
rows entered the captured corpus count.
