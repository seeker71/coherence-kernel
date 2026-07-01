# Speech native neural pair window 0019

This trains the next native neural micro-pair selected by the pair-training
action before this patch: `de<->pt-br`.

Window:

```text
pair: de<->pt-br
meaning: 301 / sarve-bhavantu-sukhinah
trained unordered neural pairs: 18 -> 19
trained directed neural routes: 36 -> 38
epochs: 19
native neural parameters: 19
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
    learn/speech-native-neural-pair-window-0019.fk \
    learn/tests/speech-native-neural-pair-window-0019-band.fk > /tmp/speech-native-neural-pair-window-0019.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0019.fk
```

```text
32767
```
