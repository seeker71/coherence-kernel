# Speech native neural pair window 0010

This trains the next native neural micro-pair selected by the pair-training
action: `zh<->ar`.

Window:

```text
pair: zh<->ar
meaning: 302 / satyam-eva-jayate
trained unordered neural pairs: 9 -> 10
trained directed neural routes: 18 -> 20
epochs: 10
native neural parameters: 10
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
    learn/speech-native-neural-pair-window-0010.fk \
    learn/tests/speech-native-neural-pair-window-0010-band.fk > /tmp/speech-native-neural-pair-window-0010.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0010.fk
```

```text
32767
```
