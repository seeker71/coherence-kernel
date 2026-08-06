# Live Sema Formant Oracle Probe

Date: 2026-06-30

This lands the live carrier that was missing from the Sema voice local-oracle
gate. The Form carrier starts from `svsl-target`, builds a target-derived
`ffmpeg aevalsrc` waveform, writes a local WAV, runs `whisper.cpp-large-v3-turbo`
locally on Apple Metal, and feeds the measured transcript into
`learn/sema-voice-local-oracle-receipt.fk`.

## What Changed

- Added `presence/macos-sema-voice-local-oracle-carrier.fk`.
- Added `presence/tests/macos-sema-voice-local-oracle-carrier-band.fk`.
- Added `docs/coherence-substrate/macos-sema-voice-local-oracle-carrier.form`.

The contract band proves the carrier law without host effects:

```text
2047
```

## Live Result

The local carrier rendered:

```text
/var/folders/xt/5zt6_wmn77x22yf_wgv97cb40000gn/T/sema-voice-local-oracle-form/sema-formant-target.wav
```

Observed file size:

```text
57678 bytes
```

Whisper loaded the local Apple Metal backend (`MTL0`, Apple M4 Max) and decoded
the generated waveform.

Live carrier verdict:

```text
479
```

Field code:

```text
110100002
```

Meaning:

- audio present: `1`
- row valid: `1`
- oracle success: `0`
- oracle WER: `100`
- route code: `2` (`oracle-guide`)

The current Sema formant waveform is observable on metal, but it does not yet
sound like the side-channel text to the local oracle. That is the right result:
the route remains `oracle-guide`, and the next improvement target is no longer
abstract.

## Witness Commands

Carrier contract:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    presence/macos-sema-voice-local-oracle-carrier.fk \
    presence/tests/macos-sema-voice-local-oracle-carrier-band.fk \
  > /tmp/macos-sema-voice-local-oracle-carrier-band.fk
./fkwu --src /tmp/macos-sema-voice-local-oracle-carrier-band.fk
```

Live carrier:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    presence/macos-sema-voice-local-oracle-carrier.fk \
  > /tmp/macos-sema-voice-local-oracle-carrier-live.fk
printf '\n(msvlc-run)\n' >> /tmp/macos-sema-voice-local-oracle-carrier-live.fk
./fkwu --src /tmp/macos-sema-voice-local-oracle-carrier-live.fk
```

Field code and WER:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    presence/macos-sema-voice-local-oracle-carrier.fk \
  > /tmp/macos-sema-voice-local-oracle-carrier-field.fk
printf '\n(msvlc-run-field-code)\n' >> /tmp/macos-sema-voice-local-oracle-carrier-field.fk
./fkwu --src /tmp/macos-sema-voice-local-oracle-carrier-field.fk

cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    presence/macos-sema-voice-local-oracle-carrier.fk \
  > /tmp/macos-sema-voice-local-oracle-carrier-wer.fk
printf '\n(msvlc-run-oracle-wer)\n' >> /tmp/macos-sema-voice-local-oracle-carrier-wer.fk
./fkwu --src /tmp/macos-sema-voice-local-oracle-carrier-wer.fk
```

Observed:

```text
110100002
100
```

## Boundary

This is not a passing Sema voice sample. It is the first live, local, metal
measurement of the Sema formant carrier against the local STT bar. Passing that
bar now requires a better acoustic model/vocoder or a text-conditioned native
speech generator, not more receipt wording.
