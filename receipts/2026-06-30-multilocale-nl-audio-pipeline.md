# 2026-06-30 -- multilocale NL/audio pipeline through neutral Form

## What Changed

Added `learn/sanskrit-locale-baseline.fk` and `learn/multilocale-nl-audio-pipeline.fk`.

The new baseline uses romanized Sanskrit seed phrases with ready renderings for `sa`, `en`, `de`, `es`, `fr`,
`id`, `pt-br`, `la`, `zh`, and `ar`. The full Coherence Network `zh` and `ar` bundles still wait for translated
rows; this small baseline now has ready script tokens.

The pipeline proves the closed-set route:

```text
text(A) -> neutral Form meaning -> text(B)
audio(A) features -> neutral Form meaning -> audio(B) target
```

Audio targets keep metadata for confidence, attunement, excitement, and cadence.

## Witness

```sh
cat learn/sanskrit-locale-baseline.fk \
    learn/tests/sanskrit-locale-baseline-band.fk > /tmp/sanskrit-locale-baseline-band.fk
./fkwu --src /tmp/sanskrit-locale-baseline-band.fk

cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/tests/multilocale-nl-audio-pipeline-band.fk > /tmp/multilocale-nl-audio-pipeline-band.fk
./fkwu --src /tmp/multilocale-nl-audio-pipeline-band.fk
```

Output:

```text
2047
8191
```

## Result

The band covers five reciprocal pairs:

```text
en <-> de
en <-> es
zh <-> ar
fr <-> id
sa <-> la
```

Untrained NL/audio routes stay below the 50% floor. Oracle-valid samples train both routes to 100% on the closed
set, and the receipt flips to `native`. Timeout, bad oracle transcript, and invalid audio metadata block credit.

## Honest Boundary

This is end-to-end NL-to-NL and audio-to-audio as a Form-native closed-set learning loop. It is not open ASR,
not open translation, and not a neural native vocoder. The local oracle can run on Metal; the selected native
learner here is transparent Form code.
