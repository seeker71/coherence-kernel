# Speech native neural pair window 0005

This trains the next native neural micro-pair selected by the pair-training
action: `en<->ar`.

Window:

```text
pair: en<->ar
meaning: 304 / lokah-samastah-sukhino-bhavantu
trained unordered neural pairs: 4 -> 5
trained directed neural routes: 8 -> 10
epochs: 5
native neural parameters: 5
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
    learn/speech-native-neural-pair-window-0005.fk \
    learn/tests/speech-native-neural-pair-window-0005-band.fk > /tmp/speech-native-neural-pair-window-0005.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0005.fk
```

```text
32767
```
