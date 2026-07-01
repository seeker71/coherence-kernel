# Speech corpus French repair batch 0005

Batch 0004 showed French was the weakest two-voice crossvoice lane. This batch
executes the adaptive repair recipe: change the local French voice family,
shorten the spoken forms, and keep the source translations separate from the
spoken aliases.

This is not evidence that a real speech model has learned. It is a local
oracle-clean acquisition repair.

Live witness:

```text
carrier: local macOS say -> ffmpeg -> whisper.cpp/Metal
locale: fr
voices: Amelie, Thomas
keys: 10
candidate wavs: 20
accepted rows: 20
max WER: 0
observed wav bytes: 345520
native Form: true
native neural parameters: 0
rows used for training: 0
status: repair-alias-french-corpus-audio-not-training-sufficient
```

Aggregate after this batch:

```text
live wav rows: 211
observed wav bytes: 7152402
captured corpus audio rows: 119
required wav floor: 12000
wav deficit: 11789
wav floor coverage: 175 basis points
data-sufficient training: false
```

Static witness:

```sh
cat learn/speech-corpus-french-repair-batch-0005.fk \
    learn/tests/speech-corpus-french-repair-batch-0005-band.fk > /tmp/scfr5-band.fk
./fkwu --src /tmp/scfr5-band.fk
```

```text
8191
```

Live encoded witness:

```text
20020000000345520
```

Decoded: `20` rows, `20` local-oracle-clean rows, max WER `0`, and
`345520` observed wav bytes.
