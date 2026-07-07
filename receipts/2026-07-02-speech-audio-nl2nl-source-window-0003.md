# speech-audio-nl2nl-source-window-0003

## What moved

This patch bridges the observed fi/ja Metal loopback rows from
`speech-open-asr-trial-window-0010` into the audio -> neutral Form -> audio
source-window lane.

The route is reciprocal:

- `fi -> ja`: source `minä olen`, heard `Minä olen.`, native neutral meaning
  `602`, target `私はいます`, heard `私はいます。`
- `ja -> fi`: source `私はいます`, heard `私はいます。`, native neutral meaning
  `602`, target `minä olen`, heard `Minä olen.`

The audio hashes and bytes are the already-receipted local Mac Metal loopback
evidence:

- Finnish wav: cksum `2803940054`, bytes `30744`
- Japanese wav: cksum `704718359`, bytes `52762`
- Source-window reciprocal byte total: `167012`

No new audio artifact was committed.

## Result

- source-window routes: `2/2`
- source audio oracle: `2/2`
- target audio oracle: `2/2`
- native neutral routing: `2/2`
- source-target/audio-NL2NL bridge receipts: `18 -> 20`
- target frames/audio rows: `34 -> 38`
- oracle-guided audio A/B routes: `16 -> 18`
- audio-NL2NL observed wav bytes: `761176 -> 928188`
- global live ASR/TTS authority remains oracle-held
- native vocoder authority remains false
- C seed growth: `0`

## Witnesses

```sh
( cat observe/stt-wer.fk \
    learn/speech-corpus-training-intake-0001.fk \
    learn/speech-corpus-training-intake-0002.fk \
    learn/speech-corpus-training-intake-0003.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-audio-nl2nl-source-window-0003.fk \
    learn/tests/speech-audio-nl2nl-source-window-0003-band.fk
  printf '\n(speech-audio-nl2nl-source-window-0003-band)\n' ) > /tmp/sanw3.fk
./fkwu --src /tmp/sanw3.fk
# 32767
```

Aggregate witnesses after the update:

- `speech-model-metrics-report-band` -> `32767`
- `speech-current-status-ledger-band` -> `32767`
- `speech-learning-data-sufficiency-band` -> `65535`
- `speech-pair-training-next-action-band` -> `32767`
- `speech-authority-learning-priority-band` -> `32767`

## Honest boundary

This proves another native Form neutral route over real local-oracle audio. It
does not promote the native vocoder, and it does not promote global live open
ASR/TTS authority.
