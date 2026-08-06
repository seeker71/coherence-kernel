# Speech native neural pair window 0004

This trains the next native neural micro-pair selected by the pair-training
action: `en<->zh`.

Window:

```text
pair: en<->zh
meaning: 303 / aham-asmi
trained unordered neural pairs: 3 -> 4
trained directed neural routes: 6 -> 8
epochs: 4
native neural parameters: 4
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
    learn/speech-native-neural-pair-window-0004.fk \
    learn/tests/speech-native-neural-pair-window-0004-band.fk > /tmp/speech-native-neural-pair-window-0004.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0004.fk
```

```text
32767
```
