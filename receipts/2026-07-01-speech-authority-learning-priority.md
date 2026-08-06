# Speech authority learning priority

This receipt prevents neural pair coverage from becoming drift away from the
full speech goal. Current pair coverage is real, but open ASR and Sema voice
still have measured authority gaps.

Current state:

```text
trained neural pairs: 20/55
directed neural routes: 40/110
native neural parameters: 20
background pair window: 0021 es<->id
open ASR gap: 100
Sema voice gap: 100
Sema voice native/oracle: 0/1 / 1/1
```

Selected authority action:

```text
trial: trial-open-asr-0001
gap: live-open-dictation
action: train-live-segmented-open-asr-source
target: native-segmented-acoustic-learning
route: oracle-guide
promotion: native-rate>=50-and-wer<=25-clean-controls
control: choice-cut-fail-undo-timeout
```

Witness:

```sh
cat learn/speech-neural-pair-coverage.fk \
    learn/speech-pair-training-next-action.fk \
    learn/speech-open-asr-tts-target-model.fk \
    learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-authority-learning-priority.fk \
    learn/tests/speech-authority-learning-priority-band.fk > /tmp/speech-authority-learning-priority.fk
./fkwu --src /tmp/speech-authority-learning-priority.fk
```

```text
32767
```

Boundary: this selects the next authority-learning action. It does not claim
native open ASR or Sema voice authority; both still wait for passing receipts.
