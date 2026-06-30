# 2026-06-30 -- multiseed speech learning sweep

## What Changed

Added `learn/multiseed-speech-learning-sweep.fk`.

The selected-window bridge proved one seeded A/B locale window. This sweep runs
five deterministic seeds through the same native Form receipt:

```text
0: zh <-> ar
1: en <-> id
2: sa <-> la
3: fr <-> id
4: pt-br <-> zh
```

Each window keeps A->B, B->A, A->A, and B->B lanes. The receipt counts ready
windows, before-guided routes, after-native routes, shifted windows,
choice/cut/fail/undo/timeout cleanliness, A/B promotion, local oracle/Metal,
Form-native status, pending neural Metal/diffusion status, and a pair-index
coverage code.

## Witness

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/diverse-locale-pairing.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-locale-learning-window.fk \
    learn/multiseed-speech-learning-sweep.fk \
    learn/tests/multiseed-speech-learning-sweep-band.fk > /tmp/multiseed-speech-learning-sweep.fk
./fkwu --src /tmp/multiseed-speech-learning-sweep.fk
```

Output:

```text
32767
```

## What 32767 Proves

- Five seeded windows are present.
- The sweep covers the expected diverse seed lanes with coverage code `2725`,
  including non-Latin `zh/ar` and Sanskrit/Latin `sa/la`.
- All five windows satisfy the native-ready floor.
- All five move from guided to native route.
- Controls stay clean and A/B promotion succeeds across the sweep.
- The local oracle/device are carried in the receipt.
- Form-native and local-oracle Metal flags are present.
- Neural Metal and diffusion remain pending, not claimed.

## Honest Boundary

This makes multi-pair movement executable. It does not turn the stack into open
ASR, open translation, or a native neural vocoder. The model is still the
closed-set Form learner over neutral meaning rows and audio features; the value
is that several diverse reversible windows now shift under one auditable
receipt instead of one selected seed at a time.
