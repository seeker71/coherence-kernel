# 2026-07-01 -- Speech corpus training intake 0003

## What changed

`learn/speech-corpus-training-intake-0003.fk` admits the thirty-four screened,
local-oracle-clean rows from `speech-corpus-capture-batch-0003` into the native
prototype-training floor, on top of intakes `0001` and `0002`.

The aggregate training intake now reads:

```text
batch-0003 screened candidate rows: 60
batch-0003 captured source rows: 34
batch-0003 local-oracle accepted: 34/34 = 100%
batch-0003 training-admitted rows: 34/34 = 100%
cumulative training-admitted rows: 64/64 = 100%
locales: en, de, es, fr, id, pt-br
max WER: 25
batch observed wav bytes: 1272388
cumulative source wav bytes: 2065622
data-sufficient floor: 12000 wav rows
data sufficient: false
global ASR/TTS/Sema voice authority: false
```

## Witness

```sh
cat learn/speech-corpus-training-intake-0001.fk \
    learn/speech-corpus-training-intake-0002.fk \
    learn/speech-corpus-training-intake-0003.fk \
    learn/tests/speech-corpus-training-intake-0003-band.fk > /tmp/speech-corpus-training-intake-0003.fk
./fkwu --src /tmp/speech-corpus-training-intake-0003.fk
```

Result: `32767`.

The aggregate metrics report now derives `rows-used-for-training` from intake
`0003`:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/sema-voice-trial-window.fk \
    learn/sema-voice-authority-floor.fk \
    learn/speech-corpus-training-intake-0001.fk \
    learn/speech-corpus-training-intake-0002.fk \
    learn/speech-corpus-training-intake-0003.fk \
    learn/speech-model-metrics-report.fk \
    learn/tests/speech-model-metrics-report-band.fk > /tmp/speech-model-metrics-report.fk
./fkwu --src /tmp/speech-model-metrics-report.fk
```

Result: `32767`.

## Boundary

This expands the training intake from `30` to `64` rows. It is still far below
the `12000` wav-row corpus floor and does not promote global open ASR/TTS or
Sema live voice authority. The live Sema voice lane remains oracle-guided while
the nonzero scoped training floor prevents the lane from collapsing to `0/0`.
