# 2026-07-01 -- Speech corpus training intake 0001

## What changed

`learn/speech-corpus-training-intake-0001.fk` admits the six clean rows from
`speech-corpus-capture-batch-0001` into the native prototype-training floor.

This corrects the aggregate corpus state from "captured rows exist but rows used
for training are still zero" to a nonzero floor:

```text
captured source rows: 6
local-oracle accepted: 6/6 = 100%
training-admitted rows: 6/6 = 100%
locales: en, de, es, fr, id, pt-br
max WER: 0
observed source wav bytes: 212524
data-sufficient floor: 12000 wav rows
data sufficient: false
global ASR/TTS authority: false
```

## Witness

```sh
cat learn/speech-corpus-training-intake-0001.fk \
    learn/tests/speech-corpus-training-intake-0001-band.fk > /tmp/speech-corpus-training-intake-0001.fk
./fkwu --src /tmp/speech-corpus-training-intake-0001.fk
```

Result: `32767`.

The aggregate metrics report now derives `rows-used-for-training` from that
intake:

```sh
cat learn/speech-corpus-training-intake-0001.fk \
    learn/speech-model-metrics-report.fk \
    learn/tests/speech-model-metrics-report-band.fk > /tmp/speech-model-metrics-report.fk
./fkwu --src /tmp/speech-model-metrics-report.fk
```

Result: `32767`.

## Boundary

This is training intake, not data-sufficient training and not global open
ASR/TTS authority. It moves the floor from zero to six while keeping the larger
`12000` wav-row corpus target intact.
