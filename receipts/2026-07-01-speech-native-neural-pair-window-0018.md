# Speech native neural pair window 0018

This trains the next native neural micro-pair selected by the pair-training
action before this patch: `de<->id`.

Window:

```text
pair: de<->id
meaning: 301 / sarve-bhavantu-sukhinah
trained unordered neural pairs: 17 -> 18
trained directed neural routes: 34 -> 36
epochs: 18
native neural parameters: 18
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
    learn/speech-native-neural-pair-window-0018.fk \
    learn/tests/speech-native-neural-pair-window-0018-band.fk > /tmp/speech-native-neural-pair-window-0018.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0018.fk
```

```text
32767
```
