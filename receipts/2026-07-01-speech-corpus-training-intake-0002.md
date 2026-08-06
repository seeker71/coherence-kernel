# 2026-07-01 -- Speech corpus training intake 0002

## What changed

`learn/speech-corpus-training-intake-0002.fk` admits the twenty-four clean rows
from `speech-corpus-capture-batch-0002` into the native prototype-training
floor, on top of intake `0001`.

The aggregate training intake now reads:

```text
batch-0002 captured source rows: 24
batch-0002 local-oracle accepted: 24/24 = 100%
batch-0002 training-admitted rows: 24/24 = 100%
cumulative training-admitted rows: 30/30 = 100%
locales: en, de, es, fr, id, pt-br
max WER: 0
batch observed wav bytes: 580710
cumulative source wav bytes: 793234
data-sufficient floor: 12000 wav rows
data sufficient: false
global ASR/TTS authority: false
```

## Witness

```sh
cat learn/speech-corpus-training-intake-0001.fk \
    learn/speech-corpus-training-intake-0002.fk \
    learn/tests/speech-corpus-training-intake-0002-band.fk > /tmp/speech-corpus-training-intake-0002.fk
./fkwu --src /tmp/speech-corpus-training-intake-0002.fk
```

Result: `32767`.

The aggregate metrics report now derives `rows-used-for-training` from intake
`0002`:

```sh
cat learn/speech-corpus-training-intake-0001.fk \
    learn/speech-corpus-training-intake-0002.fk \
    learn/speech-model-metrics-report.fk \
    learn/tests/speech-model-metrics-report-band.fk > /tmp/speech-model-metrics-report.fk
./fkwu --src /tmp/speech-model-metrics-report.fk
```

Result: `32767`.

## Boundary

This expands the training intake from `6` to `30` rows. It is still far below
the `12000` wav-row corpus floor and does not promote global open ASR/TTS
authority.
