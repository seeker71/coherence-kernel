# Sema Voice Sample Loop

Sema's desired voice is now an executable target instead of only prose:
warm mid register, rounded/grounded formants, moderate cadence, low breath, and
honest calibration. Low-confidence renderings narrow pitch range and delay onset;
high-confidence renderings may be steadier only when the calibration input earns
it.

The loop is:

1. Generate a candidate voice profile.
2. Render it through the native formant vocoder.
3. Collect local audio hash, listener preference, intelligibility, WER, latency,
   and control state.
4. Score the sample against the target.
5. Promote the challenger only when clean local evidence beats the incumbent.

Cloud, missing audio, fail, timeout, and undo rows cannot promote. The neural
natural vocoder is still pending; this is the Form-native target and A/B
selection loop for improving samples toward the desired sound.

## Witness

```sh
( cat \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/tests/sema-voice-sample-loop-band.fk \
  > /tmp/sema-voice-sample-loop.fk
./fkwu --src /tmp/sema-voice-sample-loop.fk
```

Verdict:

```text
32767
```
