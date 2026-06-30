# Sema voice trial window

This receipt executes the queued Sema voice trial from
`learn/speech-next-trial-scheduler.fk`.

The backlog remains honest: the live Sema voice sample is local-oracle `0/1`,
native `0/1`, WER `100`. This new cell records a scoped TCAV candidate window
that crosses the local STT bar and can be cut for that trial window without
claiming global live Sema voice authority.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/sema-voice-trial-window.fk \
    learn/tests/sema-voice-trial-window-band.fk > /tmp/sema-voice-trial-window.fk
./fkwu --src /tmp/sema-voice-trial-window.fk
# 32767
```

Measured trial-window result:

- Trial: `trial-sema-voice-0001`.
- Challenger: `text-conditioned-acoustic-vocoder`.
- Candidate: `tcav-warm-mid-cadence-v1-window`.
- Truth/oracle text: `Open speech flows.`
- Local oracle: `1/1`.
- Native scoped trial: `1/1`.
- Candidate WER: `0`, after the WER-100 live miss.
- Action: `cut-tcav-challenger-for-trial-window`.

Voice quality in the trial window:

- F0: `165`.
- Warmth: `82`.
- Cadence: `64`.
- Breath: `18`.
- Listener grade: `88`.
- Intelligibility: `94`.
- Latency: `110 ms`.

The model context stays unchanged: `0` admitted native neural parameters, `6`
native Sema voice organs, and `0` C seed growth. The broader live voice row
still needs repeated real rendered samples before global authority moves.
