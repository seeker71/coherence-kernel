# macOS Chinese teacher acoustic learning

The Arabic learner named Chinese as a local-oracle miss because the longer
baseline phrases did not transcribe exactly. A shorter baseline line did pass:
`我在`, meaning `303`.

Path:

- `learn/macos-chinese-teacher-acoustic-learning.fk` renders `Eddy (Chinese (China mainland))` with `say`.
- `ffmpeg` normalizes the rendered audio to 16 kHz mono PCM.
- local `whisper.cpp-large-v3-turbo` on Apple Metal transcribes the wav with `-l zh`.
- `observe/wav-sense.fk` reads the generated wav bytes and extracts the envelope
  inside Form.
- Form trains Chinese acoustic token prototypes from wav-envelope segment rows.
- Form decodes the learned frame stream back to Chinese text with CTC-style collapse.

Contract witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-chinese-teacher-acoustic-learning.fk \
    learn/tests/macos-chinese-teacher-acoustic-learning-band.fk > /tmp/macos-chinese-teacher-acoustic-learning.fk
./fkwu --src /tmp/macos-chinese-teacher-acoustic-learning.fk
```

Result:

```text
16383
```

Live witness on this Mac:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-chinese-teacher-acoustic-learning.fk > /tmp/mztal-live.fk
printf '\n(mztal-run-verdict)\n' >> /tmp/mztal-live.fk
./fkwu --src /tmp/mztal-live.fk
```

```text
16383
```

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-chinese-teacher-acoustic-learning.fk > /tmp/mztal-live-wer.fk
printf '\n(mztal-run-wer)\n' >> /tmp/mztal-live-wer.fk
./fkwu --src /tmp/mztal-live-wer.fk
```

```text
0
```

Observed live metrics:

```text
locale: zh
baseline meaning: 303
local oracle WER: 0
native decoded WER: 0
prototype count: 3
minimum native confidence: 96
native neural parameters: 0
```

Voice probes before the patch:

```text
Eddy (Chinese China mainland): 我在 -> 我在
Flo (Chinese China mainland): 我在 -> 我在
Grandma (Chinese China mainland): 我在 -> 我在。
Meijia: 我在 -> 我在
```

Longer Chinese baseline phrases remain misses:

```text
truth: 真理 终将 胜利
oracle: 真理中加顺利。

truth: 愿 一切 众生 快乐
oracle: 祝您感谢众生快乐
```

Boundary: this admits the short Chinese line as live teacher-acoustic prototype
learning. It does not promote the longer Chinese rows, a neural ASR model, or
native Sema TTS authority.
