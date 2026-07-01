# Speech audio NL2NL bridge

This receipt witnesses six reciprocal locale routes:

```text
audio A -> local oracle transcript -> Form neutral key -> target text B -> target audio B -> local oracle check
```

Observed live metrics:

```text
routes: 6
pairs: en->de, de->en, es->pt-br, pt-br->es, fr->id, id->fr
neutral key: common.no
source-oracle accepted: 6/6
target-oracle accepted: 6/6
native neutral routing accepted: 6/6
observed wav bytes: 243348
native vocoder: 0
native neural parameters: 0
status: oracle-guided-audio-nl2nl-audio-bridge-not-native-vocoder
```

Aggregate after this run:

```text
live wav rows: 59
observed wav bytes: 1611598
audio NL2NL bridge routes: 6
data-sufficient training: false
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-audio-nl2nl-bridge.fk \
    learn/tests/speech-audio-nl2nl-bridge-band.fk > /tmp/speech-audio-nl2nl-bridge.fk
./fkwu --src /tmp/speech-audio-nl2nl-bridge.fk
```

```text
4095
```

Live witness on Apple M4 Max Metal:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-audio-nl2nl-bridge.fk > /tmp/sanb-live.fk
printf '\n(sanb-run-verdict)\n' >> /tmp/sanb-live.fk
./fkwu --src /tmp/sanb-live.fk
```

```text
4095
```

Boundary: this is a real reciprocal audio/NL2NL bridge, but it is still
oracle-guided. Host TTS renders target audio and local Whisper listens. Native
Form owns neutral routing and target text selection; native ASR/vocoder
authority remains pending.
