# macOS Arabic teacher acoustic learning

The English teacher learner proved the local host voice can become native
prototype training input. This receipt extends that live path to Arabic.

Path:

- `learn/macos-arabic-teacher-acoustic-learning.fk` renders `Majed` with `say`.
- `ffmpeg` normalizes the rendered audio to 16 kHz mono PCM.
- local `whisper.cpp-large-v3-turbo` on Apple Metal transcribes the wav with `-l ar`.
- `observe/wav-sense.fk` reads the generated wav bytes and extracts the envelope
  inside Form.
- Form trains Arabic acoustic token prototypes from wav-envelope segment rows.
- Form decodes the learned frame stream back to Arabic text with CTC-style collapse.

Contract witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-arabic-teacher-acoustic-learning.fk \
    learn/tests/macos-arabic-teacher-acoustic-learning-band.fk > /tmp/macos-arabic-teacher-acoustic-learning.fk
./fkwu --src /tmp/macos-arabic-teacher-acoustic-learning.fk
```

Result:

```text
16383
```

Live witness on this Mac, run sequentially because the receipt uses a stable
audio id:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-arabic-teacher-acoustic-learning.fk > /tmp/matal-live.fk
printf '\n(matal-run-verdict)\n' >> /tmp/matal-live.fk
./fkwu --src /tmp/matal-live.fk
```

```text
16383
```

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-arabic-teacher-acoustic-learning.fk > /tmp/matal-live-wer.fk
printf '\n(matal-run-wer)\n' >> /tmp/matal-live-wer.fk
./fkwu --src /tmp/matal-live-wer.fk
```

```text
0
```

Observed live metrics:

```text
locale: ar
baseline meaning: 302
local oracle WER: 0
native decoded WER: 0
prototype count: 4
minimum native confidence: 96
native neural parameters: 0
```

Chinese boundary observed before this patch:

```text
truth: 真理 终将 胜利
oracle: 真理中加顺利。

truth: 愿 一切 众生 快乐
oracle: 祝您感谢众生快乐
```

That means Arabic is admitted as a live diverse teacher-acoustic pass now, while
Chinese remains a named local-oracle miss for the next A/B learner. Boundary:
this is actual native prototype learning from a real local Arabic wav. It is
not a neural ASR model and not native Sema TTS authority.
