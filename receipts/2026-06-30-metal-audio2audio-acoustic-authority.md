# Metal Audio2Audio Acoustic Authority

Date: 2026-06-30

The multilocale decoded-token acoustic sweep and the seven live Metal pair
anchors now meet in a Form-native authority row:

```text
acoustic sweep: 10 rows, 5 reciprocal pairs, route native
live Metal anchors: 7 observed, 7 ready, route full-metal-native
combined route: metal-witnessed-audio2audio-acoustic
source authority: decoded-token-source
live open mic: pending
neural Metal: pending
```

The authority cell uses scalar summary values rather than nested receipt
transfer because the current direct-source lane can invalidate older list
receipts after later allocations. The source bands are still witnessed
separately.

## Witness

```sh
cat learn/metal-audio2audio-acoustic-authority.fk \
    learn/tests/metal-audio2audio-acoustic-authority-band.fk \
  > /tmp/metal-audio2audio-acoustic-authority.fk
./fkwu --src /tmp/metal-audio2audio-acoustic-authority.fk
```

Observed:

```text
32767
```

Source witnesses:

```text
multilocale-audio2audio-acoustic-sweep-band -> 32767
metal-live-pair-anchors-band -> 32767
speech-model-auto-selection-band -> 33554431
```

## Selector Shift

`speech-model-auto-selection` now selects
`native-audio2audio-acoustic-vocoder` for the audio2audio arm in the scoped
decoded-token, metal-witnessed acoustic path.

## Boundary

This is not open microphone authority. It is not a natural neural vocoder. The
source side is still decoded tokens from the ASR/segmentation side, and neural
Metal remains pending.
