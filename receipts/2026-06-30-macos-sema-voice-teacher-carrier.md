# macOS Sema voice teacher carrier

Ran a real local voice sweep before adding this carrier.

Manual sweep:

```text
Samantha, Flo (English US), Grandma (English US), Eddy (English US),
Daniel, Karen, Moira, Tessa, Zoe, Serena
```

Each voice rendered `Open speech flows.` with local `say`, converted through
local `ffmpeg`, and transcribed through local `whisper-cli` on Apple Metal. All
10 returned the exact transcript `Open speech flows.`.

Added `presence/macos-sema-voice-teacher-carrier.fk`, which makes that path
repeatable from Form for the selected teacher voice `Flo (English (US))`.

Witnesses:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    presence/macos-sema-voice-teacher-carrier.fk \
    presence/tests/macos-sema-voice-teacher-carrier-band.fk > /tmp/macos-sema-voice-teacher-carrier.fk
./fkwu --src /tmp/macos-sema-voice-teacher-carrier.fk
```

Result:

```text
4095
```

Live carrier run:

```sh
./fkwu --src /tmp/macos-sema-voice-teacher-live.fk
```

Results:

```text
mstc-run-verdict = 255
mstc-run-wer = 0
```

Boundary: this is actual local audio generation and local Metal STT, but it is
a host teacher carrier. It is not native Sema TTS authority. The current native
formant carrier still returns WER `100`; these intelligible local wavs are
teacher material for the native acoustic/vocoder learner.
