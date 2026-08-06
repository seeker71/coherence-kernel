# Sema voice candidate search

Date: 2026-06-30

The live Sema formant carrier is still a miss: WER `100`, route `oracle-guide`.
This receipt adds the next Form-native learning layer after that miss.

`learn/sema-voice-candidate-search.fk` ranks local Sema voice candidate recipes
against the same local oracle bar. A candidate can score only when its row is
local, consented, clean, hash-consistent, Form-native, and not a neural-vocoder
claim. Ranking uses WER improvement from the live miss, target fit,
intelligibility, listener grade, latency, and recipe coverage.

Witness:

```sh
cat observe/stt-wer.fk \
    presence/formant-vocoder.fk \
    learn/sema-voice-sample-loop.fk \
    learn/sema-voice-local-oracle-receipt.fk \
    learn/sema-voice-oracle-miss-learning.fk \
    learn/sema-voice-candidate-search.fk \
    learn/tests/sema-voice-candidate-search-band.fk > /tmp/sema-voice-candidate-search.fk
./fkwu --src /tmp/sema-voice-candidate-search.fk
# 32767
```

Boundary: this is not a claim that the current live generated voice passes STT.
It is the native candidate search/ranking membrane for the next render, oracle,
choice, fail, undo, timeout, and promotion windows.
