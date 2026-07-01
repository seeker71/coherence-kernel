# Speech native neural pair window 0001

This makes trained neural pair coverage non-zero with an executable Form-native
micro-kernel receipt.

Window:

```text
pair: en<->fr
trained unordered neural pairs: 1
trained directed neural routes: 2
epochs: 1
native neural parameters: 1
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
    learn/speech-form-pair-window-0008.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0001.fk \
    learn/tests/speech-native-neural-pair-window-0001-band.fk > /tmp/speech-native-neural-pair-window-0001.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0001.fk
```

```text
32767
```
