# Sema Voice Local Oracle Receipt

The Sema voice loop now has an explicit local STT transcription bar.

Added `learn/sema-voice-local-oracle-receipt.fk`, which joins:

- `observe/stt-wer.fk` for token WER.
- `presence/formant-vocoder.fk` for the native source-filter carrier.
- `learn/sema-voice-sample-loop.fk` for target, sample score, and A/B promotion.

The receipt admits a generated Sema voice sample only when the sample is local,
the audio hash is present and matches the sample, consent is present, the local
oracle transcript reaches side-channel truth under the WER threshold, and
fail/timeout/undo/cloud/missing-audio conditions are absent. A sample can promote
only when that local oracle bar and the existing Sema voice sample A/B loop both
pass.

## Witness

```sh
( cat \
    observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    learn/tests/sema-voice-local-oracle-receipt-band.fk \
  > /tmp/sema-voice-local-oracle-receipt.fk
./fkwu --src /tmp/sema-voice-local-oracle-receipt.fk
```

Verdict:

```text
32767
```

## Honest Boundary

This is the Form-owned gate for the question "does our generated Sema voice pass
the local oracle STT bar?" It does not claim a fresh live wav from the Sema
formant voice has already passed `whisper.cpp` on Apple Metal. Earlier live
receipts prove macOS `say` and closed-prompt carriers against local Whisper; the
next receipt should render a Sema voice sample from this loop, feed that wav to
the local oracle, and submit the measured transcript row to this cell.
