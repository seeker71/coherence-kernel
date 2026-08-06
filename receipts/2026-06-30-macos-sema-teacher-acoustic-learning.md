# macOS Sema teacher acoustic learning

The previous carrier made local teacher audio repeatable. This receipt consumes
that audio as native learning input.

Path:

- `learn/macos-sema-teacher-acoustic-learning.fk` renders the local Flo voice with `say`.
- `ffmpeg` normalizes the rendered audio to 16 kHz mono PCM.
- local `whisper.cpp-large-v3-turbo` on Apple Metal transcribes the wav.
- `observe/wav-sense.fk` reads the generated wav bytes and extracts the envelope
  inside Form.
- Form trains acoustic token prototypes from wav-envelope segment rows.
- Form decodes the learned frame stream back to text with CTC-style collapse.

Contract witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/tests/macos-sema-teacher-acoustic-learning-band.fk > /tmp/macos-sema-teacher-acoustic-learning.fk
./fkwu --src /tmp/macos-sema-teacher-acoustic-learning.fk
```

Result:

```text
4095
```

Live run functions:

```text
mstal-run-verdict
mstal-run-wer
```

Live witness on this Mac:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk > /tmp/mstal-live.fk
printf '\n(mstal-run-verdict)\n' >> /tmp/mstal-live.fk
./fkwu --src /tmp/mstal-live.fk
```

```text
4095
```

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk > /tmp/mstal-live-wer.fk
printf '\n(mstal-run-wer)\n' >> /tmp/mstal-live-wer.fk
./fkwu --src /tmp/mstal-live-wer.fk
```

```text
0
```

Observed live metrics:

```text
local oracle WER: 0
native decoded WER: 0
prototype count: 4
minimum native confidence: 96
native neural parameters: 0
```

Boundary: this is actual native prototype learning from a real local wav. It is
not a neural ASR model and not native Sema TTS authority. Neural parameters
remain `0`.
