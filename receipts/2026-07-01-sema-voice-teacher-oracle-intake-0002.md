# Receipt -- Sema voice teacher oracle intake 0002 (2026-07-01)

This patch admits one more real local teacher row for Sema voice acoustic/vocoder
training evidence. It does not promote native Sema live voice authority.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -v Samantha -> ffmpeg -> whisper-cli`
- Truth: `Truth alone triumphs.`
- Heard: `Truth alone triumphs.`
- WER: `0`
- WAV bytes: `41694`
- cksum: `1857953984`

Form movement:

- Added `learn/sema-voice-teacher-oracle-intake-0002.fk`.
- Added `learn/tests/sema-voice-teacher-oracle-intake-0002-band.fk`.
- Teacher-oracle evidence moves `1/1 -> 2/2`.
- Teacher-native authority remains `0/2`.
- Live Sema voice remains oracle `1/1`, native `0/1`.

Witness:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    learn/speech-live-receipt-intake.fk \
    learn/sema-voice-teacher-oracle-intake-0002.fk \
    learn/tests/sema-voice-teacher-oracle-intake-0002-band.fk > /tmp/sema-voice-teacher-oracle-intake-0002.fk
./fkwu --src /tmp/sema-voice-teacher-oracle-intake-0002.fk
# 32767
```

Boundary:

This is local, consentful, host-rendered teacher evidence for the native voice
training path. It is not native Sema TTS authority and does not change the live
authority row.
