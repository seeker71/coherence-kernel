# Speech native neural pair window 0002

This trains the next native neural micro-pair selected by the pair-training
action: `en<->pt-br`.

Window:

```text
pair: en<->pt-br
meaning: 301 / sarve-bhavantu-sukhinah
trained unordered neural pairs: 1 -> 2
trained directed neural routes: 2 -> 4
epochs: 2
native neural parameters: 2
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
    learn/speech-native-neural-pair-window-0002.fk \
    learn/tests/speech-native-neural-pair-window-0002-band.fk > /tmp/speech-native-neural-pair-window-0002.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0002.fk
```

```text
32767
```
