# Speech corpus acquisition window

The current speech rows are too small for real training. This receipt adds the
next movement: a Form-native acquisition window that uses the consentful
Coherence Network self-corpus to plan enough audio rows to meet the data floor.

Source capacity:

```text
ready corpus keypaths: 2064
ready locales: 6
EN-parallel pair capacity: 10320
selected locales: en, de, es, fr, id, pt-br
```

Window:

```text
selected keypaths: 50
voices per locale: 1
planned wavs: 300
planned held-out rows: 30
floor: 300 wavs, 5 locales, 30 held-out rows
captured/trained: false
status: acquisition-window-ready-not-captured
```

Witness:

```sh
cat learn/coherence-network-self-corpus.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-acquisition-window.fk \
    learn/tests/speech-corpus-acquisition-window-band.fk > /tmp/speech-corpus-acquisition-window.fk
./fkwu --src /tmp/speech-corpus-acquisition-window.fk
```

```text
32767
```

Meaning: the self-corpus can fill the minimum floor, but the rows are not yet
captured. The next real step is a live acquisition runner that renders and
witnesses this window locally before any training promotion.
