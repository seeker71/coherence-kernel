# Speech neural pair coverage

This receipt answers the locale-pair training question without collapsing
Form-native seeded windows into neural training.

Current neural pair coverage:

```text
trained unordered neural pairs: 0
trained directed neural pair routes: 0
neural training epochs: 0
native neural parameters: 0
broad ready pair space: 55 unordered / 110 directed
neural coverage: 0 basis points
```

Current Form-native seeded reciprocal coverage:

```text
seeded reciprocal pair windows: 7
directed cross-locale directions: 14
roundtrip lanes: 28 A->B, B->A, A->A, and B->B lanes across NL and audio windows
ready rate: 100
broad ready coverage: 1272 basis points
Sanskrit-baseline pair space: 45 unordered / 90 directed
Sanskrit-baseline coverage: 1555 basis points
```

Observed Form-native/prototype pairs:

```text
Form windows: zh<->ar, en<->id, sa<->la, fr<->id, pt-br<->zh, en<->de, en<->es
segmented source-ASR prototypes: sa<->la, en<->zh, ar<->en
live Metal anchors: en<->de, en<->es, en<->id, en<->fr, en<->it, en<->zh, en<->ar
audio NL2NL bridge routes: 12 oracle-guided routes, not native vocoder and not neural
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

Meaning: locale `A => neural => B` and `B => neural => A` coverage is still
zero. The current movement is real but it is Form-native/prototype and
closed-set, guided by local oracles and the Sanskrit/multi-locale baseline.
