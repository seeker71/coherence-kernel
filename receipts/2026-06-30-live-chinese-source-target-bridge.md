# Live Chinese source-to-target acoustic bridge

The teacher-acoustic learners proved live source ASR for English, Arabic, and a
short Chinese line. This receipt connects the Chinese source receipt to a target
locale through the Sanskrit baseline and emits target acoustic data in Form.

Path:

- `learn/macos-chinese-teacher-acoustic-learning.fk` renders `我在`, transcribes
  it with local `whisper.cpp-large-v3-turbo` on Apple Metal, and decodes it
  natively with WER `0`.
- `learn/sanskrit-locale-baseline.fk` maps meaning `303` to English target
  tokens.
- `learn/live-chinese-source-target-bridge.fk` emits compact target acoustic
  frames from those target tokens.

Contract witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-chinese-teacher-acoustic-learning.fk \
    learn/live-chinese-source-target-bridge.fk \
    learn/tests/live-chinese-source-target-bridge-band.fk > /tmp/live-chinese-source-target-bridge.fk
./fkwu --src /tmp/live-chinese-source-target-bridge.fk
```

Result:

```text
8191
```

Live witness on this Mac:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-chinese-teacher-acoustic-learning.fk \
    learn/live-chinese-source-target-bridge.fk > /tmp/lzst-live.fk
printf '\n(lzst-run-verdict)\n' >> /tmp/lzst-live.fk
./fkwu --src /tmp/lzst-live.fk
```

```text
8191
```

Observed live metrics:

```text
source locale: zh
target locale: en
neutral meaning: 303
source ASR WER: 0
target tokens: 2
target acoustic frames: 2
minimum source confidence: 96
native neural parameters: 0
route: native-source-target-acoustic
```

Learning audit:

```text
training kind: one-pass supervised prototype insertion upstream, deterministic target-frame emission in this bridge
live teacher utterances admitted: 3
observed teacher wav bytes on this Mac: 163440
feature rows: 9
nonblank learned token prototypes: 8
prototype rows including scoped blanks: 11
effective epochs per sample: 1
neural training epochs: 0
in-sample native teacher accuracy: 3/3 = 100%
held-out live samples: 0
held-out/generalized accuracy: unproven
```

Boundary: this is the first compact live source-audio to target-acoustic bridge
over the Chinese teacher receipt. It is not the full TCAV stack, not a neural
vocoder, and not global audio2audio authority. The full TCAV composition still
needs either a smaller shared lane or more direct-source capacity before it can
run beside the live Chinese learner.
