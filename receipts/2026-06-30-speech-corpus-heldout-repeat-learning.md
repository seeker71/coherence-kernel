# Speech corpus held-out repeat learning

This receipt moves beyond captured-row counting. It trains six Form-native
full-envelope acoustic prototypes from consentful Coherence Network corpus
phrases, then evaluates six separately rendered, volume-shifted wavs of the
same phrases against all six prototypes.

Observed live metrics:

```text
train wavs: 6
eval wavs: 6
locales: en, de, es, fr, id, pt-br
local-oracle accepted evals: 6/6
native prototype accepted evals: 6/6
train/eval hashes differ: 6/6
observed wav bytes: 302968
native neural parameters: 0
status: scoped-heldout-repeat-not-global-authority
```

Aggregate after this run:

```text
live wav rows: 47
observed wav bytes: 1368250
held-out repeat rows: 7
cross-phrase held-out rows: 0
cross-voice held-out rows: 0
data-sufficient training: false
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-heldout-repeat-learning.fk \
    learn/tests/speech-corpus-heldout-repeat-learning-band.fk > /tmp/speech-corpus-heldout-repeat-learning.fk
./fkwu --src /tmp/speech-corpus-heldout-repeat-learning.fk
```

```text
16383
```

Live witness on Apple M4 Max Metal:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-heldout-repeat-learning.fk > /tmp/schr-live.fk
printf '\n(schr-run-verdict)\n' >> /tmp/schr-live.fk
./fkwu --src /tmp/schr-live.fk
```

```text
16383
```

Boundary: this is real Form-native learning from local audio, but it is still
same-phrase held-out repeat evidence. It does not prove cross-phrase,
cross-voice, neural, or global ASR/TTS authority.
