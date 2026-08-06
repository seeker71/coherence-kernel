# Sema voice vocoder oracle bridge

Date: 2026-06-30

This receipt adds the executable handoff from the text-conditioned acoustic
vocoder sample row shape into the Sema local-oracle STT bar.

`learn/sema-voice-vocoder-oracle-bridge.fk` consumes a TCAV sample row shape and
converts it into:

- a Sema voice sample profile,
- a `sema-voice-local-oracle-row`,
- and a receipt preserving WER, route, oracle/device, audio hash, source,
  native Form, and neural-pending evidence.

Witness:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    observe/speech-token-stream.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    learn/sema-voice-vocoder-oracle-bridge.fk \
    learn/tests/sema-voice-vocoder-oracle-bridge-band.fk > /tmp/sema-voice-vocoder-oracle-bridge.fk
./fkwu --src /tmp/sema-voice-vocoder-oracle-bridge.fk
# 32767
```

Boundary: this is not a live passing voice claim. The full TCAV plus candidate
search composition is not loaded into one source-runner file here because that
crosses today's direct-source ceiling; this receipt proves the row handoff that
lets those lanes meet through a split receipt.
