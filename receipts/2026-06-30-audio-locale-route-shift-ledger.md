# 2026-06-30 -- audio locale route-shift ledger

## What Changed

Added `learn/audio-locale-route-shift-ledger.fk`.

The audio locale trainer already learned from oracle-valid reciprocal samples and routed native over a floor. This
adds the before/after witness row for that movement:

```text
before native score/rate
after native score/rate
before A->B and B->A rates
after A->B and B->A rates
before route
after route
shifted flag
```

## Witness

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    observe/wav-sense.fk \
    learn/audio-locale-native-training.fk \
    learn/audio-locale-route-shift-ledger.fk \
    learn/tests/audio-locale-route-shift-ledger-band.fk > /tmp/audio-locale-route-shift-ledger-band.fk
./fkwu --src /tmp/audio-locale-route-shift-ledger-band.fk
```

Output:

```text
8191
```

## Live Composition

The macOS carrier remains independent so it can run without the ledger. To compose the live Metal carrier with the
route-shift ledger, load the carrier first and the ledger second:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    observe/wav-sense.fk \
    learn/audio-locale-native-training.fk \
    presence/macos-speech-roundtrip-carrier.fk \
    learn/audio-locale-route-shift-ledger.fk > /tmp/masr-shift-composed.fk
```

Then evaluate the composed receipt over `masr-live-candidates` and `masr-live-samples`. The witnessed output was:

```lisp
(do
  (let _mk (fs_mkdir (masr-out-dir)))
  (let candidates (masr-live-candidates))
  (let samples (masr-live-samples (masr-model)))
  (let r (alrs-trained-receipt samples candidates
                               (masr-a) (masr-b)
                               (masr-max-wer) (masr-floor)
                               (masr-oracle) (masr-device)))
  (add (mul (alrs-rec-shifted r) 1000000000000000)
       (alrs-metric-code r)))
```

```text
1012100010008301
```

That is `shifted=1` plus metric code `12100010008301`:

```text
count=12, oracle_ok=10, before_native=0, after_native=10, before_rate=0, after_rate=83, shifted=1
```

## Honest Boundary

This is still closed-prompt audio learning, not open ASR. The value is that the live Metal carrier now has an
observable before/after route-shift witness that composes with the standalone Form trainer without growing the C
seed or hiding the local oracle.
