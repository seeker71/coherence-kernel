# Speech native neural pair window 0017

This trains the next native neural micro-pair selected by the pair-training
action: `de<->fr`.

Window:

```text
pair: de<->fr
meaning: 301 / sarve-bhavantu-sukhinah
trained unordered neural pairs: 16 -> 17
trained directed neural routes: 32 -> 34
epochs: 17
native neural parameters: 17
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
    learn/speech-native-neural-pair-window-0017.fk \
    learn/tests/speech-native-neural-pair-window-0017-band.fk > /tmp/speech-native-neural-pair-window-0017.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0017.fk
```

```text
32767
```
