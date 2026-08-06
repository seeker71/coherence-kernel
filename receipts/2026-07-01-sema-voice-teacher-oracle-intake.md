# Sema voice teacher oracle intake

This receipt records the first passing local teacher voice row as Sema voice
training evidence without promoting native Sema voice authority.

Live metal run:

- Renderer: macOS `say`, voice `Flo (English (US))`.
- Text: `Open speech flows.`
- Local oracle: `whisper.cpp-large-v3-turbo` on Apple M4 Max Metal.
- Verdict: `255`.
- WER: `0`.
- Witnessed audio hash: `497318870`.

The Form intake records this as oracle-positive and native-held:

- Oracle teacher evidence: `1/1 = 100%`.
- Native Sema voice authority: `0/1 = 0%`.
- Route: `oracle-teacher-positive-native-held`.
- Boundary: host teacher training evidence, not native Sema authority.

Witness:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    presence/macos-sema-voice-teacher-carrier.fk \
    learn/speech-live-receipt-intake.fk \
    learn/sema-voice-teacher-oracle-intake.fk \
    learn/tests/sema-voice-teacher-oracle-intake-band.fk > /tmp/sema-voice-teacher-oracle-intake.fk
./fkwu --src /tmp/sema-voice-teacher-oracle-intake.fk
```

Result: `32767`.

This closes the `0/0` teacher-oracle hole. It does not close the native live
Sema voice hole; the native formant/vocoder still needs to make Whisper hear
the phrase from native audio.
