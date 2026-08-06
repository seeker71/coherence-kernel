# Speech native neural pair window 0008

This trains the next native neural micro-pair selected by the pair-training
action: `en<->es`.

Window:

```text
pair: en<->es
meaning: 302 / satyam-eva-jayate
trained unordered neural pairs: 7 -> 8
trained directed neural routes: 14 -> 16
epochs: 8
native neural parameters: 8
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
    learn/speech-native-neural-pair-window-0008.fk \
    learn/tests/speech-native-neural-pair-window-0008-band.fk > /tmp/speech-native-neural-pair-window-0008.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0008.fk
```

```text
32767
```
