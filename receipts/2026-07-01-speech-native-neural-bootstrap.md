# Speech native neural bootstrap

This receipt fixes the zero-as-gate problem without pretending a trained speech
model exists.

Result:

```text
enabled slots: 1
native neural parameters: 1
training epochs: 1
sample count: 1
weight: 1 -> 2
prediction: 1 -> 2
loss: 1 -> 0
bootstrap ready: true
route enabled: true
trained locale pairs: 0
```

Meaning: the route now has a real Form-native neural micro-kernel to train
toward. Locale `A=>neural=>B` pair authority remains unclaimed until the pair
itself has receipts.

Witness:

```sh
cat learn/speech-native-neural-bootstrap.fk \
    learn/tests/speech-native-neural-bootstrap-band.fk > /tmp/speech-native-neural-bootstrap.fk
./fkwu --src /tmp/speech-native-neural-bootstrap.fk
```

```text
32767
```
