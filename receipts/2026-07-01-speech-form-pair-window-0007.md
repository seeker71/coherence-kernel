# Speech Form pair window 0007

This executes the next selected reciprocal Form-native pair window: `en<->es`.
It expands observed Form-native pair coverage. It does not train or promote a
neural model.

Window:

```text
pair: en<->es
meaning: 303 / aham-asmi
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
Form pair windows: 6 -> 7
directed cross-locale directions: 12 -> 14
roundtrip lanes: 24 -> 28
neural pair windows: 0 -> 0
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-form-pair-window-0007.fk \
    learn/tests/speech-form-pair-window-0007-band.fk > /tmp/speech-form-pair-window-0007.fk
./fkwu --src /tmp/speech-form-pair-window-0007.fk
```

```text
32767
```
