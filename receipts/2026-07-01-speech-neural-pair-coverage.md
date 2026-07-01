# Speech neural pair coverage

This receipt answers the locale-pair training question without collapsing
Form-native seeded windows into neural training.

Current neural pair coverage:

```text
trained unordered neural pairs: 13
trained directed neural pair routes: 26
neural training epochs: 13
native neural parameters: 13
broad ready pair space: 55 unordered / 110 directed
neural coverage: 2363 basis points
```

Current Form-native seeded reciprocal coverage:

```text
seeded reciprocal pair windows: 8
directed cross-locale directions: 16
roundtrip lanes: 32 A->B, B->A, A->A, and B->B lanes across NL and audio windows
ready rate: 100
broad ready coverage: 1454 basis points
Sanskrit-baseline pair space: 45 unordered / 90 directed
Sanskrit-baseline coverage: 1777 basis points
```

Observed Form-native/prototype pairs:

```text
Form windows: zh<->ar, en<->id, sa<->la, fr<->id, pt-br<->zh, en<->de, en<->es, en<->fr
segmented source-ASR prototypes: sa<->la, en<->zh, ar<->en
live Metal anchors: en<->de, en<->es, en<->id, en<->fr, en<->it, en<->zh, en<->ar
audio NL2NL bridge routes: 12 oracle-guided routes, not native vocoder and not neural
```

Trained neural micro-pair:

```text
pairs: en<->fr, en<->pt-br, en<->id, en<->zh, en<->ar, en<->la, en<->de, en<->es, en<->sa, zh<->ar, sa<->la, zh<->sa, ar<->sa
files: learn/speech-native-neural-pair-window-0001.fk, learn/speech-native-neural-pair-window-0002.fk, learn/speech-native-neural-pair-window-0003.fk, learn/speech-native-neural-pair-window-0004.fk, learn/speech-native-neural-pair-window-0005.fk, learn/speech-native-neural-pair-window-0006.fk, learn/speech-native-neural-pair-window-0007.fk, learn/speech-native-neural-pair-window-0008.fk, learn/speech-native-neural-pair-window-0009.fk, learn/speech-native-neural-pair-window-0010.fk, learn/speech-native-neural-pair-window-0011.fk, learn/speech-native-neural-pair-window-0012.fk, learn/speech-native-neural-pair-window-0013.fk
boundary: non-zero trained pair coverage, not full open ASR/TTS authority
```

Witness:

```sh
cat learn/speech-neural-pair-coverage.fk \
    learn/tests/speech-neural-pair-coverage-band.fk > /tmp/speech-neural-pair-coverage.fk
./fkwu --src /tmp/speech-neural-pair-coverage.fk
```

```text
32767
```

Meaning: locale `A => neural => B` and `B => neural => A` coverage is now
non-zero. The current movement is trained Form-native neural micro-pairs,
guided by local oracles and the Sanskrit/multi-locale baseline; full open
ASR/TTS authority remains the target, not the current claim.
