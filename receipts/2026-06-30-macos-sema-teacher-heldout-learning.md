# macOS Sema teacher held-out learning

This receipt moves the teacher learner past exact-wav replay. It trains native
Form acoustic prototypes on one local teacher render and decodes a separately
rendered same-text eval wav.

Path:

- `say` renders `Open speech flows.` with `Flo (English (US))` to
  `teacher-984711.wav`.
- `say` renders the same phrase again and `ffmpeg` writes `teacher-984712.wav`
  with `volume=0.98`, giving a distinct eval wav while preserving the phrase.
- `whisper.cpp-large-v3-turbo` on Apple Metal transcribes the eval wav.
- Form reads both wavs, trains prototypes from the train wav, and decodes the
  eval wav through the trained prototypes and CTC collapse.

Contract witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-sema-teacher-heldout-learning.fk \
    learn/tests/macos-sema-teacher-heldout-learning-band.fk > /tmp/macos-sema-teacher-heldout-learning.fk
./fkwu --src /tmp/macos-sema-teacher-heldout-learning.fk
```

Expected:

```text
65535
```

Live witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/macos-sema-teacher-heldout-learning.fk > /tmp/msth-live.fk
printf '\n(msth-run-verdict)\n' >> /tmp/msth-live.fk
./fkwu --src /tmp/msth-live.fk
```

Expected:

```text
65535
```

Observed data:

```text
train wav bytes: 54304
eval wav bytes: 54304
train/eval hashes differ: true
training feature rows: 3
eval feature rows: 3
prototype rows including blank: 4
held-out WER: 0
minimum held-out confidence: >= 80
eval transform: volume=0.98
effective epochs: 1
native neural parameters: 0
```

Boundary: this is a held-out repeat, not broad generalization. The voice and
phrase are the same; the wav file is separate. It is the next honest floor
between exact-wav replay and cross-phrase/cross-voice ASR.
