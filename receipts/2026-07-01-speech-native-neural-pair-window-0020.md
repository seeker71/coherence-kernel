# Speech native neural pair window 0020

This trains the next native neural micro-pair selected by the pair-training
action before this patch: `es<->fr`.

Window:

```text
pair: es<->fr
meaning: 301 / sarve-bhavantu-sukhinah
trained unordered neural pairs: 19 -> 20
trained directed neural routes: 38 -> 40
epochs: 20
native neural parameters: 20
```

Result:

```text
neural rate: 0 -> 100
Form NL rate: 100
Form audio rate: 100
bootstrap loss: 1 -> 0
open ASR/TTS authority: false
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0020.fk \
    learn/tests/speech-native-neural-pair-window-0020-band.fk > /tmp/speech-native-neural-pair-window-0020.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0020.fk
```

```text
32767
```
