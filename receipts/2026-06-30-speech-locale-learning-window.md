# 2026-06-30 -- speech locale learning window

## What Changed

Added `learn/speech-locale-learning-window.fk`.

The previous cells proved the pieces separately:

```text
diverse pair selection
Sanskrit baseline coverage
NL/audio through neutral Form meaning
per-pair route-shift ledger
speech model AutoML selector
```

This receipt adds the executable bridge for one selected window. Seed `2` maps to the Sanskrit/Latin baseline
lane (`sa<->la`) and produces a numeric window row:

```text
locale indexes
A->B, B->A, A->A, B->B lane count
self-corpus vs baseline readiness flags
before/after NL rates
before/after audio rates
before/after route codes
shifted flag
audio metadata confidence
clean-control flag
A/B promotion flag
local oracle/Metal and Form-native flags
pending neural Metal and diffusion flags
```

## Witness

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/diverse-locale-pairing.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-locale-learning-window.fk \
    learn/tests/speech-locale-learning-window-band.fk > /tmp/speech-locale-learning-window.fk
./fkwu --src /tmp/speech-locale-learning-window.fk
```

Output:

```text
16383
```

## What 16383 Proves

- Seed `2` selects the Sanskrit/Latin baseline lane by catalog indexes.
- Both sides are ready through baseline rows while full self-corpus bundles remain absent.
- The selected pair exposes A->B, B->A, A->A, and B->B lanes.
- NL and audio rates move from below floor to `100`.
- Route code shifts from `0` (`oracle-guide`) to `1` (`native`).
- Audio target metadata preserves confidence.
- Clean controls and A/B evidence promote the challenger.
- Local oracle/Metal and Form-native flags are present.
- Neural Metal and diffusion remain pending, not claimed.

## Honest Boundary

The window is numeric on purpose. The current direct-source lane does not safely retain every string-rich row when
the pair guide, multilocale pipeline, route ledger, observed controller, and model selector are all loaded into one
large composition. The string-rich route ledger (`4095`) and model selector (`4095`) remain separate executable
witnesses; this cell is the stable selected-window bridge that can keep learning moving without growing the C seed.
