# Speech oracle/native backlog

This receipt adds an executable backlog for the two speech gaps still held by
the local oracle.

It answers the uncomfortable counters directly:

- Native neural weight parameters admitted: `0`.
- Native Sema voice organs/components present: `6`.
- Live open dictation: local oracle `4/4 = 100%`, native `0/4 = 0%`.
- Sema live voice sample: local oracle `0/1 = 0%`, native `0/1 = 0%`, WER `100`.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/tests/speech-oracle-native-backlog-band.fk > /tmp/speech-oracle-native-backlog.fk
./fkwu --src /tmp/speech-oracle-native-backlog.fk
# 32767
```

The interpretation is narrow: `0` native neural parameters means no native
neural weight recipe-data has been admitted yet. It does not mean there is no
native speech body. The current body has Form-native speech organs, carriers,
locale routes, source-window audio2audio authority, and candidate vocoder
recipes, but the live Sema voice waveform has not crossed the local STT bar.

Next learning actions:

- Train the live segmented open-ASR source from the oracle-passing dictation
  receipts.
- Render the next Sema voice candidate and keep it under the same local STT
  oracle until the native sample becomes intelligible.
- Admit native ASR/vocoder weight recipe-data only when it is consented, local,
  observable, and witnessed by receipts.
