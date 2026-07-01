# Speech native neural pair window 0003

This trains the next native neural micro-pair selected by the pair-training
action: `en<->id`.

Window:

```text
pair: en<->id
meaning: 302 / satyam-eva-jayate
trained unordered neural pairs: 2 -> 3
trained directed neural routes: 4 -> 6
epochs: 3
native neural parameters: 3
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
    learn/speech-native-neural-pair-window-0003.fk \
    learn/tests/speech-native-neural-pair-window-0003-band.fk > /tmp/speech-native-neural-pair-window-0003.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0003.fk
```

```text
32767
```
