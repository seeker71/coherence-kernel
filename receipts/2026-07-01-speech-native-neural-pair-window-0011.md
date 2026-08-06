# Speech native neural pair window 0011

This trains the next native neural micro-pair selected by the pair-training
action: `sa<->la`.

Window:

```text
pair: sa<->la
meaning: 304 / lokah-samastah-sukhino-bhavantu
trained unordered neural pairs: 10 -> 11
trained directed neural routes: 20 -> 22
epochs: 11
native neural parameters: 11
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
    learn/speech-native-neural-pair-window-0011.fk \
    learn/tests/speech-native-neural-pair-window-0011-band.fk > /tmp/speech-native-neural-pair-window-0011.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0011.fk
```

```text
32767
```
