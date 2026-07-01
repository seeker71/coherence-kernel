# Speech Form pair window 0008

This executes the next selected reciprocal Form-native pair window: `en<->fr`.
It expands observed Form-native pair coverage. It does not train or promote a
neural model.

Window:

```text
pair: en<->fr
meaning: 304 / lokah-samastah-sukhino-bhavantu
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
Form pair windows: 7 -> 8
directed cross-locale directions: 14 -> 16
roundtrip lanes: 28 -> 32
neural pair windows: 0 -> 0
```

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-form-pair-window-0008.fk \
    learn/tests/speech-form-pair-window-0008-band.fk > /tmp/speech-form-pair-window-0008.fk
./fkwu --src /tmp/speech-form-pair-window-0008.fk
```

```text
32767
```
