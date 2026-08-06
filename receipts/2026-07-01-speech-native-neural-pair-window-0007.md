# Speech native neural pair window 0007

This trains the next native neural micro-pair selected by the pair-training
action: `en<->de`.

Window:

```text
pair: en<->de
meaning: 302 / satyam-eva-jayate
trained unordered neural pairs: 6 -> 7
trained directed neural routes: 12 -> 14
epochs: 7
native neural parameters: 7
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
    learn/speech-native-neural-pair-window-0007.fk \
    learn/tests/speech-native-neural-pair-window-0007-band.fk > /tmp/speech-native-neural-pair-window-0007.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0007.fk
```

```text
32767
```
