# Speech Form pair window 0006

This executes the next selected reciprocal Form-native pair window: `en<->de`.
It expands observed Form-native pair coverage. It does not train or promote a
neural model.

Window:

```text
pair: en<->de
meaning: 302 / satyam-eva-jayate
lanes: A->B, B->A, A->A, B->B for NL and audio
native Form: true
neural: false
```

Result:

```text
NL rate: 0 -> 100
audio rate: 0 -> 100
route: oracle-guide -> native
shifted: true
diffusion: false
neural training: false
```

Coverage effect:

```text
Form pair windows: 5 -> 6
directed cross-locale directions: 10 -> 12
roundtrip lanes: 20 -> 24
neural pair windows: 0 -> 0
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-form-pair-window-0006.fk \
    learn/tests/speech-form-pair-window-0006-band.fk > /tmp/speech-form-pair-window-0006.fk
./fkwu --src /tmp/speech-form-pair-window-0006.fk
```

```text
32767
```
