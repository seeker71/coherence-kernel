# Speech corpus cross-phrase learning

This receipt moves the speech learner beyond held-out repeat. Each row trains a
same-locale prototype from one captured corpus phrase, evaluates a different
phrase from the same locale/voice, and compares it with an explicit
different-locale control. Native Form passes only when the eval phrase is closer
to the same-locale different-phrase prototype than to the control, while the
local Whisper/Metal oracle accepts the eval transcript.

Live result:

```text
controlled cross-phrase rows: 6
local oracle accepted: 6/6
native same-locale over control: 6/6
max WER: 25
observed wav bytes: 712626
native neural parameters: 0
global authority: false
```

Aggregate after this receipt:

```text
live wav rows: 123
observed wav bytes: 3901796
cross-phrase rows: 6 / 1000 floor
cross-voice rows: 0 / 300 floor
data-sufficient training: false
status: tiny-corpus-not-data-sufficient-training
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-crossphrase-learning.fk \
    learn/tests/speech-corpus-crossphrase-learning-band.fk > /tmp/speech-corpus-crossphrase-learning.fk
./fkwu --src /tmp/speech-corpus-crossphrase-learning.fk
```

```text
65535
```

Meaning: this is real movement toward learning because it tests different
phrases, not repeat audio. It is still scoped and controlled; it does not prove
open ASR, cross-voice generalization, native TTS, or a trained neural model.
