# Speech corpus cross-voice learning

This receipt moves the speech learner beyond same-voice evidence. Each row
trains a same-text prototype from one local voice, evaluates the same text from a
different local voice, and compares it with a different-text control rendered in
the eval voice. Native Form passes only when the eval voice is closer to the
same-text different-voice prototype than to the control, while the local
Whisper/Metal oracle accepts the eval transcript.

Live result:

```text
controlled cross-voice rows: 6
local oracle accepted: 6/6
native same-text over control: 6/6
max WER: 25
observed wav bytes: 755060
native neural parameters: 0
global authority: false
```

Aggregate after this receipt:

```text
live wav rows: 141
observed wav bytes: 4656856
cross-phrase rows: 6 / 1000 floor
cross-voice rows: 6 / 300 floor
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
    learn/speech-corpus-crossvoice-learning.fk \
    learn/tests/speech-corpus-crossvoice-learning-band.fk > /tmp/speech-corpus-crossvoice-learning.fk
./fkwu --src /tmp/speech-corpus-crossvoice-learning.fk
```

```text
65535
```

Meaning: this is real movement toward voice robustness because it tests
different voices. It is still scoped and controlled; it does not prove open ASR,
native TTS, cross-device microphone authority, or a trained neural model.
