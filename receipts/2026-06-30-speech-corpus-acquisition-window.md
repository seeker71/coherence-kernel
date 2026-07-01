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
selected keypaths: 2000
voices per locale: 1
planned wavs: 12000
planned held-out rows: 1200
floor: 12000 wavs, 6 locales, 1200 held-out rows
captured: 64 admitted rows in batches 0001 through 0003; full 12000-row window not captured
trained: false
status: corpus-scale-window-open-not-trained
```

Cross-voice window:

```text
ready cross-voice locales on this Mac: 5
voices per cross-voice locale: 2
planned cross-voice wavs: 20000
planned cross-voice held-out rows: 2000
boundary: Indonesian has one host voice here, so it is not counted in the two-voice host-ready lane
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
65535
```

Meaning: the self-corpus can fill the first corpus-scale wav floor, but only the
first 64 rows have been captured so far. The next real step is expanding the
live acquisition runner until the full raw and cross-voice windows are rendered
and witnessed locally before any training promotion.
