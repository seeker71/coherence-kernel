# Speech corpus capture batch 0001

This receipt captures the first real audio rows from the consentful Coherence
Network self-corpus. It renders six selected corpus rows with local macOS
voices, normalizes them to wav, transcribes them with local whisper.cpp on
Metal, and checks oracle WER.

Rows:

```text
en    common.tryAgain    Try again           Flo (English US)
de    common.tryAgain    Erneut versuchen    Flo (German Germany)
es    nav.search         Buscar              Flo (Spanish Spain)
fr    nav.vision         Vision              Amélie
id    common.tryAgain    Coba lagi           Damayanti
pt-br common.tryAgain    Tentar novamente    Flo (Portuguese Brazil)
```

Observed live metrics:

```text
captured rows: 6
local-oracle accepted rows: 6
max WER: 0
wav bytes: 212524
locales: 6
rows used for training: 0
status: captured-corpus-audio-not-training-sufficient
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
    learn/speech-corpus-capture-batch-0001.fk \
    learn/tests/speech-corpus-capture-batch-0001-band.fk > /tmp/speech-corpus-capture-batch-0001.fk
./fkwu --src /tmp/speech-corpus-capture-batch-0001.fk
```

```text
4095
```

Live witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/coherence-network-self-corpus.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-acquisition-window.fk \
    learn/speech-corpus-capture-batch-0001.fk > /tmp/sccb-live.fk
printf '\n(sccb-run-verdict)\n' >> /tmp/sccb-live.fk
./fkwu --src /tmp/sccb-live.fk
```

```text
4095
```

Boundary: these rows reduce the acquisition gap; they do not make the model
data-sufficient, neural, or globally authoritative.
