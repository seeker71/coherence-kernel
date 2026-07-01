# Speech audio NL2NL multi-key bridge

This receipt expands the audio NL2NL bridge beyond `common.no`. The live loop
renders source audio, asks the local Whisper/Metal oracle for the source text,
routes through native Form neutral keys, renders the target text as target audio,
and checks that target audio with the local oracle.

Routes:

```text
nav.search: de -> es, es -> de
nav.vision: en -> fr, fr -> en, id -> pt-br, pt-br -> id
source oracle: 6/6
target oracle: 6/6
native neutral routing: 6/6
observed bridge wav bytes: 305184
```

Aggregate after this receipt:

```text
live wav rows: 71
observed wav bytes: 1916782
audio NL2NL bridge routes: 12
audio NL2NL bridge wav bytes: 548532
native vocoder: 0
native neural parameters: 0
data-sufficient training: false
data floor: 12000 wavs, 1200 held-out rows, 1000 cross-phrase rows, 300 cross-voice rows
status: tiny-corpus-not-data-sufficient-training
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-audio-nl2nl-multikey-bridge.fk \
    learn/tests/speech-audio-nl2nl-multikey-bridge-band.fk > /tmp/speech-audio-nl2nl-multikey-bridge.fk
./fkwu --src /tmp/speech-audio-nl2nl-multikey-bridge.fk
```

```text
8191
```

Meaning: this is a real end-to-end audio bridge receipt over multiple neutral
keys, but it is not a trained native voice model. The current 71 wavs are
observation and routing evidence only; the next honest movement is consentful
corpus-scale capture and held-out/cross-phrase/cross-voice measurement before
any model-training claim.
