# 2026-06-30 -- multilocale route-shift ledger

## What Changed

Added `learn/multilocale-route-shift-ledger.fk`.

The multilocale pipeline already had an aggregate receipt. This adds per-pair rows for:

```text
before NL rate
after NL rate
before audio rate
after audio rate
before route
after route
shifted flag
```

The covered reciprocal pairs are still:

```text
en <-> de
en <-> es
zh <-> ar
fr <-> id
sa <-> la
```

## Witness

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/multilocale-route-shift-ledger.fk \
    learn/tests/multilocale-route-shift-ledger-band.fk > /tmp/multilocale-route-shift-ledger-band.fk
./fkwu --src /tmp/multilocale-route-shift-ledger-band.fk
```

Output:

```text
4095
```

## Result

The band proves five pair rows, five oracle-guided-to-native shifts, five native after-routes, and five
oracle-guide before-routes. It also proves the ledger does not flatten mixed evidence: the `en<->de` row can carry
before-NL success while before-audio is still zero, so the route remains guided until both channels hold.

One-way evidence stays `oracle-guide`; reciprocal A/B and B/A evidence is required. The receipt carries the local
Metal oracle/device fields and Form-native learner flag while keeping native neural Metal pending.

## Honest Boundary

This is observability over the closed-set NL/audio loop. It is not open ASR, open translation, or a neural native
vocoder. It makes the route shift inspectable pair by pair so later live carrier receipts can be compared without
collapsing everything into one aggregate score.
