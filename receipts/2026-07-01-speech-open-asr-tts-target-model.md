# Speech open ASR/TTS target model

The target is a native model that can reach or beat full open ASR/TTS. The
micro-pair is not the ceiling; it is the first non-zero training foothold.

```text
target open ASR: true
target open TTS: true
beat local oracle: true
target rate: 100
current open ASR native/oracle: 0/100
current Sema voice native/oracle eval: 0/1 / 0/1
trained neural pairs enabled: 12
native neural parameters enabled: 12
route enabled: true
```

Witness:

```sh
cat learn/speech-open-asr-tts-target-model.fk \
    learn/tests/speech-open-asr-tts-target-model-band.fk > /tmp/speech-open-asr-tts-target-model.fk
./fkwu --src /tmp/speech-open-asr-tts-target-model.fk
```

```text
32767
```
